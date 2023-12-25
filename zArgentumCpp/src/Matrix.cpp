#include "Matrix.h"
using namespace godot;

template<typename T>
matrix<T>::matrix(const SafeVec size) : size(size), area(size.lef*size.RIGHT)
{
    flattenedMatrix.reserve(area);
    flattenedMatrix.resize(area);
}

template<typename T>
T& matrix<T>::operator[](const SafeVec coords)
{
    return flattenedMatrix[size.lef*coords.lef + coords.RIGHT];
}

template<typename T>
T& matrix<T>::at(const SafeVec coords)
{
    return flattenedMatrix.at(size.lef*coords.lef + coords.RIGHT);
}


