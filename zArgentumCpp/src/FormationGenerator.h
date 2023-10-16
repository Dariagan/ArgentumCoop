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
            static std::vector<StringName> getTiles(TilePicker tilePicker, std::unordered_map<std::string, bool> &data);
            static void placeTile(std::vector<std::vector<std::vector<StringName>>>& worldMatrix, Vector2i origin, Vector2i relativeCoords, StringName tileId);

        public:
            FormationGenerator();
            ~FormationGenerator();

            virtual void generate(std::vector<std::vector<std::vector<StringName>>>& worldMatrix, Vector2i origin, Vector2i area, TilePicker tilePicker = TEMPERATE, int64_t seed = 0);//TODO ENUM TILEPICKER
    };
}

VARIANT_ENUM_CAST(TilePicker);

#endif // __FORMATION_GENERATOR_H__