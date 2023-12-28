#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H
#include "BeingsModule.h"
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
    //CREO QUE NO DEBERÍA ESTAR DISPONIBLE EL SIZE (POSIBLES DOWNSCALING BUGS SI SE HACEN for i MANUALES)
    const SafeVec SIZE;

    typedef unsigned short int beingkind_uid;
    typedef unsigned short int spawnweight;

                     //key: beingKIND's uid; value: weight
#define SWM_MAP_TYPES SpawnWeightsMatrix::beingkind_uid, SpawnWeightsMatrix::spawnweight

    std::unordered_map<SWM_MAP_TYPES>& operator[](const SafeVec worldMatrixCoords);
    std::unordered_map<SWM_MAP_TYPES>& at(const SafeVec worldMatrixCoords);

    void clearAt(const SafeVec worldMatrixCoords);

    short unsigned int countAt(const SafeVec coords) const;
    bool isEmptyAt(const SafeVec coords) const;
    bool hasSpawnsAt (const SafeVec coords) const;

    SpawnWeightsMatrix(const SafeVec WORLD_MATRIX_SIZE);

    //TODO HACER ITERATORS!!!
    //O ALGO PARA ITERAR SOBRE UN CIERTO ÁREA DE SPAWNWEIGHTS, SAFELY SIN CAGARLA (DEBIDO AL DOWNSCALING)
        
private: 
    std::unique_ptr<matrix<std::unordered_map<SWM_MAP_TYPES>>> mWeightsMatrix;
};  


}
#endif //SPAWN_WEIGHTS_MATRIX_H
