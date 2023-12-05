#ifndef __TILE_SELECTOR_H__
#define __TILE_SELECTOR_H__
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/defs.hpp>
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

#include "GlobalData.h"
#include "WorldMatrix.cpp"

namespace godot
{
class TileSelector
{
    public:
        TileSelector(const Ref<Resource>& gdTileSelection, int seed) try : 
            TARGETS_COUNT(((Array)gdTileSelection->get("targets")).size())
        {
            RandomEngine.seed(seed);
            TypedArray<String> gd_targets = gdTileSelection->get("targets");

            AvailableTargets.reserve(TARGETS_COUNT);
            AvailableTargets.resize(TARGETS_COUNT);
            TileUidOrGroup.resize(TARGETS_COUNT);
            TileUidOrGroup.reserve(TARGETS_COUNT);
            IdsDistributionOfGroups.resize(TARGETS_COUNT);
            IdsDistributionOfGroups.reserve(TARGETS_COUNT);

            TypedArray<String> gd_tile_to_place = gdTileSelection->get("tile_to_place");
            TypedArray<Dictionary> gd_grouped_prob_weighted_tiles = gdTileSelection->get("grouped_prob_weighted_tiles");

            if (gd_tile_to_place.size() < TARGETS_COUNT)
            {
                UtilityFunctions::printerr("TileSelection.tres error");
                return;
            }
                        
            for (short unsigned int i = 0; i < TARGETS_COUNT; i++)
            {
                AvailableTargets[i] = ((String)(gd_targets[i])).utf8().get_data();

                if(((String)gd_tile_to_place[i])[0] != '_')
                    TileUidOrGroup[i] = GlobalData::getTileUid((String)gd_tile_to_place[i]);
                else
                {
                    TileUidOrGroup[i] = true;

                    Dictionary group_dict = gd_grouped_prob_weighted_tiles[i];
                    uint16_t DICT_SIZE = group_dict.keys().size();

                    std::vector<uint16_t> groupedTileUid(DICT_SIZE);
                    groupedTileUid.reserve(DICT_SIZE);

                    std::vector<uint16_t> groupTileUidWeight(DICT_SIZE);
                    groupTileUidWeight.reserve(DICT_SIZE);

                    for (uint16_t j = 0; j < DICT_SIZE; j++)
                    {
                        groupedTileUid[j] = GlobalData::getTileUid((String)(group_dict.keys()[j])).value_or(std::numeric_limits<uint16_t>::max());
                    
                        groupTileUidWeight[j] = (int)group_dict.values()[j];
                    }
                    
                    const auto GROUP_P_DISTRIBUTION = std::discrete_distribution<uint16_t>(groupTileUidWeight.begin(), groupTileUidWeight.end());

                    IdsDistributionOfGroups[i] = std::make_pair(groupedTileUid, GROUP_P_DISTRIBUTION);
                }
            }  
        } catch (const std::exception& e) {
            UtilityFunctions::printerr("An exception occurred (TileSelector): ", e.what());
        }
        ~TileSelector(){};

        uint16_t getTileUidForTarget(const std::string inputTargetTofill)
        {
            auto it = std::find_if(AvailableTargets.begin(), AvailableTargets.end(), [&](const std::string& availableTarget) {
                return std::strcmp(availableTarget.c_str(), &inputTargetTofill[0]) == 0;
            });

            if (it != AvailableTargets.end())
            {
                auto index = std::distance(AvailableTargets.begin(), it);
                try
                {
                    const auto& optTileID = std::get<std::optional<uint16_t>>(TileUidOrGroup[index]); // w contains int, not float: will throw
                    return optTileID.value_or(WorldMatrix::NULL_UID);
                }
                catch (const std::bad_variant_access& ex)
                {
                    auto pair = IdsDistributionOfGroups[index];
                    return pair.first[pair.second(RandomEngine)];
                }          
            }
            UtilityFunctions::printerr("couldn't find any candidate tile for the target to be filled: \"",&inputTargetTofill[0],"\" (at TileSelector.cpp::getTileId())");
            return WorldMatrix::NULL_UID;
        }

        void reseed(unsigned int seed){RandomEngine.seed(seed);};

    private:
        std::default_random_engine RandomEngine;

        const unsigned int TARGETS_COUNT;

        std::vector<std::string> AvailableTargets; 
        std::vector<std::variant<std::optional<uint16_t>, bool>> TileUidOrGroup; //bool: is group
        std::vector<std::pair<std::vector<uint16_t>, std::discrete_distribution<uint16_t>>> IdsDistributionOfGroups;

};
}
#endif
