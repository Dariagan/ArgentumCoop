#ifndef SPAWN_WEIGHTS_MATRIX_H
#define SPAWN_WEIGHTS_MATRIX_H
#include "BeingsModule.hpp"
#include "SafeVector.cpp"
#include "matrix.hpp"
#include "beingtypes.hpp"
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

    using ElementType = std::pair<std::vector<BeingKindId>, std::vector<SpawnWeight>>;

    const ElementType& operator[] (const SafeVec& worldMatrixCoords) const;
    const ElementType& at (const SafeVec& worldMatrixCoords) const;
    const ElementType& atNoDownscale (const SafeVec& coords) const;
    
    void insertAt(const SafeVec& coords,
        const BeingKindId& beingkindUid, const SpawnWeight weight);
    void clearAt(const SafeVec& worldMatrixCoords);

    short int countAt(const SafeVec& coords) const;
    bool isEmptyAt(const SafeVec& coords) const;
    bool hasSpawnsAt (const SafeVec& coords) const;

    SpawnWeightsMatrix(const SafeVec& WORLD_MATRIX_SIZE);
        
private: 
    std::unique_ptr<matrix<ElementType>> mWeightsMatrix;
};  


}
#endif //SPAWN_WEIGHTS_MATRIX_H
