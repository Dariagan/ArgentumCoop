#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H
#include "SafeVector.cpp"
#include "typealiases.h"
#include <utility>
#include <type_traits>
#include <limits>
#include <vector>
#include <unordered_map>

namespace godot{


class SpawnWeightsMatrix
{
public:
    static constexpr u_char DOWNSCALING_FACTOR = 20;
    const SafeVec SIZE;
    typedef unsigned short int spawnweight;

    #define SWM_MAP_TYPES uint16_t, SpawnWeightsMatrix::spawnweight

    std::unordered_map<SWM_MAP_TYPES>& operator[](const SafeVec worldMatrixCoords);
    std::unordered_map<SWM_MAP_TYPES>& at(const SafeVec worldMatrixCoords);

    void clearAt(const SafeVec worldMatrixCoords);

    short unsigned int countAt(const SafeVec coords);
    bool isEmptyAt(const SafeVec coords);
    bool hasSpawnsAt(const SafeVec coords);

    SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE);
        
private:
    
    

    //key: beingkind's uid; value: weight
    std::vector<std::unordered_map<SWM_MAP_TYPES>> flattenedSpawnWeightsMatrix;

    //todo meterle la spawnweightmatrix?
    
    void resize();
    
};  


}
#endif //SPAWN_WEIGHTS_MATRIX_H
