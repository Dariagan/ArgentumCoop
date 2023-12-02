#include "ArgentumTileMap.h"
#include "GdStringExtractor.cpp"

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
        UtilityFunctions::print("time taken to generate formation: ", duration.count(),"ms");
        emit_signal("formation_formed");
    }
}

void ArgentumTileMap::load_tiles_around(const Vector2& global_coords, const Vector2i& CHUNK_SIZE, const int being_uid)//hacer q el chunk size cambie según el zoom ingame??
{    
    const SafeVec beingCoords = local_to_map(global_coords);
    
    for (int i = -CHUNK_SIZE.x/2; i <= CHUNK_SIZE.x/2; i++) {
    for (int j = -CHUNK_SIZE.y/2; j <= CHUNK_SIZE.y/2; j++) 
    {
        const SafeVec sTileMapCoords(beingCoords.lef + i, beingCoords.RIGHT + j);
        if (sTileMapCoords.isNonNegative() && sTileMapCoords.lef < m_worldMatrix.size() && sTileMapCoords.RIGHT < m_worldMatrix[0].size())
        {
            if (m_tileSharedLoadsCount.count(sTileMapCoords) == 0)
            {
                m_tileSharedLoadsCount.insert({sTileMapCoords, 1});

                if (m_frozenBeings.count(sTileMapCoords))
                {
                    auto& tileFrozenBeings = m_frozenBeings[sTileMapCoords];
                    auto it = tileFrozenBeings.begin();

                    while (it != tileFrozenBeings.end())
                    {
                        const Vector2& global_coords = it->first;
                        const int individual_unique_id = it->second;
                        emit_signal("being_unfrozen", global_coords, individual_unique_id);
                        it = tileFrozenBeings.erase(it);
                    }
                }
                if (m_worldMatrix[sTileMapCoords.lef][sTileMapCoords.RIGHT].size() > 0)//if more than 0 tileIds at coords:
                    for (const std::array<char, 32>& id : m_worldMatrix[sTileMapCoords.lef][sTileMapCoords.RIGHT])
                        {setCell(&id[0], sTileMapCoords);}
                else
                    {setCell("ocean_water", sTileMapCoords);}
            }
            else if (m_beingLoadedTiles[being_uid].count(sTileMapCoords) == 0)
            {
               m_tileSharedLoadsCount[sTileMapCoords] += 1;
            }
            m_beingLoadedTiles[being_uid].insert(sTileMapCoords);
        }
    }}
    const SafeVec topLeftCornerCoords = beingCoords - SafeVec(CHUNK_SIZE.x/2, CHUNK_SIZE.y/2);

    unloadExcessTiles(topLeftCornerCoords, CHUNK_SIZE, being_uid);
}

void ArgentumTileMap::unloadExcessTiles(const SafeVec& topLeftCornerCoords, const SafeVec& CHUNK_SIZE, const int being_uid)
{
    auto loadedTilesIter = m_beingLoadedTiles[being_uid].begin();

    while (loadedTilesIter != m_beingLoadedTiles[being_uid].end())
    {
        const SafeVec& tileCoord = *loadedTilesIter;
        
        if (!withinChunkBounds(tileCoord, topLeftCornerCoords, CHUNK_SIZE))
        {
            decrementSharedCount(tileCoord);

            loadedTilesIter = m_beingLoadedTiles[being_uid].erase(loadedTilesIter);
        }
        else{++loadedTilesIter;}
    }
}
void ArgentumTileMap::decrementSharedCount(const SafeVec& tileCoord)
{
    m_tileSharedLoadsCount[tileCoord] -= 1;
    if (m_tileSharedLoadsCount[tileCoord] <= 0)
    {
        for (int layer_i = 0; layer_i < get_layers_count(); layer_i++)
            {erase_cell(layer_i, tileCoord);}

        //TODO freezear a aquellos beings q se encuentren dentro de esta tile borrada

        m_tileSharedLoadsCount.erase(tileCoord);
    }
}

bool ArgentumTileMap::setCell(const std::string &TILE_ID, const SafeVec &coords)
{
    std::unordered_map<StringName, Variant> tileData;
    try{tileData = m_cppTilesData.at(TILE_ID);}
    catch(const std::out_of_range& e)
    {UtilityFunctions::printerr(&TILE_ID[0], " not found in cppTilesData (at ArgentumTileMap.cpp::load_tiles_around())");
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
void ArgentumTileMap::set_tiles_data(Dictionary all_tiles_data)
{
    this->tiles_data = all_tiles_data;
    for(auto &tileData : m_cppTilesData)
        tileData.second.clear();
    m_cppTilesData.clear();
    
    for(int i = 0; i < all_tiles_data.values().size(); i++)
    {
        const String& gd_current_tile_key = (String)all_tiles_data.keys()[i];
        const std::string keyAsCppString = &extractGdString(gd_current_tile_key)[0];
        
        const Ref<Resource>& tile = Object::cast_to<Resource>(all_tiles_data.values()[i]);

        const Dictionary& gd_tile_data = tile->call("get_data");

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

        m_cppTilesData.insert({keyAsCppString, cppTileData});                
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
    const std::array<char, 32>& tileId, const bool deletePreviousTiles){try
{
    const SafeVec absoluteCoords = formationOrigin + coordsRelativeToFormationOrigin;
    auto& otherTilesAtPos = m_worldMatrix.at(absoluteCoords.lef).at(absoluteCoords.RIGHT);
    
    if (deletePreviousTiles){otherTilesAtPos.clear();}
    otherTilesAtPos.push_back(tileId);
}
catch(const std::exception& e){UtilityFunctions::printerr("ArgentumTileMap.cpp::placeTile() exception: ", e.what());}}

//con un builder se podrían repetir los settings tmb
//TODO hacer un builder de BeingData quien sea q chequee el build y printee errores. desp pasarle el objeto a este
//ojo estas coords son absolutas, no relativas al origin de la formation
bool godot::ArgentumTileMap::birthBeing(const Vector2i& coords, const BeingBuilder& beingBuilder)
{//shouldn't print anything, that's the builder's task
    if(beingBuilder.getResult().has_value())
    {
        emit_signal("birth_being_w_init_data", coords, beingBuilder.getResult().value());
        return true;
    }
    return false;
}

void ArgentumTileMap::birthBeingOfKind(const String& being_kind_id){emit_signal("birth_of_being_of_kind", being_kind_id);}
void ArgentumTileMap::freeze_and_store_being(const Vector2& glb_coords, const int individual_unique_id)
{
    SafeVec localCoords = local_to_map(glb_coords);
    m_frozenBeings[localCoords].push_back({glb_coords, individual_unique_id});
    for (const auto& loadedTile: m_beingLoadedTiles[individual_unique_id])
    {
        decrementSharedCount(loadedTile);
    }
    m_beingLoadedTiles.erase(individual_unique_id);
}


ArgentumTileMap::ArgentumTileMap(){srand(time(NULL));}
ArgentumTileMap::~ArgentumTileMap(){}

//se pueden llamar metodos de godot de guardar desde acá
bool godot::ArgentumTileMap::persist(String file_name)
{
    return false;
}

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
    
    ClassDB::bind_method(D_METHOD("freeze_and_store_being", "glb_coords", "individual_unique_id"), &ArgentumTileMap::freeze_and_store_being);

    ADD_SIGNAL(MethodInfo("formation_formed"));

    ADD_SIGNAL(MethodInfo("birth_being", PropertyInfo(Variant::VECTOR2I, "coords"), PropertyInfo(Variant::STRING_NAME, "pawnkind_id")));
    ADD_SIGNAL(MethodInfo("birth_being_w_init_data", PropertyInfo(Variant::VECTOR2I, "coords"), PropertyInfo(Variant::DICTIONARY, "init_data")));
    ADD_SIGNAL(MethodInfo("being_unfrozen", PropertyInfo(Variant::VECTOR2, "glb_coords"), PropertyInfo(Variant::INT, "being_uid")));
    ADD_SIGNAL(MethodInfo("birth_of_being_of_kind", PropertyInfo(Variant::STRING, "being_kind_id")));
}