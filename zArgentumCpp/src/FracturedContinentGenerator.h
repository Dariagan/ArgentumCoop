#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>


namespace godot {


    class FracturedContinentGenerator : public FormationGenerator{
        GDCLASS(FracturedContinentGenerator, FormationGenerator)

        private:
            FastNoiseLite continenter, peninsuler, bigLaker, smallLaker, bigbeacher, smallBeacher;

        protected:
            static void _bind_methods();

        public:
            FracturedContinentGenerator();
            ~FracturedContinentGenerator();

            void generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, Vector2i origin, Vector2i area, TilePicker tilePicker = TEMPERATE, int64_t seed = 0) override;
    };
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__