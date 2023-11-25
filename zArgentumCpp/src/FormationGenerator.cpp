#include "FormationGenerator.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <string.h>



using namespace godot;

//esta spawnweightsmatrix debería ser 1/16 del size de la world matrix. para el chunk en una cierta pos, se elije el punto más cercano y para spawnear se elige un punto al azar entre (px<->16, py<->16) de alguna tile q sea del mismo tipo
// void FormationGenerator::placeSpawnWeight(std::vector<std::vector<std::vector<std::array<char, 32>>>>& spawnWeightsMatrix, 
//     const SafeVec& origin, const MatrixCoords& coordsRelativeToFormationOrigin, 
//     const std::array<char, 32>& pawnDefId, const bool deleteOtherWeights)
// {try
// {
//     const SafeVec ABSOLUTE_COORDS = origin + coordsRelativeToFormationOrigin;
//     auto& spawnWeightsAtPos = spawnWeightsMatrix.at(ABSOLUTE_COORDS.lef).at(ABSOLUTE_COORDS.RIGHT);
//     if (deleteOtherWeights) 
//         spawnWeightsAtPos.clear();
//     spawnWeightsAtPos.push_back(pawnDefId);
// }catch(const std::exception& e){UtilityFunctions::printerr("FormationGenerator.cpp::placeSpawnWeight() exception: ", e.what());}  
// }


// el problema de esta función lineal es que te sesga las montañas según la continentness hacia el centro de la formación
// tal vez es mejor ajustar esta: (nota sin usar un array, y sumarle la mitad del size a la lef y a la RIGHT) 
//https://github.com/SebLague/Procedural-Landmass-Generation/blob/master/Proc%20Gen%20E11/Assets/Scripts/FalloffGenerator.cs
float FormationGenerator::getBorderClosenessFactor(MatrixCoords coords, const MatrixCoords& SIZE)
{
    const float I_BORDER_CLOSENESS = abs(coords.lef-SIZE.lef/2.f)/(SIZE.lef/2.f);
    const float J_BORDER_CLOSENESS = abs(coords.RIGHT-SIZE.RIGHT/2.f)/(SIZE.RIGHT/2.f);

    constexpr float POW = 3.3f;
   
    return std::max(powf(I_BORDER_CLOSENESS, POW), powf(J_BORDER_CLOSENESS, POW));
}

void godot::FormationGenerator::generate(ArgentumTileMap &argentumTileMap, const SafeVec &origin, const MatrixCoords &size, const Ref<Resource> &tileSelectionSet, const unsigned int seed, const Dictionary &data)
{
}

FormationGenerator::FormationGenerator(){

}
FormationGenerator::~FormationGenerator(){
   
}
void FormationGenerator::_bind_methods()
{


}