#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__
#include "CppFormationGenerator.hpp"
#include <thread>
namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING
class CppFracturedContinentGenerator : public CppFormationGenerator
{
GDCLASS(CppFracturedContinentGenerator, CppFormationGenerator)
public:
    CppFracturedContinentGenerator();
    ~CppFracturedContinentGenerator();

    FastNoiseLite mContinenter, mPeninsuler, mBigLaker, 
    mSmallLaker, mBigBeacher, mSmallBeacher, mForest;
    double mContinentalCutoff, mPeninsulerCutoff, 
    mBigLakeCutoff, mSmallLakeCutoff, mBeachCutoff, mTreeCutoff;

    double gGetContinentalCutoff() const; void gSetContinentalCutoff(double cutoff);

//en vez de poner optional parameters así, declarar varios métodos overloaded, hacer q el de menos llame al de más, y bindear los dos
    void generate(CppArgentumTileMap& argentumTileMap, 
        const SafeVec& origin, const SafeVec& size, const Ref<Resource>& tileSelectionSet,
        const unsigned int seed = 0, const Dictionary& data = Dictionary()) override;

private:
    SafeVec mOrigin;
    SafeVec mSize;

    RandomNumberGenerator mRng;    
    std::unordered_set<SafeVec, SafeVec::hash> mTrees, mBushes;
    void generateSubSection(const SafeVec& rangelef, CppArgentumTileMap &argentumTileMap,
        const SafeVec& origin, std::unordered_set<SafeVec, SafeVec::hash>& myBushes,
        std::unordered_set<SafeVec, SafeVec::hash>& myTrees, const std::int_fast8_t thread_i);

    inline bool clearOf(const std::unordered_set<SafeVec, SafeVec::hash>& setToCheck, 
        SafeVec coords, std::uint_fast16_t radius, bool checkForwards = false) const;
    inline bool isPeninsulerCaved(SafeVec coords) const;
    inline bool isLake(SafeVec coords) const;
    inline bool isContinental(SafeVec coords) const;
    inline double getContinentness(SafeVec coords) const;
    inline double getBeachness(SafeVec coords) const;
    void placeDungeonEntrances(CppArgentumTileMap& argentumTileMap, const std::uint_fast8_t nDungeonsToPlace);
    inline void resetState();                                        //add to the left of cave
    enum Target { beach,  lake,  cont,  tree,  bush,  ocean,  cave_0,  cave_1,  cave_2, N_TARGETS}; 
    static constexpr std::array<const char*, N_TARGETS> TARGETS={"beach","lake","cont","tree","bush","ocean","cave_0","cave_1","cave_2"};//DON'T FORGET TO ADD ANY MISSING ENUM LITERALS
        //en vez de esto, hacer que sea un array inicializado de uids, después pasarselo a algún lugar que lo rellene.
        //desp el for-for simplemente usa el uid que haya sido rellenado en la correspondiente i (Target enum val)

    static constexpr std::uint_fast8_t N_CAVES = Target::N_TARGETS - Target::cave_0;   
    
    //TIENE QUE SER UN NÚMERO FIJO PARA QUE NO HAYA DESYNCS DE GENERACIÓN ALEATORIA ENTRE PCS POR TENER UN DISTINTO Nº DE THREADS
    //sino se generan distinto los trees y bushes
    static constexpr std::int_fast8_t N_THREADS = 30;//no poner demasiado porq se van a notar las líneas de no-arboles

protected: static void _bind_methods();

};//ENDCLASS
}


#endif // __FRACTUREDCONTINENT_GENERATOR_H__