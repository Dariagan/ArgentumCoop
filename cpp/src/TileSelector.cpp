#include "TileSelector.hpp"
using namespace godot;

TileSelector::TileSelector(const Ref<Resource>& gdTileSelection, const CppArgentumTileMap& argentumTileMap, const std::uint_fast32_t seed, const std::uint_fast8_t input_n_threads) try : 
    TARGETS_COUNT(((Array)gdTileSelection->get("targets")).size()), N_THREADS(input_n_threads)
{
    if (input_n_threads < 1){UtilityFunctions::printerr("TileSelector.cpp: passed input_n_threads is less than 1");return;}

    mRandomEngines.resize(N_THREADS); this->reseedEngines(seed);

    TypedArray<String> gd_targets = gdTileSelection->get("targets");

    mAvailableTargets.reserve(TARGETS_COUNT);
    mAvailableTargets.resize(TARGETS_COUNT);
    mTileUidOrGroup.resize(TARGETS_COUNT);
    mTileUidOrGroup.reserve(TARGETS_COUNT);
    mIdsDistributionOfGroups.resize(TARGETS_COUNT);
    mIdsDistributionOfGroups.reserve(TARGETS_COUNT);

    TypedArray<String> gd_tile_to_place = gdTileSelection->get("tile_to_place");
    TypedArray<Dictionary> gd_grouped_prob_weighted_tiles = gdTileSelection->get("grouped_prob_weighted_tiles");

    if (gd_tile_to_place.size() < TARGETS_COUNT){UtilityFunctions::printerr("TileSelector.cpp: passed dictionary doesn't cover all needed targets for the formation");return;}
                
    for (std::uint_fast16_t i = 0; i < TARGETS_COUNT; i++)
    {
        mAvailableTargets[i] = ((String)(gd_targets[i])).utf8().get_data();

        // if it isn't a group of tiles
        if(((String)gd_tile_to_place[i])[0] != '_')
        {
            let optUid = argentumTileMap.findTileUid((String)gd_tile_to_place[i]);
            if( ! optUid.has_value()) 
                UtilityFunctions::printerr("couldn't find tile_id for: ", (String)gd_tile_to_place[i], " in the tilemap's tiledata, using NULL_TILE_UID");
          
            mTileUidOrGroup[i] = optUid.value_or(WorldMatrix::NULL_TILE_UID);        
        }
        else{
            mTileUidOrGroup[i] = group(true);

            const Dictionary& group_dict = gd_grouped_prob_weighted_tiles[i];
            
            let group_dict_size = group_dict.keys().size(); if(group_dict_size >= WorldMatrix::NULL_TILE_UID){UtilityFunctions::printerr("TileSelector.cpp: passed dict is too big");}

            std::vector<TileTypeUid> groupedTileUid(group_dict_size);
            groupedTileUid.reserve(group_dict_size);

            std::vector<weight> groupTileUidWeight(group_dict_size);
            groupTileUidWeight.reserve(group_dict_size);

            for (std::uint_fast32_t j = 0; j < MIN(group_dict_size, WorldMatrix::NULL_TILE_UID-1); j++)
            {
                let optUid = argentumTileMap.findTileUid(group_dict.keys()[j]);
                if( ! optUid.has_value()){
                    UtilityFunctions::printerr("couldn't find tile_id: ", group_dict.keys()[j], " in tilemap's UID mapping, using NULL_TILE_UID");
                }
                groupedTileUid[j] = optUid.value_or(WorldMatrix::NULL_TILE_UID);
                
                groupTileUidWeight[j] = CLAMP((int64_t)group_dict.values()[j], 0, std::numeric_limits<weight>::max());
            }
            let groupProbsDistribution = std::discrete_distribution<weight>(groupTileUidWeight.begin(), groupTileUidWeight.end());

            mIdsDistributionOfGroups[i] = std::make_pair(groupedTileUid, groupProbsDistribution);
        }
    }  
} catch (const std::exception& e) {UtilityFunctions::printerr("TileSelector.cpp exception: ", e.what());}

TileSelector::~TileSelector(){};

TileTypeUid TileSelector::getTileUidForTarget(const char* inputTargetTofill, const std::uint_fast8_t thread_i)
{
    let iter = std::find_if(mAvailableTargets.begin(), mAvailableTargets.end(), 
        [&](const std::string& availableTarget) {
            return std::strcmp(availableTarget.c_str(), inputTargetTofill) == 0;
        });

    if (iter != mAvailableTargets.end())
    {
        let index = std::distance(mAvailableTargets.begin(), iter);
        letref variantValue = mTileUidOrGroup[index];

        if(std::holds_alternative<TileTypeUid>(variantValue))
        {
            return std::get<TileTypeUid>(variantValue);
        }
        auto& pair = mIdsDistributionOfGroups[index];
        return pair.first[pair.second(mRandomEngines[thread_i])];
    }
    UtilityFunctions::printerr("couldn't find any candidate tile for the target to be filled: \"",&inputTargetTofill[0],"\" (at TileSelector.cpp::getTileId())");
    return WorldMatrix::NULL_TILE_UID;
}

void TileSelector::reseedEngines(const std::uint_fast32_t seed){
    for(std::uint_fast8_t thread_i = 0; thread_i < N_THREADS; thread_i++){
        mRandomEngines[thread_i].seed(seed+thread_i);
    }
}

