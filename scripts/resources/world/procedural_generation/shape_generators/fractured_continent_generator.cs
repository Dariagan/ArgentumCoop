
/*
using Godot;
using Godot.Collections;
using System;
using System.Collections.Generic;


public partial class fractured_continent_generator : RefCounted
{
    private static TileMap _tileMap;

    public static void generate(TileMap tileMap, Script tilePicker, Vector2I center, Vector2I size, Dictionary<string, object> extraInfo, int seed = 0)
    {
        _tileMap = tileMap;

        FastNoiseLite continenter = new FastNoiseLite();
        FastNoiseLite peninsuler = new FastNoiseLite();
        FastNoiseLite laker = new FastNoiseLite();
        FastNoiseLite cBeacher = new FastNoiseLite();
        FastNoiseLite pBeacher = new FastNoiseLite();

        continenter.Seed = seed;
        peninsuler.Seed = seed;
        laker.Seed = seed;
        cBeacher.Seed = seed;
        pBeacher.Seed = seed + 1;

        RandomNumberGenerator rng = new RandomNumberGenerator();
        rng.Seed = (ulong)seed;

        continenter.NoiseType = FastNoiseLite.NoiseTypeEnum.Simplex;
        continenter.Frequency = 0.14f / size.Length();

        continenter.FractalLacunarity = 2.8f;
        continenter.FractalWeightedStrength = 0.5f;
        float continentalCutoff = 0.6f * Mathf.Pow(size.Length() / 1600f, 0.05f);

        peninsuler.NoiseType = FastNoiseLite.NoiseTypeEnum.Simplex;
        peninsuler.Frequency = 5f / size.Length();
        peninsuler.FractalGain = 0.56f;
        const float peninsulerCutoff = -0.1f;

        cBeacher.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;
        cBeacher.Frequency = 5f / size.Length();
        const float beachCutoff = 0.8f;

        pBeacher.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;
        pBeacher.Frequency = 10f / size.Length();
        pBeacher.FractalOctaves = 1;

        laker.Frequency = 15f / size.Length();
        const float lakeCutoff = 0.5f;

        while (continenter.GetNoise2Dv(center) < continentalCutoff + 0.13f)
        {
            continenter.Offset.X +=3;
            continenter.Offset.Y += 3;
        }

        PlaceDungeonEntrances(size, center, rng, continenter, continentalCutoff, peninsuler, peninsulerCutoff, laker, lakeCutoff);

        for (int i = -size.X / 2; i < size.X / 2; i++)
        {
            for (int j = -size.Y / 2; j < size.Y / 2; j++)
            {
                float bcf = GetBCF(i, j, size, center);
                float continentness = GetContinentness(i, j, continenter, bcf);
                bool continental = continentness > continentalCutoff;
                bool peninsulerCaved = peninsuler.GetNoise2D(i, j) < peninsulerCutoff;
                bool awayFromCoast = continentness > continentalCutoff + 0.05f && peninsuler.GetNoise2D(i, j) > peninsulerCutoff + 0.27f;
                float beachness = GetBeachness(i, j, cBeacher, continentness, continentalCutoff, pBeacher, peninsuler, peninsulerCutoff);
                bool beach = beachness > beachCutoff;
                bool lake = laker.GetNoise2D(i, j) / 1.3f + 1 - Mathf.Pow(beachness, 0.6f) > lakeCutoff - 0.04f;
                Dictionary<string, object> info = new Dictionary<string, object>
                {
                    ["continental"] = continental,
                    ["peninsuler_caved"] = peninsulerCaved,
                    ["away_from_coast"] = awayFromCoast,
                    ["lake"] = lake,
                    ["beach"] = beach
                };
                PlaceTiles(new Vector2I(i, j) + center, tilePicker.GetTiles(info));
            }
        }
    }

    public static void PlaceTiles(Vector2I coords, List<string> tilesToPlace)
    {
        foreach (string tileId in tilesToPlace)
        {
            Tile tile = GlobalData.tiles[tileId];
            _tileMap.SetCell(tile.Layer, coords, tile.SourceId, tile.AtlasPos, tile.AlternativeId);
        }
    }

    public static void PlaceDungeonEntrances(Vector2I size, Vector2I center, RandomNumberGenerator rng, FastNoiseLite continenter, float continentalCutoff, FastNoiseLite peninsuler, float peninsulerCutoff, FastNoiseLite laker, float lakeCutoff)
    {
        int ri = rng.RandiRange(-size.X / 2, size.X / 2);
        int rj = rng.RandiRange(-size.Y / 2, size.Y / 2);
        List<Vector2I> dungeonCoords = new List<Vector2I>();
        int tries = 0;
        float minDistanceMult = 1;

        while (dungeonCoords.Count < 3)
        {
            ri = rng.RandiRange(-size.X / 2, size.X / 2);
            rj = rng.RandiRange(-size.Y / 2, size.Y / 2);
            tries++;

            if (GetContinentness(ri, rj, continenter, GetBCF(ri, rj, size, center)) > continentalCutoff + 0.032f && peninsuler.GetNoise2D(ri, rj) > peninsulerCutoff + 0.1f && laker.GetNoise2D(ri, rj) < lakeCutoff - 0.04f)
            {
                bool farFromDungeons = true;

                foreach (Vector2I coord in dungeonCoords)
                {
                    if (farFromDungeons && Vector2.Distance(new Vector2(ri, rj), coord) < size.Length() * 0.25f * minDistanceMult)
                    {
                        farFromDungeons = false;
                    }
                }

                if (farFromDungeons)
                {
                    PlaceTiles(center + new Vector2I(ri, rj), new List<string> { "cave_mossy" });
                    dungeonCoords.Add(new Vector2I(ri, rj));
                }
                else
                {
                    minDistanceMult = Mathf.Clamp(1500f / tries, 0f, 1f);
                }
            }
        }
    }

    public static float GetBCF(int i, int j, Vector2I size, Vector2I center)
    {
        return Mathf.Max(Mathf.Abs(i - center.X) / (size.X / 2f), Mathf.Abs(j - center.Y) / (size.Y / 2f));
    }

    public static float GetContinentness(int i, int j, FastNoiseLite continenter, float bcf)
    {
        return continenter.GetNoise2D(i, j) - Mathf.Pow(bcf, 43) - bcf / 4.7f;
    }

    public static float GetBeachness(int i, int j, FastNoiseLite cBeacher, float continentness, float continentalCutoff, FastNoiseLite pBeacher, FastNoiseLite peninsuler, float peninsulerCutoff)
    {
        return Mathf.Max(0.72f + cBeacher.GetNoise2D(i, j) / 2.3f - Mathf.Pow((continentness - continentalCutoff), 0.55f), 0.8f + pBeacher.GetNoise2D(i, j) / 2.3f - Mathf.Pow((peninsuler.GetNoise2D(i, j) - peninsulerCutoff), 0.6f));
    }
}
*/