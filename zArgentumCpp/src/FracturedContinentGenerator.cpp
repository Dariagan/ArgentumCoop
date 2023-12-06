#include "FracturedContinentGenerator.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <unordered_set>
#include <algorithm>
#include <string>
#include <format>
#include <memory>
#include <thread>


using namespace godot;

FracturedContinentGenerator::FracturedContinentGenerator()
{
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

//! CLEAREAR COLECCIONES QUE SE REUTILIZAN CADA LLAMADA (SINO QUEDAN COMO TERMINARON EN LA EJECUCIÓN DE ANTERIOR, LLENAS DE ELEMENTOS)
void FracturedContinentGenerator::resetState()
{
    m_trees.clear(); m_bushes.clear(); 
}

//definir spawn points para bosses?
//es alpedo definir spawn points para mobs comúnes basados en condiciones super específicas porq merondean igualmente
void FracturedContinentGenerator::generate(
    ArgentumTileMap& argentumTileMap, //HACER Q SEA DE UN SOLO USE ONLY? (PARA PREVENIR BUGS)
    const SafeVec& origin, const SafeVec& size, const Ref<Resource>& tileSelectionSet, 
    const unsigned int SEED, const Dictionary& data)
{
    //TODO posible randomización leve de parámetros
    this->m_origin = origin; this->m_size = size;

    m_tileSelector = std::make_unique<TileSelector>(tileSelectionSet, argentumTileMap, SEED);

    {
    continenter.set_seed(SEED); peninsuler.set_seed(SEED+1); bigLaker.set_seed(SEED+2); smallLaker.set_seed(SEED+3);
    bigBeacher.set_seed(SEED+4); smallBeacher.set_seed(SEED+5); m_rng.set_seed(SEED); forest.set_seed(SEED + 9);
    
    continenter.set_frequency(0.15f/powf(size.length(), 0.995f)); peninsuler.set_frequency(5.f/powf(size.length(), 0.995f));
    bigBeacher.set_frequency(4.3f/powf(size.length(), 0.995f)); smallBeacher.set_frequency(8.f/powf(size.length(), 0.995f));
    bigLaker.set_frequency(40.f/powf(size.length(), 0.995f)); smallLaker.set_frequency(80.f/powf(size.length(), 0.995f));
    forest.set_frequency(1.8f/powf(size.length(), 0.995f));
    //TODO hacer cada frequency ajustable desde gdscript

    continental_cutoff = 0.61f * powf(size.length() / 1600.f, 0.05f);;
    }

    while(continenter.get_noise_2dv(origin+size/(short int)2) < continental_cutoff + 0.13f){//! NO METER EL PENINSULER EN ESTA CONDICIÓN, DESCENTRA LA FORMACIÓN
//EN EL CENTRO PUEDE ESTAR PENINSULEADO, HACIENDO Q EL PLAYER SPAWNEE EN EL AGUA SI EL CENTRO TIENE AGUA (EL PLAYER SPAWNEARÍA EN EL CENTRO).
// ASÍ QUE, PARA SPAWNEAR AL PLAYER ELEGIR UN PUNTO RANDOM HASTA Q TENGA UNA TILE TIERRA (COMO HAGO CON LOS DUNGEONS) 
        continenter.set_offset(continenter.get_offset() + Vector3(3,3,0));
    }

    std::vector<std::unordered_set<SafeVec, SafeVec::hash>> bushes(N_THREADS, std::unordered_set<SafeVec, SafeVec::hash>());
    std::vector<std::unordered_set<SafeVec, SafeVec::hash>> trees(N_THREADS, std::unordered_set<SafeVec, SafeVec::hash>());

    std::vector<std::thread> threads;
    threads.reserve(N_THREADS);

    for(unsigned char i = 0; i < N_THREADS; i++)
    {
        const uint16_t startlef = i*m_size.lef/N_THREADS;
        const uint16_t endlef = (i+1)*m_size.lef/N_THREADS;
        const SafeVec range(startlef, endlef);

        threads.emplace_back(std::thread(&FracturedContinentGenerator::build, this, 
                range, 
                std::ref(argentumTileMap), m_origin, std::ref(bushes[i]), std::ref(trees[i])));
    }
    for(unsigned char i = 0; i < N_THREADS; i++)
    {
        threads[i].join();
        m_bushes.insert(bushes[i].begin(), bushes[i].end());
        m_trees.insert(trees[i].begin(), trees[i].end());
    }  
    //CÓMO HACER RIOS: ELEGIR PUNTO RANDOM DE ALTA CONTINENTNESS -> "CAMINAR HACIA LA TILE ADYACENTE CON CONTINENTNESS MAS BAJA" -> HACER HASTA LLEGAR AL AGUA O LAKE
    
    placeDungeonEntrances(argentumTileMap, 3);

    this->resetState();
}


void godot::FracturedContinentGenerator::build(const SafeVec& rangelef, godot::ArgentumTileMap &argentumTileMap, const godot::SafeVec &origin, 
    std::unordered_set<SafeVec, SafeVec::hash>& myBushes, std::unordered_set<SafeVec, SafeVec::hash>& myTrees)
{
    for (uint16_t x = rangelef.lef; x < rangelef.RIGHT; x++)
    {
        for (uint16_t y = 0; y < m_size.RIGHT; y++)
        {
            const SafeVec coords(x, y);
            //! POTENCIAL BUG: STACK OVERFLOW SI SE ESCRIBE EN UN addedTargetsCount[i] CON i SIENDO MAYOR QUE EL TAMAÑO DEL ARRAY - 1
            std::array<Target, WorldMatrix::MAX_TILES_PER_POS> targetsToFill;
            unsigned char placementsCount = 0;

            const bool CONTINENTAL = isContinental(coords);

            const bool PENINSULER_CAVED = isPeninsulerCaved(coords);

            if (CONTINENTAL && !PENINSULER_CAVED)
            {
                const float BEACHNESS = getBeachness(coords);
                const bool BEACH = BEACHNESS > beachCutoff;

                // un array de chars = un string

                // lo que hace el strncpy es copiar el array de chars "beach" dentro de la posición <addedTargetsCount> del array de arrays de chars "targetsToFill"
                // el string "beach" (un string es un array de chars)
                if (BEACH)
                    targetsToFill[placementsCount++] = Target::beach;
                else
                {
                    const bool AWAY_FROM_COAST = getContinentness(coords) > continental_cutoff + 0.01f && peninsuler.get_noise_2dv(coords) > peninsuler_cutoff + 0.27f;

                    const bool LAKE = isLake(coords) && AWAY_FROM_COAST;

                    if (LAKE)
                        targetsToFill[placementsCount++] = Target::lake;
                    else
                    {
                        targetsToFill[placementsCount++] = Target::cont;
                        if (!BEACHNESS < beachCutoff - 0.05f)
                        {
                            // HAY Q USAR UNA DISCRETE DISTRIBUTION PLANA EN EL MEDIO, MU BAJA PROBABILIDAD EN LOS EXTREMOS
                            const bool GOOD_DICE_ROLL = m_rng.randf_range(0, 4) + forest.get_noise_2dv(coords) * 1.4f > treeCutoff;
                            const bool LUCKY_TREE = m_rng.randi_range(0, 1000) == 0;

                            const bool TREE = (LUCKY_TREE || GOOD_DICE_ROLL) && clearOf(myTrees, coords, 3);
                            if (TREE)
                            {
                                myTrees.insert(coords);
                                targetsToFill[placementsCount++] = Target::tree;
                            }
                            else if (m_rng.randi_range(0, 400) == 0 && clearOf(myBushes, coords, 1))
                            {
                                myBushes.insert(coords);
                                targetsToFill[placementsCount++] = Target::bush;
                            }
                            //! NO ESCRIBIR POR LA DERECHA DEL PROPIO ARRAY
                            //! AL ESCRIBIR EN EL ARRAY targetsToFill,
                            //! PREVENIR QUE: addedTargetsCount >= MAX_POSSIBLE_ENTRIES,
                            //! EN TODOS LOS POSIBLES RECORRIDOS DE EJECUCIÓN
                        }
                    }
                }
            }
            else
            {
                targetsToFill[placementsCount++] = Target::ocean;
            };

            // todo poner los spawnweights con targets, como haces con las tiles

            // shallow ocean: donde continentness está high. deep ocean: donde continentness está low o si se es una empty tile fuera de cualquier generation
            for (unsigned char k = 0; k < std::min(placementsCount, WorldMatrix::MAX_TILES_PER_POS); k++)
            {
                const auto &tileUid = m_tileSelector->getTileUidForTarget(TARGETS[targetsToFill[k]]);
                //std::lock_guard<std::mutex> guard(mtx);
                argentumTileMap.placeFormationTile(origin, coords, tileUid);
            }
        }
    }
}

bool FracturedContinentGenerator::isContinental(SafeVec coords) const
{return getContinentness(coords) > continental_cutoff;}


float FracturedContinentGenerator::getContinentness(SafeVec coords) const
{
    const float BCF = FormationGenerator::getBorderClosenessFactor(coords, m_size);

    return continenter.get_noise_2dv(coords) * (1-BCF);
}
float FracturedContinentGenerator::getBeachness(SafeVec coords) const
{
    return std::max(
    0.72f + bigBeacher.get_noise_2dv(coords) / 2.3f - powf(getContinentness(coords) - continental_cutoff, 0.6f), 
    0.8f + smallBeacher.get_noise_2dv(coords) / 2.3f - powf(peninsuler.get_noise_2dv(coords) - peninsuler_cutoff, 0.45f));
}

bool FracturedContinentGenerator::clearOf(
    const std::unordered_set<SafeVec, SafeVec::hash>& setToCheck, SafeVec coords, uint16_t radius, bool checkForwards) const
{ 
    for (int x = -radius; x <= checkForwards * radius; x++)
        for (int y = -radius; y <= radius; y++){
            if (setToCheck.count(SafeVec(coords.lef+x, coords.RIGHT+y)))
                return false;
        }
    return true;
}

bool FracturedContinentGenerator::isPeninsulerCaved(SafeVec coords) const
{return peninsuler.get_noise_2dv(coords) < peninsuler_cutoff;}

bool FracturedContinentGenerator::isLake(SafeVec coords) const
{
    return (((smallLaker.get_noise_2dv(coords) + 1)*0.65f) - getBeachness(coords) > smallLakeCutoff) 
        || (((bigLaker.get_noise_2dv(coords) + 1)*0.65f) - getBeachness(coords) > bigLakeCutoff);
}

//MUST BE CALLED AFTER TREES/ROCKS/WHATEVER BLOCKING OBJECTS ARE INSERTED
void FracturedContinentGenerator::placeDungeonEntrances(
    godot::ArgentumTileMap& argentumTileMap, unsigned char dungeonsToPlace)
{
    static constexpr int MAX_TRIES = 1'000'000;

    dungeonsToPlace = std::min(dungeonsToPlace, N_CAVES);

    std::vector<SafeVec> placedDungeonsCoords(dungeonsToPlace);

    float minDistanceMultiplier = 1;
    float triesCount = 1;
    for (; placedDungeonsCoords.size() < dungeonsToPlace; triesCount++)
    {
        const SafeVec rCoords(m_rng.randi_range(0, m_size.lef), m_rng.randi_range(0, m_size.RIGHT));

        if (getContinentness(rCoords) > continental_cutoff + 0.005 
        && peninsuler.get_noise_2dv(rCoords) > peninsuler_cutoff + 0.1f 
        && !isLake(rCoords) && clearOf(m_trees, rCoords, 3, true))
        {
            const float minDistanceBetweenDungeons = m_size.length() * 0.25f * minDistanceMultiplier;

            const auto isTooClose = [&](const auto& coord){return rCoords.distanceTo(coord) <  minDistanceBetweenDungeons;};

            if (std::find_if(placedDungeonsCoords.begin(), placedDungeonsCoords.end(), isTooClose) 
                == placedDungeonsCoords.end())
            {
                placedDungeonsCoords.push_back(rCoords);

                const auto& tileUid = m_tileSelector->getTileUidForTarget(TARGETS[Target::cave_0+placedDungeonsCoords.size()-1]);

                argentumTileMap.placeFormationTile(m_origin, rCoords, tileUid);
                UtilityFunctions::print((Vector2i)rCoords);
            }
            else{minDistanceMultiplier = std::clamp(1500.f / triesCount, 0.f, 1.f);}
        }
        if (triesCount > MAX_TRIES){
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