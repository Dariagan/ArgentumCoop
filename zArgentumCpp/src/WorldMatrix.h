#ifndef WORLD_MATRIX_H
#define WORLD_MATRIX_H

#include <type_traits>
#include <limits>
#include <vector>
#include "SafeVector.cpp"

namespace godot{

using TileTypeUid = std::uint16_t;

template <unsigned size>
constexpr std::array<TileTypeUid, size> initialize_uids_array_as_empty();

class WorldMatrix
{
public:
    const SafeVec SIZE;

    static constexpr TileTypeUid NULL_TILE_UID = std::numeric_limits<std::uint16_t>::max();

    static constexpr std::uint8_t MAX_TILES_PER_POS = 5;

    std::array<TileTypeUid, MAX_TILES_PER_POS>& operator[](const SafeVec& coords);
    std::array<TileTypeUid, MAX_TILES_PER_POS>& at(const SafeVec& coords);
    std::uint_fast8_t countAt(const SafeVec& coords);
    bool isEmptyAt(const SafeVec& coords);
    bool isNotEmptyAt(const SafeVec& coords);

    WorldMatrix(const SafeVec& size); 
        
private:
    
    std::vector<std::array<TileTypeUid, MAX_TILES_PER_POS>> flattenedUidsMatrix;

    //todo meterle la spawnweightmatrix?
    
    void resize();
};     

template <unsigned size>
constexpr std::array<TileTypeUid, size> initialize_uids_array_as_empty()
{
    std::array<TileTypeUid, size> ret{};
    for (int i = 0; i < size; i++)
        ret[i] = WorldMatrix::NULL_TILE_UID;
    return ret;
};

}
#endif //WORLD_MATRIX_H
