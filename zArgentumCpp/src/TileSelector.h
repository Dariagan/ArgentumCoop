#ifndef __TILE_SELECTOR_H__
#define __TILE_SELECTOR_H__
#include "ArgentumTileMap.h"
#include "WorldMatrix.h"
#include "typealiases.h"
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
    TileSelector(const Ref<Resource>& gdTileSelection, const ArgentumTileMap& argentumTileMap, const u_int seed, const char N_THREADS);
    ~TileSelector();

    //⚠️ argument "thread_i" must be within 0 and N_THREADS-1 ⚠️
    tiletype_uid getTileUidForTarget(const char* inputTargetTofill, const u_char thread_i);

    void reseedEngines(const u_int seed);

private:

    // 1 random engine for each thread, this is for making
    // the pseudorandom generation deterministic on the seed value
    // (so world can be re-generated after leaving)
    std::vector<std::default_random_engine> mRandomEngines;

    const u_int TARGETS_COUNT;
    const u_char N_THREADS;

    typedef bool group;

    typedef unsigned int weight;

    std::vector<std::string> mAvailableTargets; 
    std::vector<std::variant<tiletype_uid, group>> mTileUidOrGroup; 
    std::vector<std::pair<std::vector<tiletype_uid>, std::discrete_distribution<u_int>>> mIdsDistributionOfGroups;
};
}
#endif

