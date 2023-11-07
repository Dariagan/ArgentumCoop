#include "FracturedContinentGenerator.h"
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <unordered_set>
#include <algorithm>
#include <string>
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
    
    peninsuler_cutoff = -0.1f; bigLakeCutoff = 0.33f; smallLakeCutoff = 0.25f; beachCutoff = 0.8f;

    continenter.set_fractal_lacunarity(2.8f); continenter.set_fractal_weighted_strength(0.5f);

    peninsuler.set_fractal_gain(0.56f);

    smallBeacher.set_fractal_octaves(3);

    forest.set_fractal_lacunarity(3); forest.set_fractal_gain(0.77);
}

void FracturedContinentGenerator::generate(
    std::vector<std::vector<std::vector<std::string>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& size, const TileSelectionSet tileSelectionSet, 
    const signed int seed, const Dictionary& data)
{
    this->origin = origin; this->size = size;
    {
    continenter.set_seed(seed); peninsuler.set_seed(seed+1); bigLaker.set_seed(seed+2); smallLaker.set_seed(seed+3);
    bigBeacher.set_seed(seed+4); smallBeacher.set_seed(seed+5); rng.set_seed(seed); forest.set_seed(seed + 9);
    
//hacer un for multiplicativo de la frequency en vez de separar en big y small. aumentar el cutoff. sumarle 1 a la seed en cada iteracion del for
    continenter.set_frequency(0.15f/powf(size.length(), 0.995f)); peninsuler.set_frequency(5.5f/powf(size.length(), 0.995f));
    bigBeacher.set_frequency(4.3f/powf(size.length(), 0.995f)); smallBeacher.set_frequency(8.f/powf(size.length(), 0.995f));
    bigLaker.set_frequency(40.f/powf(size.length(), 0.995f)); smallLaker.set_frequency(80.f/powf(size.length(), 0.995f));
    forest.set_frequency(1.8f/powf(size.length(), 0.995f));
    //TODO hacer cada frequency ajustable desde gdscript,

    continental_cutoff = 0.61f * powf(size.length() / 1600.f, 0.05f);;
    }

    while(continenter.get_noise_2dv(origin+size/2) < continental_cutoff + 0.13f){//NO METER EL PENINSULER EN ESTA CONDICIÓN, DESCENTRA LA FORMACIÓN
//EN EL CENTRO PUEDE ESTAR PENINSULEADO, HACIENDO Q EL PLAYER SPAWNEE EN EL AGUA SI EL CENTRO TIENE AGUA (EL PLAYER SPAWNEARÍA EN EL CENTRO).
// ASÍ QUE, ANTES DE SPAWNEAR AL PLAYER ELEGIR UN PUNTO RANDOM HASTA Q TENGA TIERRA (COMO HAGO CON LOS DUNGEONS) 
        continenter.set_offset(continenter.get_offset() + Vector3(3,3,0));
    }
    // COMO HACER RIOS: ELEGIR PUNTO RANDOM DE ALTA CONTINENTNESS -> "CAMINAR HACIA LA TILE ADYACENTE CON CONTINENTNESS MAS BAJA" -> HACER HASTA LLEGAR AL AGUA O LAKE
    for (int i = 0; i < size.x; i++){
    for (int j = 0; j < size.y; j++)
    {   
        bool continental = isContinental(i, j);

        bool peninsulerCaved = isPeninsulerCaved(i, j);

        std::unordered_map<std::string, std::string> data;

        if (continental && !peninsulerCaved)
        {
            data.insert({"continental",""});
            /*
            if(i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
                printf("pc=%d", peninsulerCaved);if (j % 2 == 0) std::cout << "\n"; else std::cout << "||| ";} 
            */
            float beachness = getBeachness(i, j);
            bool beach = beachness > beachCutoff;
            
            if (beach) data.insert({"beach", ""});
            else
            {
                bool awayFromCoast = getContinentness(i, j) > continental_cutoff + 0.01f 
                            && peninsuler.get_noise_2d(i, j) > peninsuler_cutoff + 0.27f;
                
                bool lake = isLake(i, j) && awayFromCoast;

                if (lake) data.insert({"lake", ""});
                else if(!beachness < beachCutoff - 0.05f ) 
                {
                    bool tree = false;

                    // HAY Q USAR UNA DISCRETE DISTRIBUTION PLANA EN EL MEDIO, MU BAJA PROBABILIDAD EN LOS EXTREMOS
                    bool diceRollSuccessfull = rng.randf_range(0, 4) + forest.get_noise_2d(i, j) * 1.4f > 4.18f;
                    bool luckyTree = rng.randi_range(0, 1000) == 0;

                    tree = (luckyTree || diceRollSuccessfull) && clearOfObjects(i, j, 3);
                    if (tree)
                    {
                        blockingObjectsCoords.insert(Vector2i(i, j));
                        data.insert({"tree", ""});
                    }
                }
            }
        }
//shallow ocean: donde continentness está high. deep ocean: donde continentness está low o si se es una empty tile fuera de cualquier generation

        auto tiles = this->tilePicker.getTiles(tileSelectionSet, data, seed);
//TODO hacer el tilepicker local al generator? se basa en un dictionary pasado desde gdscript (que es un recurso), ese dictionary se transpasa a un hashmap
//ese hashmap mapea la tile q hay q poner segun el tipo de formacion (ej: small lake->lava, big lake->evilmarsh, bigbeach->dirt, smallbeach->lunarrock)

        for(auto& tileId: tiles){
            FormationGenerator::placeTile(worldMatrix, origin, Vector2i(i, j), tileId);
        }
    }}
    placeDungeonEntrances(worldMatrix); blockingObjectsCoords.clear();
}

bool FracturedContinentGenerator::isContinental(int i, int j) const
{return getContinentness(i, j) > continental_cutoff;}


float FracturedContinentGenerator::getContinentness(int i, int j) const
{   /*
    if (i > DEBUG_RANGE_MIN && i < DEBUG_RANGE_MAX && j > DEBUG_RANGE_MIN && j < DEBUG_RANGE_MAX){
        printf(", cntness %.3f = %.3f - %.3f, ", 
            continenter.get_noise_2d(i, j) - bcf / 4.2f, 
            (float)continenter.get_noise_2d(i, j), 
            bcf / 4.2f);}
    */
    float bcf = FormationGenerator::getBorderClosenessFactor(i, j, size);

    return continenter.get_noise_2d(i, j) - bcf/4.2f - powf(bcf, 43.f);
}
float FracturedContinentGenerator::getBeachness(int i, int j) const
{
    return std::max(
    0.72f + bigBeacher.get_noise_2d(i, j) / 2.3f - powf(getContinentness(i, j) - continental_cutoff, 0.6f), 
    0.8f + smallBeacher.get_noise_2d(i, j) / 2.3f - powf(peninsuler.get_noise_2d(i, j) - peninsuler_cutoff, 0.45f));
}

bool FracturedContinentGenerator::clearOfObjects(int i, int j, int radius, bool checkForwards) const
{
    for (int x = -radius; x <= checkForwards * radius; x++)
        for (int y = -radius; y <= radius; y++)
            if (blockingObjectsCoords.count(Vector2i(i+x, j+y)))
                return false;
    return true;
}

bool FracturedContinentGenerator::isPeninsulerCaved(int i, int j) const
{return peninsuler.get_noise_2d(i, j) < peninsuler_cutoff;}

bool FracturedContinentGenerator::isLake(int i, int j) const
{
    return (((smallLaker.get_noise_2d(i, j) + 1)*0.65f) - getBeachness(i, j) > smallLakeCutoff) 
        || (((bigLaker.get_noise_2d(i, j) + 1)*0.65f) - getBeachness(i, j) > bigLakeCutoff);
}

// MUST GO AFTER TREES/ROCKS/WHATEVER BLOCKING OBJECTS ARE INSERTED
void FracturedContinentGenerator::placeDungeonEntrances(
    std::vector<std::vector<std::vector<std::string>>> & worldMatrix)
{
    int ri, rj, tries = 0;
    
    std::vector<Vector2i> dungeonsCoords;
    float minDistanceMult = 1;

    while (dungeonsCoords.size() < 3)
    {
        tries++;
        ri = rng.randi_range(0, size.x);
        rj = rng.randi_range(0, size.y);
        const Vector2i newDungeonCoords(ri,rj);

        if (getContinentness(ri,rj) > continental_cutoff + 0.005 
        && peninsuler.get_noise_2d(ri,rj) > peninsuler_cutoff + 0.1f 
        && !isLake(ri, rj) && clearOfObjects(ri, rj, 3, true))//TODO PONER BIEN
        {
            bool farFromDungeons = true;

            for (const Vector2i& coord : dungeonsCoords)
            {
                if (((Vector2)newDungeonCoords).distance_to(coord) <  size.length() * 0.25f * minDistanceMult)
                {
                    farFromDungeons = false;
                    break;
                }
            }
            if (farFromDungeons)
            {
                FormationGenerator::placeTile(worldMatrix, origin, newDungeonCoords, "cave_mossy");
                dungeonsCoords.push_back(newDungeonCoords);
                //UtilityFunctions::print(newDungeonCoords);
            }
            else{minDistanceMult = std::clamp(1500.f / tries, 0.f, 1.f);}
        }
        if (tries == 1000000){
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