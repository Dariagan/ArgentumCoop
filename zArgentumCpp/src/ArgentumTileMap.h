#ifndef __ARGENTUMTILEMAP_H__
#define __ARGENTUMTILEMAP_H__
#include "FormationGenerator.h"
#include "BeingBuilder.h"
#include "WorldMatrix.cpp"

#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>
#include <unordered_set>  
#include <unordered_map>
#include <memory>

template<class T, size_t N> 
struct std::hash<std::array<T, N>> {
    std::hash<T> hasher;
    auto operator() (const std::array<T, N>& key) const {
        size_t result = 0;
        for(size_t i = 0; i < N; ++i) {
            result = result * 31 + hasher(key[i]); 
        }
        return result;
    }
};

namespace godot 
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉN VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING AL COMPILAR 
    static const Vector2i ERROR_VECTOR = {-999999, -999999};
    class FormationGenerator;

    class ArgentumTileMap : public TileMap{ GDCLASS(ArgentumTileMap, TileMap)

        public:
            ArgentumTileMap();
            ~ArgentumTileMap();

            bool persist(String filename);

            //std::vector<std::vector<std::array<uint16_t>>>& getSpawnWeightsMatrix();

            void placeSpawnWeight(
                const SafeVec& formationOrigin, const SafeVec& coordsRelativeToFormationOrigin, 
                const uint16_t& beingKindId, const unsigned char weight, bool deleteOthers);

            //SOLO USAR PARA FORMATIONS
            void placeFormationTile(
                const SafeVec& formationOrigin, const SafeVec& tileCoordsRelativeToFormationOrigin, 
                const uint16_t optTileUid, bool deleteOthers = false);
                
            void birthBeing(const Vector2i& coords, const BeingBuilder& beingBuilder);
            
            //TODO algún método para escribir en un archivo el estado del mapa actual (intentar escribir en el .tres?)
            //TODO algún método para cargar el worldMatrix a partir de un archivo
            
            int seed = 0;int get_seed(); void set_seed(int seed);//global seed (picks random seeds for generations with a seeded gdscript RNG)

            Dictionary get_tiles_data(); void set_tiles_data(Dictionary data);//tiles_data ITSELF MUST BE PUBLIC

            Vector2i get_random_coord_with_tile_id(const Vector2i& top_left_corner, const Vector2i& bottom_right_corner, const String& tile_id) const;

            void birthBeingOfKind(const String& being_kind_id);
            void freeze_and_store_being(const Vector2& glb_coords, const int individual_unique_id);

            void generate_world_matrix(const Vector2i& size);
            void generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i& origin, const Vector2i& size, 
                 const Ref<Resource>& tileSelectionSet, signed int seed, const Dictionary& data);
            
            void load_tiles_around(const Vector2& coords, const Vector2i& chunk_size, const int uid);
            void unloadExcessTiles(const SafeVec& topLeftCornerCoords, const SafeVec& CHUNK_SIZE, const int uid);

            void set_beings_in_chunk_count(const TypedArray<Array> beings_in_chunk_count);
            TypedArray<Array> get_beings_in_chunk_count();
            

            static constexpr unsigned char MATRIXES_SIZE_RATIO = 70;

        protected:
            static void _bind_methods();

        private:
            std::unordered_map<std::string, SafeVec> m_trackedBeingsCoords;//updateado cada

            std::unique_ptr<WorldMatrix> worldMatrixPtr = nullptr;

            //chunk size: 7x7 puntos de spawnweights
            //TypedArray<TypedArray<long>>
            TypedArray<Array> beings_in_chunk_count;
            
            //contiene ids de BeingKinds
            std::vector<std::vector<std::unordered_map<uint16_t, unsigned char>>> m_spawnWeightsMatrix;

            std::unordered_map<SafeVec, std::vector<std::pair<Vector2, int>>, SafeVec::hash> m_frozenBeings;

            //el String es la uniqueid del being específico (individuo). esta unique id es pasada al GDscript-side cuando toca spawnear, 
            //en donde según la id extrae el being de un dictionary q tiene guardado

            void decrementSharedCount(const SafeVec& tileCoord);
            
            SafeVec m_worldSize;

            //keeps track of tracks of the tiles loaded by the being with the specific uid (int)
            std::unordered_map<int, std::unordered_set<SafeVec, SafeVec::hash>> m_beingLoadedTiles;

            std::unordered_map<SafeVec, int, SafeVec::hash> m_tileSharedLoadsCount;

//! CREO QUE ESTO YA NO HACE FALTA TENIENOD LA DATA EN GLOBALDATA
            std::unordered_map<std::string, std::unordered_map<StringName, Variant>> m_cppTilesData;
            static bool withinChunkBounds(const SafeVec &loadedCoordToCheck, const SafeVec &topLeftCorner, const SafeVec &chunkSize);

            bool setCell(const uint16_t uid, const SafeVec& coords);

            void doGlobalSpawnAttempts();

            Dictionary tiles_data;
    };
}

#endif // __ARGENTUMTILEMAP_H__