#ifndef ARGCPP_MATRIX_H
#define ARGCPP_MATRIX_H
#include "SafeVector.cpp"
#include "rust.h"
namespace godot{
template<typename T> 
class matrix {
private:
    const unsigned short int DOWNSCALING_FACTOR;
    std::vector<T> flattenedMatrix;

public: 
    const SafeVec SIZE;
    const unsigned long AREA;

    matrix(const SafeVec size) : 
        DOWNSCALING_FACTOR(1), SIZE(size), AREA(SIZE.lef*SIZE.RIGHT)
    {
        if( ! size.isStrictlyPositive()){
            UtilityFunctions::printerr("matrix.cpp constructor error");flattenedMatrix.reserve(0);flattenedMatrix.resize(0);
            return;
        }
        flattenedMatrix.reserve(AREA); flattenedMatrix.resize(AREA);
    }

    //downscaling constructor
    matrix(const SafeVec originalSize, const unsigned short int DOWNSCALING_FACTOR) : 
        DOWNSCALING_FACTOR(DOWNSCALING_FACTOR),
        SIZE(originalSize/DOWNSCALING_FACTOR), AREA(SIZE.lef*SIZE.RIGHT)
    {
        if( ! originalSize.isStrictlyPositive()){
            UtilityFunctions::printerr("matrix.cpp downscaling constructor error");flattenedMatrix.reserve(0);flattenedMatrix.resize(0);
            return;
        }
        flattenedMatrix.reserve(AREA); flattenedMatrix.resize(AREA);
    }

    T& operator[](const SafeVec coords)
    {
        let downscaledCoords = coords/DOWNSCALING_FACTOR;
        return flattenedMatrix[SIZE.lef*downscaledCoords.lef + downscaledCoords.RIGHT];
    }

    T& at(const SafeVec coords)
    {
        let downscaledCoords = coords/DOWNSCALING_FACTOR;
        return flattenedMatrix.at(SIZE.lef*downscaledCoords.lef + downscaledCoords.RIGHT);
    }
};
}

#endif //ARGCPP_MATRIX_H
