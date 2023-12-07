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

namespace godot{


//hacer q en el gdside si chequee si la id es válida
class BeingBuilder: public RefCounted
{
    GDCLASS(BeingBuilder, RefCounted)
    public:
        //TODO cargar los dicts del node autoload global_data???? AL FINAL PUEDE Q NO PORQ NO SE PUEDE HACER SINGLETONS
        static void load_game_data(Dictionary data); 

        BeingBuilder& setName(const String& name);
        BeingBuilder& randomizeName();
        BeingBuilder& setFactionId(const String& factionId);
        BeingBuilder& setRaceId(const String& raceId, bool controllableRace);
        BeingBuilder& setKlassId(const String& klassId);
        BeingBuilder& randomizeKlassId();
        BeingBuilder& setHeadId(const String& headId);
        BeingBuilder& randomizeHeadId();
        BeingBuilder& setBodyId(const String& bodyId);
        BeingBuilder& randomizeBodyId();
        BeingBuilder& setHeadScale(const Vector3& headScale);
        BeingBuilder& setBodyScale(const Vector3& bodyScale);
        BeingBuilder& setExtraHealthMultiplier(const float extraHealthMultiplier);

        bool build();
        std::optional<Dictionary> getResult() const;

        BeingBuilder(); ~BeingBuilder();
    protected:
        static void _bind_methods();
    private:
        // functions to be used exclusively on GDscript-side (not in C++)

        void _gd_set_name(const String& name);
        void _gd_randomize_name();
        void _gd_set_faction_id(const String& faction_id);
        void _gd_set_race_id(const String& race_id, const bool controllable_race);
        void _gd_set_klass_id(const String& klass_id);
        void _gd_randomize_klass_id();
        void _gd_set_head_id(const String& head_id);
        void _gd_randomize_head_id(); 
        void _gd_set_body_id(const String& body_id);
        void _gd_randomize_body_id(); 
        void _gd_set_head_scale(const Vector3& head_scale);
        void _gd_set_body_scale(const Vector3& body_scale);
        void _gd_set_extra_health_multiplier(const float multiplier);
        Dictionary _gd_get_result() const;

        std::optional<Dictionary> builtBeing = std::nullopt;
        String name="random", factionId="", raceId="", klassId="random", bodyId="random", headId="random";
        bool controllableRace = false;
        Vector3 headScale = Vector3(1,1,1), bodyScale = Vector3(1,1,1);
        float extraHealthMultiplier = 1;
        bool isValidBeing() const;
        void failAndPrint(bool& validationFailedMessageAlreadyPrinted) const;

};
}

#endif