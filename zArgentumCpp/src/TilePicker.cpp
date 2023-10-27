#ifndef __TILE_PICKER__
#define __TILE_PICKER__

#include <vector>
#include <string>
#include <unordered_set>
#include <FormationGenerator.h>
#include <godot_cpp/variant/utility_functions.hpp>

class TilePicker{


    public:

        TilePicker()
        {
            //TODO usar resourcelouder pa sacar la data de todas las tiles
        }
        ~TilePicker()
        {

        }
        
        std::vector<std::string> getTiles(const godot::TileSetCase tileSetCase, std::unordered_set<std::string> &data, unsigned int seed)
        {
            std::vector<std::string> tilesToPlace;
            switch (tileSetCase)
            {
                case godot::TileSetCase::TEMPERATE:
                {
                    if (data.count("tree"))
                    {
                        //TODO USAR RESOURCELOADER PARA VER LA DATA DE LA TILE EN VEZ DE HACER ESTO
                        std::string result = "tree_temp_" + std::to_string(rand() % 8);
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
                        
                        std::string result = "grass_" + std::to_string(rand() % 4);
                        tilesToPlace.push_back(result);
                    }
                    return tilesToPlace;
                }break;

                case godot::TileSetCase::DESERT:{
                    if (data.count("continental"))
                        tilesToPlace.push_back("beach_sand");
                    return tilesToPlace;
                }break;

                default:
                    godot::UtilityFunctions::printerr("passed tile picker not implemented");
                    throw std::logic_error("passed tile picker not implemented");
                break;
            }     
            
        }
};

#endif // __TILE_PICKER__


