#ifndef __FORMATION_GENERATOR_H__
#define __FORMATION_GENERATOR_H__
#include "TilePicker.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <unordered_set>
#include <memory>

namespace godot {

    class FormationGenerator : public RefCounted{
        GDCLASS(FormationGenerator, RefCounted)


        protected:
            static void _bind_methods();
            void placeTile(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& tileCoordsRelativeToFormationOrigin, const std::string& tileId, bool deleteOthers = false);
            TilePicker tilePicker;
        public:
            FormationGenerator();
            virtual ~FormationGenerator();
            
            static float getBorderClosenessFactor(int i, int j, const Vector2i& size);
            
            virtual void generate(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& size, const TileSelectionSet tileSelectionSet = TEMPERATE, 
                const signed int seed = 0, const Dictionary& data = Dictionary());
    };
}


#endif // __FORMATION_GENERATOR_H__