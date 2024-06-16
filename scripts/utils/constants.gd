class_name Constants

enum Sex { ANY = 0, MALE = 1, FEMALE = 2}

enum Layer { BOTTOM, MIDDLE, TOP}

enum TerrainType {LAND, SHALLOW_WATER, DEEP_WATER}

enum Handling {ANY, ONE_HANDED, TWO_HANDED}

const KEYS: Dictionary = { 
	NAME = &"name",
	SEX = &"sex",
	RACE = &"race",
	FACTION = &"faction",
	KLASS = &"klass",
	FOLLOWERS = &"followers",
	HEAD_SCALE = &"head_scale",
	BODY_SCALE = &"body_scale",
	HEAD = &"head",
	BODY = &"body",
	EXTRA_HEALTH_MULTI = &"eh",
	BEINGKIND = &"beingkind",
	INTERNAL_STATE = &"istate",
}
