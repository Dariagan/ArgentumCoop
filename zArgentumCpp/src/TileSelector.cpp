#include "TileSelector.h"

using namespace godot;

//TODO los groups se rompieron, arreglarlo.   

TileSelector::TileSelector(const Ref<Resource>& gdTileSelection, const ArgentumTileMap& argentumTileMap, const u_int seed, const char input_n_threads) try : 
    TARGETS_COUNT(((Array)gdTileSelection->get("targets")).size()), N_THREADS(input_n_threads)
{
    if (input_n_threads < 1)
    {
        UtilityFunctions::printerr("TIleSelector: less than 1 thread given");
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
                
    for (uint16_t i = 0; i < TARGETS_COUNT; i++)
    {
        m_availableTargets[i] = ((String)(gd_targets[i])).utf8().get_data();

        if(((String)gd_tile_to_place[i])[0] != '_')
            m_tileUidOrGroup[i] = argentumTileMap.findTileUid((String)gd_tile_to_place[i]).value_or(std::numeric_limits<uint16_t>::max());
        else
        {
            m_tileUidOrGroup[i] = true;

            const Dictionary& group_dict = gd_grouped_prob_weighted_tiles[i];
            
            uint16_t GROUP_DICT_SIZE = group_dict.keys().size();

            std::vector<uint16_t> groupedTileUid(GROUP_DICT_SIZE);
            groupedTileUid.reserve(GROUP_DICT_SIZE);

            std::vector<uint16_t> groupTileUidWeight(GROUP_DICT_SIZE);
            groupTileUidWeight.reserve(GROUP_DICT_SIZE);

            for (uint16_t j = 0; j < GROUP_DICT_SIZE; j++)
            {
                const auto& opt = argentumTileMap.findTileUid(group_dict.keys()[j]);
                groupedTileUid[j] = opt.value_or(std::numeric_limits<uint16_t>::max());
                if(!opt.has_value())
                {
                    UtilityFunctions::printerr("couldn't find tile_id for: ", group_dict.keys()[j]);
                }
                groupTileUidWeight[j] = (uint16_t)group_dict.values()[j];
            }
            
            const auto GROUP_P_DISTRIBUTION = std::discrete_distribution<uint16_t>(groupTileUidWeight.begin(), groupTileUidWeight.end());

            m_idsDistributionOfGroups[i] = std::make_pair(groupedTileUid, GROUP_P_DISTRIBUTION);
        }
    }  
} catch (const std::exception& e) {
    UtilityFunctions::printerr("An exception occurred (TileSelector): ", e.what());
}
TileSelector::~TileSelector(){};

uint16_t TileSelector::getTileUidForTarget(const char* inputTargetTofill, const u_char thread_i)
{
    auto it = std::find_if(m_availableTargets.begin(), m_availableTargets.end(), [&](const std::string& availableTarget) {
        return std::strcmp(availableTarget.c_str(), inputTargetTofill) == 0;
    });

    if (it != m_availableTargets.end())
    {
        auto index = std::distance(m_availableTargets.begin(), it);
        try
        {
            const auto& optTileID = std::get<std::optional<uint16_t>>(m_tileUidOrGroup[index]);
            
            return optTileID.value_or(std::numeric_limits<uint16_t>::max());
            
            // UtilityFunctions::printerr("nullopt");
            // return std::numeric_limits<uint16_t>::max();
        }
        catch (const std::bad_variant_access& ex)
        {
            auto& pair = m_idsDistributionOfGroups[index];
            //grabs a random tileUid from the group
            return pair.first[pair.second(m_randomEngines[thread_i])];
        }          
    }
    UtilityFunctions::printerr("couldn't find any candidate tile for the target to be filled: \"",&inputTargetTofill[0],"\" (at TileSelector.cpp::getTileId())");
    return std::numeric_limits<uint16_t>::max();
}

void TileSelector::reseed(const u_int seed){
    for(u_char threadI = 0; threadI < N_THREADS; threadI++)
    {
        m_randomEngines[threadI].seed(seed+1);
    }
}

