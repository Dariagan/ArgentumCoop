#include "FormationGenerator.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <string.h>

using namespace godot;

//esta spawnweightsmatrix debería ser 1/16 del size de la world matrix. para el chunk en una cierta pos, se elije alguno de estos puntos y se elige un punto al azar entre (px<->16, py<->16) de alguna tile q sea del mismo tipo
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

void FormationGenerator::placeTile(std::vector<std::vector<std::vector<std::array<char, 32>>>>& worldMatrix, 
    const SafeVec& origin, const MatrixCoords& coordsRelativeToFormationOrigin, 
    const std::array<char, 32>& tileId, const bool deleteBeingsAndTiles)
{try
{
    const SafeVec ABSOLUTE_COORDS = origin + coordsRelativeToFormationOrigin;
    auto& thingsAtPos = worldMatrix.at(ABSOLUTE_COORDS.lef).at(ABSOLUTE_COORDS.RIGHT);
    if (deleteBeingsAndTiles) 
        thingsAtPos.clear();
    thingsAtPos.push_back(tileId);
}catch(const std::exception& e){UtilityFunctions::printerr("FormationGenerator.cpp::placeTile() exception: ", e.what());}  
}

void FormationGenerator::placeBeing(
    const SafeVec &origin, std::vector<std::vector<std::vector<std::array<char, 32>>>> &worldMatrix,
    const MatrixCoords &coordsRelativeToFormationOrigin, const std::array<char, 32> &beingId)
{try
{
    const SafeVec ABSOLUTE_COORDS = origin + coordsRelativeToFormationOrigin;
    
    auto& thingsAtPos = worldMatrix.at(ABSOLUTE_COORDS.lef).at(ABSOLUTE_COORDS.RIGHT);
    
    for (char i = 0; i < thingsAtPos.size(); i++)
    {
        if (thingsAtPos.at(i).at(0) == '%')
        {
            thingsAtPos.erase(thingsAtPos.begin()+i);
            break;
        }
    }
    std::array<char, 32> transformedId;
    transformedId[0] = '%';
    strncpy(&transformedId[1], &beingId[0], sizeof(transformedId)-1);
    thingsAtPos.push_back(transformedId);

}catch(const std::exception& e){UtilityFunctions::printerr("FormationGenerator.cpp::placeBeing() exception: ", e.what());}  
}

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

void FormationGenerator::generate(std::vector<std::vector<std::vector<std::array<char, 32>>>> & worldMatrix, 
    const SafeVec& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionMapping, const unsigned int seed,
    const Dictionary& data)
{UtilityFunctions::printerr("Inside FormationGenerator abstract method");}



FormationGenerator::FormationGenerator(){

}
FormationGenerator::~FormationGenerator(){
   
}
void FormationGenerator::_bind_methods()
{


}