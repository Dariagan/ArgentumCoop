#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

namespace godot {

    class FracturedContinentGenerator : public FormationGenerator{
        GDCLASS(FracturedContinentGenerator, FormationGenerator)

        private:
                FastNoiseLite continenter, peninsuler, bigLaker, smallLaker, bigbeacher, smallBeacher;
                RandomNumberGenerator rng;    
                
        protected:
            static void _bind_methods();

        public:
            FracturedContinentGenerator();
            ~FracturedContinentGenerator();

            void generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, 
                const Vector2i& origin, const Vector2i& area, const TilePicker tilePicker = TEMPERATE,
                const int seed = 0, const Dictionary& data = Dictionary()) override;
    };
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__