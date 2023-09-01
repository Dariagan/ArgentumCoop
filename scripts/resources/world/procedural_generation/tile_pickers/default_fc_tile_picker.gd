

static func get_tiles(info: Dictionary) -> Array[StringName]:
	
	var tiles_to_place: Array[StringName] = []
	

			
	match info:
		
		{&"continental": true, &"peninsuler_caved": false, &"away_from_coast": false, &"beach": true, ..}:

			tiles_to_place.append(&"beach_sand")
			
		{&"continental": true, &"peninsuler_caved": false, &"away_from_coast": true, &"lake": true, ..}:

			tiles_to_place.append(&"lake")
		#[true, true, ..]: tiles_to_place.append({"atlas_id": 0, "tile": Vector2i(10, 1)})
		{&"continental": false, ..}, {&"continental": true, &"peninsuler_caved": true, ..}:#ocean

			tiles_to_place.append(&"ocean")
			#TODO HACER ANIMATED WATER
		
		{&"continental": true, &"peninsuler_caved": false, ..}:#grass
			
			tiles_to_place.append(&"grass")
			
	return tiles_to_place
"""
	match [continental, peninsuler_caved, away_from_coast, lake, beach]:
		
		[true, false, false, _, true]:#beach
			tiles_to_place.append(&"beach_sand")
			
		[true, false, true, true, ..]:#lake
			tiles_to_place.append(&"lake")
			
		[false, ..], [true, true, ..]:#ocean
			tiles_to_place.append(&"ocean")
			#TODO HACER ANIMATED WATER
		
		[true, false, ..]:#grass	
			tiles_to_place.append(&"grass")
"""
