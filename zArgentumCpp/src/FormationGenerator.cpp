#include "FormationGenerator.h"

using namespace godot;

//esta spawnweightsmatrix debería ser 1/16 del size de la world matrix. para el chunk en una cierta pos, se elije el punto más cercano y para spawnear se elige un punto al azar entre (px<->16, py<->16) de alguna tile q sea del mismo tipo
// void FormationGenerator::placeSpawnWeight(std::vector<std::vector<std::array<uint16_t>>>& spawnWeightsMatrix, 
//     const SafeVec origin, const SafeVec coordsRelativeToFormationOrigin, 
//     const uint16_t& pawnDefId, const bool deleteOtherWeights)
// {try
// {
//     const SafeVec ABSOLUTE_COORDS = origin + coordsRelativeToFormationOrigin;
//     auto& spawnWeightsAtPos = spawnWeightsMatrix.at(ABSOLUTE_COORDS.lef).at(ABSOLUTE_COORDS.RIGHT);
//     if (deleteOtherWeights) 
//         spawnWeightsAtPos.clear();
//     spawnWeightsAtPos.push_back(pawnDefId);
// }catch(const std::exception& e){UtilityFunctions::printerr("FormationGenerator.cpp::placeSpawnWeight() exception: ", e.what());}  
// }

float FormationGenerator::getBorderClosenessFactor(const SafeVec coords, const SafeVec SIZE, const float power)
{
    const float borderClosenessHorizontally = abs(coords.lef-SIZE.lef/2.f)/(SIZE.lef/2.f);
    const float borderClosenessVertically = abs(coords.RIGHT-SIZE.RIGHT/2.f)/(SIZE.RIGHT/2.f);
    
    return std::max(powf(borderClosenessHorizontally, power), powf(borderClosenessVertically, power));
}

void FormationGenerator::generate(ArgentumTileMap &argentumTileMap, const SafeVec origin, const SafeVec size, const Ref<Resource> &tileSelectionSet, const unsigned int seed, const Dictionary &data)
{
}

FormationGenerator::FormationGenerator()
{

}
FormationGenerator::~FormationGenerator(){
   
}
void FormationGenerator::_bind_methods()
{
    
}