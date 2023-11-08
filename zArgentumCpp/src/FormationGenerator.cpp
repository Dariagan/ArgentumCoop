#include "FormationGenerator.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource.hpp>

using namespace godot;

void FormationGenerator::_bind_methods()
{


    //TODO BINDEAR generate_pre_check
}


void FormationGenerator::placeTile(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
    const MatrixCoords& origin, const MatrixCoords& tileCoordsRelativeToFormationOrigin, const std::string& tileId, bool deleteOthers)
{
    const MatrixCoords ABSOLUTE_COORDS = origin + tileCoordsRelativeToFormationOrigin;
    
    auto& tilesAtPos = worldMatrix[ABSOLUTE_COORDS.i][ABSOLUTE_COORDS.j];

    if (deleteOthers) tilesAtPos.clear();
    
    tilesAtPos.push_back(tileId);
}



// el problema de esta función lineal es que te sesga las montañas según la continentness hacia el centro de la formación
// tal vez es mejor ajustar esta: (nota sin usar un array, y sumarle la mitad del size a la i y a la j) 
//https://github.com/SebLague/Procedural-Landmass-Generation/blob/master/Proc%20Gen%20E11/Assets/Scripts/FalloffGenerator.cs
float FormationGenerator::getBorderClosenessFactor(int i, int j, const MatrixCoords& size)
{
    return std::max(abs(i-size.i/2.f)/(size.i/2.f), abs(j-size.j/2.f)/(size.j/2.f));
}

void FormationGenerator::generate(std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    const MatrixCoords& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionMapping, const signed int seed,
    const Dictionary& data)
{UtilityFunctions::printerr("Inside FormationGenerator abstract method");}

FormationGenerator::FormationGenerator(){

}
FormationGenerator::~FormationGenerator(){
   
}



