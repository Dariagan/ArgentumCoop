using Godot;
using System;
using System.Collections.Generic;
using System.Linq;

public partial class ProceduralTileMap : TileMap
{
	
	private string[,] WorldMatrix;

	
	private (int, int) WorldSize;

	/*public override void _Ready()
	{
	}
	public override void _Process(double delta)
	{
	}*/

	public void GenerateWorld(Godot.Collections.Dictionary data = null, int seed = 0){
		
		(int, int) worldSize = (6000, 6000);
		WorldSize = worldSize;
		

		WorldMatrix = new string[WorldSize.Item1, WorldSize.Item2];

		ProceduralGenerator pg = (ProceduralGenerator)GD.Load("res://resources/world/temperate_continent_generator.tres");
		
		//ProceduralGenerator pg = (ProceduralGenerator)script.New();
		pg.Generate(WorldMatrix, Vector2I.Zero, new Vector2I(2500, 2500));
	}

	private HashSet<(int, int)> LoadedTiles = new();
	//private int LoadedTilesCount = 0;

	//creo q esto es más rapido dejarlo en gdscript
	public void LoadTilesAround(Vector2 coords){

		Vector2I beingCoords = LocalToMap(coords);

		(int, int) chunkSize = (90, 53);

		for (int i = -chunkSize.Item1/2; i < chunkSize.Item1/2; i++) 
		{
			for (int j = -chunkSize.Item2/2; j < chunkSize.Item2/2; j++) 
			{
				(int, int) matrixCoords = (WorldSize.Item1/2 + beingCoords.X + i, WorldSize.Item2/2 + beingCoords.Y + j);
                if (matrixCoords.Item1 < (WorldSize.Item1 - 1) && matrixCoords.Item2 < (WorldSize.Item2 - 1) && matrixCoords.Item1 >= 0 && matrixCoords.Item2 >= 0)
                    foreach (string tile_id in WorldMatrix[matrixCoords.Item1, matrixCoords.Item2].Split("&"))
					{
						Node globalData = GetNode("/root/GlobalData");
						Godot.Collections.Dictionary tiles = (Godot.Collections.Dictionary)globalData.Get("tiles");

						Resource tile = (Resource)tiles[tile_id]  ;

						Vector2I tilemap_tile_coords = new Vector2I(beingCoords.X + i, beingCoords.Y + j);
						
						if (!LoadedTiles.Contains((matrixCoords.Item1, matrixCoords.Item2)))
						{
							SetCell((int)tile.Call("get_layer"), tilemap_tile_coords, (int)tile.Call("get_source_id"), (Vector2I)tile.Call("get_atlas_pos"), (int)tile.Call("get_alt_id"));
							LoadedTiles.Add((matrixCoords.Item1, matrixCoords.Item2));
						}
					}
				//else poner otra cosa, como fog, así no queda todo gris
			}
		}
	}
}


/*
for i in range(-chunk_size.x/2, chunk_size.x/2):
		for j in range(-chunk_size.y/2, chunk_size.y/2):
			var matrixCoords: Vector2i = Vector2i(MapSize.Item1/2 + being_coords.x + i, MapSize.Item2/2 + being_coords.y + j)
			if matrixCoords.Item1 < (MapSize.Item1 - 1) && matrixCoords.Item2 < (MapSize.Item2 - 1) && matrixCoords.Item1 >= 0 && matrixCoords.Item2 >= 0:
				for tile in world[matrixCoords.Item1][matrixCoords.Item2] as Array[Tile]:
					var tilemap_tile_coords: Vector2i = Vector2i(being_coords.x + i, being_coords.y + j)
					
					if not loaded_tiles[matrixCoords.Item1][matrixCoords.Item2]:
						set_cell(tile.layer, tilemap_tile_coords, tile.source_id, tile.atlas_pos, tile.alternative_id)
						loaded_tiles[matrixCoords.Item1][matrixCoords.Item2] = tilemap_tile_coords
						loaded_tiles_count += 1
			else:
				#poner otra cosa, como fog, así no queda todo gris
				pass
*/