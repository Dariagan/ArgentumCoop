#ifndef BEINGS_MODULE_H
#define BEINGS_MODULE_H
#include "ArgentumTileMap.h"
#include "BeingBuilder.h"
#include "SpawnWeightsMatrix.h"
#include "typealiases.h"
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
namespace godot{

typedef short unsigned int being_uid_t;
class ArgentumTileMap;
//no olvidarse de registrarla despues
class BeingsModule
{
public:
    
    void placeNaturalSpawningWeight(
        const SafeVec formationOrigin, const SafeVec coordsRelativeToFormationOrigin, 
        const uint16_t& beingKindId, const unsigned char weight, bool deleteOthers);

    void birthBeing(const Vector2i coords, const BeingBuilder& beingBuilder);
    void birthBeingOfKind(const Vector2i coords, const String& being_kind_id);
    
    BeingsModule(godot::ArgentumTileMap* argentumTileMap, const SafeVec size); 
    ~BeingsModule();
    std::unordered_map<SafeVec, std::vector<std::pair<Vector2, int>>, SafeVec::hash> mFrozenBeings;
private:
    std::unique_ptr<SpawnWeightsMatrix> mSpawnWeightsMatrix = nullptr;
    godot::ArgentumTileMap* mArgentumTileMap;
    static constexpr unsigned char SPAWNING_MACROSCOPIC_CHUNK_SIZE = 6;

};
}

#endif //BEINGS_MODULE_H
