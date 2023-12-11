#ifndef __FORMATION_GENERATOR_H__
#define __FORMATION_GENERATOR_H__
#include "SafeVector.cpp"
#include "ArgentumTileMap.h"
#include "WorldMatrix.h"
#include "TileSelector.h"
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <thread>
#include <execution>
#include "typealiases.h"
namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING AL COMPILAR
class ArgentumTileMap;
class TileSelector;
class FormationGenerator : public RefCounted
{GDCLASS(FormationGenerator, RefCounted)

public:
    FormationGenerator();
    virtual ~FormationGenerator();
    
    static float getBorderClosenessFactor(
        const SafeVec coords, const SafeVec SIZE, const float power = 3.3f);
    
    virtual void generate(ArgentumTileMap& argentumTileMap, 
        const SafeVec origin, const SafeVec size, const Ref<Resource>& tileSelectionSet, 
        const unsigned int seed = 0, const Dictionary& data = Dictionary());
protected:
    static void _bind_methods();
        
    std::unique_ptr<TileSelector> mTileSelector;
};
}
#endif // __FORMATION_GENERATOR_H__