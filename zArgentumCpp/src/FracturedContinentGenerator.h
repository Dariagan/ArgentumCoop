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

    struct pair_hash {
            inline std::size_t operator()(const Vector2i & v) const {
                return v.x*31+v.y;
            }
        };

    private:
        Vector2i origin, size;

        int DEBUG_RANGE_MIN, DEBUG_RANGE_MAX; 

        RandomNumberGenerator rng;    
        std::unordered_set<Vector2i, pair_hash> treesCoords;
        bool clearOfTrees(int i, int j, int radius = 3, bool checkForward = false) const;
        bool isPeninsulerCaved(int i, int j) const;
        bool isLake(int i, int j) const;
        bool isContinental(int i, int j) const;
        float getBorderClosenessFactor(int i, int j) const;
        float getContinentness(int i, int j) const;
        float getBeachness(int i, int j) const;
        void placeDungeonEntrances(std::vector<std::vector<std::vector<std::string>>> & worldMatrix);
            
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
            const Vector2i& origin, const Vector2i& size, const TileSelectionSet tileSelectionSet = TEMPERATE,
            const signed int seed = 0, const Dictionary& data = Dictionary()) override;
};
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__