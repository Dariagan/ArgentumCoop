using Godot;
using System;

public partial class ProceduralTileMap : TileMap
{
	private string[,] WorldMatrix = new string[1500,1500];

	public override void _Ready()
	{
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}

	public void GenerateWorld(Godot.Collections.Dictionary data = null, int seed = 0){
		
		ProceduralGenerator pg = (ProceduralGenerator)GD.Load("res://resources/world/temperate_continent_generator.tres");
		
		//ProceduralGenerator pg = (ProceduralGenerator)script.New();
		pg.Generate(WorldMatrix, Vector2I.Zero, new Vector2I(1000, 1000));
	}

	public void LoadTilesAround(Vector2 coord){


	}
}
