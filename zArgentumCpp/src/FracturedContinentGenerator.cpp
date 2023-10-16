#include "FracturedContinentGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void FracturedContinentGenerator::_bind_methods(){}


void FracturedContinentGenerator::generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, 
    const Vector2i& origin, const Vector2i& area, const TilePicker tilePicker, const int seed, const Dictionary& data)
{
    for (int i = -area.x/2; i < area.x/2; i++){
        for (int j = -area.y/2; j < area.y/2; j++)
        {
            FormationGenerator::placeTile(worldMatrix, origin, Vector2i(i, j), "grass");
        }
    }

    
}

FracturedContinentGenerator::FracturedContinentGenerator(){}
FracturedContinentGenerator::~FracturedContinentGenerator(){}



