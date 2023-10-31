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
    const Vector2i& origin, const Vector2i& tileCoordsRelativeToFormationOrigin, const std::string& tileId, bool deleteOthers)
{
    Vector2i absoluteCoords = origin + tileCoordsRelativeToFormationOrigin;
    
    auto& tilesAtPos = worldMatrix[absoluteCoords.x][absoluteCoords.y];

    if (deleteOthers) tilesAtPos.clear();
    
    tilesAtPos.push_back(tileId);
}

// el problema de esta función lineal es que te sesga las montañas según la continentness hacia el centro de la formación
// tal vez es mejor ajustar esta: (nota sin usar un array, y sumarle la mitad del size a la i y a la j) 
//https://github.com/SebLague/Procedural-Landmass-Generation/blob/master/Proc%20Gen%20E11/Assets/Scripts/FalloffGenerator.cs
float FormationGenerator::getBorderClosenessFactor(int i, int j, const Vector2i& size)
{
    return std::max(abs(i-size.x/2.f)/(size.x/2.f), abs(j-size.y/2.f)/(size.y/2.f));
}

void FormationGenerator::generate(std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& size, const TileSelectionSet tileSelectionSet, const signed int seed,
    const Dictionary& data)
{UtilityFunctions::printerr("Inside FormationGenerator abstract method");}

FormationGenerator::FormationGenerator(){

}
FormationGenerator::~FormationGenerator(){
   
}



