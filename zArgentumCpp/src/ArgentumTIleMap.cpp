#include "ArgentumTileMap.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void ArgentumTilemap::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("generate_formation", "formation_generator", "origin", "area", "tile_picker", "seed"), &ArgentumTilemap::generate_formation);
    ClassDB::bind_method(D_METHOD("generate_world_matrix", "size"), &ArgentumTilemap::generate_world_matrix);
    ClassDB::bind_method(D_METHOD("load_tiles_around", "coords", "chunk_size"), &ArgentumTilemap::load_tiles_around);

    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &ArgentumTilemap::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &ArgentumTilemap::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_tiles_data", "tiles_data"), &ArgentumTilemap::set_tiles_data);
    ClassDB::bind_method(D_METHOD("get_tiles_data"), &ArgentumTilemap::get_tiles_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "tiles_data"), "set_tiles_data", "get_tiles_data");
}

void ArgentumTilemap::generate_world_matrix(Vector2i size)
{
    if(!worldGenerated){
        worldMatrix.resize(size.AXIS_X, std::vector<std::vector<StringName>>(size.AXIS_Y));
        this->worldSize = size;
        worldGenerated = true;
    } else{
        UtilityFunctions::printerr("World matrix was already generated, cannot be resized.");
    }
}

int64_t ArgentumTilemap::get_seed(){return seed;}; 
void ArgentumTilemap::set_seed(int64_t seed){this->seed = seed;};

Dictionary ArgentumTilemap::get_tiles_data(){return tiles_data;}; 
void ArgentumTilemap::set_tiles_data(Dictionary tiles_data){
    this->tiles_data = tiles_data;

    for(auto cppTileData : cppTilesData)
        cppTileData.second.clear();
    cppTilesData.clear();
    
    for(int i = 0; i < tiles_data.values().size(); i++){

        Ref<Resource> tile = Object::cast_to<Resource>(tiles_data.values()[i]);
        Dictionary tile_data = tile->call("get_data");

        std::unordered_map<StringName, Variant> cppTileData;

        for(int j = 0; j < tile_data.values().size(); j++){
            cppTileData.insert({tile_data.keys()[j], tile_data.values()[j]});
        }

        cppTilesData.insert({tiles_data.keys()[i], std::move(cppTileData)});
    }
    return;
};

void ArgentumTilemap::generate_formation(Ref<FormationGenerator> formation_generator, Vector2i origin, Vector2i area, TilePicker tile_picker, int64_t seed)
{
    if(seed < 0) seed *= -1;

    formation_generator->generate(worldMatrix, origin, area, tile_picker, seed);
}

void ArgentumTilemap::load_tiles_around(Vector2 coords, Vector2i chunk_size){

    Vector2i beingCoords = local_to_map(coords);

    for (int i = -chunk_size.AXIS_X/2; i < chunk_size.AXIS_X/2; i++) 
    {
        for (int j = -chunk_size.AXIS_Y/2; j < chunk_size.AXIS_Y/2; j++) 
        {
            Vector2i matrixCoords(worldSize.AXIS_X/2  + beingCoords.AXIS_X + i, worldSize.AXIS_Y/2 + beingCoords.AXIS_Y + j);
            if (matrixCoords.AXIS_X < (worldSize.AXIS_X - 1) && matrixCoords.AXIS_Y < (worldSize.AXIS_Y - 1) && matrixCoords.AXIS_X >= 0 && matrixCoords.AXIS_Y >= 0)
            {
                Vector2i tileMapTileCoords(beingCoords.AXIS_X + i, beingCoords.AXIS_Y + j);
                if (loadedTiles.count(tileMapTileCoords) == 0)
                {
                    if (worldMatrix[matrixCoords.AXIS_X][matrixCoords.AXIS_Y].size() > 0)
                    
                        for (StringName tile_id : worldMatrix[matrixCoords.AXIS_X][matrixCoords.AXIS_Y])
                        {					
                            std::unordered_map<StringName, Variant>& tileData = cppTilesData.at(tile_id);

                            set_cell(tileData.at("layer"), tileMapTileCoords, tileData.at("source_id"), tileData.at("atlas_pos"), tileData.at("alt_id"));
                        }
                    else
                        set_cell(0, tileMapTileCoords, 2, Vector2i(0,0), 0);

                    loadedTiles.insert(tileMapTileCoords);
                }
            }
        }
    }
    //unloadExcessTiles(beingCoords);

}

ArgentumTilemap::ArgentumTilemap()
{
     
}

ArgentumTilemap::~ArgentumTilemap()
{
    for(auto cppTileData : cppTilesData)
        cppTileData.second.clear();
    cppTilesData.clear();

    for(auto worldRow : worldMatrix){
        for(auto tile : worldRow)
            tile.clear();
        worldRow.clear();
    }worldMatrix.clear();
}

