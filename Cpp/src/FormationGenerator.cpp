#include "FormationGenerator.hpp"

using namespace godot;


double FormationGenerator::getBorderClosenessFactor(const SafeVec& coords, const SafeVec& SIZE, const double power)
{
    const double borderClosenessHorizontally = abs(coords.lef-SIZE.lef/2.)/(SIZE.lef/2.);
    const double borderClosenessVertically = abs(coords.RIGHT-SIZE.RIGHT/2.)/(SIZE.RIGHT/2.);
    
    return std::max(pow(borderClosenessHorizontally, power), pow(borderClosenessVertically, power));
}

void FormationGenerator::generate(ArgentumTileMap &argentumTileMap, const SafeVec& origin, const SafeVec& size, const Ref<Resource> &tileSelectionSet, const unsigned int seed, const Dictionary &data)
{
}

FormationGenerator::FormationGenerator()
{

}
FormationGenerator::~FormationGenerator(){
   
}
void FormationGenerator::_bind_methods()
{
    
}