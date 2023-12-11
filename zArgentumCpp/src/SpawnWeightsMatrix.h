#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H
#include <type_traits>
#include <limits>
#include <vector>
#include <unordered_map>
#include "SafeVector.cpp"
#include "typealiases.h"

namespace godot{

class SpawnWeightsMatrix
{
public:
    const SafeVec SIZE;

    std::unordered_map<uint16_t, uint16_t>& operator[](const SafeVec worldMatrixCoords);
    std::unordered_map<uint16_t, uint16_t>& at(const SafeVec worldMatrixCoords);

    void clearAt(const SafeVec worldMatrixCoords);

    uint16_t countAt(const SafeVec coords);
    bool isEmptyAt(const SafeVec coords);
    bool hasSpawnsAt(const SafeVec coords);

    SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE);
        
private:
    
    static constexpr u_char DOWNSCALING_FACTOR = 20;

    //key: beingkind's uid; value: weight
    std::vector<std::unordered_map<uint16_t, uint16_t>> flattenedSpawnWeightsMatrix;

    //todo meterle la spawnweightmatrix?
    
    void resize();
    
};       
}
#endif //SPAWN_WEIGHTS_MATRIX_H
