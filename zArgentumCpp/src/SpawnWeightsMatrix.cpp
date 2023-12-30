#include "SpawnWeightsMatrix.h"
using namespace godot;

const SWMAT_TYPE& SpawnWeightsMatrix::operator[] (const SafeVec worldMatrixCoords) const
{return SpawnWeightsMatrix::mWeightsMatrix->operator[](worldMatrixCoords);}
const SWMAT_TYPE& SpawnWeightsMatrix::at (const SafeVec worldMatrixCoords) const
{return SpawnWeightsMatrix::mWeightsMatrix->at(worldMatrixCoords);}
const SWMAT_TYPE& SpawnWeightsMatrix::atNoDownscale (const SafeVec coords) const
{return SpawnWeightsMatrix::mWeightsMatrix->atNoDownscale(coords);}

void SpawnWeightsMatrix::insertAt(
    const SafeVec coords, const beingkind_id beingkindUid, const spawnweight weight)
{
    auto& pairAtPos = SpawnWeightsMatrix::mWeightsMatrix->operator[](coords);
    
    pairAtPos.first.push_back(beingkindUid);
    pairAtPos.second.push_back(weight);
}

void SpawnWeightsMatrix::clearAt(const SafeVec worldMatrixCoords)
{
    auto& pairAtPos = SpawnWeightsMatrix::mWeightsMatrix->operator[](worldMatrixCoords);
    pairAtPos.first.clear(); pairAtPos.second.clear();
}

short int SpawnWeightsMatrix::countAt(const SafeVec coords) const
{
    const auto& spawnWeightsAtPos = SpawnWeightsMatrix::mWeightsMatrix->operator[](coords);
    return spawnWeightsAtPos.first.size();
}
bool SpawnWeightsMatrix::isEmptyAt(const SafeVec coords)const{return countAt(coords) == 0;}
bool SpawnWeightsMatrix::hasSpawnsAt(const SafeVec coords)const{return countAt(coords) > 0;}

SpawnWeightsMatrix::SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE) : SIZE(WORLD_MATRIX_SIZE)
{
    mWeightsMatrix = std::make_unique<matrix<SWMAT_TYPE>>(WORLD_MATRIX_SIZE, SpawnWeightsMatrix::DOWNSCALING_FACTOR);
} 

