#ifndef __FRACTUREDCONTINENT_GENERATOR_H__
#define __FRACTUREDCONTINENT_GENERATOR_H__

#include "WorldMatrix.cpp"
#include "FormationGenerator.h"



namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING
class FracturedContinentGenerator : public FormationGenerator
{
    GDCLASS(FracturedContinentGenerator, FormationGenerator)

    private:
        SafeVec mOrigin;
        SafeVec mSize;

        RandomNumberGenerator mRng;    
        std::unordered_set<SafeVec, SafeVec::hash> mTrees, mBushes;
        void generateSubSection(const SafeVec& rangelef, godot::ArgentumTileMap &argentumTileMap, const godot::SafeVec &origin, 
            std::unordered_set<SafeVec, SafeVec::hash>& myBushes, std::unordered_set<SafeVec, SafeVec::hash>& myTrees, const char thread_i);

        bool clearOf(const std::unordered_set<SafeVec, SafeVec::hash>& setToCheck, SafeVec coords, uint16_t radius, bool checkForwards = false) const;
        bool isPeninsulerCaved(SafeVec coords) const;
        bool isLake(SafeVec coords) const;
        bool isContinental(SafeVec coords) const;
        float getContinentness(SafeVec coords) const;
        float getBeachness(SafeVec coords) const;
        void placeDungeonEntrances(ArgentumTileMap& argentumTileMap, const u_char nDungeonsToPlace);
        void resetState();                                        //add to the left of cave
        enum Target { beach,  lake,  cont,  tree,  bush,  ocean,  cave_0,  cave_1,  cave_2, N_TARGETS}; static constexpr std::array<const char*, N_TARGETS>
            TARGETS={"beach","lake","cont","tree","bush","ocean","cave_0","cave_1","cave_2"};//DON'T FORGET TO ADD ANY MISSING ENUM LITERALS
        

        static constexpr u_char N_CAVES = Target::N_TARGETS - Target::cave_0;   
        
        //TIENE QUE SER UN NÚMERO FIJO PARA QUE NO HAYA DESYNCS DE GENERACIÓN ALEATORIA ENTRE PCS POR TENER UN DISTINTO Nº DE THREADS
        //sino se generan distinto los trees y bushes
        static constexpr char N_THREADS = 16;

    protected:
        static void _bind_methods();

    public:
        FracturedContinentGenerator();
        ~FracturedContinentGenerator();

        FastNoiseLite mContinenter, mPeninsuler, mBigLaker, mSmallLaker, mBigBeacher, mSmallBeacher, mForest;
        float mContinentalCutoff, mPeninsulerCutoff, mBigLakeCutoff, mSmallLakeCutoff, mBeachCutoff, mTreeCutoff;

        float get_continental_cutoff() const; void set_continental_cutoff(float cutoff);

//en vez de poner optional parameters así, declarar varios métodos overloaded, hacer q el de menos llame al de más, y bindear los dos
        void generate(ArgentumTileMap& argentumTileMap, 
            const SafeVec& origin, const SafeVec& size, const Ref<Resource>& tileSelectionSet,
            const unsigned int seed = 0, const Dictionary& data = Dictionary()) override;


};//ENDCLASS
}


#endif // __FRACTUREDCONTINENT_GENERATOR_H__