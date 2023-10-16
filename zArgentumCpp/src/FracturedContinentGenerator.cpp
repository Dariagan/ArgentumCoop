#include "FracturedContinentGenerator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void FracturedContinentGenerator::_bind_methods()
{
    
}


void FracturedContinentGenerator::generate(std::vector<std::vector<std::vector<StringName>>> & worldMatrix, Vector2i origin, Vector2i area, TilePicker tilePicker, int64_t seed)
{


    placeTile(worldMatrix, origin, Vector2i(0,0), "grass");
}

FracturedContinentGenerator::FracturedContinentGenerator(){}
FracturedContinentGenerator::~FracturedContinentGenerator(){}



