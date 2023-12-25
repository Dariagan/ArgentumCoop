#include "SpawnWeightsMatrix.h"
using namespace godot;

std::unordered_map<SWM_MAP_TYPES>& SpawnWeightsMatrix::operator[](const SafeVec worldMatrixCoords)
{
    const SafeVec downScaledCoords = worldMatrixCoords/SpawnWeightsMatrix::DOWNSCALING_FACTOR;
    return SpawnWeightsMatrix::flattenedSpawnWeightsMatrix[SIZE.lef*downScaledCoords.lef + downScaledCoords.RIGHT];
}

std::unordered_map<SWM_MAP_TYPES>& SpawnWeightsMatrix::at(const SafeVec worldMatrixCoords)
{
    const SafeVec downScaledCoords = worldMatrixCoords/SpawnWeightsMatrix::DOWNSCALING_FACTOR;
    return SpawnWeightsMatrix::flattenedSpawnWeightsMatrix.at(SIZE.lef*downScaledCoords.lef + downScaledCoords.RIGHT);
}

void SpawnWeightsMatrix::clearAt(const SafeVec worldMatrixCoords){operator[](worldMatrixCoords).clear();}

short unsigned int SpawnWeightsMatrix::countAt(const SafeVec coords)
{
    const auto& spawnWeightsAtPos = this->operator[](coords); 
    return spawnWeightsAtPos.size();
}
bool SpawnWeightsMatrix::isEmptyAt(const SafeVec coords){return countAt(coords) == 0;}
bool SpawnWeightsMatrix::hasSpawnsAt(const SafeVec coords){return countAt(coords) != 0;}

SpawnWeightsMatrix::SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE) : SIZE(WORLD_MATRIX_SIZE/SpawnWeightsMatrix::DOWNSCALING_FACTOR)
{resize();} 
    
void SpawnWeightsMatrix::resize()
{
    flattenedSpawnWeightsMatrix.reserve(SIZE.area());
    flattenedSpawnWeightsMatrix.resize(SIZE.area());
}
