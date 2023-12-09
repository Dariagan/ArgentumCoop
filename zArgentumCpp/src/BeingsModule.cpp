#include "BeingsModule.h"

using namespace godot;


void BeingsModule::placeNaturalSpawningWeight(
    const SafeVec formationOrigin, const SafeVec coordsRelativeToFormationOrigin, 
    const uint16_t& beingKindId, const unsigned char weight, bool deleteOthers)
{
    const SafeVec absoluteCoordinates = (formationOrigin + coordsRelativeToFormationOrigin);

    if(deleteOthers) {mSpawnWeightsMatrix->clearAt(absoluteCoordinates);}
    
    (*mSpawnWeightsMatrix)[absoluteCoordinates][beingKindId] = weight;
}

//ojo estas coords son absolutas, no relativas al origin de la formation
void BeingsModule::birthBeing(const Vector2i coords, const BeingBuilder& beingBuilder)
{
    if(beingBuilder.getResult().has_value())
    {
        //emit_signal("birth_being_w_init_data", coords, beingBuilder.getResult().value());
    }//shouldn't print anything otherwise, that's the builder's task when building
}

void BeingsModule::birthBeingOfKind(const String& being_kind_id)
{/*emit_signal("birth_of_being_of_kind", being_kind_id);*/}

BeingsModule::BeingsModule(godot::ArgentumTileMap* argentumTileMap, const SafeVec size)
{
    this->mArgentumTileMap = argentumTileMap;
    this->mSpawnWeightsMatrix = std::make_unique<SpawnWeightsMatrix>(size);
}