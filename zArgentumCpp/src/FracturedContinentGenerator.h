#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <memory>

namespace godot 
{
class FracturedContinentGenerator : public FormationGenerator
{
    GDCLASS(FracturedContinentGenerator, FormationGenerator)

    private:
        Vector2i origin, size;

        int DEBUG_RANGE_MIN, DEBUG_RANGE_MAX; 

        RandomNumberGenerator rng;    
        float getBorderClosenessFactor(int i, int j) const;
        float getContinentness(int i, int j, float bcf) const;
        float getBeachness(int i, int j, float continentness) const;

            
    protected:
        static void _bind_methods();

    public:
        FracturedContinentGenerator();
        ~FracturedContinentGenerator();

        FastNoiseLite continenter, peninsuler, bigLaker, smallLaker, bigBeacher, smallBeacher, forest;
        float continental_cutoff, peninsuler_cutoff, bigLakeCutoff, smallLakeCutoff, beachCutoff;

        float get_continental_cutoff() const; void set_continental_cutoff(float cutoff);

//en vez de poner optional parameters así, declarar varios métodos overloaded, hacer q el de menos llame al de más, y bindear los dos
        void generate(std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
            const Vector2i& origin, const Vector2i& size, const TileSetCase tilePicker = TEMPERATE,
            const signed int seed = 0, const Dictionary& data = Dictionary()) override;
};
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__