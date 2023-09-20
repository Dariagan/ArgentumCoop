

static func get_tiles(info: Dictionary) -> Array[StringName]:
	
	var tiles_to_place: Array[StringName] = []
	
	match info:
		
		{&"continental": true, &"peninsuler_caved": false, &"away_from_coast": false, &"beach": true, ..}:

			tiles_to_place.append(&"beach_sand")
			
		{&"continental": true, &"peninsuler_caved": false, &"away_from_coast": true, &"lake": true, ..}:

			tiles_to_place.append(&"lake")
		{&"continental": false, ..}, {&"continental": true, &"peninsuler_caved": true, ..}:#ocean

			tiles_to_place.append(&"ocean")		
		{&"continental": true, &"peninsuler_caved": false, ..}:#grass
			
			tiles_to_place.append(&"grass")
			
	return tiles_to_place

