#include "CppFracturedContinentGenerator.hpp"

using namespace godot;

CppFracturedContinentGenerator::CppFracturedContinentGenerator()
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
void CppFracturedContinentGenerator::resetState()
{
    mTrees.clear(); mBushes.clear(); 
}

//definir spawn points para bosses?
//es alpedo definir spawn points para mobs comúnes basados en condiciones super específicas porq merondean igualmente
void CppFracturedContinentGenerator::generate(
    CppArgentumTileMap& argentumTileMap,
    const SafeVec& origin, const SafeVec& size, 
    const Ref<Resource>& tileSelectionSet, 
    const unsigned int seed, const Dictionary& data)
{
    //TODO posible randomización leve de parámetros
    this->mOrigin = origin; this->mSize = size;
    mTileSelector = std::make_unique<TileSelector>(tileSelectionSet, argentumTileMap, seed, N_THREADS);
    
    {//configuration of noises 
        mContinenter.set_seed(seed); mPeninsuler.set_seed(seed+1); mBigLaker.set_seed(seed+2); mSmallLaker.set_seed(seed+3);
        mBigBeacher.set_seed(seed+4); mSmallBeacher.set_seed(seed+5); mRng.set_seed(seed); mForest.set_seed(seed + 9);
        
        mContinenter.set_frequency(0.15f/pow(size.length(), 0.995)); mPeninsuler.set_frequency(5./pow(size.length(), 0.995));
        mBigBeacher.set_frequency(4.3f/pow(size.length(), 0.995)); mSmallBeacher.set_frequency(8./pow(size.length(), 0.995));
        mBigLaker.set_frequency(40./pow(size.length(), 0.995)); mSmallLaker.set_frequency(80./pow(size.length(), 0.995));
        mForest.set_frequency(1.6/pow(size.length(), 0.995));
        //TODO hacer cada frequency ajustable desde gdscript

        mContinentalCutoff = 0.61 * pow(size.length() / 1600., 0.05);;
    }

    while(mContinenter.get_noise_2dv(origin+size/(int)2) < mContinentalCutoff + 0.13){//! NO METER EL PENINSULER EN ESTA CONDICIÓN, DESCENTRA LA FORMACIÓN
//EN EL CENTRO PUEDE ESTAR PENINSULEADO, HACIENDO Q EL PLAYER SPAWNEE EN EL AGUA SI EL CENTRO TIENE AGUA (EL PLAYER SPAWNEARÍA EN EL CENTRO).
// ASÍ QUE, PARA SPAWNEAR AL PLAYER ELEGIR UN PUNTO RANDOM HASTA Q TENGA UNA TILE TIERRA (COMO HAGO CON LOS DUNGEONS) 
        mContinenter.set_offset(mContinenter.get_offset() + Vector3(3,3,0));
    }
    std::array<std::unordered_set<SafeVec, SafeVec::hash>, N_THREADS> bushesOfThread;

    std::array<std::unordered_set<SafeVec, SafeVec::hash>, N_THREADS> treesOfThread;

    std::array<std::thread, N_THREADS> threads;

    for(std::int_fast8_t thread_i = 0; thread_i < N_THREADS; thread_i++)
    {
        const std::uint_fast32_t startlef = thread_i*mSize.lef/N_THREADS;
        const std::uint_fast32_t endlef = (thread_i+1)*mSize.lef/N_THREADS;
        const SafeVec horizontalRange(startlef, endlef);

        threads[thread_i] = std::thread(
            &CppFracturedContinentGenerator::generateSubSection, this, horizontalRange, 
            std::ref(argentumTileMap), mOrigin, std::ref(bushesOfThread[thread_i]), 
            std::ref(treesOfThread[thread_i]), thread_i);
    }

    std::for_each(std::execution::par_unseq, threads.begin(), threads.end(), 
        [&](std::thread& thread) {
            const std::int_fast8_t thread_i = &thread - &threads[0];    
            thread.join(); 
            mBushes.insert(bushesOfThread[thread_i].begin(), bushesOfThread[thread_i].end());
            mTrees.insert(treesOfThread[thread_i].begin(), treesOfThread[thread_i].end());
        });
//CÓMO HACER RIOS: ELEGIR PUNTO RANDOM DE ALTA CONTINENTNESS -> "CAMINAR HACIA LA TILE ADYACENTE CON CONTINENTNESS MAS BAJA" -> HACER HASTA LLEGAR AL AGUA O LAKE
    
    placeDungeonEntrances(argentumTileMap, 3);

    this->resetState();
}

void godot::CppFracturedContinentGenerator::generateSubSection(
    const SafeVec& horizontalRange, godot::CppArgentumTileMap &argentumTileMap, 
    const SafeVec& origin, std::unordered_set<SafeVec, SafeVec::hash>& myBushes, 
    std::unordered_set<SafeVec, SafeVec::hash>& myTrees, const std::int_fast8_t thread_i)
{
    for (std::uint_fast16_t x = horizontalRange.lef; x < horizontalRange.RIGHT; x++)
    for (std::uint_fast16_t y = 0; y < mSize.RIGHT; y++){
        const SafeVec coords(x, y);

        std::array<Target, WorldMatrix::MAX_TILES_PER_POS> targetsToFill;
        std::uint_fast8_t placementsCount = 0;

        const bool CONTINENTAL = isContinental(coords);

        const bool PENINSULER_CAVED = isPeninsulerCaved(coords);

        if (CONTINENTAL && !PENINSULER_CAVED)
        {
            const double BEACHNESS = getBeachness(coords);
            const bool BEACH = BEACHNESS > mBeachCutoff;

            if (BEACH)
                targetsToFill[placementsCount++] = Target::beach;
            else
            {
                const bool AWAY_FROM_COAST =
                    getContinentness(coords) > mContinentalCutoff + 0.01 
                    && mPeninsuler.get_noise_2dv(coords) > mPeninsulerCutoff + 0.27;

                const bool LAKE = isLake(coords) && AWAY_FROM_COAST;

                if (LAKE)
                    targetsToFill[placementsCount++] = Target::lake;
                else
                {
                    targetsToFill[placementsCount++] = Target::cont;
                    if (!BEACHNESS < mBeachCutoff - 0.05)
                    {
                        const bool kGoodDiceRoll = 
                            mRng.randf_range(0, 4) + mForest.get_noise_2dv(coords)*1.4 
                            > mTreeCutoff;

                        const bool kLuckyTree = mRng.randi_range(0, 1000) == 0;
                        const bool kNotOnHoriRangeLimit = (x != horizontalRange.RIGHT - 1);
                        const bool kTree = (kLuckyTree || kGoodDiceRoll) 
                            && clearOf(myTrees, coords, 3) && kNotOnHoriRangeLimit;
                        if (kTree)
                        {
                            myTrees.insert(coords);
                            targetsToFill[placementsCount++] = Target::tree;
                        }
                        else if (mRng.randi_range(0, 400) == 0 
                            && clearOf(myBushes, coords, 1) 
                            && kNotOnHoriRangeLimit)
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
        for (std::uint_fast8_t k = 0; k < std::min(placementsCount, WorldMatrix::MAX_TILES_PER_POS); k++)
        {
            const auto &tileUid = mTileSelector->getTileUidForTarget(TARGETS[targetsToFill[k]], thread_i);
            argentumTileMap.placeFormationTile(origin, coords, tileUid);
        }
    }
}

bool CppFracturedContinentGenerator::isContinental(SafeVec coords) const
{return getContinentness(coords) > mContinentalCutoff;}


double CppFracturedContinentGenerator::getContinentness(SafeVec coords) const
{
    const double BCF = CppFormationGenerator::getBorderClosenessFactor(coords, mSize);

    return mContinenter.get_noise_2dv(coords) * (1-BCF);
}
double CppFracturedContinentGenerator::getBeachness(SafeVec coords) const
{
    return std::max(
    0.72f + mBigBeacher.get_noise_2dv(coords) / 2.3 - pow(getContinentness(coords) - mContinentalCutoff, 0.6), 
    0.8f + mSmallBeacher.get_noise_2dv(coords) / 2.3 - pow(mPeninsuler.get_noise_2dv(coords) - mPeninsulerCutoff, 0.45));
}

bool CppFracturedContinentGenerator::clearOf(
    const std::unordered_set<SafeVec, SafeVec::hash>& setToCheck, 
    SafeVec coords, std::uint_fast16_t radius, bool checkForwards) const
{ 
    for (int x = -radius; x <= checkForwards * radius; x++)
        for (int y = -radius; y <= radius; y++){
            if (setToCheck.count(SafeVec(coords.lef+x, coords.RIGHT+y)))
                return false;
        }
    return true;
}

bool CppFracturedContinentGenerator::isPeninsulerCaved(SafeVec coords) const
{return mPeninsuler.get_noise_2dv(coords) < mPeninsulerCutoff;}

bool CppFracturedContinentGenerator::isLake(SafeVec coords) const
{
    return (((mSmallLaker.get_noise_2dv(coords) + 1)*0.65) - getBeachness(coords) > mSmallLakeCutoff) 
        || (((mBigLaker.get_noise_2dv(coords) + 1)*0.65) - getBeachness(coords) > mBigLakeCutoff);
}

//MUST BE CALLED AFTER TREES/ROCKS/WHATEVER BLOCKING OBJECTS ARE INSERTED
void CppFracturedContinentGenerator::placeDungeonEntrances(
    godot::CppArgentumTileMap& argentumTileMap, std::uint_fast8_t dungeonsToPlace)
{
    static constexpr int MAX_TRIES = 1'000'000;

    dungeonsToPlace = std::min(dungeonsToPlace, N_CAVES);

    std::vector<SafeVec> placedDungeonsCoords(dungeonsToPlace);

    double minDistanceMultiplier = 1;
    unsigned long triesCount = 1;
    for (; placedDungeonsCoords.size() < dungeonsToPlace; triesCount++)
    {
        const SafeVec rCoords(mRng.randi_range(0, mSize.lef), mRng.randi_range(0, mSize.RIGHT));

        if (getContinentness(rCoords) > mContinentalCutoff + 0.005 
        && mPeninsuler.get_noise_2dv(rCoords) > mPeninsulerCutoff + 0.1f
        && !isLake(rCoords) && clearOf(mTrees, rCoords, 3, true))
        {
            const double minDistanceBetweenDungeons = mSize.length() * 0.25 * minDistanceMultiplier;

            const auto isTooClose = [&](const auto& coord){return rCoords.distanceTo(coord) <  minDistanceBetweenDungeons;};

            if (std::find_if(placedDungeonsCoords.begin(), placedDungeonsCoords.end(), isTooClose) 
                == placedDungeonsCoords.end())
            {
                placedDungeonsCoords.push_back(rCoords);

                const auto& tileUid = mTileSelector->getTileUidForTarget(TARGETS[Target::cave_0+placedDungeonsCoords.size()-1], 0);

                argentumTileMap.placeFormationTile(mOrigin, rCoords, tileUid);
                UtilityFunctions::print((Vector2i)rCoords);
            }
            else{minDistanceMultiplier = std::clamp(1500. / triesCount, 0., 1.);}
        }
        if (triesCount > MAX_TRIES){
            UtilityFunctions::printerr("Dungeon placement condition unmeetable! (CppFracturedContinentGenerator::placeDungeonEntrances())");
            break;
        }
    }
}
double CppFracturedContinentGenerator::gGetContinentalCutoff()const{return mContinentalCutoff;}
void CppFracturedContinentGenerator::gSetContinentalCutoff(double cutoff){mContinentalCutoff = cutoff;} 
void CppFracturedContinentGenerator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_continental_cutoff", "continental_cutoff"), &CppFracturedContinentGenerator::gSetContinentalCutoff);
    ClassDB::bind_method(D_METHOD("get_continental_cutoff"), &CppFracturedContinentGenerator::gGetContinentalCutoff);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "continental_cutoff"), "set_continental_cutoff", "get_continental_cutoff");
}
CppFracturedContinentGenerator::~CppFracturedContinentGenerator(){}