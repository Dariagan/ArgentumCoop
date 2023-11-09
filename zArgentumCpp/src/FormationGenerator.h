#ifndef __FORMATION_GENERATOR_H__
#define __FORMATION_GENERATOR_H__
#include "TileSelector.cpp"
#include "MatrixCoords.cpp"

#include <godot_cpp/classes/ref_counted.hpp>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace godot 
{
    class FormationGenerator : public RefCounted{
        GDCLASS(FormationGenerator, RefCounted)

        protected:
            static void _bind_methods();
            static void placeTile(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const MatrixCoords& tileCoordsRelativeToFormationOrigin, 
                const std::string& tileId, bool deleteOthers = false);

            static void placeBeing(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const MatrixCoords& tileCoordsRelativeToFormationOrigin, 
                const std::string& beingId);
            
            std::unique_ptr<TileSelector> tileSelector;

        public:
            FormationGenerator();
            virtual ~FormationGenerator();
            
            static float getBorderClosenessFactor(u_int16_t i, u_int16_t j, const MatrixCoords& SIZE);
            
            virtual void generate(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionSet, 
                const unsigned int seed = 0, const Dictionary& data = Dictionary());
    };
}


#endif // __FORMATION_GENERATOR_H__