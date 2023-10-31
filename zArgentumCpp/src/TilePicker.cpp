
#include "TilePicker.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <string>
#include <vector>
#include <stdexcept>

using namespace godot;

//std::vector<unordered_map<std::string,std::string>> tilePickers/ cada i del vector: un tilepicker. el map se prepara de antemano leyendo un godot resource
//la indexación numérica de los TileSelection's se prepara de antemano en esta clase. esta clase debe tener un método público q te diga la i del stringname del tileselector q le pasas
// (hacer q se ejecute una sola vez, no llamar al metodo en el for doble)

std::vector<std::string> TilePicker::getTiles(
    const TileSelectionSet tileSelectionSet, std::unordered_map<std::string, std::string> &data, unsigned int seed)
{
    std::vector<std::string> tilesToPlace;
    switch (tileSelectionSet)//cambiar el switch por acceder a tilePickers[tileSelectionSet]
    {
        case TEMPERATE:
        {
            if (data.count("continental"))//esta lógica debería existir acá, solo las ids pusheadas a TilesToPlace se tienen q sacar de un map<string, string> precargado
            {
                if (data.count("beach")) {tilesToPlace.push_back("beach_sand");}//esta id debería ser sacada un recurso godot
                else if (data.count("lake")) {tilesToPlace.push_back("lake");}
                else{
                    tilesToPlace.push_back("grass");
                    if (data.count("tree"))
                    {
                        //TODO USAR RESOURCELOADER PARA VER LA DATA DE LA TILE EN VEZ DE HACER ESTO
                        std::string result = "tree_temp_" + std::to_string(rand() % 8);
                        tilesToPlace.push_back(result);
                    }  
                } 
            }
            else {tilesToPlace.push_back("ocean");}

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