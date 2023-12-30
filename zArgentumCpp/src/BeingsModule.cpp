#include "BeingsModule.h"

using namespace godot;


void BeingsModule::placeNaturalSpawningWeight(
    const SafeVec formationOrigin, const SafeVec coordsRelativeToFormationOrigin, 
    const beingkind_id& beingKindId, const spawnweight weight, bool deleteOthers)
{
    let absoluteCoordinates = formationOrigin + coordsRelativeToFormationOrigin;

    if(deleteOthers) {mSpawnWeightsMatrix->clearAt(absoluteCoordinates);}
    
    mSpawnWeightsMatrix->insertAt(absoluteCoordinates, beingKindId, weight);
}

//ojo estas coords son absolutas, no relativas al origin de la formation
void BeingsModule::birthBeing(const Vector2i coords, const BeingBuilder& beingBuilder)
{
    if(beingBuilder.getResult().has_value())
    {
        mArgentumTileMap->emit_signal("birth_being_w_init_data", coords, beingBuilder.getResult().value());
    }
}

void BeingsModule::birthBeingOfKind(const Vector2i local_coords, const String& being_kind_id)
{mArgentumTileMap->emit_signal("birth_of_being_of_kind", local_coords, being_kind_id);}

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

//hacerlo async
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
                let randomBeingkindUid = beingkindUids[distribution(mBeingkindUidEngine)];

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
