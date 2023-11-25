#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "FormationGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING
class FracturedContinentGenerator : public FormationGenerator
{
    GDCLASS(FracturedContinentGenerator, FormationGenerator)

    private:
        SafeVec m_origin;
        MatrixCoords m_size;

        static constexpr short int DEBUG_RANGE_MAX = 50, DEBUG_RANGE_MIN = -DEBUG_RANGE_MAX; 

        RandomNumberGenerator m_rng;    
        std::unordered_set<MatrixCoords, MatrixCoords::hash> m_trees, m_bushes;
        bool clearOf(const std::unordered_set<MatrixCoords, MatrixCoords::hash>& setToCheck, MatrixCoords coords, uint16_t radius, bool checkForwards = false) const;
        bool isPeninsulerCaved(MatrixCoords coords) const;
        bool isLake(MatrixCoords coords) const;
        bool isContinental(MatrixCoords coords) const;
        float getContinentness(MatrixCoords coords) const;
        float getBeachness(MatrixCoords coords) const;
        void placeDungeonEntrances(ArgentumTileMap& argentumTileMap, const unsigned char DUNGEONS_TO_PLACE);
            
    protected:
        static void _bind_methods();

    public:
        FracturedContinentGenerator();
        ~FracturedContinentGenerator();

        FastNoiseLite continenter, peninsuler, bigLaker, smallLaker, bigBeacher, smallBeacher, forest;
        float continental_cutoff, peninsuler_cutoff, bigLakeCutoff, smallLakeCutoff, beachCutoff, treeCutoff;

        float get_continental_cutoff() const; void set_continental_cutoff(float cutoff);

//en vez de poner optional parameters así, declarar varios métodos overloaded, hacer q el de menos llame al de más, y bindear los dos
        void generate(ArgentumTileMap& argentumTileMap, 
            const SafeVec& origin, const MatrixCoords& size, const Ref<Resource>& tileSelectionSet,
            const unsigned int seed = 0, const Dictionary& data = Dictionary()) override;
};
}

#endif // __FRACTUREDCONTINENT_GENERATOR_H__