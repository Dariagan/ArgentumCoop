#include "ArgentumTileMap.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <regex>
#include <string>

using namespace godot;

void ArgentumTilemap::_bind_methods()
{   
    ClassDB::bind_method(D_METHOD("generate_formation", "formation_generator", "origin", "size", "tile_selection_set", "seed", "data"), &ArgentumTilemap::generate_formation);
    ClassDB::bind_method(D_METHOD("generate_world_matrix", "size"), &ArgentumTilemap::generate_world_matrix);
    
    ClassDB::bind_method(D_METHOD("load_tiles_around", "coords", "chunk_size"), &ArgentumTilemap::load_tiles_around);

    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &ArgentumTilemap::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &ArgentumTilemap::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_tiles_data", "tiles_data"), &ArgentumTilemap::set_tiles_data);
    ClassDB::bind_method(D_METHOD("get_tiles_data"), &ArgentumTilemap::get_tiles_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "tiles_data"), "set_tiles_data", "get_tiles_data");

    ADD_SIGNAL(MethodInfo("formation_formed"));
}

void ArgentumTilemap::generate_world_matrix(const Vector2i& size)
{
    if(!worldGenerated)
    {
        worldMatrix.resize(size.x, std::vector<std::vector<std::string>>(size.y, std::vector<std::string>()));

        this->worldSize = size;
        worldGenerated = true;
    } else{
        UtilityFunctions::printerr("World matrix was already generated, cannot be resized.");
    }
}

int ArgentumTilemap::get_seed(){return seed;}; 
void ArgentumTilemap::set_seed(signed int seed){this->seed = seed;};

Dictionary ArgentumTilemap::get_tiles_data(){return tiles_data;}; 
void ArgentumTilemap::set_tiles_data(Dictionary tiles_data)
{
    this->tiles_data = tiles_data;
    for(auto &tileData : cppTilesData)
        tileData.second.clear();
    cppTilesData.clear();
    
    for(int i = 0; i < tiles_data.values().size(); i++){

        Ref<Resource> tile = Object::cast_to<Resource>(tiles_data.values()[i]);
        Dictionary tile_data = tile->call("get_data");

        std::unordered_map<StringName, Variant> tileData;

        for(int j = 0; j < tile_data.values().size(); j++)
        {
            tileData.insert({tile_data.keys()[j], tile_data.values()[j]});
        }
        std::string keyAsCppString = ((String)tiles_data.keys()[i]).utf8().get_data();
        UtilityFunctions::print(keyAsCppString.c_str());
        cppTilesData.insert({keyAsCppString, tileData});                
    }
};

void ArgentumTilemap::generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i& origin, 
    const Vector2i& size, const TileSelectionSet tileSelectionSet, signed int seed, const Dictionary& data)
{
    formation_generator->generate(worldMatrix, origin, size, tileSelectionSet, seed, data);
    emit_signal("formation_formed");
}

void ArgentumTilemap::load_tiles_around(const Vector2& coords, const Vector2i& chunk_size)
{    
    Vector2i beingCoords = local_to_map(coords);
    
    for (int i = -chunk_size.x/2; i < chunk_size.x/2; i++) {
    for (int j = -chunk_size.y/2; j < chunk_size.y/2; j++) 
    {
        const Vector2i matrixPos(worldSize.x/2  + beingCoords.x + i, worldSize.y/2 + beingCoords.y + j);
        if (matrixPos.x < worldSize.x && matrixPos.y < worldSize.y && matrixPos.x >= 0 && matrixPos.y >= 0)
        {
            const Vector2i tileMapTileCoords(beingCoords.x + i, beingCoords.y + j);
            if (loadedTiles.count(tileMapTileCoords) == 0)
            {
                if (worldMatrix[matrixPos.x][matrixPos.y].size() > 0)
                {
                    for (const std::string& TILE_ID : worldMatrix[matrixPos.x][matrixPos.y])
                    {					
                        std::unordered_map<StringName, Variant> tileData;
                        try{tileData = cppTilesData.at(TILE_ID);}
                        catch(const std::out_of_range& e)
                        {UtilityFunctions::printerr(TILE_ID.c_str(), 
                        " not found in cppTilesData (ArgentumTileMap.cpp::load_tiles_around)");}

                        //TODO HACER EL AUTOTILING MANUALMENTE EN ESTA PARTE SEGÚN LAS 4 TILES Q SE TENGA ADYACENTES EN LA WORLDMATRIX
                        //PONER EL AGUA EN UNA LAYER INFERIOR? 

                        Vector2i atlasOriginPosition; 
                         
                        try{
                            atlasOriginPosition = ((Vector2i)tileData.at("op"));
                        }
                        catch(...){UtilityFunctions::printerr("couldn't get atlas origin position for tile_id \"",TILE_ID.c_str(),"\" (ArgentumTileMap.cpp::load_tiles_around)");
                        continue;}

                        Vector2i atlasPositionOffset(0, 0);

                        try{
                            Vector2i moduloTilePickingArea = (Vector2i)tileData.at("ma");
                            if(moduloTilePickingArea.x >= 1 && moduloTilePickingArea.y >= 1)
                            {
                                atlasPositionOffset.x = matrixPos.x % moduloTilePickingArea.x;
                                atlasPositionOffset.y = matrixPos.y % moduloTilePickingArea.y;
                            }
                            else UtilityFunctions::printerr("moduloTilePickingArea ",moduloTilePickingArea," not admitted for ",TILE_ID.c_str()," (ArgentumTileMap.cpp::load_tiles_around)");
                        }
                        catch(...){UtilityFunctions::printerr("couldn't access \"ma\" key for ", TILE_ID.c_str()," (ArgentumTileMap.cpp::load_tiles_around)");}
             
                        //hacer q la atlas positionV se mueva según el mod de la global position, dentro de la tile 4x4
                        set_cell(tileData.at("layer"), tileMapTileCoords, tileData.at("source_id"), 
                                atlasOriginPosition + atlasPositionOffset, tileData.at("alt_id"));
                    }
                }else
                {
                    //TODO (matrixpos.x)%4 && (matrixpos.y)%4->  
                    set_cell(0, tileMapTileCoords, 2, Vector2i(6,0), 0);
                }                   
                loadedTiles.insert(tileMapTileCoords);
            }
        }
    }}    
    unloadExcessTiles(beingCoords);
}

void ArgentumTilemap::unloadExcessTiles(const Vector2i& coords)
{
    const int MAX_LOADED_TILES = 30000;

    if (loadedTiles.size() > MAX_LOADED_TILES)
    {
        std::vector<Vector2i> tilesToErase;
        for (const Vector2i& tileCoord : loadedTiles)
        {
            if (((Vector2)tileCoord).distance_squared_to(coords) > 27000)
            {
                for (int layer_i = 0; layer_i < get_layers_count(); layer_i++)
                {
                    erase_cell(layer_i, tileCoord);
                }
                tilesToErase.push_back(tileCoord);
            }			
        }
        for (const Vector2i& tileCoord : tilesToErase){
            loadedTiles.erase(tileCoord);
        }
    }
}

ArgentumTilemap::ArgentumTilemap()
{
     
}

ArgentumTilemap::~ArgentumTilemap()
{
    
}

