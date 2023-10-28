#include "FracturedContinentGenerator.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <unordered_set>
#include <algorithm>
#include <string>

using namespace godot;

void FracturedContinentGenerator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_continental_cutoff", "continental_cutoff"), &FracturedContinentGenerator::set_continental_cutoff);
    ClassDB::bind_method(D_METHOD("get_continental_cutoff"), &FracturedContinentGenerator::get_continental_cutoff);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "continental_cutoff"), "set_continental_cutoff", "get_continental_cutoff");
}

float FracturedContinentGenerator::get_continental_cutoff()const{return continental_cutoff;}
void FracturedContinentGenerator::set_continental_cutoff(float cutoff){continental_cutoff = cutoff;} 

void FracturedContinentGenerator::generate(
    std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& size, const TileSelectionSet tileSelectionSet, 
    const signed int seed, const Dictionary& data)
{
    //TODO SNAPPEAR EL ORIGIN AL GRID (CON EL % 4)
    this->origin = origin;
    this->size = size;
    {
    continenter.set_seed(seed); peninsuler.set_seed(seed+1); bigLaker.set_seed(seed+2); smallLaker.set_seed(seed+3);
    bigBeacher.set_seed(seed+4); smallBeacher.set_seed(seed+5); rng.set_seed(seed); forest.set_seed(seed + 9);
//hacer un for multiplicativo de la frequency en vez de separar en big y small. aumentar el cutoff. sumarle 1 a la seed en cada iteracion del for
    continenter.set_frequency(0.15f/powf(size.length(), 0.995f)); peninsuler.set_frequency(5.5f/powf(size.length(), 0.995f));
    bigBeacher.set_frequency(4.3f/powf(size.length(), 0.99f)); smallBeacher.set_frequency(8.f/powf(size.length(), 0.995f));
    bigLaker.set_frequency(37.f/powf(size.length(), 0.991f)); smallLaker.set_frequency(80.f/powf(size.length(), 0.991f));
    forest.set_frequency(3.f/powf(size.length(), 0.991f));

    continental_cutoff = 0.6f * powf(size.length() / 1600.f, 0.05f);;
    }
      
    while(continenter.get_noise_2dv(origin) < continental_cutoff + 0.13f){
        continenter.set_offset(continenter.get_offset() + Vector3(3,3,0));
    }

    for (int i = -size.x/2; i < size.x/2; i+=4){
    for (int j = -size.y/2; j < size.y/2; j+=4)
    {   
        float bcf = getBorderClosenessFactor(i, j);
        float continentness = getContinentness(i, j, bcf);
        bool continental = continentness > continental_cutoff;
        bool peninsulerCaved = peninsuler.get_noise_2d(i, j) < peninsuler_cutoff;
        /*
        if(i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
            printf("pc=%d", peninsulerCaved);if (j % 2 == 0) std::cout << "\n"; else std::cout << "||| ";} 
        */
        bool awayFromCoast = continentness > continental_cutoff + 0.01f && peninsuler.get_noise_2d(i, j) > peninsuler_cutoff + 0.27f;

        float beachness = getBeachness(i, j, continentness);

        bool beach = beachness > beachCutoff;
        
        bool lake = (((smallLaker.get_noise_2d(i, j) + 1)*0.65f) - beachness > smallLakeCutoff) 
                    || (((bigLaker.get_noise_2d(i, j) + 1)*0.65f) - beachness > bigLakeCutoff);
        
        bool tree = false;
        if(continental && !peninsulerCaved && beachness < beachCutoff - 0.03f && !lake)
        {
            bool diceRollSuccessfull = rng.randf_range(0,4) + forest.get_noise_2d(i, j)*1.5f > 3.3f;
            tree = diceRollSuccessfull && i % 4 == 0 && j % 4 == 0;
        } 

        std::unordered_set<std::string> data;

        if (continental) data.insert("continental");
        if (peninsulerCaved) data.insert("peninsuler_caved");
        if (awayFromCoast) data.insert("away_from_coast");
        if (lake) data.insert("lake");
        if (beach) data.insert("beach");
        if (tree) data.insert("tree");

        auto tiles = this->tilePicker.getTiles(tileSelectionSet, data, seed);

        for(auto tileId: tiles){
            FormationGenerator::placeTile(worldMatrix, origin, Vector2i(i, j), tileId);
        }
    }}
}

float FracturedContinentGenerator::getBorderClosenessFactor(int i, int j) const
{   /*
    if (i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
        printf("%d %d", i, j); 
        printf(", bcf %.2f = max(%.2f, %.2f)", 
            std::max(abs(i - size.x) / (size.x/2.f), abs(j - size.y) / (size.y/2.f)), 
            abs(i - size.x) / (size.x/2.f), 
            abs(j - size.y) / (size.y/2.f));}
    */
    return std::max(
        abs(i - origin.x) / (size.x/2.f), 
        abs(j - origin.y) / (size.y/2.f));
}
float FracturedContinentGenerator::getContinentness(int i, int j, float bcf) const
{   /*
    if (i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
        printf(", cntness %.3f = %.3f - %.3f, ", 
            continenter.get_noise_2d(i, j) - bcf / 4.2f, 
            (float)continenter.get_noise_2d(i, j), 
            bcf / 4.2f);}
    */
    return (float)continenter.get_noise_2d(i, j) -  bcf / 4.2f - powf(bcf, 43.f);
}
float FracturedContinentGenerator::getBeachness(int i, int j, float continentness) const
{
    return std::max(
    0.72f + bigBeacher.get_noise_2d(i, j) / 2.3f - powf(continentness - continental_cutoff, 0.6f), 
    0.8f + smallBeacher.get_noise_2d(i, j) / 2.3f - powf(peninsuler.get_noise_2d(i, j) - peninsuler_cutoff, 0.45f));
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
    forest.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX);
    

    peninsuler_cutoff = -0.1f; bigLakeCutoff = 0.2f; smallLakeCutoff = 0.20f; beachCutoff = 0.8f;

    continenter.set_fractal_lacunarity(2.8f); continenter.set_fractal_weighted_strength(0.5f);
    peninsuler.set_fractal_gain(0.56f);

    smallBeacher.set_fractal_octaves(3);

    forest.set_fractal_lacunarity(3);
    forest.set_fractal_gain(0.7);
    //forest.set_fractal_octaves(1);
}
FracturedContinentGenerator::~FracturedContinentGenerator(){}

