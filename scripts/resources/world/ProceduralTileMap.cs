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
		TilesData = (Godot.Collections.Dictionary)globalData.Get("tiles");
	}
	/*
	public override void _Process(double delta)
	{
	}*/

	public void GenerateWorld(Godot.Collections.Dictionary data = null, int seed = 0)
	{	
		Vector2I worldSize = new(400, 400);
		WorldSize = worldSize;
		
		WorldMatrix = new List<string>[WorldSize.X, WorldSize.Y];

		ProceduralGenerator pg = (ProceduralGenerator)GD.Load("res://resources/world/temperate_continent_generator.tres");
		
		//ProceduralGenerator pg = (ProceduralGenerator)script.New();
		pg.Generate(WorldMatrix, Vector2I.Zero, new Vector2I(400, 400), null, 30);
	}

	private readonly HashSet<Vector2I> LoadedTiles = new();

	private Godot.Collections.Dictionary TilesData;

	//hacer que solo genere chunks en direccion donde esté mirando el being
	public void LoadTilesAround(Vector2 coords){

		Vector2I beingCoords = LocalToMap(coords);

		Vector2I chunkSize = new(100, 60);

		for (int i = -chunkSize.X/2; i < chunkSize.X/2; i++) 
		{
			for (int j = -chunkSize.Y/2; j < chunkSize.Y/2; j++) 
			{
				Vector2I matrixCoords = new(WorldSize.X/2 + beingCoords.X + i, WorldSize.Y/2 + beingCoords.Y + j);
				if (matrixCoords.X < (WorldSize.X - 1) && matrixCoords.Y < (WorldSize.Y - 1) && matrixCoords.X >= 0 && matrixCoords.Y >= 0)
				{
					Vector2I tileMapTileCoords = new(beingCoords.X + i, beingCoords.Y + j);
					if (!LoadedTiles.Contains(tileMapTileCoords))
					{
						if (WorldMatrix[matrixCoords.X, matrixCoords.Y] != null)
						
							foreach (string tile_id in WorldMatrix[matrixCoords.X, matrixCoords.Y])
							{								
								Resource tile = (Resource)TilesData[tile_id];
								Godot.Collections.Dictionary tileData = (Godot.Collections.Dictionary)tile.Call("get_data");

								SetCell((int)tileData["layer"], tileMapTileCoords, (int)tileData["source_id"], (Vector2I)tileData["atlas_pos"], (int)tileData["alt_id"]);

								LoadedTiles.Add(tileMapTileCoords);
							}
						else
							//ocean water
							SetCell(0, tileMapTileCoords, 2, new Vector2I(0,0), 0);
							LoadedTiles.Add(tileMapTileCoords);
					}
				}
			}
		}
		UnloadExcessTiles(beingCoords, LoadedTiles);
	}
	private void UnloadExcessTiles(Vector2I beingCoords, HashSet<Vector2I> loadedTiles)
	{
		const int MAX_LOADED_TILES = 30000;

		if (loadedTiles.Count > MAX_LOADED_TILES)
		{
			foreach (Vector2I tileCoord in LoadedTiles)
			{
				if (((Vector2)tileCoord).DistanceSquaredTo(beingCoords) > 27000)
				{
					for (int layer_i = 0; layer_i < GetLayersCount(); layer_i++)
					{
						EraseCell(layer_i, tileCoord);
					}
					loadedTiles.Remove(tileCoord);	
				}			
			}
		}
	}
}
