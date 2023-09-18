using Godot;
using Godot.Collections;
using System;

public partial class ProceduralGenerator: Resource
{
    [Export]
    private CSharpScript ShapeGenerator;

    [Export]
    private GDScript TilePicker;

    public string[,] Generate(string[,] worldMatrix, Vector2I generationCenter, Vector2I generationSize, Dictionary data = null, int seed = 0)
    {
        return ((ShapeGenerator)ShapeGenerator.New()).Generate(TilePicker, worldMatrix, generationCenter, generationSize, data, seed);
    }
}
