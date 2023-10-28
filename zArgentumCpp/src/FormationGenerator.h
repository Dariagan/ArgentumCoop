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
                const Vector2i& origin, const Vector2i& relativeCoords, const std::string& tileId, bool deleteOthers = false);
            TilePicker tilePicker;
        public:
            FormationGenerator();
            ~FormationGenerator();
            
            void generate_pre_check(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                Vector2i origin, const Vector2i& size, const TileSelectionSet tileSelectionSet = TEMPERATE, 
                const signed int seed = 0, const Dictionary& data = Dictionary());

            virtual void generate(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& size, const TileSelectionSet tileSelectionSet = TEMPERATE, 
                const signed int seed = 0, const Dictionary& data = Dictionary());
    };
}


#endif // __FORMATION_GENERATOR_H__