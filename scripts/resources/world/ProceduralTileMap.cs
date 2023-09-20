using Godot;
using System;
using System.Collections.Generic;
using System.Linq;

public partial class ProceduralTileMap : TileMap
{
	
	private List<string>[,] WorldMatrix; // hacer un Dictionary<Vector2I, List<string>>?

	
	private Vector2I WorldSize;

	public override void _Ready()
	{

		Node globalData = GetNode("/root/GlobalData");
		Tiles = (Godot.Collections.Dictionary)globalData.Get("tiles");
	}
	/*
	public override void _Process(double delta)
	{
	}*/

	public void GenerateWorld(Godot.Collections.Dictionary data = null, int seed = 0){
		
		Vector2I worldSize = new(5000, 5000);
		WorldSize = worldSize;
		

		WorldMatrix = new List<string>[WorldSize.X, WorldSize.Y];

		ProceduralGenerator pg = (ProceduralGenerator)GD.Load("res://resources/world/temperate_continent_generator.tres");
		
		//ProceduralGenerator pg = (ProceduralGenerator)script.New();
		pg.Generate(WorldMatrix, Vector2I.Zero, new Vector2I(4000, 4000));
	}

	private HashSet<Vector2I> LoadedTiles = new();

	private Godot.Collections.Dictionary Tiles;

	public void LoadTilesAround(Vector2 coords){

		Vector2I beingCoords = LocalToMap(coords);

		Vector2I chunkSize = new(90, 53);

		for (int i = -chunkSize.X/2; i < chunkSize.X/2; i++) 
		{
			for (int j = -chunkSize.Y/2; j < chunkSize.Y/2; j++) 
			{
				Vector2I matrixCoords = new(WorldSize.X/2 + beingCoords.X + i, WorldSize.Y/2 + beingCoords.Y + j);
                if (matrixCoords.X < (WorldSize.X - 1) && matrixCoords.Y < (WorldSize.Y - 1) && matrixCoords.X >= 0 && matrixCoords.Y >= 0)
                    foreach (string tile_id in WorldMatrix[matrixCoords.X, matrixCoords.Y])
					{
						Resource tile = (Resource)Tiles[tile_id] ;
						Vector2I tilemap_tile_coords = new Vector2I(beingCoords.X + i, beingCoords.Y + j);
						
						if (!LoadedTiles.Contains(tilemap_tile_coords))
						{
							SetCell((int)tile.Call("get_layer"), tilemap_tile_coords, (int)tile.Call("get_source_id"), (Vector2I)tile.Call("get_atlas_pos"), (int)tile.Call("get_alt_id"));
							LoadedTiles.Add(tilemap_tile_coords);
						}
					}
				//else poner otra cosa, como fog, así no queda todo gris
			}
		}
		UnloadExcessTiles(beingCoords, LoadedTiles);
	}
	
	private void UnloadExcessTiles(Vector2I beingCoords, HashSet<Vector2I> loadedTiles)
	{
		const int MAX_LOADED_TILES = 30000;

		if (loadedTiles.Count > MAX_LOADED_TILES)
		{
			foreach (Vector2I tile in LoadedTiles)
			{
				if (((Vector2)tile).DistanceSquaredTo(beingCoords) > 27000)
				{
					for (int layer_i = 0; layer_i < GetLayersCount(); layer_i++)
					{
						EraseCell(layer_i, tile);
					}
					loadedTiles.Remove(tile);	
				}			
			}
		}
	}
}