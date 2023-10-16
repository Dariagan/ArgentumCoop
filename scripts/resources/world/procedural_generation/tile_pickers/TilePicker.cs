using System.Collections.Generic;
using Godot;

public abstract partial class TilePicker: CSharpScript
{
	public abstract List<string> GetTiles(Dictionary<string, object> info);//ojo esto es un dictionary de c#
}
