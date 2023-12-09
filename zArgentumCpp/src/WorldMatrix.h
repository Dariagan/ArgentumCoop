#ifndef WORLD_MATRIX_H
#define WORLD_MATRIX_H

#include <type_traits>
#include <limits>
#include <vector>

#include "SafeVector.cpp"

namespace godot{

typedef unsigned short int tile_uid_t;

template <unsigned size>
constexpr std::array<tile_uid_t, size> initialize_uids_array_as_empty();

class WorldMatrix
{
public:
    const SafeVec SIZE;

    static constexpr tile_uid_t NULL_TILE_UID = std::numeric_limits<u_int16_t>::max();//!DEBE SER IGUAL

    static constexpr unsigned char MAX_TILES_PER_POS = 5;

    std::array<tile_uid_t, MAX_TILES_PER_POS>& operator[](const SafeVec coords);
    std::array<tile_uid_t, MAX_TILES_PER_POS>& at(const SafeVec coords);
    unsigned char countAt(const SafeVec coords);
    bool isEmptyAt(const SafeVec coords);
    bool isNotEmptyAt(const SafeVec coords);

    WorldMatrix(const SafeVec size); 
        
private:
    
    std::vector<std::array<tile_uid_t, MAX_TILES_PER_POS>> flattenedUidsMatrix;

    //todo meterle la spawnweightmatrix?
    
    void resize();
};     

template <unsigned size>
constexpr std::array<tile_uid_t, size> initialize_uids_array_as_empty()
{
    std::array<tile_uid_t, size> ret{};
    for (int i = 0; i < size; i++)
        ret[i] = WorldMatrix::NULL_TILE_UID;//!DEBE SER IGUAL
    return ret;
};

}
#endif //WORLD_MATRIX_H
