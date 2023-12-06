#ifndef WORLD_MATRIX_H
#define WORLD_MATRIX_H

#include <type_traits>
#include <limits>
#include <vector>

#include "SafeVector.cpp"

namespace godot{

static constexpr uint16_t NULL_TILE_UID = std::numeric_limits<uint16_t>::max();

template <unsigned size>
constexpr std::array<uint16_t, size> initialize_uids_array_as_empty()
{
    std::array<uint16_t, size> ret{};
    for (int i = 0; i < size; i++)
        ret[i] = NULL_TILE_UID;
    return ret;
}


class WorldMatrix
{
    public:
        const SafeVec SIZE;

        static constexpr unsigned char MAX_TILES_PER_POS = 5;

        std::array<uint16_t, MAX_TILES_PER_POS>& operator[](const SafeVec& coords)
        {
            return flattenedUidsMatrix[SIZE.lef*coords.lef + coords.RIGHT];
        }

        unsigned char countAt(const SafeVec& coords)
        {
            unsigned char count = 0;
            const auto& tilesAtPos = this->operator[](coords); 
            for(int i = 0; i < MAX_TILES_PER_POS; i++)
            {
                count += tilesAtPos[i] != NULL_TILE_UID;
            }
            return count;
        }

        bool isEmptyAt(const SafeVec& coords)
        {
            return countAt(coords) == 0;
        }
        bool isNotEmptyAt(const SafeVec& coords)
        {
            return countAt(coords) != 0;
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
                constexpr static std::array<uint16_t, MAX_TILES_PER_POS> arrayOfZeroes{ initialize_uids_array_as_empty<MAX_TILES_PER_POS>()};// DON'T FORGET TO ADD ZEROES IF
                flattenedUidsMatrix[i] = arrayOfZeroes;
            }
        }
    };       
}

#endif //WORLD_MATRIX_H
