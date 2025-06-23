#include "CppFormationGenerator.hpp"

using namespace godot;


double CppFormationGenerator::getBorderClosenessFactor(const SafeVec& coords, const SafeVec& SIZE, const double power)
{
    const double borderClosenessHorizontally = abs(coords.lef-SIZE.lef/2.)/(SIZE.lef/2.);
    const double borderClosenessVertically = abs(coords.RIGHT-SIZE.RIGHT/2.)/(SIZE.RIGHT/2.);
    
    return std::max(pow(borderClosenessHorizontally, power), pow(borderClosenessVertically, power));
}

void CppFormationGenerator::generate(CppArgentumTileMap &argentumTileMap, const SafeVec& origin, const SafeVec& size, const Ref<Resource> &tileSelectionSet, const unsigned int seed, const Dictionary &data)
{
}

CppFormationGenerator::CppFormationGenerator()
{

}
CppFormationGenerator::~CppFormationGenerator(){
   
}
void CppFormationGenerator::_bind_methods()
{
    
}