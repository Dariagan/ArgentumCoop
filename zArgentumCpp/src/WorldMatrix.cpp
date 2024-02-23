#include "WorldMatrix.h"

using namespace godot;

WorldMatrix::WorldMatrix(const SafeVec& size): SIZE(size){resize();} 

std::array<TileTypeUid, WorldMatrix::MAX_TILES_PER_POS>& WorldMatrix::operator[](const SafeVec& coords)
{
    return flattenedUidsMatrix[SIZE.lef*coords.lef + coords.RIGHT];
}
std::array<TileTypeUid, WorldMatrix::MAX_TILES_PER_POS>& WorldMatrix::at(const SafeVec& coords)
{
    return flattenedUidsMatrix.at(SIZE.lef*coords.lef + coords.RIGHT);
}
std::uint_fast8_t WorldMatrix::countAt(const SafeVec& coords)
{
    std::uint_fast8_t count = 0;
    const auto& tilesAtPos = this->operator[](coords); 
    for(std::uint_fast8_t i = 0; i < MAX_TILES_PER_POS; i++)
    {
        count += tilesAtPos[i] != NULL_TILE_UID;
    }
    return count;
}
bool WorldMatrix::isEmptyAt(const SafeVec& coords){return countAt(coords) == 0;}
bool WorldMatrix::isNotEmptyAt(const SafeVec& coords){return countAt(coords) != 0;}

void WorldMatrix::resize()
{
    flattenedUidsMatrix.reserve(SIZE.area());
    flattenedUidsMatrix.resize(SIZE.area());
    for(unsigned int i = 0; i < SIZE.area(); i++)
    {
        constexpr static std::array<TileTypeUid, MAX_TILES_PER_POS> ARRAY_OF_NULL_TILES{initialize_uids_array_as_empty<MAX_TILES_PER_POS>()};// DON'T FORGET TO ADD ZEROES IF
        flattenedUidsMatrix[i] = ARRAY_OF_NULL_TILES;
    }
}

