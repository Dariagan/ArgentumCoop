using Godot;
using System;
using System.Collections.Generic;

public partial class TemperateTilePicker: TilePicker
{
    public override List<string> GetTiles(Dictionary<string, object> info)//ojo esto es un dictionary de c#
    {
        List<string> tilesToPlace = new();

        if ((bool)info["continental"] && !(bool)info["peninsuler_caved"] && !(bool)info["away_from_coast"] && (bool)info["beach"])
        {
            tilesToPlace.Add("beach_sand");
        }

        else if ((bool)info["continental"] && !(bool)info["peninsuler_caved"] && (bool)info["away_from_coast"] && (bool)info["lake"])
        {
            tilesToPlace.Add("lake");
        }

        else if (!(bool)info["continental"] || ((bool)info["continental"] && (bool)info["peninsuler_caved"]))
        {
            tilesToPlace.Add("ocean");
        }

        else if ((bool)info["continental"] && !(bool)info["peninsuler_caved"])
        {
            tilesToPlace.Add("grass");
        }

        return tilesToPlace;
    }
}
