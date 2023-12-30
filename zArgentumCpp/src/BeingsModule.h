#ifndef BEINGS_MODULE_H
#define BEINGS_MODULE_H
#include "ArgentumTileMap.h"
#include "BeingBuilder.h"
#include "SpawnWeightsMatrix.h"
#include "typealiases.h"
#include "matrix.h"
#include "beingtypes.h"
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
#include <random>
namespace godot{

class SpawnWeightsMatrix;
class ArgentumTileMap;
//no olvidarse de registrarla despues
class BeingsModule
{
public:
    
    void placeNaturalSpawningWeight(
        const SafeVec formationOrigin, const SafeVec coordsRelativeToFormationOrigin, 
        const beingkind_id& beingKindId, const spawnweight weight, bool deleteOthers);

    void birthBeing(const Vector2i coords, const BeingBuilder& beingBuilder);
    void birthBeingOfKind(const Vector2i coords, const String& being_kind_id);
    
    BeingsModule(ArgentumTileMap* argentumTileMap, const SafeVec size); 
    ~BeingsModule();
    std::unordered_map<SafeVec, std::vector<std::pair<Vector2, being_uid>>, SafeVec::hash> mFrozenBeings;

//get_node("/root/GlobalData"); USAR get_node para acceder a un nodo específico directamente (útil par acceder a GlobalData)

private:
    std::unique_ptr<SpawnWeightsMatrix> mSpawnWeightsMatrix;
    ArgentumTileMap* mArgentumTileMap;
    static constexpr u_char MACROSCOPIC_SPAWNING_CHUNK_SIZE = 6;
    static constexpr u_int16_t BEING_LIMIT_PER_MACROSCOPIC_SPAWNING_CHUNK = 100;

    std::default_random_engine mBeingkindUidEngine{std::default_random_engine(rand())};

    std::unique_ptr<matrix<u_int16_t>> mBeingsInChunkCount;

    void updateChunkBeingCounts();

    void doNaturalSpawning();

};
}

#endif //BEINGS_MODULE_H
