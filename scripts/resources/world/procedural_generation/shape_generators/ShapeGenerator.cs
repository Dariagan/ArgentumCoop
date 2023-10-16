using Godot;
using Godot.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
public abstract partial class ShapeGenerator: CSharpScript
{
	public abstract List<string>[,] Generate(CSharpScript tilePickerScript, List<string>[,] worldMatrix, Vector2I generationOrigin, Vector2I generationSize, Godot.Collections.Dictionary data = null, int seed = 0);
}
