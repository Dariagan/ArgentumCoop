#include "CppArgentumTileMap.hpp"
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <chrono>

using namespace godot;
void CppArgentumTileMap::generate_formation(const Ref<CppFormationGenerator>& formation_generator, const Vector2i origin, 
    const Vector2i size, const Ref<Resource>& tileSelectionSet, unsigned int seed, const Dictionary& data)
{   
    
    if (size.x < 0 || size.y < 0){
        UtilityFunctions::printerr("Negatively sized formation not allowed");return;}
    constexpr std::uint_fast16_t MIN_SIZE = 600;
    if (size.x < MIN_SIZE || size.y < MIN_SIZE){
        UtilityFunctions::printerr("World size is too small, must be at least ",MIN_SIZE,"x",MIN_SIZE);return;}
    const bool outOfBoundsEast = origin.x + size.x > mWorldMatrixPtr->SIZE.lef;
    const bool outOfBoundsSouth = origin.y + size.y > mWorldMatrixPtr->SIZE.RIGHT;
    const bool negativeOrigin = origin.x < 0 || origin.y < 0;
    const bool outOfBounds = outOfBoundsEast || outOfBoundsSouth || negativeOrigin;

    if (outOfBounds){
        UtilityFunctions::printerr("CANCELLED FORMATION: out of bounds in the world matrix. make the world matrix bigger, move the origin, or resize the formation. Reasons:");
        if(negativeOrigin) UtilityFunctions::printerr("-Negative origin is never allowed for any formation");
        if(outOfBoundsEast||outOfBoundsSouth){
            UtilityFunctions::printerr("-Formation starting at:",origin," of size:",size," is out of worldbounds: ");
            if(outOfBoundsEast) UtilityFunctions::printerr("    -East");
            if(outOfBoundsSouth) UtilityFunctions::printerr("   -South");
        }
    }
    else{
        auto start = std::chrono::high_resolution_clock::now();
        formation_generator->generate(*this, origin, SafeVec(size), tileSelectionSet, seed, data);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        UtilityFunctions::print("time taken to generate formation: ", duration.count(),"ms");
        emit_signal("formation_formed");
    }
}
void CppArgentumTileMap::gLoadTilesAround(const Vector2 global_coords, const Vector2i CHUNK_SIZE, const int being_uid)//hacer q el chunk size cambie según el zoom ingame??
{   
    const SafeVec beingCoords = local_to_map(global_coords);
    
    for (int x = -CHUNK_SIZE.x/2; x <= CHUNK_SIZE.x/2; x++) 
    for (int y = -CHUNK_SIZE.y/2; y <= CHUNK_SIZE.y/2; y++) 
    {
        const SafeVec tileMapCoords(beingCoords.lef + x, beingCoords.RIGHT + y);
        if (!(tileMapCoords.isNonNegative() && tileMapCoords.isStrictlySmallerThan(mWorldMatrixPtr->SIZE)))
        continue;
        
        if (mTileSharedLoadsCount.count(tileMapCoords) == 0)
        {
            mTileSharedLoadsCount.insert({tileMapCoords, 1});

            if (mBeingsModule->mFrozenBeings.count(tileMapCoords))
            {
                auto& tileFrozenBeings = mBeingsModule->mFrozenBeings[tileMapCoords];

                for (auto it = tileFrozenBeings.begin(); it != tileFrozenBeings.end(); it = tileFrozenBeings.erase(it)){
                    const Vector2 global_coords = it->first;
                    const int individual_unique_id = it->second;
                    emit_signal("being_unfrozen", global_coords, individual_unique_id);
                }
            }
            if (mWorldMatrixPtr->isNotEmptyAt(tileMapCoords))//if more than 0 tileIds at coords:
                for (const TileTypeUid& uid : (*mWorldMatrixPtr)[tileMapCoords])
                    {setCell(uid, tileMapCoords);}
            //TODO
            //else
                //  {setCell("ocean_water", tileMapCoords);}
        }
        else if (mBeingLoadedTiles[being_uid].count(tileMapCoords) == 0)
        {
            mTileSharedLoadsCount[tileMapCoords] += 1;
        }
        mBeingLoadedTiles[being_uid].insert(tileMapCoords);
    }
    const SafeVec topLeftCornerCoords = beingCoords - SafeVec(CHUNK_SIZE.x/2, CHUNK_SIZE.y/2);

    unloadExcessTiles(topLeftCornerCoords, CHUNK_SIZE, being_uid);
}

void CppArgentumTileMap::unloadExcessTiles(const SafeVec& topLeftCornerCoords, const SafeVec& CHUNK_SIZE, const int being_uid)
{
    auto loadedTilesIter = mBeingLoadedTiles[being_uid].begin();

    while (loadedTilesIter != mBeingLoadedTiles[being_uid].end())
    {
        const SafeVec tileCoord = *loadedTilesIter;
        
        if (!withinChunkBounds(tileCoord, topLeftCornerCoords, CHUNK_SIZE))
        {
            decrementSharedCount(tileCoord);

            loadedTilesIter = mBeingLoadedTiles[being_uid].erase(loadedTilesIter);
        }
        else{++loadedTilesIter;}
    }
}
void CppArgentumTileMap::decrementSharedCount(const SafeVec& tileCoord)
{
    if (-- mTileSharedLoadsCount[tileCoord] <= 0)
    {
        for (std::uint_fast8_t layer_i = 0; layer_i < get_layers_count(); layer_i++)
            {erase_cell(layer_i, tileCoord);}

        emit_signal("tile_unloaded", (Vector2i)tileCoord);
        mTileSharedLoadsCount.erase(tileCoord);
    }
}

#define ID StringName("id", true)
#define SOURCE_ATLAS StringName("source_atlas", true)
#define LAYER StringName("layer", true)
#define ATLAS_ORIGIN_POSITION StringName("op", true)
#define ALTERNATIVE_ID StringName("alt_id", true)
#define MODULO_TILING_AREA StringName("ma", true)

bool CppArgentumTileMap::setCell(const TileTypeUid uid, const SafeVec& coords)
{
    if(uid == WorldMatrix::NULL_TILE_UID) return false;

    std::unordered_map<StringName, Variant> tileData;
    StringName TILE_ID = getTileId(uid);
    try{tileData = mCppTilesData.at(TILE_ID);}
    catch(const std::out_of_range& e)
    {UtilityFunctions::printerr(TILE_ID, " not found in cppTilesData (at CppArgentumTileMap.cpp::", __func__,")");
     return false;
    }

    //TODO HACER EL AUTOTILING MANUALMENTE EN ESTA PARTE SEGÚN LAS 4 TILES Q SE TENGA ADYACENTES EN LA WORLDMATRIX
    //PONER EL AGUA EN UNA LAYER INFERIOR? 

    const SafeVec atlasOriginPosition = (SafeVec)tileData[ATLAS_ORIGIN_POSITION];; 

    const SafeVec moduloTilingArea = (SafeVec)tileData[MODULO_TILING_AREA];

    const SafeVec atlasPositionOffset(coords.lef % moduloTilingArea.lef, coords.RIGHT % moduloTilingArea.RIGHT);
    
    int alt_id = 0;
    bool flipped = false;
    try
    {//temp code, replace trees by scenes to make scale and color randomizeable
        alt_id = (int)tileData.at(ALTERNATIVE_ID);
        const bool flippedAtRandom = tileData.at("fr");
 
        // PROBLEMÓN: DESYNC MULTIPLAYER
        flipped = flippedAtRandom && rand() % 2;
    }
    catch(const std::exception& e)
    {
        UtilityFunctions::printerr(e.what());
    }
    set_cell(tileData[LAYER], coords, tileData[SOURCE_ATLAS], atlasOriginPosition + atlasPositionOffset, alt_id + flipped);
    return true;
}
void CppArgentumTileMap::gGenerateWorldMatrix(const Vector2i size, const Dictionary& tiles_data)
{
    if (size.x <= 0 || size.y <= 0){
        UtilityFunctions::printerr("Negatively sized world matrix not allowed");
        return;
    }
    if(mWorldMatrixPtr == nullptr)
    {
        mgGlobalData = get_node<Node>("/root/GlobalData");
        if(mgGlobalData == NULL)
        {
            UtilityFunctions::printerr("couldn't get mgGlobalData node (ArgentumTilemap.cpp)");
        }

        mWorldMatrixPtr = std::make_unique<WorldMatrix>(size);
        gSetTilesData(tiles_data);
        mBeingsModule = std::make_unique<BeingsModule>(this, size);
    } 
    else UtilityFunctions::printerr("World matrix was already generated, cannot be re-generated.");
}

int CppArgentumTileMap::get_seed(){return seed;}
void CppArgentumTileMap::set_seed(const std::uint_fast32_t seed){this->seed=seed; srand(seed);}

void CppArgentumTileMap::replaceTilesDataProperly(const Dictionary& input_tiles_data)
{
    if(m_tiles_data.is_empty())
    {
        const std::uint_fast16_t TILES_COUNT = input_tiles_data.size();

        if(exceedsTileLimit(TILES_COUNT)) return;

        m_tiles_data = input_tiles_data;
        //m
        mTilesUidMapping.resize(TILES_COUNT);
        for(std::uint_fast16_t i = 0; i < TILES_COUNT; i++)
            {mTilesUidMapping[i] = input_tiles_data.keys()[i];}                    
    }
    else overrideTilesDataAndAddNewMappings(input_tiles_data);   
}
void CppArgentumTileMap::overrideTilesDataAndAddNewMappings(const Dictionary& input_tiles_data)
{
    m_tiles_data = input_tiles_data;
    const std::uint_fast16_t NEW_TILES_COUNT = m_tiles_data.size();

    if(exceedsTileLimit(NEW_TILES_COUNT)) return;

    for(std::uint_fast16_t i = 0; i < input_tiles_data.size(); i++)
    {
        const auto& tile_id = input_tiles_data.keys()[i];

        if( ! findTileUid(tile_id).has_value())
            mTilesUidMapping.push_back(tile_id);
    }
}

//constinit int compile_time_value = 42;
CppArgentumTileMap::CppArgentumTileMap(){}

CppArgentumTileMap::~CppArgentumTileMap(){}

std::optional<TileTypeUid> CppArgentumTileMap::findTileUid(const StringName& stringId) const
{
    for(std::uint_fast16_t i = 0; i < mTilesUidMapping.size(); i++)
        if(mTilesUidMapping[i] == stringId) return i;
    return std::nullopt;
}

const StringName CppArgentumTileMap::getTileId(TileTypeUid uid) const
{
    if(uid < mTilesUidMapping.size()) 
        return mTilesUidMapping[uid];
    
    UtilityFunctions::printerr("Couldn't find tile.id for uid:", uid, "argentumtilemap.cpp");
    return "no se encuentra id";
}

Dictionary CppArgentumTileMap::gGetTilesData(){return m_tiles_data;}; 


void CppArgentumTileMap::gSetTilesData(const Dictionary& input_tiles_data)
{
    for(auto &tileData : mCppTilesData){tileData.second.clear();}mCppTilesData.clear();
   
    replaceTilesDataProperly(input_tiles_data);
    
    for(int i = 0; i < m_tiles_data.values().size(); i++)
    {
        const StringName& gd_current_tile_key = (StringName)m_tiles_data.keys()[i];
        //const std::string keyAsCppString = gd_current_tile_key.utf8().get_data();
        
        const Ref<Resource>& tile = Object::cast_to<Resource>(m_tiles_data.values()[i]);

        const Dictionary& gd_tile_data = tile->call("get_data");

        if(gd_tile_data.is_empty() || (StringName)gd_tile_data[ID] == StringName("", true) || (int)gd_tile_data[SOURCE_ATLAS] == -1)
        {
            UtilityFunctions::printerr("read gd_tile_data is not valid (CppArgentumTileMap.cpp::gSetTilesData())");
            continue;
        }

        std::unordered_map<StringName, Variant> cppTileData;

        for(int j = 0; j < gd_tile_data.values().size(); j++)
        {
            const StringName& kgTileFieldKey = gd_tile_data.keys()[j];
            if (kgTileFieldKey == ATLAS_ORIGIN_POSITION)
            {
                const Vector2i kgAtlasOriginPosition = gd_tile_data.values()[j];
                if (((SafeVec)kgAtlasOriginPosition).isAnyCompNegative())//ESTO ESTÁ MAL, SE DEBERÍA CHEQUEAR EN EL MOMENTO CUANDO SE AGREGA TILEDATA, SINO CAUSA SLOWDOWN SI SE CHEQUEA CADA ITERACIÓN
                {
                    UtilityFunctions::printerr("Atlas origin position ",kgAtlasOriginPosition," is negative for tile_id \"",gd_current_tile_key,"\", using (0,0) (at CppArgentumTileMap.cpp::gSetTilesData())");
                    cppTileData.insert({gd_tile_data.keys()[j], Vector2i(0, 0)});
                    continue;
                }
            }
            else if (kgTileFieldKey == MODULO_TILING_AREA)
            {
                const Vector2i kgModuloArea = gd_tile_data.values()[j];
                if (((SafeVec)kgModuloArea).isStrictlyPositive() == false)//ESTO ESTÁ MAL, SE DEBERÍA CHEQUEAR EN EL MOMENTO CUANDO SE AGREGA TILEDATA, SINO CAUSA SLOWDOWN SI SE CHEQUEA CADA ITERACIÓN
                {
                    UtilityFunctions::printerr("non-positive MODULO_TILING_AREA ",kgModuloArea," is not admitted. tile_id:",gd_current_tile_key," (at CppArgentumTileMap.cpp::gSetTilesData())");
                    cppTileData.insert({gd_tile_data.keys()[j], Vector2i(1, 1)});
                    continue;
                }
            }
            cppTileData.insert({gd_tile_data.keys()[j], gd_tile_data.values()[j]});
        }
        if (cppTileData.count(ATLAS_ORIGIN_POSITION) == 0)
        {
            UtilityFunctions::printerr("op not found, using (0,0) (at CppArgentumTileMap.cpp::gSetTilesData())");
            cppTileData.insert({ATLAS_ORIGIN_POSITION, Vector2i(0, 0)});
        }
        if (cppTileData.count(MODULO_TILING_AREA) == 0)
        {
            UtilityFunctions::printerr("ma not found, using (1,1) (at CppArgentumTileMap.cpp::gSetTilesData())");
            cppTileData.insert({MODULO_TILING_AREA, Vector2i(1, 1)});
        }

        mCppTilesData.insert({gd_current_tile_key, cppTileData});                
    }
}
// Vector2i godot::CppArgentumTileMap::get_random_coord_with_tile_id(
//     const Vector2i &top_left_corner, const Vector2i &bottom_right_corner, const String &tile_id) const
// {
//     //TODO CHEQUEAR QUE LA BOTTOM RIGHT CORNER SEA MAYOR QUE LA TOP LEFT CORNER NUMÉRICAMENTE
//     const SafeVec sv_topLeftCorner(top_left_corner), sv_bottomRightCorner(bottom_right_corner);

//     if (sv_topLeftCorner.isAnyCompNegative() || sv_bottomRightCorner.isAnyCompNegative())
//     {
//         UtilityFunctions::printerr("CppArgentumTileMap::get_random_coord_with_tile_id: one given corner is negative");
//         return ERROR_VECTOR;
//     }
//     const auto searchedTileId = extractGdString(tile_id);

//     constexpr int MAX_TRIES = 100'000;

//     RandomNumberGenerator rng;

//     for(double triesCount = 0; triesCount < MAX_TRIES; triesCount++)
//     {
//         const SafeVec rCoords(
//             rng.randi_range(sv_topLeftCorner.lef, sv_bottomRightCorner.lef), 
//             rng.randi_range(sv_topLeftCorner.RIGHT, sv_bottomRightCorner.RIGHT));
//         try
//         {
//             const auto& TILES_AT_POS = m_worldMatrix.at(rCoords.lef).at(rCoords.RIGHT);
            
//             if(std::find(TILES_AT_POS.begin(), TILES_AT_POS.end(), searchedTileId) != TILES_AT_POS.end())
//             {
//                 return rCoords;
//             }
//         }
//         catch(const std::exception& e) {UtilityFunctions::printerr(e.what()); return ERROR_VECTOR;} 
//     }
//     UtilityFunctions::printerr("CppArgentumTileMap::get_random_coord_with_tile_id: couldn't get random coord");
//     return ERROR_VECTOR;
// }
bool CppArgentumTileMap::withinChunkBounds(
    const SafeVec& loadedCoordToCheck, const SafeVec& chunkTopLeftCorner, const SafeVec& CHUNK_SIZE)
{
    return loadedCoordToCheck.lef >= chunkTopLeftCorner.lef 
        && loadedCoordToCheck.RIGHT >= chunkTopLeftCorner.RIGHT 
        && loadedCoordToCheck.lef <= chunkTopLeftCorner.lef + CHUNK_SIZE.lef
        && loadedCoordToCheck.RIGHT <= chunkTopLeftCorner.RIGHT + CHUNK_SIZE.RIGHT;
}
// GUARDAR LAS POS CON ARRAYS MODIFICADOS
// GUARDAR EN ALGUN LUGAR LAS TILES SELECCIONADAS ALEATORIAMENTE
void CppArgentumTileMap::placeFormationTile( 
    const SafeVec& formationOrigin, const SafeVec& coordsRelativeToFormationOrigin, 
    const TileTypeUid newTile, const bool deletePreviousTiles){try
{
    const SafeVec absoluteCoords = formationOrigin + coordsRelativeToFormationOrigin;
    auto& otherTilesAtPos = (*mWorldMatrixPtr)[absoluteCoords];
    
    if (deletePreviousTiles){
        otherTilesAtPos[0] = newTile;
        for(std::uint_fast8_t i = 1; i < WorldMatrix::MAX_TILES_PER_POS; i++)
            otherTilesAtPos[i] = WorldMatrix::NULL_TILE_UID;
    }
    else
    for(std::uint_fast8_t i = 0; i < WorldMatrix::MAX_TILES_PER_POS; i++)
        if(otherTilesAtPos[i] == WorldMatrix::NULL_TILE_UID){
            otherTilesAtPos[i] = newTile;      
            break;
        }
}
catch(const std::exception& e){UtilityFunctions::printerr("CppArgentumTileMap.cpp::placeTile() exception: ", e.what());}}

void CppArgentumTileMap::freeze_and_store_being(const Vector2 loc_coords, const BeingUid individual_unique_id)
{
    //TODO ARREGLAR PARA MULTIPLAYER
    //get_multiplayer()
    const SafeVec tileMapCoords = local_to_map(loc_coords);
    mBeingsModule->mFrozenBeings[tileMapCoords].push_back({loc_coords, individual_unique_id});

    for (const auto& loadedTile: mBeingLoadedTiles[individual_unique_id])
    {
        decrementSharedCount(loadedTile);
    }
    mBeingLoadedTiles.erase(individual_unique_id);
}

bool CppArgentumTileMap::exceedsTileLimit(const TileTypeUid count){if (count >= WorldMatrix::NULL_TILE_UID - 1){UtilityFunctions::printerr("too many tiles (GlobalDataInterface.cpp::set_tiles())");return true;}return false;}

//se pueden llamar metodos de godot de guardar desde acá, aviso
//solo guardar los arrays modificados
bool CppArgentumTileMap::persist(String file_name)
{
    return false;
}

void CppArgentumTileMap::_bind_methods()
{   
    //TODO hacer el tileselector compartible de alguna forma así no hay q hacer las godot calls cada vez q se llame un formation generator?
    ClassDB::bind_method(D_METHOD("generate_formation", "formation_generator", "origin", "size", "tile_selection_set", "seed", "data"), &CppArgentumTileMap::generate_formation);
    ClassDB::bind_method(D_METHOD("generate_world_matrix", "size"), &CppArgentumTileMap::gGenerateWorldMatrix);
    
    ClassDB::bind_method(D_METHOD("load_tiles_around", "coords", "chunk_size", "uid"), &CppArgentumTileMap::gLoadTilesAround);

    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &CppArgentumTileMap::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &CppArgentumTileMap::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_tiles_data", "tiles_data"), &CppArgentumTileMap::gSetTilesData);
    ClassDB::bind_method(D_METHOD("get_tiles_data"), &CppArgentumTileMap::gGetTilesData);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "tiles_data"), "set_tiles_data", "get_tiles_data");
    
    ClassDB::bind_method(D_METHOD("freeze_and_store_being", "loc_coords", "individual_unique_id"), &CppArgentumTileMap::freeze_and_store_being);
    
    ADD_SIGNAL(MethodInfo("load_state_into_tilenaseblabldinstance_w_uid", PropertyInfo(Variant::INT, "tileinstance_uid")));

    ADD_SIGNAL(MethodInfo("formation_formed"));

    ADD_SIGNAL(MethodInfo("birth_being_kind", PropertyInfo(Variant::VECTOR2I, "local_coords"), PropertyInfo(Variant::STRING_NAME, "being_kind_id")));

    ADD_SIGNAL(MethodInfo("birth_being_kind_within_area", PropertyInfo(Variant::VECTOR2I, "top_left"), PropertyInfo(Variant::VECTOR2I, "bottom_right"), PropertyInfo(Variant::STRING_NAME, "being_kind_id")));

    ADD_SIGNAL(MethodInfo("birth_being_w_init_data", PropertyInfo(Variant::VECTOR2I, "local_coords"), PropertyInfo(Variant::DICTIONARY, "init_data")));

    ADD_SIGNAL(MethodInfo("tile_unloaded", PropertyInfo(Variant::VECTOR2I, "local_coords")));

    ADD_SIGNAL(MethodInfo("being_unfrozen", PropertyInfo(Variant::VECTOR2, "loc_coords"), PropertyInfo(Variant::INT, "being_uid")));
    ADD_SIGNAL(MethodInfo("birth_of_being_of_kind", PropertyInfo(Variant::STRING, "being_kind_id")));
}