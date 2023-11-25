#ifndef __FORMATION_GENERATOR_H__
#define __FORMATION_GENERATOR_H__
#include "TileSelector.cpp"
#include "MatrixCoords.cpp"
#include "ArgentumTileMap.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING AL COMPILAR
    class ArgentumTileMap;
    class FormationGenerator : public RefCounted{
        GDCLASS(FormationGenerator, RefCounted)

        protected:
            static void _bind_methods();
             

            static void placeBeing(const SafeVec &origin, std::vector<std::vector<std::vector<std::array<char, 32>>>> &worldMatrix,
                const MatrixCoords &coordsRelativeToFormationOrigin, const std::array<char, 32> &beingId);
            
            std::unique_ptr<TileSelector> m_tileSelector;

        public:
            FormationGenerator();
            virtual ~FormationGenerator();
            
            static float getBorderClosenessFactor(MatrixCoords coords, const MatrixCoords& SIZE);
            
            virtual void generate(ArgentumTileMap& argentumTileMap, 
                const SafeVec& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionSet, 
                const unsigned int seed = 0, const Dictionary& data = Dictionary());
    };
}


#endif // __FORMATION_GENERATOR_H__