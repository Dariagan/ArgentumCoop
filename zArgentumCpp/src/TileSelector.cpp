#include "TileSelector.h"

using namespace godot;

TileSelector::TileSelector(const Ref<Resource>& gdTileSelection, const ArgentumTileMap& argentumTileMap, const u_int seed, const char input_n_threads) try : 
    TARGETS_COUNT(((Array)gdTileSelection->get("targets")).size()), N_THREADS(input_n_threads)
{
    if (input_n_threads < 1)
    {
        UtilityFunctions::printerr("TIleSelector: thread count is less than 1");
        return;
    }

    m_randomEngines.reserve(N_THREADS);
    m_randomEngines.resize(N_THREADS);
    this->reseed(seed);

    TypedArray<String> gd_targets = gdTileSelection->get("targets");

    m_availableTargets.reserve(TARGETS_COUNT);
    m_availableTargets.resize(TARGETS_COUNT);
    m_tileUidOrGroup.resize(TARGETS_COUNT);
    m_tileUidOrGroup.reserve(TARGETS_COUNT);
    m_idsDistributionOfGroups.resize(TARGETS_COUNT);
    m_idsDistributionOfGroups.reserve(TARGETS_COUNT);

    TypedArray<String> gd_tile_to_place = gdTileSelection->get("tile_to_place");
    TypedArray<Dictionary> gd_grouped_prob_weighted_tiles = gdTileSelection->get("grouped_prob_weighted_tiles");

    if (gd_tile_to_place.size() < TARGETS_COUNT)
    {
        UtilityFunctions::printerr("TileSelection.tres error");
        return;
    }
                
    for (u_int16_t i = 0; i < TARGETS_COUNT; i++)
    {
        m_availableTargets[i] = ((String)(gd_targets[i])).utf8().get_data();

        if(((String)gd_tile_to_place[i])[0] != '_')
            m_tileUidOrGroup[i] = argentumTileMap.findTileUid((String)gd_tile_to_place[i]).value_or(WorldMatrix::NULL_TILE_UID);
        else
        {
            m_tileUidOrGroup[i] = IS_A_GROUP;

            const Dictionary& group_dict = gd_grouped_prob_weighted_tiles[i];
            
            u_int16_t GROUP_DICT_SIZE = group_dict.keys().size();

            std::vector<tile_class_uid> groupedTileUid(GROUP_DICT_SIZE);
            groupedTileUid.reserve(GROUP_DICT_SIZE);

            std::vector<u_int> groupTileUidWeight(GROUP_DICT_SIZE);
            groupTileUidWeight.reserve(GROUP_DICT_SIZE);

            for (u_int16_t j = 0; j < GROUP_DICT_SIZE; j++)
            {
                const auto& opt = argentumTileMap.findTileUid(group_dict.keys()[j]);
                groupedTileUid[j] = opt.value_or(WorldMatrix::NULL_TILE_UID);
                if(!opt.has_value())
                {
                    UtilityFunctions::printerr("couldn't find tile_id for: ", group_dict.keys()[j]);
                }
                groupTileUidWeight[j] = (u_int)group_dict.values()[j];
            }
            
            const auto groupProbsDistribution = std::discrete_distribution<u_int>(groupTileUidWeight.begin(), groupTileUidWeight.end());

            m_idsDistributionOfGroups[i] = std::make_pair(groupedTileUid, groupProbsDistribution);
        }
    }  
} catch (const std::exception& e) {
    UtilityFunctions::printerr("An exception occurred (TileSelector): ", e.what());
}
TileSelector::~TileSelector(){};

tile_class_uid TileSelector::getTileUidForTarget(const char* inputTargetTofill, const u_char thread_i)
{
    const auto it = std::find_if(m_availableTargets.begin(), m_availableTargets.end(), [&](const std::string& availableTarget) {
        return std::strcmp(availableTarget.c_str(), inputTargetTofill) == 0;
    });

    if (it != m_availableTargets.end())
    {
        auto index = std::distance(m_availableTargets.begin(), it);
        try
        {
            const auto& optTileID = std::get<std::optional<tile_class_uid>>(m_tileUidOrGroup[index]);
            
            return optTileID.value_or(WorldMatrix::NULL_TILE_UID);
        }
        catch (const std::bad_variant_access& ex)
        {
            auto& pair = m_idsDistributionOfGroups[index];
            //grabs a random tileUid from the group
            return pair.first[pair.second(m_randomEngines[thread_i])];
        }          
    }
    UtilityFunctions::printerr("couldn't find any candidate tile for the target to be filled: \"",&inputTargetTofill[0],"\" (at TileSelector.cpp::getTileId())");
    return WorldMatrix::NULL_TILE_UID;
}

void TileSelector::reseed(const u_int seed){
    for(u_char thread_i = 0; thread_i < N_THREADS; thread_i++)
    {
        m_randomEngines[thread_i].seed(seed+thread_i);
    }
}

