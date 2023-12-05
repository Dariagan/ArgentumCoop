// #ifndef GLOBAL_DATA_SINGLETON
// #define GLOBAL_DATA_SINGLETON
// #include <godot_cpp/classes/node.hpp> 
// #include <godot_cpp/variant/array.hpp>
// #include <godot_cpp/variant/string.hpp>
// #include <godot_cpp/variant/string_name.hpp>
// #include <godot_cpp/variant/dictionary.hpp>
// #include <godot_cpp/core/defs.hpp>
// #include <godot_cpp/core/class_db.hpp>
// #include <godot_cpp/variant/utility_functions.hpp>

// #include <godot_cpp/variant/typed_array.hpp>
// #include <godot_cpp/godot.hpp>
// #include <unordered_map>
// #include <algorithm>
// #include <optional>
// #include <limits>
// #include <memory>

// #include "WorldMatrix.cpp"

// namespace godot
// {

// class GlobalDataCpp
// {
//     // PONER POR ARRIBA PORQ SINO NO LOS ENCUENTRA
//     private:
//         static bool exceedsTileLimit(const u_int16_t count)
//         {
//             if (count >= NULL_TILE_UID - 1){
//                 UtilityFunctions::printerr("too many tiles (GlobalDataCpp.cpp::set_tiles())");
//                 return true;
//             }
//             return false;
//         }
      
//     public:
//         static Dictionary active_beings;//guarda referencias a Beings. cuando se erasee una tile se mira este diccionario pa ver si la local_to_map(being.position) está en la pos

//         static Dictionary tiles;
  
//     //     //el godot side se tiene q encargar de agregar los beings

        

//         static Dictionary get_tiles();

        
// };
// }

// #endif //GLOBAL_DATA_SINGLETON