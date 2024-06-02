#include "SpawnWeightsMatrix.hpp"
using namespace godot;

const SpawnWeightsMatrix::ElementType& SpawnWeightsMatrix::operator[] (const SafeVec& worldMatrixCoords) const
{return SpawnWeightsMatrix::mWeightsMatrix->operator[](worldMatrixCoords);}
const SpawnWeightsMatrix::ElementType& SpawnWeightsMatrix::at (const SafeVec& worldMatrixCoords) const
{return SpawnWeightsMatrix::mWeightsMatrix->at(worldMatrixCoords);}
const SpawnWeightsMatrix::ElementType& SpawnWeightsMatrix::atNoDownscale (const SafeVec& coords) const
{return SpawnWeightsMatrix::mWeightsMatrix->atNoDownscale(coords);}

void SpawnWeightsMatrix::insertAt(
    const SafeVec& coords, const BeingKindId& beingkindId, const SpawnWeight newWeight)
{
    auto& pairAtPos = SpawnWeightsMatrix::mWeightsMatrix->operator[](coords);
    auto& beingkindIds = pairAtPos.first; auto& weights = pairAtPos.second;

    const auto kIter = std::find(beingkindIds.begin(), beingkindIds.end(), beingkindId);
    if (kIter == beingkindIds.end())
    {
        beingkindIds.push_back(beingkindId);
        weights.push_back(newWeight);
    }
    else weights[std::distance(beingkindIds.begin(), kIter)] = newWeight;
}

void SpawnWeightsMatrix::clearAt(const SafeVec& worldMatrixCoords)
{
    auto& pairAtPos = SpawnWeightsMatrix::mWeightsMatrix->operator[](worldMatrixCoords);
    pairAtPos.first.clear(); pairAtPos.second.clear();
}

short int SpawnWeightsMatrix::countAt(const SafeVec& coords) const
{
    const auto& spawnWeightsAtPos = SpawnWeightsMatrix::mWeightsMatrix->operator[](coords);
    return spawnWeightsAtPos.first.size();
}
bool SpawnWeightsMatrix::isEmptyAt(const SafeVec& coords)const{return countAt(coords) == 0;}
bool SpawnWeightsMatrix::hasSpawnsAt(const SafeVec& coords)const{return countAt(coords) > 0;}

SpawnWeightsMatrix::SpawnWeightsMatrix(const SafeVec& WORLD_MATRIX_SIZE) : SIZE(WORLD_MATRIX_SIZE)
{
    mWeightsMatrix = std::make_unique<matrix<ElementType>>(WORLD_MATRIX_SIZE, SpawnWeightsMatrix::DOWNSCALING_FACTOR);
} 

