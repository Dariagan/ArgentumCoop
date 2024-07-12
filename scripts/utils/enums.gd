class_name Enums

enum Sex { ANY = 0, MALE = 1, FEMALE = 2}

enum TerrainType {LAND, SHALLOW_WATER, DEEP_WATER}

enum Handling {ANY, ONE_HANDED, TWO_HANDED}

enum ClothingLayer {BOTTOM, MIDDLE, TOP}

enum Dir {LEFT, RIGHT, UP, DOWN}

#chequear que esté igual que como está puesto en rust
enum TileZLevel {Soil = 0, Water, Floor, Stain, Structure, Roof}
const TileZLevelKey:Array[StringName] = [&"Soil", &"Floor", &"Stain", &"Structure", &"Roof"]

#WARNING chequear que esté igual que como está definido en Rust
enum AnimationState { IDLE, WALK, JOG, EXTRA0, EXTRA1, EXTRA2, EXTRA3 }

enum AiState { WANDER, CHASE, ATTACK_RANGED, FLEE,}
