#include "FormationGenerator.h"

#include <godot_cpp/variant/utility_functions.hpp>

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

//USAR OPTIONAL?
std::vector<StringName> FormationGenerator::getTiles(const TilePicker tilePicker, std::unordered_set<std::string> &data, unsigned int seed)
{
    std::vector<StringName> tilesToPlace;

    switch (tilePicker)
    {
        case TEMPERATE:
        {
            if (data.count("tree"))
            {
                char result[12] = "tree_temp_";
                char randChar = '0' + rand() % 8;
                result[10] = randChar;
                result[11] = '\0';

                tilesToPlace.push_back(result);
            } 

            if (data.count("continental") && !data.count("peninsuler_caved") && data.count("away_from_coast") && data.count("lake"))
            {
                tilesToPlace.push_back("lake");
            }
            else if (data.count("continental") && !data.count("peninsuler_caved") && data.count("beach"))
            {
                tilesToPlace.push_back("beach_sand");
            }
            else if (!data.count("continental") || data.count("peninsuler_caved"))
            {
                tilesToPlace.push_back("ocean");
            }
            else if (data.count("continental") && !data.count("peninsuler_caved"))
            {
                tilesToPlace.push_back("grass");
            }
            return tilesToPlace;
        }break;

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



