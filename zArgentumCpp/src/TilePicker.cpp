
#include "TilePicker.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <string>
#include <vector>
#include <stdexcept>

using namespace godot;

// "afc" significa: away from coast

std::vector<std::string> TilePicker::getTiles(
    const TileSelectionSet tileSelectionSet, std::unordered_map<std::string, std::string> &data, unsigned int seed)
{
    std::vector<std::string> tilesToPlace;
    switch (tileSelectionSet)
    {
        case TEMPERATE:
        {
            if (data.count("tree"))
            {
                //TODO USAR RESOURCELOADER PARA VER LA DATA DE LA TILE EN VEZ DE HACER ESTO
                std::string result = "tree_temp_" + std::to_string(rand() % 8);
                tilesToPlace.push_back(result);
            } 
            if (data.count("continental") && !data.count("peni_caved") && data.count("afc") && data.count("lake"))
            {
                tilesToPlace.push_back("lake");
            }
            else if (data.count("continental") && !data.count("peni_caved") && data.count("beach"))
            {
                tilesToPlace.push_back("beach_sand");
            }
            else if (!data.count("continental") || data.count("peni_caved"))
            {
                tilesToPlace.push_back("ocean");
            }
            else if (data.count("continental") && !data.count("peni_caved"))
            {
                tilesToPlace.push_back("grass");
            }
            return tilesToPlace;
        }break;

        case DESERT:
        {
            if (data.count("continental"))
                tilesToPlace.push_back("beach_sand");
            return tilesToPlace;
        }break;

        default:
            godot::UtilityFunctions::printerr("passed TileSelectionSet not implemented");
            throw std::logic_error("passed TileSelectionSet not implemented");
        break;
    }     
    

}

TilePicker::TilePicker()
{
    //TODO USAR RESOURCELOADER
}

TilePicker::~TilePicker(){
    
}