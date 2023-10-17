#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <memory>

using namespace godot;

void FormationGenerator::_bind_methods()
{
    BIND_ENUM_CONSTANT(TEMPERATE);
}

void FormationGenerator::placeTile(std::vector<std::vector<std::vector<StringName>>>& worldMatrix, 
    const Vector2i& origin, const Vector2i& relativeCoords, const StringName& tileId)
{
    Vector2i absoluteCoords = Vector2i(worldMatrix.size()/2, worldMatrix[0].size()/2) + origin + relativeCoords;
    
    worldMatrix[absoluteCoords.x][absoluteCoords.y].push_back(tileId);
}

std::vector<StringName> FormationGenerator::getTiles(const TilePicker tilePicker, std::unordered_map<std::string, bool> &data)
{
    std::vector<StringName> tilesToPlace;

    switch (tilePicker)
    {
        case TEMPERATE:    
            if (data.at("continental") && !data.at("peninsuler_caved") && data.at("away_from_coast") && data.at("lake"))
            {
                tilesToPlace.push_back("lake");
            }
            else if (data.at("continental") && !data.at("peninsuler_caved") && data["beach"])
            {
                tilesToPlace.push_back("beach_sand");
            }
            else if (!data.at("continental") ||  data.at("peninsuler_caved"))
            {
                tilesToPlace.push_back("ocean");
            }
            else if (data.at("continental") && !data["peninsuler_caved"])
            {
                tilesToPlace.push_back("grass");
            }
            return tilesToPlace;
        break;

        default:
            UtilityFunctions::printerr("passed tile picker not implemented");
            throw std::logic_error("passed tile picker not implemented");
        break;
    }
}
void FormationGenerator::generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& size, const TilePicker tilePicker, const signed int seed,
    const Dictionary& data
    )
{
    UtilityFunctions::printerr("Inside FormationGenerator abstract method");
}

FormationGenerator::FormationGenerator(){}
FormationGenerator::~FormationGenerator(){}



