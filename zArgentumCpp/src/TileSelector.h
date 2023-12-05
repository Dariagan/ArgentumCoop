#ifndef __TILE_SELECTOR_H__
#define __TILE_SELECTOR_H__
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>
#include <unordered_map>
#include <vector>
#include <random>
#include <utility> 
#include <optional>
#include <algorithm>
#include <limits>
#include <variant>
#include "ArgentumTileMap.h"

namespace godot
{   
class ArgentumTileMap; 
class TileSelector
{
public:
    TileSelector():TARGETS_COUNT(2){};

    TileSelector(const Ref<Resource>& gdTileSelection, ArgentumTileMap* argentumTileMap, const unsigned int seed);
    ~TileSelector();

    uint16_t getTileUidForTarget(const char* inputTargetTofill);

    void reseed(unsigned int seed);

private:
    std::default_random_engine m_randomEngine;

    const unsigned int TARGETS_COUNT;

    std::vector<std::string> m_availableTargets; 
    std::vector<std::variant<std::optional<uint16_t>, bool>> m_tileUidOrGroup; //bool: is group
    std::vector<std::pair<std::vector<uint16_t>, std::discrete_distribution<uint16_t>>> m_idsDistributionOfGroups;

};
}
#endif
