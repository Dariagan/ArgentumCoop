#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <memory>

namespace godot {

    class FracturedContinentGenerator : public FormationGenerator{
        GDCLASS(FracturedContinentGenerator, FormationGenerator)

        private:
            Vector2i origin, size;

            RandomNumberGenerator rng;    
            float getBorderClosenessFactor(int i, int j);
            float getContinentness(int i, int j, float bcf);
            float getBeachness(int i, int j, float continentness);
                
        protected:
            static void _bind_methods();

        public:
            FracturedContinentGenerator();
            ~FracturedContinentGenerator();

            FastNoiseLite continenter, peninsuler, bigLaker, smallLaker, bigBeacher, smallBeacher;
            float continentalCutoff, peninsulerCutoff, bigLakeCutoff, smallLakeCutoff, beachCutoff;

//en vez de poner optional parameters así, declarar varios métodos overloaded, hacer q el de menos llame al de más, y bindear los dos
            void generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, 
                const Vector2i& origin, const Vector2i& size, const TilePicker tilePicker = TEMPERATE,
                const signed int seed = 0, const Dictionary& data = Dictionary()) override;
    };
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__