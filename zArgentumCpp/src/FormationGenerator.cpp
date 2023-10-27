#include "FormationGenerator.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void FormationGenerator::_bind_methods()
{
    BIND_ENUM_CONSTANT(TEMPERATE);
    BIND_ENUM_CONSTANT(DESERT);

    //TODO BINDEAR generate_pre_check
}

void FormationGenerator::placeTile(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
    const Vector2i& origin, const Vector2i& relativeCoords, const std::string& tileId, bool deleteOthers)
{
    Vector2i absoluteCoords = Vector2i(worldMatrix.size()/2, worldMatrix[0].size()/2) + origin + relativeCoords;
    
    auto tilesAtPos = worldMatrix[absoluteCoords.x][absoluteCoords.y];

    if (deleteOthers) tilesAtPos.clear();

    tilesAtPos.push_back(tileId);
}

void FormationGenerator::generate_pre_check(std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    Vector2i origin, const Vector2i& size, const TileSetCase tilePicker, const signed int seed,
    const Dictionary& data
    )
{
    if (origin.x >= 0) origin.x -= (origin.x + 1) % 4;
    else origin.x += abs(origin.x) % 4;

    if (origin.y >= 0) origin.y -= (origin.y + 1) % 4;
    else origin.y += abs(origin.y) % 4;

    generate(worldMatrix, origin, size, tilePicker, seed, data);
}

void FormationGenerator::generate(std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& size, const TileSetCase tilePicker, const signed int seed,
    const Dictionary& data
    )
{
    UtilityFunctions::printerr("Inside FormationGenerator abstract method");
}

FormationGenerator::FormationGenerator(){
    this->tilePicker = std::unique_ptr<TilePicker>(new TilePicker());
}
FormationGenerator::~FormationGenerator(){
   
}



