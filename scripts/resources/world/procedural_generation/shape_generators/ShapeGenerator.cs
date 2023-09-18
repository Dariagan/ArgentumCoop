using Godot;
using Godot.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
public abstract partial class ShapeGenerator: CSharpScript
{
    public abstract string[,] Generate(GDScript tilePicker, string[,] worldMatrix, Vector2I generationCenter, Vector2I generationSize, Godot.Collections.Dictionary data = null, int seed = 0);
}