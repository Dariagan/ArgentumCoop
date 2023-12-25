#ifndef ARGCPP_MATRIX_H
#define ARGCPP_MATRIX_H
#include "SafeVector.cpp"

namespace godot{

/* RENAME THE CLASS USING: CTRL + F2 */
template<typename T> 
class matrix 
{
public: 
    
    matrix(const SafeVec size);
    const SafeVec size;
    const SafeVec area;

    T& operator[](const SafeVec coords);
    T& at(const SafeVec coords);

private:
    std::vector<T> flattenedMatrix;

};
}

#endif //ARGCPP_MATRIX_H
