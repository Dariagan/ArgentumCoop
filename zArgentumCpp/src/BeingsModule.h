#ifndef NATURAL_SPAWNING_MODULE_H
#define NATURAL_SPAWNING_MODULE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>
#include <memory>
#include "ArgentumTileMap.h"
#include "BeingBuilder.h"
#include "SpawnWeightsMatrix.cpp"

namespace godot{

//no olvidarse de registrarla despues

class BeingsModule//TODO no sé si hacerlo un Node o RefCounted, o nada. tal vez un node por las signals
{

public:
    void placeNaturalSpawningWeight(
        const SafeVec formationOrigin, const SafeVec coordsRelativeToFormationOrigin, 
        const uint16_t& beingKindId, const unsigned char weight, bool deleteOthers);

    void birthBeing(const Vector2i coords, const BeingBuilder& beingBuilder);
    void birthBeingOfKind(const String& being_kind_id);
    
    BeingsModule(const ArgentumTileMap& argentumTileMap, const SafeVec size); ~BeingsModule(){};
    std::unordered_map<SafeVec, std::vector<std::pair<Vector2, int>>, SafeVec::hash> mFrozenBeings;
private:
    std::unique_ptr<SpawnWeightsMatrix> mSpawnWeightsMatrix = nullptr;
    ArgentumTileMap& ArgentumTileMap;

protected: static void _bind_methods();//EL CUERPO DE ESTE VA SÍ O SÍ EN .cpp    
};
}

#endif //NATURAL_SPAWNING_MODULE_H
