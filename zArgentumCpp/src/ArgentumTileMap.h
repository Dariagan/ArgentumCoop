#ifndef __ARGENTUMTILEMAP_H__
#define __ARGENTUMTILEMAP_H__
#include "FormationGenerator.h"

#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>
#include <unordered_set>  
#include <unordered_map>


namespace godot {
    

    struct hash
    {
        size_t operator()(const char* s)
        {
            constexpr int A = 54059;constexpr int B = 76963;constexpr int C = 86969;constexpr int FIRSTH = 37;
            unsigned h = FIRSTH;
            while (*s) {
                h = (h * A) ^ (s[0] * B);
                s++;
            }
            return h; // or return h % C;
        };
    };


    class ArgentumTileMap : public TileMap{
        GDCLASS(ArgentumTileMap, TileMap)

        private:
            std::vector<std::vector<std::vector<std::array<char, 32>>>> worldMatrix;//hacer esto un array bidimensional de C predimensionado y usar el Vec2i worldsize como bound?
            std::vector<std::vector<std::vector<std::array<char, 32>>>> spawnWeightsMatrix;
            
            SafeVec worldSize;
            std::unordered_set<SafeVec, SafeVec::hash> loadedTiles;//compartido por todos los beings del world activos en esta pc
            std::unordered_map<std::string, std::unordered_map<StringName, Variant>> cppTilesData;
            static bool withinChunkBounds(const SafeVec &LOADED_COORD_TO_CHECK, const SafeVec &TL_CORNER, const MatrixCoords &CHUNK_SIZE);

            bool setCell(const std::string& TILE_ID, const SafeVec& coords);

        protected:
            static void _bind_methods();

        public:
            ArgentumTileMap();
            ~ArgentumTileMap();
            
            //TODO algún método para escribir en un archivo el estado del mapa actual
            //TODO algún método para cargar el worldMatrix a partir de un archivo
            
            signed int seed = 0; int get_seed(); void set_seed(signed int seed);//global seed (picks random seeds for generations with a seeded gdscript RNG)

            Dictionary tiles_data; Dictionary get_tiles_data(); void set_tiles_data(Dictionary data);

            void generate_world_matrix(const Vector2i& size);
            void generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i& origin, const Vector2i& size, 
                 const Ref<Resource>& tileSelectionSet, signed int seed, const Dictionary& data);
            
            void load_tiles_around(const Vector2& coords, const Vector2i& chunk_size);
            void unloadExcessTiles(const SafeVec& topLeftCornerCoords, const MatrixCoords& CHUNK_SIZE);
    };
}

#endif // __ARGENTUMTILEMAP_H__