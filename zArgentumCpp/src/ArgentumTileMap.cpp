#include "ArgentumTileMap.h"


#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <chrono>
#include <thread>


using namespace godot;

void ArgentumTileMap::generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i& origin, 
    const Vector2i& size, const Ref<Resource>& tileSelectionSet, signed int seed, const Dictionary& data)
{   
    if (size.x < 0 || size.y < 0)
    {
        UtilityFunctions::printerr("Negatively sized formation not allowed");
        return;
    }
    if (size.x < 600 || size.y < 600)
    {
        UtilityFunctions::printerr("World size is too small, must be at least be 600x600");
        return;
    }

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
    else
    {
        auto start = std::chrono::high_resolution_clock::now();
        formation_generator->generate(*this, origin, SafeVec(size), tileSelectionSet, seed, data);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        UtilityFunctions::print("time taken to generate formation: ", duration.count(),"ms");
        emit_signal("formation_formed");
    }
}

void ArgentumTileMap::load_tiles_around(const Vector2& global_coords, const Vector2i& CHUNK_SIZE, const int being_uid)//hacer q el chunk size cambie según el zoom ingame??
{    
    const SafeVec beingCoords = local_to_map(global_coords);
    
    for (int i = -CHUNK_SIZE.x/2; i <= CHUNK_SIZE.x/2; i++) 
    for (int j = -CHUNK_SIZE.y/2; j <= CHUNK_SIZE.y/2; j++) 
    {
        const SafeVec sTileMapCoords(beingCoords.lef + i, beingCoords.RIGHT + j);
        if (sTileMapCoords.isNonNegative() && sTileMapCoords.isStrictlySmallerThan(mWorldMatrixPtr->SIZE))
        {
            if (mTileSharedLoadsCount.count(sTileMapCoords) == 0)
            {
                mTileSharedLoadsCount.insert({sTileMapCoords, 1});

                if (mFrozenBeings.count(sTileMapCoords))
                {
                    auto& tileFrozenBeings = mFrozenBeings[sTileMapCoords];
                    auto it = tileFrozenBeings.begin();

                    while (it != tileFrozenBeings.end())
                    {
                        const Vector2& global_coords = it->first;
                        const int individual_unique_id = it->second;
                        emit_signal("being_unfrozen", global_coords, individual_unique_id);
                        it = tileFrozenBeings.erase(it);
                    }
                }
                if (mWorldMatrixPtr->isNotEmptyAt(sTileMapCoords))//if more than 0 tileIds at coords:
                    for (const uint16_t& uid : (*mWorldMatrixPtr)[sTileMapCoords])
                        {setCell(uid, sTileMapCoords);}
                //TODO
                //else
                  //  {setCell("ocean_water", sTileMapCoords);}
            }
            else if (mBeingLoadedTiles[being_uid].count(sTileMapCoords) == 0)
            {
               mTileSharedLoadsCount[sTileMapCoords] += 1;
            }
            mBeingLoadedTiles[being_uid].insert(sTileMapCoords);
        }
    }
    const SafeVec topLeftCornerCoords = beingCoords - SafeVec(CHUNK_SIZE.x/2, CHUNK_SIZE.y/2);

    unloadExcessTiles(topLeftCornerCoords, CHUNK_SIZE, being_uid);
}

void ArgentumTileMap::unloadExcessTiles(const SafeVec& topLeftCornerCoords, const SafeVec& CHUNK_SIZE, const int being_uid)
{
    auto loadedTilesIter = mBeingLoadedTiles[being_uid].begin();

    while (loadedTilesIter != mBeingLoadedTiles[being_uid].end())
    {
        const SafeVec& tileCoord = *loadedTilesIter;
        
        if (!withinChunkBounds(tileCoord, topLeftCornerCoords, CHUNK_SIZE))
        {
            decrementSharedCount(tileCoord);

            loadedTilesIter = mBeingLoadedTiles[being_uid].erase(loadedTilesIter);
        }
        else{++loadedTilesIter;}
    }
}
void ArgentumTileMap::decrementSharedCount(const SafeVec& tileCoord)
{
    mTileSharedLoadsCount[tileCoord] -= 1;
    if (mTileSharedLoadsCount[tileCoord] <= 0)
    {
        for (u_char layer_i = 0; layer_i < get_layers_count(); layer_i++)
            {erase_cell(layer_i, tileCoord);}

        emit_signal("tile_unloaded", (Vector2i)tileCoord);
        mTileSharedLoadsCount.erase(tileCoord);
    }
}

//solo se hace sobre tiles no cargadas
void ArgentumTileMap::doGlobalSpawnAttempts()
{
    constexpr unsigned char SPAWNING_MACROSCOPIC_CHUNK_SIZE = 6;
  
}

bool ArgentumTileMap::setCell(const uint16_t uid, const SafeVec &coords)
{
    if(uid == WorldMatrix::NULL_TILE_UID) return false;

    std::unordered_map<StringName, Variant> tileData;
    StringName TILE_ID = getTileId(uid);
    try{tileData = mCppTilesData.at(TILE_ID);}
    catch(const std::out_of_range& e)
    {UtilityFunctions::printerr(TILE_ID, " not found in cppTilesData (at ArgentumTileMap.cpp::load_tiles_around())");
     return false;
    }

    //TODO HACER EL AUTOTILING MANUALMENTE EN ESTA PARTE SEGÚN LAS 4 TILES Q SE TENGA ADYACENTES EN LA WORLDMATRIX
    //PONER EL AGUA EN UNA LAYER INFERIOR? 

    const SafeVec& atlasOriginPosition = (SafeVec)tileData["op"];; 

    const SafeVec& moduloTilingArea = (SafeVec)tileData["ma"];

    const SafeVec atlasPositionOffset(coords.lef % moduloTilingArea.lef, coords.RIGHT % moduloTilingArea.RIGHT);
    
    int alt_id = 0;
    bool flipped = false;
    try
    {//temp code, replace trees by scenes to make scale and color randomizeable
        alt_id = (int)tileData.at("alt_id");
        const bool flippedAtRandom = tileData.at("fr");
 
        flipped = flippedAtRandom && rand() % 2;
    }
    catch(const std::exception& e)
    {
        UtilityFunctions::printerr(e.what());
    }
    //aviso que existe get sorrounding cells, set_cells_terrain_connect(layer, get_used_cells_by_id(watar)) si usas terrain

    set_cell(tileData.at("layer"), coords, tileData.at("source_id"), atlasOriginPosition + atlasPositionOffset, alt_id + flipped);

    return true;
}

void ArgentumTileMap::generate_world_matrix(const Vector2i& size, const Dictionary& tiles_data)
{
    
    if (size.x <= 0 || size.y <= 0)
    {
        UtilityFunctions::printerr("Negatively sized world matrix not allowed");
        return;
    }
    if(mWorldMatrixPtr == nullptr)
    {
        mWorldMatrixPtr = std::make_unique<WorldMatrix>(size);
        set_tiles_data(tiles_data);
        mNaturalSpawningModule = std::make_unique<SpawningModule>(size);
    } else{
        UtilityFunctions::printerr("World matrix was already generated, cannot be re-generated.");
    }
}

int ArgentumTileMap::get_seed(){return seed;}; 
void ArgentumTileMap::set_seed(int seed){this->seed = seed;};

void ArgentumTileMap::add_tiles_data(const Dictionary& input_tiles)
{
    m_tiles_data.merge(input_tiles, true);
    const u_int16_t NEW_TILES_COUNT = m_tiles_data.size();

    if(exceedsTileLimit(NEW_TILES_COUNT)) return;

    mTilesUidMapping.reserve(NEW_TILES_COUNT);
    mTilesUidMapping.resize(NEW_TILES_COUNT);
    for(u_int16_t i = 0; i < input_tiles.size(); i++)
    {
        const auto& tile_id = input_tiles.keys()[i];

        //if not mapped already
        if(findTileUid(tile_id).has_value())//TA BIEN
        {//map the key
            mTilesUidMapping.push_back(tile_id);
        }
    }
}

void ArgentumTileMap::replaceTilesDataProperly(const Dictionary& input_tiles_data)
{
    if(m_tiles_data.is_empty())
    {
        const u_int16_t TILES_COUNT = input_tiles_data.size();

        if(exceedsTileLimit(TILES_COUNT)) return;

        m_tiles_data = input_tiles_data;

        mTilesUidMapping.reserve(TILES_COUNT);
        mTilesUidMapping.resize(TILES_COUNT);
        for(u_int16_t i = 0; i < TILES_COUNT; i++)
            {mTilesUidMapping[i] = input_tiles_data.keys()[i];}                    
    }
    else
    {
        m_tiles_data.clear();
        add_tiles_data(input_tiles_data);
    }
}
std::optional<u_int16_t> ArgentumTileMap::findTileUid(const StringName& stringId) const
{
    
    for(u_int16_t i = 0; i < mTilesUidMapping.size(); i++)
    {
        if(mTilesUidMapping[i] == stringId)
            return i;
    }
    return {};
}

StringName ArgentumTileMap::getTileId(uint16_t uid) const
{
    try{
        return mTilesUidMapping.at(uid);
    }catch(std::exception& e)
    {
        UtilityFunctions::printerr("no se encuentra id", uid);
        return "no se encuentra id";
    }
}

Dictionary ArgentumTileMap::get_tiles_data(){return m_tiles_data;}; 
void ArgentumTileMap::set_tiles_data(const Dictionary& input_tiles_data)
{
    for(auto &tileData : mCppTilesData){tileData.second.clear();}mCppTilesData.clear();
   
    replaceTilesDataProperly(input_tiles_data);
    
    for(int i = 0; i < m_tiles_data.values().size(); i++)
    {
        const StringName& gd_current_tile_key = (StringName)m_tiles_data.keys()[i];
        //const std::string keyAsCppString = gd_current_tile_key.utf8().get_data();
        
        const Ref<Resource>& tile = Object::cast_to<Resource>(m_tiles_data.values()[i]);

        const Dictionary& gd_tile_data = tile->call("get_data");

        if(gd_tile_data.is_empty() || gd_tile_data["id"] == "" || (int)gd_tile_data["source_id"] == -1)
        {
            UtilityFunctions::printerr("read gd_tile_data is not valid (ArgentumTileMap.cpp::set_tiles_data())");
            continue;
        }

        std::unordered_map<StringName, Variant> cppTileData;

        for(int j = 0; j < gd_tile_data.values().size(); j++)
        {
            const String& tile_field_key = gd_tile_data.keys()[j];
            if (tile_field_key == "op")
            {
                const Vector2i& atlas_origin_position = gd_tile_data.values()[j];
                if (((SafeVec)atlas_origin_position).isAnyCompNegative())//ESTO ESTÁ MAL, SE DEBERÍA CHEQUEAR EN EL MOMENTO CUANDO SE AGREGA TILEDATA, SINO CAUSA SLOWDOWN SI SE CHEQUEA CADA ITERACIÓN
                {
                    UtilityFunctions::printerr("Atlas origin position ",atlas_origin_position," is negative for tile_id \"",gd_current_tile_key,"\", using (0,0) (at ArgentumTileMap.cpp::set_tiles_data())");
                    cppTileData.insert({gd_tile_data.keys()[j], Vector2i(0, 0)});
                    continue;
                }
            }
            else if (tile_field_key == "ma")
            {
                const Vector2i& modulo_area = gd_tile_data.values()[j];
                if (((SafeVec)modulo_area).isStrictlyPositive() == false)//ESTO ESTÁ MAL, SE DEBERÍA CHEQUEAR EN EL MOMENTO CUANDO SE AGREGA TILEDATA, SINO CAUSA SLOWDOWN SI SE CHEQUEA CADA ITERACIÓN
                {
                    UtilityFunctions::printerr("non-positive MODULO_TILING_AREA ",modulo_area," is not admitted. tile_id:",gd_current_tile_key," (at ArgentumTileMap.cpp::set_tiles_data())");
                    cppTileData.insert({gd_tile_data.keys()[j], Vector2i(1, 1)});
                    continue;
                }
            }
            cppTileData.insert({gd_tile_data.keys()[j], gd_tile_data.values()[j]});
        }
        if (cppTileData.count("op") == 0)
        {
            UtilityFunctions::printerr("op not found, using (0,0) (at ArgentumTileMap.cpp::set_tiles_data())");
            cppTileData.insert({"op", Vector2i(0, 0)});
        }
        if (cppTileData.count("ma") == 0)
        {
            UtilityFunctions::printerr("ma not found, using (1,1) (at ArgentumTileMap.cpp::set_tiles_data())");
            cppTileData.insert({"ma", Vector2i(1, 1)});
        }

        mCppTilesData.insert({gd_current_tile_key, cppTileData});                
    }
}
// Vector2i godot::ArgentumTileMap::get_random_coord_with_tile_id(
//     const Vector2i &top_left_corner, const Vector2i &bottom_right_corner, const String &tile_id) const
// {
//     //TODO CHEQUEAR QUE LA BOTTOM RIGHT CORNER SEA MAYOR QUE LA TOP LEFT CORNER NUMÉRICAMENTE
//     const SafeVec sv_topLeftCorner(top_left_corner), sv_bottomRightCorner(bottom_right_corner);

//     if (sv_topLeftCorner.isAnyCompNegative() || sv_bottomRightCorner.isAnyCompNegative())
//     {
//         UtilityFunctions::printerr("ArgentumTileMap::get_random_coord_with_tile_id: one given corner is negative");
//         return ERROR_VECTOR;
//     }
//     const auto searchedTileId = extractGdString(tile_id);

//     constexpr int MAX_TRIES = 100'000;

//     RandomNumberGenerator rng;

//     for(float triesCount = 0; triesCount < MAX_TRIES; triesCount++)
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
//     UtilityFunctions::printerr("ArgentumTileMap::get_random_coord_with_tile_id: couldn't get random coord");
//     return ERROR_VECTOR;
// }

bool ArgentumTileMap::withinChunkBounds(
    const SafeVec &loadedCoordToCheck, const SafeVec &chunkTopLeftCorner, const SafeVec &CHUNK_SIZE)
{
    return loadedCoordToCheck.lef >= chunkTopLeftCorner.lef 
        && loadedCoordToCheck.RIGHT >= chunkTopLeftCorner.RIGHT 
        && loadedCoordToCheck.lef <= chunkTopLeftCorner.lef + CHUNK_SIZE.lef
        && loadedCoordToCheck.RIGHT <= chunkTopLeftCorner.RIGHT + CHUNK_SIZE.RIGHT;
}

//// GUARDAR LAS POS CON ARRAYS MODIFICADOS
//// GUARDAR EN ALGUN LUGAR LAS TILES SELECCIONADAS ALEATORIAMENTE
void ArgentumTileMap::placeFormationTile( 
    const SafeVec& formationOrigin, const SafeVec& coordsRelativeToFormationOrigin, 
    const uint16_t optTileUid, const bool deletePreviousTiles)//{try
{
    const SafeVec absoluteCoords = formationOrigin + coordsRelativeToFormationOrigin;
    auto& otherTilesAtPos = mWorldMatrixPtr->operator[](absoluteCoords);
    
    if (deletePreviousTiles){initialize_uids_array_as_empty<WorldMatrix::MAX_TILES_PER_POS>();}

    for(char i = 0; i < WorldMatrix::MAX_TILES_PER_POS; i++)
    {
        if(otherTilesAtPos[i] == WorldMatrix::NULL_TILE_UID){
            otherTilesAtPos[i] = optTileUid;      
            break;
        }
    }
}
//catch(const std::exception& e){UtilityFunctions::printerr("ArgentumTileMap.cpp::placeTile() exception: ", e.what());}}





void ArgentumTileMap::freeze_and_store_being(const Vector2& glb_coords, const int individual_unique_id)
{
    SafeVec localCoords = local_to_map(glb_coords);
    mFrozenBeings[localCoords].push_back({glb_coords, individual_unique_id});
    for (const auto& loadedTile: mBeingLoadedTiles[individual_unique_id])
    {
        decrementSharedCount(loadedTile);
    }
    mBeingLoadedTiles.erase(individual_unique_id);
}


ArgentumTileMap::ArgentumTileMap(){srand(time(NULL));}
ArgentumTileMap::~ArgentumTileMap(){}

//se pueden llamar metodos de godot de guardar desde acá
bool godot::ArgentumTileMap::persist(String file_name)
{
    return false;
}

void ArgentumTileMap::set_beings_in_chunk_count(const TypedArray<Array> beings_in_chunk_count){this->beings_in_chunk_count = beings_in_chunk_count;}
TypedArray<Array> ArgentumTileMap::get_beings_in_chunk_count(){return beings_in_chunk_count;}

void ArgentumTileMap::_bind_methods()
{   
    //TODO hacer el tileselector compartible de alguna forma así no hay q hacer las godot calls cada vez q se llame un formation generator?
    ClassDB::bind_method(D_METHOD("generate_formation", "formation_generator", "origin", "size", "tile_selection_set", "seed", "data"), &ArgentumTileMap::generate_formation);
    ClassDB::bind_method(D_METHOD("generate_world_matrix", "size"), &ArgentumTileMap::generate_world_matrix);
    
    ClassDB::bind_method(D_METHOD("load_tiles_around", "coords", "chunk_size", "uid"), &ArgentumTileMap::load_tiles_around);

    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &ArgentumTileMap::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &ArgentumTileMap::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ClassDB::bind_method(D_METHOD("set_tiles_data", "tiles_data"), &ArgentumTileMap::set_tiles_data);
    ClassDB::bind_method(D_METHOD("get_tiles_data"), &ArgentumTileMap::get_tiles_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "tiles_data"), "set_tiles_data", "get_tiles_data");

    ClassDB::bind_method(D_METHOD("set_beings_in_chunk_count", "beings_in_chunk_count"), &ArgentumTileMap::set_beings_in_chunk_count);
    ClassDB::bind_method(D_METHOD("get_beings_in_chunk_count"), &ArgentumTileMap::get_beings_in_chunk_count);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "beings_in_chunk_count"), "set_beings_in_chunk_count", "get_beings_in_chunk_count");
    
    ClassDB::bind_method(D_METHOD("freeze_and_store_being", "glb_coords", "individual_unique_id"), &ArgentumTileMap::freeze_and_store_being);
    
    ADD_SIGNAL(MethodInfo("formation_formed"));

    ADD_SIGNAL(MethodInfo("birth_being_kind", PropertyInfo(Variant::VECTOR2I, "local_coords"), PropertyInfo(Variant::STRING_NAME, "being_kind_id")));

    ADD_SIGNAL(MethodInfo("birth_being_kind_within_area", PropertyInfo(Variant::VECTOR2I, "top_left"), PropertyInfo(Variant::VECTOR2I, "bottom_right"), PropertyInfo(Variant::STRING_NAME, "being_kind_id")));

    ADD_SIGNAL(MethodInfo("birth_being_w_init_data", PropertyInfo(Variant::VECTOR2I, "local_coords"), PropertyInfo(Variant::DICTIONARY, "init_data")));

    ADD_SIGNAL(MethodInfo("tile_unloaded", PropertyInfo(Variant::VECTOR2I, "local_coords")));

    ADD_SIGNAL(MethodInfo("being_unfrozen", PropertyInfo(Variant::VECTOR2, "glb_coords"), PropertyInfo(Variant::INT, "being_uid")));
    ADD_SIGNAL(MethodInfo("birth_of_being_of_kind", PropertyInfo(Variant::STRING, "being_kind_id")));
}