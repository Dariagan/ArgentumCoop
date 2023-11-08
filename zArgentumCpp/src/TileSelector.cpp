#ifndef __TILE_SELECTOR_H__
#define __TILE_SELECTOR_H__

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <random>
#include <utility> 
namespace godot
{
class TileSelector
{
    private:
        std::default_random_engine randomEngine;

        std::vector<std::string> targetsToFill;//HACER UN ARRAY DE C PRE-ALOCADO EN EL STACK? (SERÍA MÁS RÁPIDO, AUNQUE, HABRÍA QUE GUARDAR UN MAX_I)
        std::vector<std::string> tileIdOrDesignationAsGroup;
        std::vector<std::pair<std::vector<std::string>, std::discrete_distribution<int>>> idsDistributionOfGroups;

    public:
        std::string getTileId(const std::string& TARGET_TO_FILL)
        {
            for (short unsigned int i = 0; i < targetsToFill.size(); i++)
            {
                if (targetsToFill[i] == TARGET_TO_FILL)
                {
                    if (tileIdOrDesignationAsGroup[i].at(0) != '_')
                    {
                        return tileIdOrDesignationAsGroup[i];
                    }
                    else
                    {
                        auto pair = idsDistributionOfGroups[i];

                        return pair.first[pair.second(randomEngine)];
                    }
                    break;
                }    
            }
            UtilityFunctions::printerr("couldn't find filler for target: ", TARGET_TO_FILL.c_str());
            return {};
        }

        void reseed(unsigned int seed){randomEngine.seed(seed);};

        TileSelector(const Ref<Resource>& gdTileSelection, int seed)
        {
            randomEngine.seed(seed);
            TypedArray<String> gd_targets = gdTileSelection->get("targets");
            TypedArray<String> gd_tile_to_place = gdTileSelection->get("tile_to_place");
            TypedArray<Dictionary> gd_grouped_prob_weighted_tiles = gdTileSelection->get("grouped_prob_weighted_tiles");

            const short unsigned int TARGETS_COUNT = gd_targets.size();
            if (gd_tile_to_place.size() < TARGETS_COUNT)
            {
                UtilityFunctions::printerr("TileSelection.tres error");
                return;
            }
            
            for (short unsigned int i = 0; i < TARGETS_COUNT; i++)
            {
                targetsToFill.resize(TARGETS_COUNT); tileIdOrDesignationAsGroup.resize(TARGETS_COUNT); idsDistributionOfGroups.resize(TARGETS_COUNT);

                std::string targetKeyAsCppString = ((String)(gd_targets[i])).utf8().get_data();
                targetsToFill[i] = targetKeyAsCppString;

                std::string fillingTileOrGroupAsCppString = ((String)(gd_tile_to_place[i])).utf8().get_data();
                tileIdOrDesignationAsGroup[i] = fillingTileOrGroupAsCppString;

                if (tileIdOrDesignationAsGroup[i].at(0) == '_')
                {
                    Dictionary dict = gd_grouped_prob_weighted_tiles[i];
                    const short unsigned int DICT_SIZE = dict.keys().size();
                    std::vector<std::string> groupTileIds(DICT_SIZE);
                    std::vector<int> groupTileIdsProbabilities(DICT_SIZE);
                    for (short unsigned int j = 0; j < DICT_SIZE; j++)
                    {
                        std::string tileIdAsCppString = ((String)(dict.keys()[j])).utf8().get_data();
                        groupTileIds[j] = tileIdAsCppString;
                    
                        groupTileIdsProbabilities[j] = (int)dict.values()[j];
                    }
                    
                    auto groupDistribution = std::discrete_distribution<int>(groupTileIdsProbabilities.begin(), groupTileIdsProbabilities.end());

                    idsDistributionOfGroups[i] = std::make_pair(groupTileIds, groupDistribution);
                }
            }  
        }
        ~TileSelector(){};
    };
}
#endif
