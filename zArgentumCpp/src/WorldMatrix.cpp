#ifndef WORLD_MATRIX_H
#define WORLD_MATRIX_H

#include <vector>
#include <godot_cpp/godot.hpp>

#include "SafeVector.cpp"

namespace godot{

class WorldMatrix
{
    private:
        std::vector<std::vector<uint16_t>> flattenedUidsMatrix;
        int matrixWidth = 0;

        void resize(const SafeVec& size)
        {
            matrixWidth = size.lef;
            flattenedUidsMatrix.reserve(size.lef*size.RIGHT);
            flattenedUidsMatrix.resize(size.lef*size.RIGHT);
        }
            
    protected:
    public:
        std::vector<uint16_t>& operator[](const SafeVec& coords)
        {
            return flattenedUidsMatrix[matrixWidth*coords.lef + coords.RIGHT];
        }

        std::vector<uint16_t>& at(const SafeVec& coords)
        {
            return flattenedUidsMatrix.at(matrixWidth*coords.lef +coords.RIGHT);
        }

        

        WorldMatrix(const SafeVec& size)
        {
            resize(size);
        } 
        
        ~WorldMatrix();
};
}

#endif //WORLD_MATRIX_H

/* CTRL + A, CTRL + K , CTRL + U, TO UNCOMMENT ALL THE LINES */