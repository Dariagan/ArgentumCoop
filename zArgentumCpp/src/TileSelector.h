#ifndef __TILE_SELECTOR_H__
#define __TILE_SELECTOR_H__
#include "ArgentumTileMap.h"
#include "WorldMatrix.h"
#include "rust.h"
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
namespace godot
{   
class ArgentumTileMap; 
class TileSelector
{
public:
    TileSelector(const Ref<Resource>& gdTileSelection, const ArgentumTileMap& argentumTileMap, const std::uint_fast32_t seed, const std::uint_fast8_t N_THREADS);
    ~TileSelector();

    //⚠️ argument "thread_i" must be within 0 and N_THREADS-1 ⚠️
    TileTypeUid getTileUidForTarget(const char* inputTargetTofill, const std::uint_fast8_t thread_i);

    void reseedEngines(const std::uint_fast32_t seed);

private:

    // 1 random engine for each thread, this is for making
    // the pseudorandom generation deterministic on the seed value
    // (so world can be re-generated after leaving)
    std::vector<std::default_random_engine> mRandomEngines;

    const int TARGETS_COUNT;
    const std::uint_fast8_t N_THREADS;

    typedef bool group;

    typedef unsigned int weight;

    std::vector<std::string> mAvailableTargets; 
    std::vector<std::variant<TileTypeUid, group>> mTileUidOrGroup; 
    std::vector<std::pair<std::vector<TileTypeUid>, std::discrete_distribution<weight>>> mIdsDistributionOfGroups;
};
}
#endif

