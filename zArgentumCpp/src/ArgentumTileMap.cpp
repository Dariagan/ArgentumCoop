#include "ArgentumTileMap.h"
#include "GdStringExtractor.cpp"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <regex>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <chrono>

using namespace godot;

void ArgentumTileMap::generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i& origin, 
    const Vector2i& size, const Ref<Resource>& tileSelectionSet, signed int seed, const Dictionary& data)
{
    if (size.x < 0 || size.y < 0)
    {
        UtilityFunctions::printerr("Negatively sized formation not allowed");
        return;
    }
    const bool outOfBoundsEast = origin.x + size.x > m_worldSize.lef;
    const bool outOfBoundsSouth = origin.y + size.y > m_worldSize.RIGHT;
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
        UtilityFunctions::print("time taken to generate: ", duration.count());
        emit_signal("formation_formed");
    }
}

void ArgentumTileMap::load_tiles_around(const Vector2& global_coords, const Vector2i& CHUNK_SIZE)//hacer q el chunk size cambie según el zoom ingame??
{    
    const SafeVec beingCoords = local_to_map(global_coords);
    
    for (int i = -CHUNK_SIZE.x/2; i <= CHUNK_SIZE.x/2; i++) {
    for (int j = -CHUNK_SIZE.y/2; j <= CHUNK_SIZE.y/2; j++) 
    {
        const SafeVec sTileMapCoords(beingCoords.lef + i, beingCoords.RIGHT + j);
        if (sTileMapCoords.isNonNegative() && sTileMapCoords.lef < m_worldSize.lef && sTileMapCoords.RIGHT < m_worldSize.RIGHT)
        {
            if (m_loadedTiles.count(sTileMapCoords) == 0) try
            {
                m_loadedTiles.insert(sTileMapCoords);

                if (m_frozenBeings.count(sTileMapCoords))
                {
                    auto& tileFrozenBeings = m_frozenBeings[sTileMapCoords];
                    auto it = tileFrozenBeings.begin();

                    while (it != tileFrozenBeings.end())
                    {
                        const Vector2& global_coords = it->first;
                        const String& individual_unique_id = it->second;
                        emit_signal("being_unfrozen", global_coords, individual_unique_id);
                        it = tileFrozenBeings.erase(it);
                    }
                }
                if (m_worldMatrix.at(sTileMapCoords.lef).at(sTileMapCoords.RIGHT).size() > 0)
                    for (const std::array<char, 32>& id : m_worldMatrix[sTileMapCoords.lef][sTileMapCoords.RIGHT])
                    {
                        setCell(&id[0], sTileMapCoords);
                    }
                else
                {
                    setCell("ocean_water", sTileMapCoords);
                }
                
            }
            catch(const std::exception& e){UtilityFunctions::printerr("ArgentumTileMap::load_tiles_around() exception:", e.what());}
        }
    }}
    const SafeVec topLeftCornerCoords = beingCoords - SafeVec(CHUNK_SIZE.x/2, CHUNK_SIZE.y/2);

    unloadExcessTiles(topLeftCornerCoords, CHUNK_SIZE);
}

//todo hacer en vez de por distancia q se borren las tiles de loadedtiles cuyas coords no esten dentro del cuadrado actual
void ArgentumTileMap::unloadExcessTiles(const SafeVec& topLeftCornerCoords, const SafeVec& CHUNK_SIZE)//coords PUEDE SER NEGATIVO, ARREGLAR
{
    auto it = m_loadedTiles.begin();

    while (it != m_loadedTiles.end())
    {
        const SafeVec& tileCoord = *it;
         //este if en realidad tiene que chequear q pase esto para cada being existente (AND) antes de decidir borrar
        if (!withinChunkBounds(tileCoord, topLeftCornerCoords, CHUNK_SIZE))//el problema es q esto borra las tiles de otros being en el set
        {//hay q mantener un vector q guarda las positions de todos los beings y fijarse si la tile le pertenece antes de intentar borrar
            for (int layer_i = 0; layer_i < get_layers_count(); layer_i++)
            {
                erase_cell(layer_i, tileCoord);
            }
            it = m_loadedTiles.erase(it);
        }
        else{++it;}
    }
}

bool godot::ArgentumTileMap::setCell(const std::string &TILE_ID, const SafeVec &coords)
{
    std::unordered_map<StringName, Variant> tileData;
    try{tileData = m_cppTilesData.at(TILE_ID);}
    catch(const std::out_of_range& e)
    {UtilityFunctions::printerr(&TILE_ID[0], " not found in cppTilesData (at ArgentumTileMap.cpp::load_tiles_around())");}

    //TODO HACER EL AUTOTILING MANUALMENTE EN ESTA PARTE SEGÚN LAS 4 TILES Q SE TENGA ADYACENTES EN LA WORLDMATRIX
    //PONER EL AGUA EN UNA LAYER INFERIOR? 

    SafeVec atlasOriginPosition; 
        
    try{
        atlasOriginPosition = (Vector2i)tileData.at("op");
        if (atlasOriginPosition.isAnyCompNegative())
        {
            UtilityFunctions::printerr("Atlas origin position is negative for tile_id \"",TILE_ID.c_str(),"\" (at ArgentumTileMap.cpp::load_tiles_around())");
            return false;
        }
    }
    catch(...){UtilityFunctions::printerr("couldn't get atlas origin position for tile_id \"",TILE_ID.c_str(),"\" (at ArgentumTileMap.cpp::load_tiles_around())");
    return false;}

    SafeVec atlasPositionOffset(0, 0);

    try{
        const SafeVec moduloTilingArea = (SafeVec)tileData.at("ma");
        if(moduloTilingArea.isStrictlyPositive())
        {
            atlasPositionOffset.lef = coords.lef % moduloTilingArea.lef;
            atlasPositionOffset.RIGHT = coords.RIGHT % moduloTilingArea.RIGHT;
        }
        else UtilityFunctions::printerr("non-positive MODULO_TILING_AREA ",moduloTilingArea.c_str(),"is not admitted. tile_id:",TILE_ID.c_str()," (at ArgentumTileMap.cpp::load_tiles_around())");
    }
    catch(...){UtilityFunctions::printerr("couldn't access \"ma\" key for ", TILE_ID.c_str()," (at ArgentumTileMap.cpp::load_tiles_around())");}

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

    //hacer q la atlas positionV se mueva según el mod de la global position, dentro de la tile 4x4
    set_cell(tileData.at("layer"), coords, tileData.at("source_id"), atlasOriginPosition + atlasPositionOffset, alt_id + flipped);

    return true;
}

void ArgentumTileMap::generate_world_matrix(const Vector2i& size)
{
    if (size.x < 0 || size.y < 0)
    {
        UtilityFunctions::printerr("Negatively sized world matrix not allowed");
        return;
    }
    if(m_worldMatrix.size() == 0)
    {
        m_worldMatrix.resize(size.x, std::vector<std::vector<std::array<char, 32>>>(size.y, std::vector<std::array<char, 32>>()));
        m_spawnWeightsMatrix.resize(size.x/10, std::vector<std::vector<std::array<char, 32>>>(size.y/10, std::vector<std::array<char, 32>>()));
        this->m_worldSize = size;
    } else{
        UtilityFunctions::printerr("World matrix was already generated, cannot be re-generated.");
    }
}

int ArgentumTileMap::get_seed(){return seed;}; 
void ArgentumTileMap::set_seed(int seed){this->seed = seed;};

Dictionary ArgentumTileMap::get_tiles_data(){return tiles_data;}; 
void ArgentumTileMap::set_tiles_data(Dictionary tiles_data)
{
    this->tiles_data = tiles_data;
    for(auto &tileData : m_cppTilesData)
        tileData.second.clear();
    m_cppTilesData.clear();
    
    for(int i = 0; i < tiles_data.values().size(); i++)
    {
        const std::string keyAsCppString = &extractGdString((String)tiles_data.keys()[i])[0];
        
        const Ref<Resource>& tile = Object::cast_to<Resource>(tiles_data.values()[i]);

        const Dictionary& TILE_DATA = tile->call("get_data");

        std::unordered_map<StringName, Variant> tileData;

        for(int j = 0; j < TILE_DATA.values().size(); j++)
        {
            tileData.insert({TILE_DATA.keys()[j], TILE_DATA.values()[j]});
        }
        m_cppTilesData.insert({keyAsCppString, tileData});                
    }
}
Vector2i godot::ArgentumTileMap::get_random_coord_with_tile_id(
    const Vector2i &top_left_corner, const Vector2i &bottom_right_corner, const String &tile_id) const
{
    //TODO CHEQUEAR QUE LA BOTTOM RIGHT CORNER SEA MAYOR QUE LA TOP LEFT CORNER NUMÉRICAMENTE
    const SafeVec sv_topLeftCorner(top_left_corner), sv_bottomRightCorner(bottom_right_corner);

    if (sv_topLeftCorner.isAnyCompNegative() || sv_bottomRightCorner.isAnyCompNegative())
    {
        UtilityFunctions::printerr("ArgentumTileMap::get_random_coord_with_tile_id: one given corner is negative");
        return ERROR_VECTOR;
    }
    const auto searchedTileId = extractGdString(tile_id);

    constexpr int MAX_TRIES = 100'000;

    RandomNumberGenerator rng;

    for(float triesCount = 0; triesCount < MAX_TRIES; triesCount++)
    {
        const SafeVec rCoords(
            rng.randi_range(sv_topLeftCorner.lef, sv_bottomRightCorner.lef), 
            rng.randi_range(sv_topLeftCorner.RIGHT, sv_bottomRightCorner.RIGHT));
        try
        {
            const auto& TILES_AT_POS = m_worldMatrix.at(rCoords.lef).at(rCoords.RIGHT);
            
            if(std::find(TILES_AT_POS.begin(), TILES_AT_POS.end(), searchedTileId) != TILES_AT_POS.end())
            {
                return rCoords;
            }
        }
        catch(const std::exception& e) {UtilityFunctions::printerr(e.what()); return ERROR_VECTOR;} 
    }
    UtilityFunctions::printerr("ArgentumTileMap::get_random_coord_with_tile_id: couldn't get random coord");
    return ERROR_VECTOR;
}

bool ArgentumTileMap::withinChunkBounds(
    const SafeVec &loadedCoordToCheck, const SafeVec &chunkTopLeftCorner, const SafeVec &CHUNK_SIZE)
{
    return loadedCoordToCheck.lef >= chunkTopLeftCorner.lef 
        && loadedCoordToCheck.RIGHT >= chunkTopLeftCorner.RIGHT 
        && loadedCoordToCheck.lef <= chunkTopLeftCorner.lef + CHUNK_SIZE.lef
        && loadedCoordToCheck.RIGHT <= chunkTopLeftCorner.RIGHT + CHUNK_SIZE.RIGHT;
}

void ArgentumTileMap::placeFormationTile( 
    const SafeVec& formationOrigin, const SafeVec& coordsRelativeToFormationOrigin, 
    const std::array<char, 32>& tileId, const bool deleteBeingsAndTiles){try
{
    const SafeVec absoluteCoords = formationOrigin + coordsRelativeToFormationOrigin;
    auto& thingsAtPos = m_worldMatrix.at(absoluteCoords.lef).at(absoluteCoords.RIGHT);
    
    if (deleteBeingsAndTiles){thingsAtPos.clear();}
    thingsAtPos.push_back(tileId);
}
catch(const std::exception& e){UtilityFunctions::printerr("ArgentumTileMap.cpp::placeTile() exception: ", e.what());}}

void ArgentumTileMap::initializePawnKindinGdScript(const Ref<Dictionary>& data){emit_signal("initialize_pawnkind", data);}
void ArgentumTileMap::store_frozen_being(const Vector2& glb_coords, const String& individual_unique_id)
    {m_frozenBeings[local_to_map(glb_coords)].push_back({glb_coords, individual_unique_id});}


ArgentumTileMap::ArgentumTileMap(){srand(time(NULL));}
ArgentumTileMap::~ArgentumTileMap(){}

void ArgentumTileMap::_bind_methods()
{   
    //TODO hacer el tileselector compartible de alguna forma así no hay q hacer las godot calls cada vez q se llame un formation generator?
    ClassDB::bind_method(D_METHOD("generate_formation", "formation_generator", "origin", "size", "tile_selection_set", "seed", "data"), &ArgentumTileMap::generate_formation);
    ClassDB::bind_method(D_METHOD("generate_world_matrix", "size"), &ArgentumTileMap::generate_world_matrix);
    
    ClassDB::bind_method(D_METHOD("load_tiles_around", "coords", "chunk_size"), &ArgentumTileMap::load_tiles_around);

    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &ArgentumTileMap::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &ArgentumTileMap::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_tiles_data", "tiles_data"), &ArgentumTileMap::set_tiles_data);
    ClassDB::bind_method(D_METHOD("get_tiles_data"), &ArgentumTileMap::get_tiles_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "tiles_data"), "set_tiles_data", "get_tiles_data");
    
    ClassDB::bind_method(D_METHOD("store_frozen_being", "glb_coords", "individual_unique_id"), &ArgentumTileMap::store_frozen_being);

    ADD_SIGNAL(MethodInfo("formation_formed"));
    ADD_SIGNAL(MethodInfo("being_unfrozen", PropertyInfo(Variant::VECTOR2, "glb_coords"), PropertyInfo(Variant::STRING, "uid")));
    ADD_SIGNAL(MethodInfo("initialize_pawnkind", PropertyInfo(Variant::DICTIONARY, "data")));
}