#ifndef __ARGENTUMTILEMAP_H__
#define __ARGENTUMTILEMAP_H__
#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/godot.hpp>
#include <unordered_set>  
#include <unordered_map>
#include <memory>
#include <limits>
#include "rust.h"
#include "WorldMatrix.h"
#include "FormationGenerator.h"
#include "BeingsModule.h"
#include "typealiases.h"
#include "beingtypes.h"

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
{//NO PONER CUERPOS DE MÉTODOS EN LOS HEADER FILES (AUNQUE ESTÉNc+ VACÍOS). PUEDE CAUSAR PROBLEMAS DE LINKING AL COMPILAR 
static const Vector2i ERROR_VECTOR = {-999999, -999999};    

class BeingsModule;
class FormationGenerator;
class ArgentumTileMap : public TileMap{ GDCLASS(ArgentumTileMap, TileMap)

public:
    ArgentumTileMap();
    ~ArgentumTileMap();

    std::optional<tiletype_uid> findTileUid(const StringName& stringId) const;
    const StringName getTileId(tiletype_uid uid) const;

    Node* global_data;
    std::unique_ptr<BeingsModule> mBeingsModule;

    //HAY QUE ESCRIBIR EN BINARIO. NO EN ASCII. EN ASCII 1 DIGITO = 1 BYTE
    //HAY QUE GUARDAR: LA SEED + USER MODIFIED DATA
    bool persist(String filename);

    //SOLO USAR PARA FORMATIONS
    void placeFormationTile(
        const SafeVec formationOrigin, const SafeVec tileCoordsRelativeToFormationOrigin, 
        const tiletype_uid newTile, bool deletePreviousTiles = false);
    
    bool placeIngameTile(const SafeVec coords, const StringName& id);//akí la id puede ser un stringname directamente, no se está iterando y así se puede bindear a godot el method
    //false: out of array bounds u otro error (usado por el godot-side)

    //BeingModuleMethod only
    
    //TODO algún método para escribir en un archivo el estado del mapa actual (intentar escribir en el .tres?)
    //TODO algún método para cargar el worldMatrix a partir de un archivo
    
    int get_seed();void set_seed(const u_int seed);//global seed (picks random seeds for generations with a seeded gdscript RNG)

    Dictionary get_tiles_data(); void set_tiles_data(const Dictionary& data); void add_tiles_data(const Dictionary& data);

    void set_tiles_data_placeholder(const Dictionary& input_tiles_data);

    Vector2i get_random_coord_with_tile_id(const Vector2i top_left_corner, const Vector2i bottom_right_corner, const String& tile_id) const;

//todo algun método para placear/cambiar/modificar/buildear tiles cuando ya se está ingame y q se guarden en un hashmap con las modificadas
    
    void freeze_and_store_being(const Vector2 glb_coords, const being_uid individual_unique_id);

    void generate_world_matrix(const Vector2i size, const Dictionary& tiles_data);
    void generate_formation(const Ref<FormationGenerator>& formation_generator, const Vector2i origin, const Vector2i size, 
            const Ref<Resource>& tileSelectionSet, unsigned int seed, const Dictionary& data);
    
    void load_tiles_around(const Vector2 coords, const Vector2i chunk_size, const int uid);
    void unloadExcessTiles(const SafeVec topLeftCornerCoords, const SafeVec chunkSize, const int uid);
 
//!SE PUEDE USAR get_node("root/...") para conseguir un puntero a un nodo de godot, y, call sobre este para llamar un método de este
private: //!NOTA: se pueden llamar a funciones propias q estén en el nodo del lado de gdscript usando simplemente call("",""...) (llama a métodos de gdscript propios de este nodo)
    

    std::unique_ptr<WorldMatrix> mWorldMatrixPtr = nullptr;

    unsigned int seed = 0;
    Dictionary m_tiles_data; void replaceTilesDataProperly(const Dictionary& input_tiles_data);
    std::unordered_map<StringName, std::unordered_map<StringName, Variant>> mCppTilesData;
    std::vector<StringName> mTilesUidMapping;   

    std::unordered_map<SafeVec, std::array<tiletype_uid, WorldMatrix::MAX_TILES_PER_POS>, SafeVec::hash> mPositionsWithChangedTiles;

    //TIENE Q SER UN INT, PORQ SINO EN EL GODOT NO SE PUEDE VER EL ESTADO DEL OBJECT EN EL INSPECTOR. 
    //ACA HAY Q GUARDAR UN UIDQ REFERENCIE EL STATE GUARDADO EN GODOT.
    typedef unsigned int tileinstance_uid;
    //NO ES UN ARRAY LA DATA STRUCTURE INTERNA PORQ PUEDE Q EL RESTO DE TILES EN LA MISMA POS NO TENGAN STATE INICIALIZADO, Y NO SE PUEDE SABER SI SI O SI NO EN UN ARRAY
    
    //update: creo q no hace falta esto. con un dictionary en godot con key coordx_coordy_coordz y value el state ya se puede hacer todo
    std::unordered_map<SafeVec, std::unordered_set<tileinstance_uid>, SafeVec::hash> mTileInstancesState;
    std::unordered_map<SafeVec, std::unordered_map<u_char, Object>, SafeVec::hash> mTileInstancesState1;
    std::unordered_map<SafeVec, std::unordered_map<u_char, tileinstance_uid>, SafeVec::hash> mTileInstancesState2;

    std::unordered_map<std::string, SafeVec> mTrackedBeingsCoords;//updateado cada
   
    friend class BeingsModule; 
    
    //USAR ESTO EN VEZ DE add_child

    //el String es la uniqueid del being específico (individuo). esta unique id es pasada al GDscript-side cuando toca spawnear, 
    //en donde según la id extrae el being de un dictionary q tiene guardado

    void decrementSharedCount(const SafeVec tileCoord);

    //keeps track of tracks of the tiles loaded by the being with the specific uid (int)
    std::unordered_map<being_uid, std::unordered_set<SafeVec, SafeVec::hash>> mBeingLoadedTiles;

    std::unordered_map<SafeVec, int16_t, SafeVec::hash> mTileSharedLoadsCount;
    
    static bool withinChunkBounds(const SafeVec loadedCoordToCheck, const SafeVec topLeftCorner, const SafeVec chunkSize);

    bool setCell(const tiletype_uid newTile, const SafeVec coords);

    static bool exceedsTileLimit(const tiletype_uid count);

    protected: static void _bind_methods();
};
}

#endif // __ARGENTUMTILEMAP_H__