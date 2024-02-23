#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H
#include "BeingsModule.h"
#include "SafeVector.cpp"
#include "matrix.h"
#include "beingtypes.h"
#include <utility>
#include <type_traits>
#include <limits>
#include <vector>
#include <memory>
namespace godot{

class SpawnWeightsMatrix
{
public:
    static constexpr std::uint_fast8_t DOWNSCALING_FACTOR = 40;
    //CREO QUE NO DEBERÍA ESTAR DISPONIBLE EL SIZE (POSIBLES DOWNSCALING BUGS SI SE HACEN for i MANUALES)
    const SafeVec& SIZE;

#define SWMAT_TYPE std::pair<std::vector<BeingKindId>, std::vector<SpawnWeight>>

    const SWMAT_TYPE& operator[] (const SafeVec& worldMatrixCoords) const;
    const SWMAT_TYPE& at (const SafeVec& worldMatrixCoords) const;
    const SWMAT_TYPE& atNoDownscale (const SafeVec& coords) const;
    
    void insertAt(const SafeVec& coords,
        const BeingKindId& beingkindUid, const SpawnWeight weight);
    void clearAt(const SafeVec& worldMatrixCoords);

    short int countAt(const SafeVec& coords) const;
    bool isEmptyAt(const SafeVec& coords) const;
    bool hasSpawnsAt (const SafeVec& coords) const;

    SpawnWeightsMatrix(const SafeVec& WORLD_MATRIX_SIZE);
        
private: 
    std::unique_ptr<matrix<SWMAT_TYPE>> mWeightsMatrix;
};  


}
#endif //SPAWN_WEIGHTS_MATRIX_H
