#include "FracturedContinentGenerator.h"
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <format>
using namespace godot;

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
    
    peninsuler_cutoff = -0.1f; bigLakeCutoff = 0.33f; smallLakeCutoff = 0.25f; beachCutoff = 0.8f, treeCutoff = 4.3f;

    continenter.set_fractal_lacunarity(2.8f); continenter.set_fractal_weighted_strength(0.5f);

    peninsuler.set_fractal_gain(0.56f);

    smallBeacher.set_fractal_octaves(3);

    forest.set_fractal_lacunarity(3); forest.set_fractal_gain(0.77);
}

void FracturedContinentGenerator::generate(
    std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    const MatrixCoords& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionSet, 
    const unsigned int SEED, const Dictionary& data)
{
    this->origin = origin; this->size = size;

    this->tileSelector = std::make_unique<TileSelector>(tileSelectionSet, SEED);

    {
    continenter.set_seed(SEED); peninsuler.set_seed(SEED+1); bigLaker.set_seed(SEED+2); smallLaker.set_seed(SEED+3);
    bigBeacher.set_seed(SEED+4); smallBeacher.set_seed(SEED+5); rng.set_seed(SEED); forest.set_seed(SEED + 9);
    
//hacer un for multiplicativo de la frequency en vez de separar en big y small. aumentar el cutoff. sumarle 1 a la seed en cada iteracion del for
    continenter.set_frequency(0.15f/powf(size.length(), 0.995f)); peninsuler.set_frequency(5.5f/powf(size.length(), 0.995f));
    bigBeacher.set_frequency(4.3f/powf(size.length(), 0.995f)); smallBeacher.set_frequency(8.f/powf(size.length(), 0.995f));
    bigLaker.set_frequency(40.f/powf(size.length(), 0.995f)); smallLaker.set_frequency(80.f/powf(size.length(), 0.995f));
    forest.set_frequency(1.8f/powf(size.length(), 0.995f));
    //TODO hacer cada frequency ajustable desde gdscript,

    continental_cutoff = 0.61f * powf(size.length() / 1600.f, 0.05f);;
    }

    while(continenter.get_noise_2dv(origin+size/(short int)2) < continental_cutoff + 0.13f){//NO METER EL PENINSULER EN ESTA CONDICIÓN, DESCENTRA LA FORMACIÓN
//EN EL CENTRO PUEDE ESTAR PENINSULEADO, HACIENDO Q EL PLAYER SPAWNEE EN EL AGUA SI EL CENTRO TIENE AGUA (EL PLAYER SPAWNEARÍA EN EL CENTRO).
// ASÍ QUE, ANTES DE SPAWNEAR AL PLAYER ELEGIR UN PUNTO RANDOM HASTA Q TENGA TIERRA (COMO HAGO CON LOS DUNGEONS) 
        continenter.set_offset(continenter.get_offset() + Vector3(3,3,0));
    }

    // COMO HACER RIOS: ELEGIR PUNTO RANDOM DE ALTA CONTINENTNESS -> "CAMINAR HACIA LA TILE ADYACENTE CON CONTINENTNESS MAS BAJA" -> HACER HASTA LLEGAR AL AGUA O LAKE
    for (uint16_t i = 0; i < size.i; i++){
    for (uint16_t j = 0; j < size.j; j++)
    {   
        const bool CONTINENTAL = isContinental(i, j);

        const bool PENINSULER_CAVED = isPeninsulerCaved(i, j);

        std::array<std::string, 3> targetsToFill;
        unsigned char addedTargets = 0;

        if (CONTINENTAL && !PENINSULER_CAVED)
        {
            /*
            if(i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
                printf("pc=%d", peninsulerCaved);if (j % 2 == 0) std::cout << "\n"; else std::cout << "||| ";} 
            */
            const float BEACHNESS = getBeachness(i, j);
            const bool BEACH = BEACHNESS > beachCutoff;
            
            if (BEACH) targetsToFill[addedTargets++] = "beach";
            else
            {
                const bool AWAY_FROM_COAST = getContinentness(i, j) > continental_cutoff + 0.01f && peninsuler.get_noise_2d(i, j) > peninsuler_cutoff + 0.27f;
                
                const bool LAKE = isLake(i, j) && AWAY_FROM_COAST;

                if (LAKE) targetsToFill[addedTargets++] = "lake";
                else {
                    targetsToFill[addedTargets++] = "cont";
                    if(!BEACHNESS < beachCutoff - 0.05f ) 
                    {
                        // HAY Q USAR UNA DISCRETE DISTRIBUTION PLANA EN EL MEDIO, MU BAJA PROBABILIDAD EN LOS EXTREMOS
                        const bool GOOD_DICE_ROLL = rng.randf_range(0, 4) + forest.get_noise_2d(i, j) * 1.4f > treeCutoff;
                        const bool LUCKY_TREE = rng.randi_range(0, 1000) == 0;

                        const bool TREE = (LUCKY_TREE || GOOD_DICE_ROLL) && clearOfObjects(i, j, 3);
                        if (TREE)
                        {
                            blockingObjectsCoords.insert(MatrixCoords(i, j));
                            targetsToFill[addedTargets++] = "tree";
                        }
                    }
                }
            }
        } else targetsToFill[addedTargets++] = "ocean";

//shallow ocean: donde continentness está high. deep ocean: donde continentness está low o si se es una empty tile fuera de cualquier generation

        std::array<std::string, 3> collectedTileIds;
        for(unsigned char l = 0; l < addedTargets; l++)
        {
            collectedTileIds[l] = (this->tileSelector->getTileId(targetsToFill[l]));
        }
        
        for(unsigned char l = 0; l < addedTargets; l++){
            FormationGenerator::placeTile(worldMatrix, origin, MatrixCoords(i, j), collectedTileIds[l]);
        }
    }}
    placeDungeonEntrances(worldMatrix, 3);
    blockingObjectsCoords.clear();
}



bool FracturedContinentGenerator::isContinental(uint16_t i, uint16_t j) const
{return getContinentness(i, j) > continental_cutoff;}


float FracturedContinentGenerator::getContinentness(uint16_t i, uint16_t j) const
{   /*
    if (i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
        printf(", cntness %.3f = %.3f - %.3f, ", 
            continenter.get_noise_2d(i, j) - bcf / 4.2f, 
            (float)continenter.get_noise_2d(i, j), 
            bcf / 4.2f);}
    */
    const float BCF = FormationGenerator::getBorderClosenessFactor(i, j, size);

    return continenter.get_noise_2d(i, j) * (1-BCF);
}
float FracturedContinentGenerator::getBeachness(uint16_t i, uint16_t j) const
{
    return std::max(
    0.72f + bigBeacher.get_noise_2d(i, j) / 2.3f - powf(getContinentness(i, j) - continental_cutoff, 0.6f), 
    0.8f + smallBeacher.get_noise_2d(i, j) / 2.3f - powf(peninsuler.get_noise_2d(i, j) - peninsuler_cutoff, 0.45f));
}

bool FracturedContinentGenerator::clearOfObjects(uint16_t i, uint16_t j, uint16_t radius, bool checkForwards) const
{
    for (int x = -radius; x <= checkForwards * radius; x++)
        for (int y = -radius; y <= radius; y++)
            if (blockingObjectsCoords.count(MatrixCoords(i+x, j+y)))
                return false;
    return true;
}

bool FracturedContinentGenerator::isPeninsulerCaved(uint16_t i, uint16_t j) const
{return peninsuler.get_noise_2d(i, j) < peninsuler_cutoff;}

bool FracturedContinentGenerator::isLake(uint16_t i, uint16_t j) const
{
    return (((smallLaker.get_noise_2d(i, j) + 1)*0.65f) - getBeachness(i, j) > smallLakeCutoff) 
        || (((bigLaker.get_noise_2d(i, j) + 1)*0.65f) - getBeachness(i, j) > bigLakeCutoff);
}

// MUST GO AFTER TREES/ROCKS/WHATEVER BLOCKING OBJECTS ARE INSERTED
void FracturedContinentGenerator::placeDungeonEntrances(
    std::vector<std::vector<std::vector<std::string>>> & worldMatrix, const int DUNGEONS_COUNT)
{
   //"cave_i"

    uint16_t ri, rj;
    
    float tries = 0;

    std::array<MatrixCoords, 3> dungeonsCoords;
    unsigned char dungeonsI = 0;

    float minDistanceMult = 1;

    while (dungeonsI < DUNGEONS_COUNT)
    {
        tries++;
        ri = rng.randi_range(0, size.i);
        rj = rng.randi_range(0, size.j);
        const MatrixCoords newDungeonCoords(ri,rj);

        if (getContinentness(ri,rj) > continental_cutoff + 0.005 
        && peninsuler.get_noise_2d(ri,rj) > peninsuler_cutoff + 0.1f 
        && !isLake(ri, rj) && clearOfObjects(ri, rj, 3, true))//TODO PONER BIEN
        {
            bool farFromDungeons = true;

            for (const MatrixCoords& coord : dungeonsCoords)
            {
                if (newDungeonCoords.distanceTo(coord) <  size.length() * 0.25f * minDistanceMult)
                {
                    farFromDungeons = false;
                    break;
                }
            }
            if (farFromDungeons)
            {
                char buffer[9];
                sprintf(buffer, "cave_%d", dungeonsI);
                const std::string TILE_ID = this->tileSelector->getTileId(buffer);
                FormationGenerator::placeTile(worldMatrix, origin, newDungeonCoords, TILE_ID);
                dungeonsCoords[dungeonsI++] = newDungeonCoords;
                //UtilityFunctions::print(newDungeonCoords);
            }
            else{minDistanceMult = std::clamp(1500.f / tries, 0.f, 1.f);}
        }
        if (tries > 1000000){
            UtilityFunctions::printerr("Dungeon placement condition unmeetable! (FracturedContinentGenerator::placeDungeonEntrances())");
            break;
        }
    }
}
float FracturedContinentGenerator::get_continental_cutoff()const{return continental_cutoff;}
void FracturedContinentGenerator::set_continental_cutoff(float cutoff){continental_cutoff = cutoff;} 
void FracturedContinentGenerator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_continental_cutoff", "continental_cutoff"), &FracturedContinentGenerator::set_continental_cutoff);
    ClassDB::bind_method(D_METHOD("get_continental_cutoff"), &FracturedContinentGenerator::get_continental_cutoff);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "continental_cutoff"), "set_continental_cutoff", "get_continental_cutoff");
}
FracturedContinentGenerator::~FracturedContinentGenerator(){}