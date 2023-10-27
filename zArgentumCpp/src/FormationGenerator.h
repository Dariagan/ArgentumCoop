#ifndef __FORMATION_GENERATOR_H__
#define __FORMATION_GENERATOR_H__

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <unordered_set>
#include "TilePicker.cpp"
#include <memory>

namespace godot {

    enum TileSetCase{
        TEMPERATE = 0,
        DESERT = 1
    };

    class FormationGenerator : public RefCounted{
        GDCLASS(FormationGenerator, RefCounted)

        protected:
            static void _bind_methods();
            static std::vector<std::string> getTiles(const TileSetCase tilePicker, std::unordered_set<std::string> &data, unsigned int seed = 0);
            void placeTile(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& relativeCoords, const std::string& tileId, bool deleteOthers = false);
            std::unique_ptr<TilePicker> tilePicker;
        public:
            FormationGenerator();
            ~FormationGenerator();
            
            void generate_pre_check(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                Vector2i origin, const Vector2i& size, const TileSetCase tilePicker = TEMPERATE, 
                const signed int seed = 0, const Dictionary& data = Dictionary());

            virtual void generate(std::vector<std::vector<std::vector<std::string>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& size, const TileSetCase tilePicker = TEMPERATE, 
                const signed int seed = 0, const Dictionary& data = Dictionary());//TODO ENUM TILEPICKER
    };
}
VARIANT_ENUM_CAST(TileSetCase);

#endif // __FORMATION_GENERATOR_H__