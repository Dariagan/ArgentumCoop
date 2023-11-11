#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

namespace godot 
{
class FracturedContinentGenerator : public FormationGenerator
{
    GDCLASS(FracturedContinentGenerator, FormationGenerator)

    private:
        SafeVec origin;
        MatrixCoords size;

        static constexpr short int DEBUG_RANGE_MAX = 50, DEBUG_RANGE_MIN = -DEBUG_RANGE_MAX; 

        RandomNumberGenerator rng;    
        std::unordered_set<MatrixCoords, MatrixCoords::hash> blockingObjectsCoords;
        bool clearOfObjects(MatrixCoords coords, uint16_t radius = 3, bool checkForward = false) const;
        bool isPeninsulerCaved(MatrixCoords coords) const;
        bool isLake(MatrixCoords coords) const;
        bool isContinental(MatrixCoords coords) const;
        float getContinentness(MatrixCoords coords) const;
        float getBeachness(MatrixCoords coords) const;
        void placeDungeonEntrances(std::vector<std::vector<std::vector<std::array<char, 32>>>> & worldMatrix, const int DUNGEON_COUNT);
            
    protected:
        static void _bind_methods();

    public:
        FracturedContinentGenerator();
        ~FracturedContinentGenerator();

        FastNoiseLite continenter, peninsuler, bigLaker, smallLaker, bigBeacher, smallBeacher, forest;
        float continental_cutoff, peninsuler_cutoff, bigLakeCutoff, smallLakeCutoff, beachCutoff, treeCutoff;

        float get_continental_cutoff() const; void set_continental_cutoff(float cutoff);

//en vez de poner optional parameters así, declarar varios métodos overloaded, hacer q el de menos llame al de más, y bindear los dos
        void generate(std::vector<std::vector<std::vector<std::array<char, 32>>>> & worldMatrix, 
            const SafeVec& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionSet,
            const unsigned int seed = 0, const Dictionary& data = Dictionary()) override;
};
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__