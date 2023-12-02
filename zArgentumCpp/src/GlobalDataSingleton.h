#ifndef GLOBAL_DATA_SINGLETON
#define GLOBAL_DATA_SINGLETON
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>

namespace godot{

//TODO AGREGAR ESTA CLASE AL GODOT-SIDE? PODRÍA SER MUY ÚTIL
class GlobalDataSingleton: public RefCounted
{
    GDCLASS(GlobalDataSingleton, RefCounted)
    private:


    protected:
        static void _bind_methods();
    public:

        GlobalDataSingleton(); ~GlobalDataSingleton();
};
}

#endif //GLOBAL_DATA_SINGLETON