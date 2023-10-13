
using Godot;
using System;
using System.Collections.Generic;
using System.Linq;

//recommended size: 4000x4000
public partial class FracturedContinentGenerator: ShapeGenerator
{
    private List<string>[,] WorldMatrix;
    private Vector2I GenerationOrigin;

    private Vector2I GenerationSize;

    private FastNoiseLite Continenter = new();
    private FastNoiseLite Peninsuler = new();
    private FastNoiseLite BigLaker = new();
    private FastNoiseLite SmallLaker = new();
    private FastNoiseLite BigBeacher = new();
    private FastNoiseLite SmallBeacher = new();
    private FastNoiseLite ForestMap = new();

    public override List<string>[,] Generate(CSharpScript tilePickerScript, List<string>[,] worldMatrix, Vector2I generationOrigin, Vector2I generationSize, Godot.Collections.Dictionary data = null, int seed = 0)
    {
        WorldMatrix = worldMatrix;
        GenerationOrigin = generationOrigin;
        GenerationSize = generationSize;

        Continenter.Seed = seed;
        Peninsuler.Seed = seed + 1;
        BigLaker.Seed = seed + 2;
        SmallLaker.Seed = seed + 3;
        BigBeacher.Seed = seed + 4;
        SmallBeacher.Seed = seed + 5;
        ForestMap.Seed = seed + 6;

        RandomNumberGenerator rng = new(){Seed = (ulong)seed};

        Continenter.NoiseType = FastNoiseLite.NoiseTypeEnum.Simplex;
        Continenter.Frequency = 0.15f / Mathf.Pow(generationSize.Length(), 0.995f);

        Continenter.FractalLacunarity = 2.8f;
        Continenter.FractalWeightedStrength = 0.5f;
        float continentalCutoff = 0.6f * Mathf.Pow(generationSize.Length() / 1600f, 0.05f);

        Peninsuler.NoiseType = FastNoiseLite.NoiseTypeEnum.Simplex;
        Peninsuler.Frequency = 5.5f / Mathf.Pow(generationSize.Length(), 0.995f);
        Peninsuler.FractalGain = 0.56f;
        const float peninsulerCutoff = -0.1f;

        BigBeacher.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;
        BigBeacher.Frequency = 3f / Mathf.Pow(generationSize.Length(), 0.99f);
        float beachCutoff = 0.8f;

        ForestMap.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;

        SmallBeacher.NoiseType = FastNoiseLite.NoiseTypeEnum.SimplexSmooth;
        SmallBeacher.Frequency = 9f / Mathf.Pow(generationSize.Length(), 0.995f);
        SmallBeacher.FractalOctaves = 1;

        BigLaker.NoiseType = FastNoiseLite.NoiseTypeEnum.ValueCubic;
        BigLaker.FractalOctaves = 2;
        BigLaker.FractalLacunarity = 1.3f;
        BigLaker.Frequency = 110 / Mathf.Pow(generationSize.Length(), 0.995f);
        const float bigLakeCutoff = -.9f;

        SmallLaker.NoiseType = FastNoiseLite.NoiseTypeEnum.ValueCubic;
        SmallLaker.FractalOctaves = 2;
    
        SmallLaker.Frequency = 240 / Mathf.Pow(generationSize.Length(), 0.991f);
        const float smallLakeCutoff = -.9f;

        while (Continenter.GetNoise2Dv(GenerationOrigin) < continentalCutoff + 0.13f)
        {
            Continenter.Offset += new Vector3(3, 3, 0);
        }
        
        
        TilePicker tilePicker = (TilePicker)tilePickerScript.New();
        for (int i = -generationSize.X / 2; i < generationSize.X / 2; i++)
        {
            for (int j = -generationSize.Y / 2; j < generationSize.Y / 2; j++)
            {
                float bcf = GetBorderClosenessFactor(i, j);
                float continentness = GetContinentness(i, j, Continenter, bcf);
                bool continental = continentness > continentalCutoff;
                bool peninsulerCaved = Peninsuler.GetNoise2D(i, j) < peninsulerCutoff;
                bool awayFromCoast = continentness > continentalCutoff + 0.01 && Peninsuler.GetNoise2D(i, j) > peninsulerCutoff + 0.27;
                float beachness = GetBeachness(i, j, BigBeacher, continentness, continentalCutoff, SmallBeacher, Peninsuler, peninsulerCutoff);
                bool beach = beachness > beachCutoff;

                float bigLakerNoise = (float)((BigLaker.GetNoise2D(i, j) + 1)*0.65);
                float smallLakerNoise = (float)((SmallLaker.GetNoise2D(i, j) + 1)*0.65);
                

                bool lake = (((SmallLaker.GetNoise2D(i, j) + 1)*0.65) - beachness > smallLakeCutoff) || (((BigLaker.GetNoise2D(i, j) + 1)*0.65) - beachness > bigLakeCutoff);                
                

                Dictionary<string, object> info = new()
                {
                    ["continental"] = continental,
                    ["peninsuler_caved"] = peninsulerCaved,
                    ["away_from_coast"] = awayFromCoast,
                    ["lake"] = lake,
                    ["beach"] = beach
                };

                /*if (new Vector2I(i, j) + generationCenter == new Vector2I(-127, -249)){
                    GD.Print(beachness);
                    GD.Print(smallLaker.GetNoise2D(i, j) / 1.3f + 1);
                    GD.Print(bigLaker.GetNoise2D(i, j) / 1.3f + 0.8);
                    GD.Print(Mathf.Pow(beachness, 0.6f));

                    PlaceTile(new Vector2I(i, j) + generationCenter, "ocean");
                }else*/
                foreach (string tileId in tilePicker.GetTiles(info))
                {   
                    PlaceTile((i, j), tileId);
                }
            }
        }

        PlaceDungeonEntrances();

        return WorldMatrix;
    }

    private void PlaceTile(Vector2I coordsRelativeToCenter, string tileId)
    {PlaceTile((coordsRelativeToCenter.X, coordsRelativeToCenter.Y), tileId);}
    private void PlaceTile((int, int) coordsRelativeToCenter, string tileId)
    {
        (int, int) absoluteCoords = 
        (coordsRelativeToCenter.Item1 + GenerationOrigin.X + WorldMatrix.GetLength(0)/2, 
        coordsRelativeToCenter.Item2 + GenerationOrigin.Y + WorldMatrix.GetLength(1)/2);

        if (WorldMatrix[absoluteCoords.Item1, absoluteCoords.Item2] == null)
        {
            WorldMatrix[absoluteCoords.Item1, absoluteCoords.Item2] = new List<string>(2);  
        }
        WorldMatrix[absoluteCoords.Item1, absoluteCoords.Item2].Add(tileId);
    }

    private void PlaceDungeonEntrances(RandomNumberGenerator rng)
    {
        int ri;
        int rj;
        
        List<Vector2I> dungeonsCoords = new();
        int tries = 0;
        float minDistanceMult = 1;

        while (dungeonsCoords.Count < 3)
        {
            ri = rng.RandiRange(-GenerationSize.X / 2, GenerationSize.X / 2);
            rj = rng.RandiRange(-GenerationSize.Y / 2, GenerationSize.Y / 2);
            Vector2I newDungeonCoords = new(ri, rj);

            tries++;
            //TODO en vez de esto, hacer q se fije en la worldmatrix si esta clear el area adyacente a la posición, 
            //así no dependes de la lógica y/o parámetros de generación para la casilla y no hay q reajustar
            if ()
            {
                bool farFromDungeons = true;

                foreach (Vector2I coord in dungeonsCoords)
                {
                    if (((Vector2)newDungeonCoords).DistanceTo(coord) <  GenerationSize.Length() * 0.25f * minDistanceMult)
                    {
                        farFromDungeons = false;
                        break;
                    }
                }
                if (farFromDungeons)
                {
                    PlaceTile(newDungeonCoords, "cave_mossy");
                    dungeonsCoords.Add(newDungeonCoords);
                }
                else
                {
                    minDistanceMult = Mathf.Clamp(1500f / tries, 0f, 1f);
                }
            }
            if (tries == 1000000)
                GD.PrintErr("dungeon placement condition unmeetable");
        }
    }

    private float GetBorderClosenessFactor(int i, int j)
    {
        return Math.Max(Mathf.Abs(i - GenerationOrigin.X) / (GenerationSize.X / 2f), Mathf.Abs(j - GenerationOrigin.Y) / (GenerationSize.Y / 2f));
    }

    private float GetContinentness(int i, int j, FastNoiseLite continenter, float bcf)
    {
        return continenter.GetNoise2D(i, j) - Mathf.Pow(bcf, 43) - bcf / 4.2f;
    }

    private float GetBeachness(int i, int j,
    FastNoiseLite cBeacher, float continentness, float continentalCutoff, 
    FastNoiseLite pBeacher, FastNoiseLite peninsuler, float peninsulerCutoff
    ){
        return Math.Max(
            0.72f + cBeacher.GetNoise2D(i, j) / 2.3f - Mathf.Pow(continentness - continentalCutoff, 0.6f), 
            0.8f + pBeacher.GetNoise2D(i, j) / 2.3f - Mathf.Pow(peninsuler.GetNoise2D(i, j) - peninsulerCutoff, 0.45f));
    }

}
