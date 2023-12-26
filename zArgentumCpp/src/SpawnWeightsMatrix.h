#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H
#include "SafeVector.cpp"
#include "typealiases.h"
#include "matrix.h"
#include <utility>
#include <type_traits>
#include <limits>
#include <vector>
#include <unordered_map>
#include <memory>
namespace godot{

class SpawnWeightsMatrix
{
public:
    static constexpr u_char DOWNSCALING_FACTOR = 20;
    const SafeVec SIZE;
    typedef unsigned short int spawnweight;

    #define SWM_MAP_TYPES uint16_t, SpawnWeightsMatrix::spawnweight

    std::unordered_map<SWM_MAP_TYPES>& operator[](const SafeVec worldMatrixCoords) const;
    std::unordered_map<SWM_MAP_TYPES>& at(const SafeVec worldMatrixCoords) const;

    void clearAt(const SafeVec worldMatrixCoords) const;

    short unsigned int countAt(const SafeVec coords) const;
    bool isEmptyAt(const SafeVec coords) const;
    bool hasSpawnsAt (const SafeVec coords) const;

    SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE);
        
private:

    //key: beingkind's uid; value: weight
    std::unique_ptr<matrix<std::unordered_map<SWM_MAP_TYPES>>> mWeightsMatrix;

    //todo meterle la spawnweightmatrix?
};  


}
#endif //SPAWN_WEIGHTS_MATRIX_H
