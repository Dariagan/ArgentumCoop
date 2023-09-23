
using Godot;
using System;
using System.Collections.Generic;
using System.Linq;

//recommended size: 4000x4000
public partial class FracturedContinentGenerator: ShapeGenerator
{
    private List<string>[,] WorldMatrix;

    public override List<string>[,] Generate(CSharpScript tilePickerScript, List<string>[,] worldMatrix, Vector2I generationCenter, Vector2I generationSize, Godot.Collections.Dictionary data = null, int seed = 0)
    {
        WorldMatrix = worldMatrix;

        FastNoiseLite continenter = new();
        FastNoiseLite peninsuler = new();
        FastNoiseLite bigLaker = new();
        FastNoiseLite smallLaker = new();
        FastNoiseLite bigBeacher = new();
        FastNoiseLite smallBeacher = new();
        FastNoiseLite treeDensity = new();

        continenter.Seed = seed;
        peninsuler.Seed = seed + 1;
        bigLaker.Seed = seed + 2;
        smallLaker.Seed = seed + 3;
        bigBeacher.Seed = seed + 4;
        smallBeacher.Seed = seed + 5;
        treeDensity.Seed = seed + 6;

        RandomNumberGenerator rng = new(){Seed = (ulong)seed};

        continenter.NoiseType = FastNoiseLite.NoiseTypeEnum.Simplex;
        continenter.Frequency = 0.15f / Mathf.Pow(generationSize.Length(), 0.995f);

        continenter.FractalLacunarity = 2.8f;
        continenter.FractalWeightedStrength = 0.5f;
        float continentalCutoff = 0.6f * Mathf.Pow(generationSize.Length() / 1600f, 0.05f);

        peninsuler.NoiseType = FastNoiseLite.NoiseTypeEnum.Simplex;
        peninsuler.Frequency = 5.5f / Mathf.Pow(generationSize.Length(), 0.995f);
        peninsuler.FractalGain = 0.56f;
        const float peninsulerCutoff = -0.1f;

        bigBeacher.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;
        bigBeacher.Frequency = 3f / Mathf.Pow(generationSize.Length(), 0.99f);
        float beachCutoff = 0.8f;

        treeDensity.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;

        smallBeacher.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;
        smallBeacher.Frequency = 9f / Mathf.Pow(generationSize.Length(), 0.995f);
        smallBeacher.FractalOctaves = 1;

        bigLaker.NoiseType = FastNoiseLite.NoiseTypeEnum.ValueCubic;
        bigLaker.FractalOctaves = 2;
        bigLaker.FractalLacunarity = 1.3f;
        bigLaker.Frequency = 110 / Mathf.Pow(generationSize.Length(), 0.995f);
        const float bigLakeCutoff = 0.2f;

        smallLaker.NoiseType = FastNoiseLite.NoiseTypeEnum.ValueCubic;
        smallLaker.FractalOctaves = 2;
        
        smallLaker.Frequency = 230 / Mathf.Pow(generationSize.Length(), 0.991f);
        const float smallLakeCutoff = 0.4f;

        while (continenter.GetNoise2Dv(generationCenter) < continentalCutoff + 0.13f)
        {
            continenter.Offset += new Vector3(3, 3, 0);
        }
        PlaceDungeonEntrances(generationSize, generationCenter, rng, continenter, continentalCutoff, peninsuler, peninsulerCutoff, smallLaker, smallLakeCutoff);
        
        TilePicker tilePicker = (TilePicker)tilePickerScript.New();
        for (int i = -generationSize.X / 2; i < generationSize.X / 2; i++)
        {
            for (int j = -generationSize.Y / 2; j < generationSize.Y / 2; j++)
            {
                float bcf = GetBCF(i, j, generationSize, generationCenter);
                float continentness = GetContinentness(i, j, continenter, bcf);
                bool continental = continentness > continentalCutoff;
                bool peninsulerCaved = peninsuler.GetNoise2D(i, j) < peninsulerCutoff;
                bool awayFromCoast = continentness > continentalCutoff + 0.01 && peninsuler.GetNoise2D(i, j) > peninsulerCutoff + 0.27;
                float beachness = GetBeachness(i, j, bigBeacher, continentness, continentalCutoff, smallBeacher, peninsuler, peninsulerCutoff);
                bool beach = beachness > beachCutoff;

                bool lake = (smallLaker.GetNoise2D(i, j) / 1.3f + 1 - Mathf.Pow(beachness, 0.6f) > smallLakeCutoff) || (bigLaker.GetNoise2D(i, j) / 1.3f + 0.8 - Mathf.Pow(beachness, 0.6f) > bigLakeCutoff);

                Dictionary<string, object> info = new()
                {
                    ["continental"] = continental,
                    ["peninsuler_caved"] = peninsulerCaved,
                    ["away_from_coast"] = awayFromCoast,
                    ["lake"] = lake,
                    ["beach"] = beach
                };

                foreach (string tileId in tilePicker.GetTiles(info))
                {   
                    PlaceTile(new Vector2I(i, j) + generationCenter, tileId);
                }
            }
        }
     
        return WorldMatrix;
    }

    private Dictionary<string, object> GetData(Vector2I pos){
        return null;
    }

    private void PlaceTile(Vector2I coords, string tileId)
    {
        if (WorldMatrix[coords.X + WorldMatrix.GetLength(0)/2, coords.Y + WorldMatrix.GetLength(1)/2] == null)
        {
            WorldMatrix[coords.X + WorldMatrix.GetLength(0)/2, coords.Y + WorldMatrix.GetLength(1)/2] = new List<string>(2);  
        }
        WorldMatrix[coords.X + WorldMatrix.GetLength(0)/2, coords.Y + WorldMatrix.GetLength(1)/2].Add(tileId);
    }

    private void PlaceDungeonEntrances(Vector2I size, Vector2I center, RandomNumberGenerator rng, FastNoiseLite continenter, float continentalCutoff, FastNoiseLite peninsuler, float peninsulerCutoff, FastNoiseLite laker, float lakeCutoff)
    {
        int ri;
        int rj;
        
        List<Vector2I> dungeonsCoords = new();
        int tries = 0;
        float minDistanceMult = 1;

        while (dungeonsCoords.Count < 3)
        {
            ri = rng.RandiRange(-size.X / 2, size.X / 2);
            rj = rng.RandiRange(-size.Y / 2, size.Y / 2);
            Vector2I newDungeonCoords = new(ri, rj);

            tries++;

            if (GetContinentness(ri, rj, continenter, GetBCF(ri, rj, size, center)) > continentalCutoff + 0.032f && peninsuler.GetNoise2D(ri, rj) > peninsulerCutoff + 0.1f && laker.GetNoise2D(ri, rj) < lakeCutoff - 0.04f)
            {
                bool farFromDungeons = true;

                foreach (Vector2I coord in dungeonsCoords)
                {
                    if (((Vector2)newDungeonCoords).DistanceTo(coord) <  size.Length() * 0.25f * minDistanceMult)
                    {
                        farFromDungeons = false;
                        break;
                    }
                }
                if (farFromDungeons)
                {
                    PlaceTile(center + newDungeonCoords, "cave_mossy");
                    dungeonsCoords.Add(newDungeonCoords);
                }
                else
                {
                    minDistanceMult = Mathf.Clamp(1500f / tries, 0f, 1f);
                }
            }
        }
    }

    private static float GetBCF(int i, int j, Vector2I size, Vector2I center)
    {
        return Math.Max(Mathf.Abs(i - center.X) / (size.X / 2f), Mathf.Abs(j - center.Y) / (size.Y / 2f));
    }

    private static float GetContinentness(int i, int j, FastNoiseLite continenter, float bcf)
    {
        return continenter.GetNoise2D(i, j) - Mathf.Pow(bcf, 43) - bcf / 4.2f;
    }

    private static float GetBeachness(int i, int j, FastNoiseLite cBeacher, float continentness, float continentalCutoff, FastNoiseLite pBeacher, FastNoiseLite peninsuler, float peninsulerCutoff)
    {
        return Math.Max(
            0.72f + cBeacher.GetNoise2D(i, j) / 2.3f - Mathf.Pow(continentness - continentalCutoff, 0.6f), 
            0.8f + pBeacher.GetNoise2D(i, j) / 2.3f - Mathf.Pow(peninsuler.GetNoise2D(i, j) - peninsulerCutoff, 0.45f));
    }

}
