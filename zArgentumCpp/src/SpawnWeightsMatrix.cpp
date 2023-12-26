#include "SpawnWeightsMatrix.h"
using namespace godot;

std::unordered_map<SWM_MAP_TYPES>& SpawnWeightsMatrix::operator[](const SafeVec worldMatrixCoords) const
{
    return SpawnWeightsMatrix::mWeightsMatrix->operator[](worldMatrixCoords);
}

std::unordered_map<SWM_MAP_TYPES>& SpawnWeightsMatrix::at(const SafeVec worldMatrixCoords) const
{ 
    return SpawnWeightsMatrix::mWeightsMatrix->at(worldMatrixCoords);
}

void SpawnWeightsMatrix::clearAt(const SafeVec worldMatrixCoords) const {operator[](worldMatrixCoords).clear();}

short unsigned int SpawnWeightsMatrix::countAt(const SafeVec coords) const
{
    const auto& spawnWeightsAtPos = this->operator[](coords); 
    return spawnWeightsAtPos.size();
}
bool SpawnWeightsMatrix::isEmptyAt(const SafeVec coords)const{return countAt(coords) == 0;}
bool SpawnWeightsMatrix::hasSpawnsAt(const SafeVec coords)const{return countAt(coords) != 0;}

SpawnWeightsMatrix::SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE) : SIZE(WORLD_MATRIX_SIZE/SpawnWeightsMatrix::DOWNSCALING_FACTOR)
{
    mWeightsMatrix = std::make_unique<matrix<std::unordered_map<SWM_MAP_TYPES>>>
        (WORLD_MATRIX_SIZE, SpawnWeightsMatrix::DOWNSCALING_FACTOR);
} 

