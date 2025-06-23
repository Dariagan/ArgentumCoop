#include "register_types.h"
#include "CppFormationGenerator.hpp"
#include "CppFracturedContinentGenerator.hpp"
#include "CppArgentumTileMap.hpp"
#include "BeingBuilder.hpp"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <iostream>
#include <fstream>


using namespace godot;

// IMPORTANTE: AL CREAR UNA CLASE QUE HEREDE DE UNA CLASE DE GODOT:
// HAY QUE ANTEPONERLE "public" A LA CLASE DE GODOT 

void initialize_argentum_cpp(ModuleInitializationLevel p_level)
{
    if(p_level != MODULE_INITIALIZATION_LEVEL_SCENE){return;}

    ClassDB::register_class<CppArgentumTileMap>();
    ClassDB::register_class<BeingBuilder>();
    ClassDB::register_abstract_class<CppFormationGenerator>();
    ClassDB::register_class<CppFracturedContinentGenerator>();
    
    // register custom classes here in order for them to show up in Godot!
}
void uninitialize_argentum_cpp(ModuleInitializationLevel p_level)
{
    if(p_level != MODULE_INITIALIZATION_LEVEL_SCENE){
        return;
    }
}

extern "C"{
    GDExtensionBool GDE_EXPORT argentum_cpp_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_argentum_cpp);
        init_obj.register_terminator(uninitialize_argentum_cpp);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
        return init_obj.init();
    }        
}