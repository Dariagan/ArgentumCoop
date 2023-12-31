#include "FracturedContinentGenerator.h"

using namespace godot;

FracturedContinentGenerator::FracturedContinentGenerator()
{
    mContinenter.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX);

    mPeninsuler.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX);
    
    mBigLaker.set_noise_type(FastNoiseLite::NoiseType::TYPE_VALUE_CUBIC);
    mSmallLaker.set_noise_type(FastNoiseLite::NoiseType::TYPE_VALUE_CUBIC);
    mBigBeacher.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX_SMOOTH);
    mSmallBeacher.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX_SMOOTH);
    mForest.set_noise_type(FastNoiseLite::NoiseType::TYPE_SIMPLEX);
    
    mPeninsulerCutoff = -0.1f; mBigLakeCutoff = 0.33f; mSmallLakeCutoff = 0.25f; mBeachCutoff = 0.8f, mTreeCutoff = 4.3f;

    mContinenter.set_fractal_lacunarity(2.8f); mContinenter.set_fractal_weighted_strength(0.5f);

    mPeninsuler.set_fractal_gain(0.56f);

    mSmallBeacher.set_fractal_octaves(3);

    mForest.set_fractal_lacunarity(3); mForest.set_fractal_gain(0.77);

}

//! CLEAREAR COLECCIONES QUE SE REUTILIZAN CADA LLAMADA (SINO QUEDAN COMO TERMINARON EN LA EJECUCIÓN DE ANTERIOR, LLENAS DE ELEMENTOS)
void FracturedContinentGenerator::resetState()
{
    mTrees.clear(); mBushes.clear(); 
}

//definir spawn points para bosses?
//es alpedo definir spawn points para mobs comúnes basados en condiciones super específicas porq merondean igualmente
void FracturedContinentGenerator::generate(
    ArgentumTileMap& argentumTileMap,
    const SafeVec origin, const SafeVec size, 
    const Ref<Resource>& tileSelectionSet, 
    const unsigned int seed, const Dictionary& data)
{
    //TODO posible randomización leve de parámetros
    this->mOrigin = origin; this->mSize = size;

    mTileSelector = std::make_unique<TileSelector>(tileSelectionSet, argentumTileMap, seed, N_THREADS);
    {
    mContinenter.set_seed(seed); mPeninsuler.set_seed(seed+1); mBigLaker.set_seed(seed+2); mSmallLaker.set_seed(seed+3);
    mBigBeacher.set_seed(seed+4); mSmallBeacher.set_seed(seed+5); mRng.set_seed(seed); mForest.set_seed(seed + 9);
    
    mContinenter.set_frequency(0.15f/powf(size.length(), 0.995f)); mPeninsuler.set_frequency(5.f/powf(size.length(), 0.995f));
    mBigBeacher.set_frequency(4.3f/powf(size.length(), 0.995f)); mSmallBeacher.set_frequency(8.f/powf(size.length(), 0.995f));
    mBigLaker.set_frequency(40.f/powf(size.length(), 0.995f)); mSmallLaker.set_frequency(80.f/powf(size.length(), 0.995f));
    mForest.set_frequency(1.6f/powf(size.length(), 0.995f));
    //TODO hacer cada frequency ajustable desde gdscript

    mContinentalCutoff = 0.61f * powf(size.length() / 1600.f, 0.05f);;
    }

    while(mContinenter.get_noise_2dv(origin+size/(int)2) < mContinentalCutoff + 0.13f){//! NO METER EL PENINSULER EN ESTA CONDICIÓN, DESCENTRA LA FORMACIÓN
//EN EL CENTRO PUEDE ESTAR PENINSULEADO, HACIENDO Q EL PLAYER SPAWNEE EN EL AGUA SI EL CENTRO TIENE AGUA (EL PLAYER SPAWNEARÍA EN EL CENTRO).
// ASÍ QUE, PARA SPAWNEAR AL PLAYER ELEGIR UN PUNTO RANDOM HASTA Q TENGA UNA TILE TIERRA (COMO HAGO CON LOS DUNGEONS) 
        mContinenter.set_offset(mContinenter.get_offset() + Vector3(3,3,0));
    }
    std::vector<std::unordered_set<SafeVec, SafeVec::hash>> bushesOfThread(
        N_THREADS, std::unordered_set<SafeVec, SafeVec::hash>());

    std::vector<std::unordered_set<SafeVec, SafeVec::hash>> treesOfThread(
        N_THREADS, std::unordered_set<SafeVec, SafeVec::hash>());

    std::vector<std::thread> threads;
    threads.reserve(N_THREADS);

    for(char thread_i = 0; thread_i < N_THREADS; thread_i++)
    {
        const uint16_t startlef = thread_i*mSize.lef/N_THREADS;
        const uint16_t endlef = (thread_i+1)*mSize.lef/N_THREADS;
        const SafeVec horizontalRange(startlef, endlef);

        threads.emplace_back(std::thread(
            &FracturedContinentGenerator::generateSubSection, this, horizontalRange, 
            std::ref(argentumTileMap), mOrigin, std::ref(bushesOfThread[thread_i]), 
            std::ref(treesOfThread[thread_i]), thread_i));
    }

    std::for_each(std::execution::par_unseq, threads.begin(), threads.end(), 
        [&](std::thread& thread) {
            const char thread_i = &thread - &threads[0];    
            thread.join(); 
            mBushes.insert(bushesOfThread[thread_i].begin(), bushesOfThread[thread_i].end());
            mTrees.insert(treesOfThread[thread_i].begin(), treesOfThread[thread_i].end());
        });
//CÓMO HACER RIOS: ELEGIR PUNTO RANDOM DE ALTA CONTINENTNESS -> "CAMINAR HACIA LA TILE ADYACENTE CON CONTINENTNESS MAS BAJA" -> HACER HASTA LLEGAR AL AGUA O LAKE
    
    placeDungeonEntrances(argentumTileMap, 3);

    this->resetState();
}

void godot::FracturedContinentGenerator::generateSubSection(
    const SafeVec horizontalRange, godot::ArgentumTileMap &argentumTileMap, 
    const SafeVec origin, std::unordered_set<SafeVec, SafeVec::hash>& myBushes, 
    std::unordered_set<SafeVec, SafeVec::hash>& myTrees, const char thread_i)
{
    for (uint16_t x = horizontalRange.lef; x < horizontalRange.RIGHT; x++)
    for (uint16_t y = 0; y < mSize.RIGHT; y++){
        const SafeVec coords(x, y);

        std::array<Target, WorldMatrix::MAX_TILES_PER_POS> targetsToFill;
        u_char placementsCount = 0;

        const bool CONTINENTAL = isContinental(coords);

        const bool PENINSULER_CAVED = isPeninsulerCaved(coords);

        if (CONTINENTAL && !PENINSULER_CAVED)
        {
            const float BEACHNESS = getBeachness(coords);
            const bool BEACH = BEACHNESS > mBeachCutoff;

            if (BEACH)
                targetsToFill[placementsCount++] = Target::beach;
            else
            {
                const bool AWAY_FROM_COAST =
                    getContinentness(coords) > mContinentalCutoff + 0.01f 
                    && mPeninsuler.get_noise_2dv(coords) > mPeninsulerCutoff + 0.27f;

                const bool LAKE = isLake(coords) && AWAY_FROM_COAST;

                if (LAKE)
                    targetsToFill[placementsCount++] = Target::lake;
                else
                {
                    targetsToFill[placementsCount++] = Target::cont;
                    if (!BEACHNESS < mBeachCutoff - 0.05f)
                    {
                        const bool GOOD_DICE_ROLL = 
                            mRng.randf_range(0, 4) + mForest.get_noise_2dv(coords)*1.4f 
                            > mTreeCutoff;

                        const bool LUCKY_TREE = mRng.randi_range(0, 1000) == 0;
                        const bool NOT_ON_HORI_RANGE_LIMIT = (x != horizontalRange.RIGHT - 1);
                        const bool TREE = (LUCKY_TREE || GOOD_DICE_ROLL) 
                            && clearOf(myTrees, coords, 3) && NOT_ON_HORI_RANGE_LIMIT;
                        if (TREE)
                        {
                            myTrees.insert(coords);
                            targetsToFill[placementsCount++] = Target::tree;
                        }
                        else if (mRng.randi_range(0, 400) == 0 
                            && clearOf(myBushes, coords, 1) 
                            && NOT_ON_HORI_RANGE_LIMIT)
                        {
                            myBushes.insert(coords);
                            targetsToFill[placementsCount++] = Target::bush;
                        }
                    }
                }
            }
        }else{targetsToFill[placementsCount++] = Target::ocean;}
        
        argentumTileMap.mBeingsModule->placeNaturalSpawningWeight(origin, coords, "asd", 10);

// shallow ocean: donde continentness está high. deep ocean: donde continentness está low o si se es una empty tile fuera de cualquier generation
        for (u_char k = 0; k < std::min(placementsCount, WorldMatrix::MAX_TILES_PER_POS); k++)
        {
            const auto &tileUid = mTileSelector->
                getTileUidForTarget(TARGETS[targetsToFill[k]], thread_i);
            argentumTileMap.placeFormationTile(origin, coords, tileUid);
        }
    }
}

bool FracturedContinentGenerator::isContinental(SafeVec coords) const
{return getContinentness(coords) > mContinentalCutoff;}


float FracturedContinentGenerator::getContinentness(SafeVec coords) const
{
    const float BCF = FormationGenerator::getBorderClosenessFactor(coords, mSize);

    return mContinenter.get_noise_2dv(coords) * (1-BCF);
}
float FracturedContinentGenerator::getBeachness(SafeVec coords) const
{
    return std::max(
    0.72f + mBigBeacher.get_noise_2dv(coords) / 2.3f - powf(getContinentness(coords) - mContinentalCutoff, 0.6f), 
    0.8f + mSmallBeacher.get_noise_2dv(coords) / 2.3f - powf(mPeninsuler.get_noise_2dv(coords) - mPeninsulerCutoff, 0.45f));
}

bool FracturedContinentGenerator::clearOf(
    const std::unordered_set<SafeVec, SafeVec::hash>& setToCheck, 
    SafeVec coords, uint16_t radius, bool checkForwards) const
{ 
    for (int x = -radius; x <= checkForwards * radius; x++)
        for (int y = -radius; y <= radius; y++){
            if (setToCheck.count(SafeVec(coords.lef+x, coords.RIGHT+y)))
                return false;
        }
    return true;
}

bool FracturedContinentGenerator::isPeninsulerCaved(SafeVec coords) const
{return mPeninsuler.get_noise_2dv(coords) < mPeninsulerCutoff;}

bool FracturedContinentGenerator::isLake(SafeVec coords) const
{
    return (((mSmallLaker.get_noise_2dv(coords) + 1)*0.65f) - getBeachness(coords) > mSmallLakeCutoff) 
        || (((mBigLaker.get_noise_2dv(coords) + 1)*0.65f) - getBeachness(coords) > mBigLakeCutoff);
}

//MUST BE CALLED AFTER TREES/ROCKS/WHATEVER BLOCKING OBJECTS ARE INSERTED
void FracturedContinentGenerator::placeDungeonEntrances(
    godot::ArgentumTileMap& argentumTileMap, u_char dungeonsToPlace)
{
    static constexpr int MAX_TRIES = 1'000'000;

    dungeonsToPlace = std::min(dungeonsToPlace, N_CAVES);

    std::vector<SafeVec> placedDungeonsCoords(dungeonsToPlace);

    float minDistanceMultiplier = 1;
    float triesCount = 1;
    for (; placedDungeonsCoords.size() < dungeonsToPlace; triesCount++)
    {
        const SafeVec rCoords(mRng.randi_range(0, mSize.lef), mRng.randi_range(0, mSize.RIGHT));

        if (getContinentness(rCoords) > mContinentalCutoff + 0.005 
        && mPeninsuler.get_noise_2dv(rCoords) > mPeninsulerCutoff + 0.1f 
        && !isLake(rCoords) && clearOf(mTrees, rCoords, 3, true))
        {
            const float minDistanceBetweenDungeons = mSize.length() * 0.25f * minDistanceMultiplier;

            const auto isTooClose = [&](const auto& coord){return rCoords.distanceTo(coord) <  minDistanceBetweenDungeons;};

            if (std::find_if(placedDungeonsCoords.begin(), placedDungeonsCoords.end(), isTooClose) 
                == placedDungeonsCoords.end())
            {
                placedDungeonsCoords.push_back(rCoords);

                const auto& tileUid = mTileSelector->getTileUidForTarget(TARGETS[Target::cave_0+placedDungeonsCoords.size()-1], 0);

                argentumTileMap.placeFormationTile(mOrigin, rCoords, tileUid);
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
float FracturedContinentGenerator::get_continental_cutoff()const{return mContinentalCutoff;}
void FracturedContinentGenerator::set_continental_cutoff(float cutoff){mContinentalCutoff = cutoff;} 
void FracturedContinentGenerator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_continental_cutoff", "continental_cutoff"), &FracturedContinentGenerator::set_continental_cutoff);
    ClassDB::bind_method(D_METHOD("get_continental_cutoff"), &FracturedContinentGenerator::get_continental_cutoff);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "continental_cutoff"), "set_continental_cutoff", "get_continental_cutoff");
}
FracturedContinentGenerator::~FracturedContinentGenerator(){}