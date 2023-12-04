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

namespace godot
{
class TileSelector
{
    private:
        std::default_random_engine randomEngine;

        static constexpr unsigned char MAX_GROUPED_TILES_COUNT = 64;
        

        std::vector<std::string> TargetsToFill; //hacerlo std::string?
        std::vector<uint16_t> tileUidOrGroupPlaceHolder;
        std::vector<std::pair<std::array<uint16_t, MAX_GROUPED_TILES_COUNT>, std::discrete_distribution<int>>> idsDistributionOfGroups;

    public:
        std::optional<uint16_t> getTileUid(const std::array<char, 32>& argTargetToFill)
        {
            auto it = std::find_if(TargetsToFill.begin(), TargetsToFill.end(), [&](const std::string& target) {
                return std::strcmp(target.c_str(), &argTargetToFill[0]) == 0;
            });

            if (it != TargetsToFill.end())
            {
                auto index = std::distance(TargetsToFill.begin(), it);

                if (tileUidOrGroupPlaceHolder[index] != -1)
                {
                    return tileUidOrGroupPlaceHolder[index];
                }
                else
                {
                    auto pair = idsDistributionOfGroups[index];
                    return pair.first[pair.second(randomEngine)];
                }
            }
            UtilityFunctions::printerr("couldn't find any candidate tile for the target to be filled: \"",&argTargetToFill[0],"\" (at TileSelector.cpp::getTileId())");
            return {};
        }

        void reseed(unsigned int seed){randomEngine.seed(seed);};

        

        TileSelector(const Ref<Resource>& gdTileSelection, int seed) try
        {
            randomEngine.seed(seed);
            TypedArray<String> gd_targets = gdTileSelection->get("targets");

            const unsigned int TARGETS_COUNT = gd_targets.size();

            TargetsToFill.reserve(TARGETS_COUNT);
            TargetsToFill.resize(TARGETS_COUNT);

            TypedArray<String> gd_tile_to_place = gdTileSelection->get("tile_to_place");
            TypedArray<Dictionary> gd_grouped_prob_weighted_tiles = gdTileSelection->get("grouped_prob_weighted_tiles");

            if (gd_tile_to_place.size() < TARGETS_COUNT)
            {
                UtilityFunctions::printerr("TileSelection.tres error");
                return;
            }
                        
            for (short unsigned int i = 0; i < TARGETS_COUNT; i++)
            {
                TargetsToFill[i] = ((String)(gd_targets[i])).utf8().get_data();

                if(((String)gd_tile_to_place[i])[0] != '_')
                    //tileIdOrDesignatedGroupId[i] = GlobalSingleton.getUid((String)gd_tile_to_place[i]);//uncomment later
                    char a = 1;
                else
                {
                    Dictionary group_dict = gd_grouped_prob_weighted_tiles[i];
                    const short unsigned int DICT_SIZE = group_dict.keys().size();

                    std::vector<uint16_t> groupTileUids(DICT_SIZE);
                    groupTileUids.reserve(DICT_SIZE);

                    std::vector<int> groupTileUidsProbabilities(DICT_SIZE);
                    for (short unsigned int j = 0; j < DICT_SIZE; j++)
                    {
                        //groupTileUids[j] = GlobalSingleton.getUid((String)(group_dict.keys()[j]));
                    
                        groupTileUidsProbabilities[j] = (int)group_dict.values()[j];
                    }
                    
                    const auto GROUP_P_DISTRIBUTION = std::discrete_distribution<int>(groupTileUidsProbabilities.begin(), groupTileUidsProbabilities.end());

                    //idsDistributionOfGroups[i] = std::make_pair(groupTileUids, GROUP_P_DISTRIBUTION);
                }
            }  
        } catch (const std::exception& e) {
            UtilityFunctions::printerr("An exception occurred (TileSelector): ", e.what());
        }
        ~TileSelector(){};
    };
}
#endif
