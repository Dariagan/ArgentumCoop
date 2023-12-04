#ifndef WORLD_MATRIX_H
#define WORLD_MATRIX_H

#include <vector>

#include "SafeVector.cpp"
#include <type_traits>
#include <limits>

namespace godot{

template <unsigned len, uint16_t val>
constexpr std::array<uint16_t, len> initialize_array()
{
    std::array<uint16_t, len> ret{};
    for (int i = 0; i < len; i++)
        ret[i] = val;
    return ret;
}

class WorldMatrix
{
    public:
        const SafeVec SIZE;

        static constexpr uint16_t NULL_UID = std::numeric_limits<uint16_t>::max();

        static constexpr unsigned char MAX_TILES_PER_POS = 4;

        std::array<uint16_t, MAX_TILES_PER_POS>& operator[](const SafeVec& coords)
        {
            return flattenedUidsMatrix[SIZE.lef*coords.lef + coords.RIGHT];
        }

        std::array<uint16_t, MAX_TILES_PER_POS>& at(const SafeVec& coords)
        {
            return flattenedUidsMatrix.at(SIZE.lef*coords.lef + coords.RIGHT);
        }

        WorldMatrix(const SafeVec& size) : SIZE(size)
        {
            resize(); 
        } 
            
    private:
        
        std::vector<std::array<uint16_t, MAX_TILES_PER_POS>> flattenedUidsMatrix;

        //todo meterle la spawnweightmatrix?
        
        void resize()
        {
            flattenedUidsMatrix.reserve(SIZE.area());
            flattenedUidsMatrix.resize(SIZE.area());
            for(unsigned int i = 0; i < SIZE.area(); i++)
            {
                constexpr static std::array<uint16_t, MAX_TILES_PER_POS> arrayOfZeroes{ initialize_array<MAX_TILES_PER_POS, NULL_UID>()};// DON'T FORGET TO ADD ZEROES IF
                flattenedUidsMatrix[i] = arrayOfZeroes;
            }
        }
    };       
}

#endif //WORLD_MATRIX_H

/* CTRL + A, CTRL + K , CTRL + U, TO UNCOMMENT ALL THE LINES */