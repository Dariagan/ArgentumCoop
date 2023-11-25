#ifndef __ARGENTUMTILEMAP_H__
#define __ARGENTUMTILEMAP_H__

#include "FormationGenerator.h"

#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>
#include <unordered_set>  
#include <unordered_map>

namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING AL COMPILAR 
    static const Vector2i ERROR_VECTOR = {-999999, -999999};
    class FormationGenerator;
    class ArgentumTileMap : public TileMap{
        GDCLASS(ArgentumTileMap, TileMap)

        private:
            std::unordered_map<std::string, SafeVec> m_trackedBeingsCoords;//updateado cada

            std::vector<std::vector<std::vector<std::array<char, 32>>>> m_worldMatrix;
            std::vector<std::vector<std::vector<std::array<char, 32>>>> m_spawnWeightsMatrix;
            std::unordered_map<SafeVec, std::vector<std::pair<Vector2, String>>, SafeVec::hash> m_frozenBeings;

            //el String es la uniqueid del being específico (individuo). esta unique id es pasada al GDscript-side cuando toca spawnear, 
            //en donde según la id extrae el being de un dictionary q tiene guardado
            
            SafeVec m_worldSize;
            std::unordered_set<SafeVec, SafeVec::hash> m_loadedTiles;//compartido por todos los beings del world activos en esta pc
            std::unordered_map<std::string, std::unordered_map<StringName, Variant>> m_cppTilesData;
            static bool withinChunkBounds(const SafeVec &loadedCoordToCheck, const SafeVec &topLeftCorner, const MatrixCoords &chunkSize);

            bool setCell(const std::string& tileId, const SafeVec& coords);

        protected:
            static void _bind_methods();

        public:
            ArgentumTileMap();
            ~ArgentumTileMap();
            //std::vector<std::vector<std::vector<std::array<char, 32>>>>& getWorldMatrix();
            //std::vector<std::vector<std::vector<std::array<char, 32>>>>& getSpawnWeightsMatrix();

            //SOLO USAR PARA FORMATIONS
            void placeFormationTile(//TODO hacerlo friend method
                const SafeVec& formationOrigin, const MatrixCoords& tileCoordsRelativeToFormationOrigin, 
                const std::array<char, 32>& tileId, bool deleteOthers = false);
                
            
            //TODO algún método para escribir en un archivo el estado del mapa actual (intentar escribir en el .tres?)
            //TODO algún método para cargar el worldMatrix a partir de un archivo
            
            int seed = 0;int get_seed(); void set_seed(int seed);//global seed (picks random seeds for generations with a seeded gdscript RNG)

            Dictionary tiles_data; Dictionary get_tiles_data(); void set_tiles_data(Dictionary data);//tiles_data ITSELF MUST BE PUBLIC

            Vector2i get_random_coord_with_tile_id(const Vector2i& top_left_corner, const Vector2i& bottom_right_corner, const String& tile_id) const;

            void initializePawnKindinGdScript(const Ref<Dictionary>& data);
            void store_frozen_being(const Vector2& glb_coords, const String& individual_unique_id);

            void generate_world_matrix(const Vector2i& size);
            void generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i& origin, const Vector2i& size, 
                 const Ref<Resource>& tileSelectionSet, signed int seed, const Dictionary& data);
            
            void load_tiles_around(const Vector2& coords, const Vector2i& chunk_size);
            void unloadExcessTiles(const SafeVec& topLeftCornerCoords, const MatrixCoords& CHUNK_SIZE);
    };
}

#endif // __ARGENTUMTILEMAP_H__