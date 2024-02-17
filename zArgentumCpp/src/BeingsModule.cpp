#include "BeingsModule.h"

using namespace godot;


void BeingsModule::placeNaturalSpawningWeight(
    const SafeVec formationOrigin, const SafeVec coordsRelativeToFormationOrigin, 
    const beingkind_id& beingKindId, const spawnweight weight)
{
    let absoluteCoordinates = formationOrigin + coordsRelativeToFormationOrigin;

    //if(deleteOthers) {mSpawnWeightsMatrix->clearAt(absoluteCoordinates);}
    
    mSpawnWeightsMatrix->insertAt(absoluteCoordinates, beingKindId, weight);
}

//ojo estas coords son absolutas, no relativas al origin de la formation
void BeingsModule::birthBeing(const Vector2i coords, const BeingBuilder& beingBuilder)
{
    if(beingBuilder.getResult().has_value()){
        mArgentumTileMap->emit_signal("birth_being_w_init_data", coords, beingBuilder.getResult().value());
    }
}

void BeingsModule::birthBeingOfKind(const Vector2i tile_map_coords, const beingkind_id& being_kind_id)
{mArgentumTileMap->emit_signal("birth_of_being_of_kind", tile_map_coords, being_kind_id);}

void BeingsModule::birthBeingOfKind(
    const Vector2i tl_tile_map_coords, const Vector2i br_tile_map_coords, const beingkind_id& being_kind_id)
{
    const Variant& getbeingkinds_result = mArgentumTileMap->global_data->get("beingkinds");
    if(getbeingkinds_result.get_type() != Variant::DICTIONARY)
    {
        UtilityFunctions::printerr("couldn't get dict beingkinds from global_data");
        return;
    }
    const Dictionary& beingkinds = (const Dictionary&)getbeingkinds_result;

    if( ! beingkinds.has(being_kind_id))
    {
        UtilityFunctions::printerr("beingkind with id \"",being_kind_id,"\" not found. (birthBeingOfKind() - BeingsModule.cpp)");return;
    }
    const Variant& get_whitelisted_tiles_for_spawning_result = beingkinds[being_kind_id].get("whitelisted_tiles_for_spawning");
    if(get_whitelisted_tiles_for_spawning_result.get_type() != Variant::DICTIONARY)
    {
        UtilityFunctions::printerr("couldn't get whitelisted_tiles_for_spawning from ", being_kind_id);return;
    }
    const Dictionary& whitelisted_tiles = (const Dictionary&)get_whitelisted_tiles_for_spawning_result;
    //TODO HANDLEAR blacklisted tiles
    //TODO handlear objectos con colisión/impasables en layers superiores

    std::uniform_int_distribution<> distr_x(tl_tile_map_coords.x, br_tile_map_coords.x-1);
    std::uniform_int_distribution<> distr_y(tl_tile_map_coords.y, br_tile_map_coords.y-1);

    unsigned long tries = 0;
    bool suitableTileFound = false;
    Vector2i randPos;
    do{
        if(++ tries > (br_tile_map_coords-tl_tile_map_coords).length()*10 || tries >= std::numeric_limits<long>::max() - 1)
        {
            UtilityFunctions::printerr("too many unsuccessful tries"); return;
        }
        randPos = Vector2i(distr_x(mEngine), distr_y(mEngine));

        letref tilesAtPos = mArgentumTileMap->mWorldMatrixPtr->operator[](randPos);
        const StringName tile_id0 = mArgentumTileMap->getTileId(tilesAtPos[0]);
        const StringName tile_id1 = mArgentumTileMap->getTileId(tilesAtPos[1]);

        suitableTileFound = whitelisted_tiles.has(tile_id0) && (tile_id1==WorldMatrix::NULL_TILE_UID || whitelisted_tiles.has(tile_id1));
    } 
    while( ! suitableTileFound);

    birthBeingOfKind(randPos, being_kind_id);
}

void BeingsModule::updateChunkBeingCounts()
{
    std::for_each(std::execution::par_unseq, mFrozenBeings.begin(), mFrozenBeings.end(), 
        [&](const std::pair<const SafeVec, std::vector<std::pair<Vector2, being_uid>>> & frozenBeing)
        { 
            const auto&[coords, vec] = frozenBeing;
            (*mBeingsInChunkCount)[coords] = vec.size();
        }
    );
}

//! hacerlo async (no bloqueante)
//solo debería ejecutar esto el host y desp retransmitir los spawneos específicos
void BeingsModule::doNaturalSpawning()
{
    for(u_int16_t chunk_i = 0; chunk_i < mBeingsInChunkCount->SIZE.lef; chunk_i++)
    for(u_int16_t chunk_j = 0; chunk_j < mBeingsInChunkCount->SIZE.RIGHT; chunk_j++)
    {
        const SafeVec chunkCoords(chunk_i, chunk_j);
        for(u_int16_t sw_i = 0; sw_i < MACROSCOPIC_SPAWNING_CHUNK_SIZE; sw_i++)
        for(u_int16_t sw_j = 0; sw_j < MACROSCOPIC_SPAWNING_CHUNK_SIZE; sw_j++)
        {
            if((*mBeingsInChunkCount)[chunkCoords] < BEING_LIMIT_PER_MACROSCOPIC_SPAWNING_CHUNK)
            {
                const SafeVec swCoords = chunkCoords*MACROSCOPIC_SPAWNING_CHUNK_SIZE + SafeVec(sw_i, sw_j);

                letref spawnWeightsMapping = mSpawnWeightsMatrix->atNoDownscale(swCoords);
                letref weights = spawnWeightsMapping.second;

                std::discrete_distribution<spawnweight> distribution(weights.begin(), weights.end());

                letref beingkindUids = spawnWeightsMapping.first;
                let randomBeingkindUid = beingkindUids[distribution(mEngine)];

                birthBeingOfKind(swCoords*SpawnWeightsMatrix::DOWNSCALING_FACTOR, randomBeingkindUid);
                (*mBeingsInChunkCount)[chunkCoords]++;
            }
            else goto nextchunk;   
        }
        nextchunk:;
    }
    
}

BeingsModule::BeingsModule(godot::ArgentumTileMap* argentumTileMap, const SafeVec size)
{
    this->mArgentumTileMap = argentumTileMap;
    this->mBeingsInChunkCount = std::make_unique<matrix<u_int16_t>>(size, MACROSCOPIC_SPAWNING_CHUNK_SIZE*mSpawnWeightsMatrix->DOWNSCALING_FACTOR);
    this->mSpawnWeightsMatrix = std::make_unique<SpawnWeightsMatrix>(size);
    
}

BeingsModule::~BeingsModule()
{
    
}
