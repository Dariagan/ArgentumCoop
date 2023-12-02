#include "BeingBuilder.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void BeingBuilder::_gd_set_name(const String& name){this->name = name;}
void BeingBuilder::_gd_set_faction_id(const String& faction_id){this->factionId = faction_id;};
void BeingBuilder::_gd_set_race_id(const String& race_id){this->raceId = race_id;}
void BeingBuilder::_gd_set_klass_id(const String& klass_id){this->klassId = klass_id;}
void BeingBuilder::_gd_randomize_klass_id(){this->klassId = "random";}
void BeingBuilder::_gd_set_head_id(const String& head_id){this->headId = head_id;}
void BeingBuilder::_gd_randomize_head_id(){this->headId = "random";}
void BeingBuilder::_gd_set_body_id(const String& body_id){this->bodyId = body_id;}
void BeingBuilder::_gd_randomize_body_id(){this->bodyId = "random";}
void BeingBuilder::_gd_set_head_scale(const Vector3& head_scale){this->headScale = head_scale;}
void BeingBuilder::_gd_set_body_scale(const Vector3& body_scale){this->bodyScale = body_scale;}
void BeingBuilder::_gd_set_extra_health_multiplier(const float multiplier){this->extraHealthMultiplier = multiplier;}

//TODO CARGAR TODAS LAS RACES, KLASSES AL C++ SIDE PA VER SI EXISTE LA ID PASADA AL CONSTRUIR DENTRO DEL DICT.

//hacer q solo se imprima 1 vez el error por cada id no encontrada, usando algun hashset estatico

BeingBuilder& BeingBuilder::setName(const String& name)
{
    this->name = name;
    return *this;
}
BeingBuilder& BeingBuilder::setFactionId(const String& factionId)
{
    this->factionId = factionId;
    return *this;
}
BeingBuilder& BeingBuilder::setRaceId(const String& raceId, bool controllableRace)
{
    this->raceId = raceId;
    this->controllableRace = controllableRace;
    return *this;
}
BeingBuilder& BeingBuilder::setKlassId(const String& klassId)
{
    this->klassId = klassId;
    return *this;
}
BeingBuilder& BeingBuilder::randomizeKlassId()
{
    this->klassId = "random";
    return *this;
}
BeingBuilder& BeingBuilder::setHeadId(const String& bodyId)
{
    this->headId = headId;
    return *this;
}
BeingBuilder& BeingBuilder::randomizeHeadId()
{
    this->headId = "random";
    return *this;
}
BeingBuilder& BeingBuilder::setBodyId(const String& bodyId)
{
    this->bodyId = bodyId;
    return *this;
}
BeingBuilder& BeingBuilder::randomizeBodyId()
{
    this->bodyId = "random";
    return *this;
}
BeingBuilder& BeingBuilder::setHeadScale(const Vector3& headScale)//agregar reset
{
    this->headScale = headScale;
    return *this;
}
BeingBuilder& BeingBuilder::setBodyScale(const Vector3& bodyScale)//agregar reset
{
    this->bodyScale = bodyScale;
    return *this;
}
BeingBuilder& BeingBuilder::setExtraHealthMultiplier(const float extraHealthMultiplier)//agregar reset
{
    this->extraHealthMultiplier = extraHealthMultiplier;
    return *this;
}

bool BeingBuilder::validate() const
{
    return false;
}

//MIRAR LAS OPERACIONES FUNCIONALES
//https://en.cppreference.com/w/cpp/utility/optional
bool BeingBuilder::build(){
    
    if (validate()){
        Dictionary data;
        data["name"] = name;
        data["fac"] = factionId;
        data["klass"] = klassId;
        //TODO
        data.make_read_only();
        builtBeing = std::make_optional<Dictionary>(data);
        return true;
    }
    UtilityFunctions::printerr("couldn't validate (BeingBuilder.cpp::build())");
    builtBeing = std::nullopt;
    return false;
}

std::optional<Dictionary> BeingBuilder::getResult() const
{return builtBeing;}

//usar solo del lado de gdscript
Dictionary BeingBuilder::_gd_get_result() const{

    if (builtBeing.has_value())
    {
        return builtBeing.value();
    }else{
        UtilityFunctions::printerr("warning: empty dict returned from get_result()");
        return Dictionary();
    }
}
BeingBuilder::BeingBuilder(){}
BeingBuilder::~BeingBuilder(){}
void BeingBuilder::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_name", "name"), &BeingBuilder::_gd_set_name);
    ClassDB::bind_method(D_METHOD("set_faction_id", "faction_id"), &BeingBuilder::_gd_set_faction_id);
    ClassDB::bind_method(D_METHOD("set_race_id", "race_id"), &BeingBuilder::_gd_set_race_id);
    ClassDB::bind_method(D_METHOD("set_klass_id", "klass_id"), &BeingBuilder::_gd_set_klass_id);
    ClassDB::bind_method(D_METHOD("randomize_klass_id"), &BeingBuilder::_gd_randomize_klass_id);
    ClassDB::bind_method(D_METHOD("set_head_id", "head_id"), &BeingBuilder::_gd_set_head_id);
    ClassDB::bind_method(D_METHOD("randomize_head_id"), &BeingBuilder::_gd_randomize_head_id);
    ClassDB::bind_method(D_METHOD("set_body_id", "body_id"), &BeingBuilder::_gd_set_body_id);
    ClassDB::bind_method(D_METHOD("randomize_body_id"), &BeingBuilder::_gd_randomize_body_id);
    ClassDB::bind_method(D_METHOD("set_head_scale", "head_scale"), &BeingBuilder::_gd_set_head_scale);
    ClassDB::bind_method(D_METHOD("set_body_scale", "body_scale"), &BeingBuilder::_gd_set_body_scale);
    ClassDB::bind_method(D_METHOD("set_extra_health_multiplier", "extra_health_multiplier"), &BeingBuilder::_gd_set_extra_health_multiplier);

    ClassDB::bind_method(D_METHOD("build"), &BeingBuilder::build);

    ClassDB::bind_method(D_METHOD("get_result"), &BeingBuilder::_gd_get_result);
}
