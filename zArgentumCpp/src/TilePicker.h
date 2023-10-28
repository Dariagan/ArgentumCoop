
#ifndef __TILE_PICKER_H__
#define __TILE_PICKER_H__

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/object.hpp>

namespace godot
{
    enum TileSelectionSet{
        TEMPERATE = 0,
        DESERT = 1
    };

    class TilePicker{        

        public:

            TilePicker();
            ~TilePicker();
            
            std::vector<std::string> getTiles(const TileSelectionSet tileSelectionSet, std::unordered_map<std::string, std::string> &data, unsigned int seed);
    };
} // namespace godot

VARIANT_ENUM_CAST(TileSelectionSet);


#endif // __TILE_PICKER_H__
