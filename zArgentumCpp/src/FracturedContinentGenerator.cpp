#include "FracturedContinentGenerator.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <algorithm>
#include <unordered_map>

using namespace godot;

void FracturedContinentGenerator::_bind_methods(){}


void FracturedContinentGenerator::generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& size, const TilePicker tilePicker, 
    const signed int seed, const Dictionary& data)
{
    this->origin = origin;
    this->size = size;
    
    {
        continenter.set_seed(seed); peninsuler.set_seed(seed+1); bigLaker.set_seed(seed+2); smallLaker.set_seed(seed+3);
        bigBeacher.set_seed(seed+4); smallBeacher.set_seed(seed+5); rng.set_seed(seed);
//hacer un for multiplicativo de la frequency en vez de separar en big y small. aumentar el cutoff. sumarle 1 a la seed en cada iteracion del for
        continenter.set_frequency(0.15f/powf(size.length(), 0.995f)); peninsuler.set_frequency(5.5f/powf(size.length(), 0.995f));
        bigBeacher.set_frequency(3.f/powf(size.length(), 0.99f)); smallBeacher.set_frequency(9.f/powf(size.length(), 0.995f));
        bigLaker.set_frequency(110.f/powf(size.length(), 0.995f)); smallLaker.set_frequency(240.f/powf(size.length(), 0.991f));

        continentalCutoff = 0.f;
    }
      

    while(continenter.get_noise_2dv(origin) < continentalCutoff + 0.13f){
        continenter.set_offset(continenter.get_offset() + Vector3(3,3,0));
    }

    for (int i = -size.x/2; i < size.x/2; i++){
    for (int j = -size.y/2; j < size.y/2; j++)
    {   
        float bcf = getBorderClosenessFactor(i, j);
        float continentness = getContinentness(i, j, bcf);
        bool continental = continentness > continentalCutoff;
        bool peninsulerCaved = (float)peninsuler.get_noise_2d(i, j) < peninsulerCutoff;

        //if(i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){printf("pc=%d", peninsulerCaved);if (j % 2 == 0) std::cout << "\n"; else std::cout << "||| ";} 

        bool awayFromCoast = continentness > continentalCutoff + 0.01f && peninsuler.get_noise_2d(i, j) > peninsulerCutoff + 0.27f;
        float beachness = (float)getBeachness(i, j, continentness);
        bool beach = beachness > beachCutoff;
        
        bool lake = (((smallLaker.get_noise_2d(i, j) + 1)*0.65f) - beachness > smallLakeCutoff) 
                    || (((bigLaker.get_noise_2d(i, j) + 1)*0.65f) - beachness > bigLakeCutoff);
        
        std::unordered_map<std::string, bool> data = {
            {"continental", continental},
            {"peninsuler_caved", peninsulerCaved},
            {"away_from_coast", awayFromCoast},
            {"lake", lake},
            {"beach", beach}
        };        
        auto tiles = FormationGenerator::getTiles(tilePicker, data);

        for(auto tileId: tiles){
            FormationGenerator::placeTile(worldMatrix, origin, Vector2i(i, j), tileId);
        }
        tiles.clear();       
    }}
}

float FracturedContinentGenerator::getBorderClosenessFactor(int i, int j)
{
    //if (i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){printf("%d %d", i, j);printf(", bcf %.2f = max(%.2f, %.2f)", std::max((float)abs(i - size.x) / ((float)size.x/2.f), (float)abs(j - size.y) / ((float)size.y/2.f)), (float)abs(i - size.x) / ((float)size.x/2.f), (float)abs(j - size.y) / ((float)size.y/2.f));}
    return std::max(
        (float)abs(i - origin.x) / ((float)size.x/2.f), 
        (float)abs(j - origin.y) / ((float)size.y/2.f));
}
float FracturedContinentGenerator::getContinentness(int i, int j, float bcf)
{
    //if (i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){printf(", cntness %.3f = %.3f - %.3f, ", (float)continenter.get_noise_2d(i, j) - bcf / 4.2f, (float)continenter.get_noise_2d(i, j), bcf/4.2f);}
    return (float)continenter.get_noise_2d(i, j) -  bcf / 4.2f - powf(bcf, 43.f);
}
float FracturedContinentGenerator::getBeachness(int i, int j, float continentness)
{
    return std::max(
        0.72f + (float)bigBeacher.get_noise_2d(i, j) / 2.3f - powf(continentness - continentalCutoff, 0.6f), 
        0.8f + (float)smallBeacher.get_noise_2d(i, j) / 2.3f - powf((float)peninsuler.get_noise_2d(i, j) - peninsulerCutoff, 0.45f));
}

FracturedContinentGenerator::FracturedContinentGenerator()
{
    DEBUG_RANGE_MIN = -50; DEBUG_RANGE_MAX = DEBUG_RANGE_MIN + 100;
    continenter.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX);
    peninsuler.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX);
    bigLaker.set_noise_type(FastNoiseLite::NoiseType::TYPE_VALUE_CUBIC);
    smallLaker.set_noise_type(FastNoiseLite::NoiseType::TYPE_VALUE_CUBIC);
    bigBeacher.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX_SMOOTH);
    smallBeacher.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX_SMOOTH);
    
    continentalCutoff = 0.6f;//overriden
    peninsulerCutoff = -0.1f; bigLakeCutoff = -0.9f; smallLakeCutoff = -0.9f; beachCutoff = 0.8f;

    continenter.set_fractal_lacunarity(2.8f); continenter.set_fractal_weighted_strength(0.5f);
    peninsuler.set_fractal_gain(0.56f);
    smallBeacher.set_fractal_octaves(1);
    bigLaker.set_fractal_lacunarity(1.3f); bigLaker.set_fractal_octaves(2);
    smallLaker.set_fractal_octaves(2);

}
FracturedContinentGenerator::~FracturedContinentGenerator(){}

