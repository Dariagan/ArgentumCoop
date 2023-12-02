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

#include "GdStringExtractor.cpp"
namespace godot
{
class TileSelector
{
    private:
        std::default_random_engine randomEngine;

        static constexpr unsigned char MAX_TARGETS_COUNT = 64;
        static constexpr unsigned char MAX_GROUPED_TILES_COUNT = 64;
        const unsigned char TARGETS_COUNT = 0;

        std::array<std::array<char, 32>, MAX_TARGETS_COUNT> targetsToFill; //OJO NO PASARSE
        std::array<std::array<char, 32>, MAX_TARGETS_COUNT> tileIdOrDesignatedGroupId;
        std::array<std::pair<std::array<std::array<char, 32>, MAX_GROUPED_TILES_COUNT>, std::discrete_distribution<int>>, MAX_TARGETS_COUNT> idsDistributionOfGroups;

    public:
        std::optional<std::array<char, 32>> getTileId(const std::array<char, 32>& TARGET_TO_FILL)
        {
            auto it = std::find_if(targetsToFill.begin(), targetsToFill.end(), [&](const auto& target) {
                return std::strcmp(&target[0], &TARGET_TO_FILL[0]) == 0;
            });

            if (it != targetsToFill.end())
            {
                auto index = std::distance(targetsToFill.begin(), it);

                if (tileIdOrDesignatedGroupId[index].at(0) != '_')
                {
                    return tileIdOrDesignatedGroupId[index];
                }
                else
                {
                    auto pair = idsDistributionOfGroups[index];
                    return pair.first[pair.second(randomEngine)];
                }
            }
            UtilityFunctions::printerr("couldn't find any candidate tile for the target to be filled: \"",&TARGET_TO_FILL[0],"\" (at TileSelector.cpp::getTileId())");
            return {};
        }

        void reseed(unsigned int seed){randomEngine.seed(seed);};

        

        TileSelector(const Ref<Resource>& gdTileSelection, int seed) try : TARGETS_COUNT(
            ((TypedArray<String>)gdTileSelection->get("targets")).size())
        {
            randomEngine.seed(seed);
            TypedArray<String> gd_targets = gdTileSelection->get("targets");
            TypedArray<String> gd_tile_to_place = gdTileSelection->get("tile_to_place");
            TypedArray<Dictionary> gd_grouped_prob_weighted_tiles = gdTileSelection->get("grouped_prob_weighted_tiles");


            if (TARGETS_COUNT > MAX_TARGETS_COUNT)
            {
                UtilityFunctions::printerr("Passed 64 targets limit for tileselector");
                return;
            }

            if (gd_tile_to_place.size() < TARGETS_COUNT)
            {
                UtilityFunctions::printerr("TileSelection.tres error");
                return;
            }
                        
            for (short unsigned int i = 0; i < TARGETS_COUNT; i++)
            {
                targetsToFill[i] = extractGdString((String)(gd_targets[i]));
                tileIdOrDesignatedGroupId[i] = extractGdString((String)(gd_tile_to_place[i]));

                // std::array<char, 32> targetKeyAsCppString;
                // strncpy(&targetKeyAsCppString[0], &((String)(gd_targets[i])).utf8().get_data()[0], sizeof(targetKeyAsCppString));
                 
                // targetsToFill[i] = targetKeyAsCppString;

                // std::array<char, 32> fillingTileOrGroupAsCppString;
                // strncpy(&fillingTileOrGroupAsCppString[0], &((String)(gd_tile_to_place[i])).utf8().get_data()[0], sizeof(fillingTileOrGroupAsCppString));

                // tileIdOrDesignatedGroupId[i] = fillingTileOrGroupAsCppString;

                if (tileIdOrDesignatedGroupId[i].at(0) == '_')
                {
                    Dictionary dict = gd_grouped_prob_weighted_tiles[i];
                    const short unsigned int DICT_SIZE = dict.keys().size();

                    if (DICT_SIZE > MAX_GROUPED_TILES_COUNT)
                    {
                        UtilityFunctions::printerr("error TODO TileSelector.cpp (constructor)");
                        return;
                    }

                    std::array<std::array<char, 32>, MAX_GROUPED_TILES_COUNT> groupTileIds;
                    std::vector<int> groupTileIdsProbabilities(DICT_SIZE);
                    for (short unsigned int j = 0; j < DICT_SIZE; j++)
                    {
                        groupTileIds[j] = extractGdString((String)(dict.keys()[j]));
                    
                        groupTileIdsProbabilities[j] = (int)dict.values()[j];
                    }
                    
                    const auto GROUP_P_DISTRIBUTION = std::discrete_distribution<int>(groupTileIdsProbabilities.begin(), groupTileIdsProbabilities.end());

                    idsDistributionOfGroups[i] = std::make_pair(groupTileIds, GROUP_P_DISTRIBUTION);
                }
            }  
        } catch (const std::exception& e) {
            UtilityFunctions::printerr("An exception occurred (TileSelector): ", e.what());
        }
        ~TileSelector(){};
    };
}
#endif
