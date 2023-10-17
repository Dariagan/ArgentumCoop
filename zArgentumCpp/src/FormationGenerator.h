#ifndef __FORMATION_GENERATOR_H__
#define __FORMATION_GENERATOR_H__

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/godot.hpp>


namespace godot {

    enum TilePicker{
        TEMPERATE = 0
    };

    class FormationGenerator : public RefCounted{
        GDCLASS(FormationGenerator, RefCounted)

        protected:
            static void _bind_methods();
            static std::vector<StringName> getTiles(const TilePicker tilePicker, std::unordered_map<std::string, bool> &data);
            static void placeTile(std::vector<std::vector<std::vector<StringName>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& relativeCoords, const StringName& tileId);
        public:
            FormationGenerator();
            ~FormationGenerator();

            virtual void generate(std::vector<std::vector<std::vector<StringName>>>& worldMatrix, 
                const Vector2i& origin, const Vector2i& size, const TilePicker tilePicker = TEMPERATE, 
                const signed int seed = 0, const Dictionary& data = Dictionary());//TODO ENUM TILEPICKER
    };
}
VARIANT_ENUM_CAST(TilePicker);

#endif // __FORMATION_GENERATOR_H__