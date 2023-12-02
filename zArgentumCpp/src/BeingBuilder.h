#ifndef BEING_BUILDER
#define BEING_BUILDER
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>
#include <optional>
#include <memory>

namespace godot{

//TODO AGREGAR ESTA CLASE AL GODOT-SIDE? PODRÍA SER MUY ÚTIL
class BeingBuilder: public RefCounted
{
    GDCLASS(BeingBuilder, RefCounted)
    private:
        std::optional<Dictionary> builtBeing = std::nullopt;
        String name, factionId, raceId="", klassId="random", bodyId="random", headId="random";
        bool controllableRace;
        Vector3 headScale = Vector3(1,1,1), bodyScale = Vector3(1,1,1);
        float extraHealthMultiplier = 1;
        bool validate() const;
    protected:
        static void _bind_methods();
    public:

        static void load_game_data(Dictionary data); 

        void _gd_set_name(const String& name);
        void _gd_set_faction_id(const String& faction_id);
        void _gd_set_race_id(const String& race_id);
        void _gd_set_klass_id(const String& klass_id);
        void _gd_randomize_klass_id();
        void _gd_set_head_id(const String& head_id);
        void _gd_randomize_head_id(); 
        void _gd_set_body_id(const String& body_id);
        void _gd_randomize_body_id(); 
        void _gd_set_head_scale(const Vector3& head_scale);
        void _gd_set_body_scale(const Vector3& body_scale);
        void _gd_set_extra_health_multiplier(const float multiplier);

        BeingBuilder& setName(const String& name);
        BeingBuilder& setFactionId(const String& factionId);
        BeingBuilder& setRaceId(const String& raceId, bool controllableRace);
        BeingBuilder& setKlassId(const String& klassId);
        BeingBuilder& randomizeKlassId();
        BeingBuilder& setHeadId(const String& bodyId);
        BeingBuilder& randomizeHeadId();
        BeingBuilder& setBodyId(const String& bodyId);
        BeingBuilder& randomizeBodyId();
        BeingBuilder& setHeadScale(const Vector3& headScale);
        BeingBuilder& setBodyScale(const Vector3& bodyScale);
        BeingBuilder& setExtraHealthMultiplier(const float extraHealthMultiplier);
        //MIRAR LAS OPERACIONES FUNCIONALES
        //https://en.cppreference.com/w/cpp/utility/optional
        
        //usado por build
        
        bool build();

        Dictionary _gd_get_result() const;

        std::optional<Dictionary> getResult() const;

        //usar solo del lado de gdscript
        
        BeingBuilder();
        ~BeingBuilder();
};
}

#endif