#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H

#include <type_traits>
#include <limits>
#include <vector>
#include <unordered_map>
#include "SafeVector.cpp"

namespace godot{

class SpawnWeightsMatrix
{
public:
    const SafeVec SIZE;

    auto& operator[](const SafeVec worldMatrixCoords)
    {
        const SafeVec downScaledCoords = worldMatrixCoords/DOWNSCALING_FACTOR;
        return flattenedSpawnWeightsMatrix[SIZE.lef*downScaledCoords.lef + downScaledCoords.RIGHT];
    }
    auto& at(const SafeVec worldMatrixCoords)
    {
        const SafeVec downScaledCoords = worldMatrixCoords/DOWNSCALING_FACTOR;
        return flattenedSpawnWeightsMatrix.at(SIZE.lef*downScaledCoords.lef + downScaledCoords.RIGHT);
    }

    void clearAt(const SafeVec worldMatrixCoords){operator[](worldMatrixCoords).clear();}

    uint16_t countAt(const SafeVec coords)
    {
        const auto& spawnWeightsAtPos = this->operator[](coords); 
        return spawnWeightsAtPos.size();
    }
    bool isEmptyAt(const SafeVec coords){return countAt(coords) == 0;}
    bool hasSpawnsAt(const SafeVec coords){return countAt(coords) != 0;}

    SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE) : SIZE(WORLD_MATRIX_SIZE/DOWNSCALING_FACTOR)
    {resize();} 
        
private:
    
    static constexpr u_char DOWNSCALING_FACTOR = 20;

    //key: beingkind's uid; value: weight
    std::vector<std::unordered_map<uint16_t, uint16_t>> flattenedSpawnWeightsMatrix;

    //todo meterle la spawnweightmatrix?
    
    void resize()
    {
        flattenedSpawnWeightsMatrix.reserve(SIZE.area());
        flattenedSpawnWeightsMatrix.resize(SIZE.area());
    }
};       
}
#endif //SPAWN_WEIGHTS_MATRIX_H
