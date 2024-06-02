#ifndef ARGCPP_MATRIX_H
#define ARGCPP_MATRIX_H
#include "SafeVector.cpp"
#include "rust.hpp"
#include <vector>

namespace godot{
template<typename T> 
class matrix {
private:
    const unsigned int DOWNSCALE_FACTOR;
    std::vector<T> flattenedMatrix;

public: 
    const SafeVec SIZE;
    const unsigned long AREA;

    matrix(const SafeVec& size) : 
        DOWNSCALE_FACTOR(1), SIZE(size/1), AREA(SIZE.area())
    {
        if( ! size.isStrictlyPositive()){
            UtilityFunctions::printerr("matrix.cpp constructor error");flattenedMatrix.resize(0);
            return;
        }
        flattenedMatrix.reserve(AREA); flattenedMatrix.resize(AREA);
    }
    //downscaling constructor
    matrix(const SafeVec& originalSize, const unsigned int DOWNSCALE_FACTOR) : 
        DOWNSCALE_FACTOR(MAX(DOWNSCALE_FACTOR, 1)),
        SIZE(originalSize/DOWNSCALE_FACTOR), AREA(SIZE.area())
    {
        if( ! originalSize.isStrictlyPositive()){
            UtilityFunctions::printerr("matrix.cpp downscaling constructor error");flattenedMatrix.resize(0);
            return;
        }
        flattenedMatrix.reserve(AREA); flattenedMatrix.resize(AREA);
    }

    T& operator[](const SafeVec& coords)  
    {
        let downscaledCoords = coords/DOWNSCALE_FACTOR;
        return flattenedMatrix[SIZE.lef*downscaledCoords.lef + downscaledCoords.RIGHT];
    }
    const T& operator[](const SafeVec& coords) const
    {
        let downscaledCoords = coords/DOWNSCALE_FACTOR;
        return flattenedMatrix[SIZE.lef*downscaledCoords.lef + downscaledCoords.RIGHT];
    }

    T& atNoDownscale(const SafeVec& coords)  
    {
        return flattenedMatrix[SIZE.lef*coords.lef + coords.RIGHT];
    }
    const T& atNoDownscale(const SafeVec& coords) const
    {
        return flattenedMatrix[SIZE.lef*coords.lef + coords.RIGHT];
    }

    T& at(const SafeVec& coords)  
    {
        let downscaledCoords = coords/DOWNSCALE_FACTOR;
        return flattenedMatrix.at(SIZE.lef*downscaledCoords.lef + downscaledCoords.RIGHT);
    }
    const T& at(const SafeVec& coords) const
    {
        let downscaledCoords = coords/DOWNSCALE_FACTOR;
        return flattenedMatrix.at(SIZE.lef*downscaledCoords.lef + downscaledCoords.RIGHT);
    }
};
}

#endif //ARGCPP_MATRIX_H
