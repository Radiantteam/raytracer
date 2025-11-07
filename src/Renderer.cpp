/**
 * ============================================================================
 * Renderer.cpp - Main Rendering Pipeline with Multithreading
 * ============================================================================
 *
 * WHAT THIS FILE DOES:
 * This is the "conductor" of the ray tracing orchestra. It coordinates all the
 * pieces (scene setup, camera configuration, ray tracing, anti-aliasing, and
 * multithreading) to produce the final rendered image. Think of it as the main
 * control center that orchestrates the entire rendering process.
 *
 * THE BIG PICTURE - RENDERING PIPELINE:
 *
 * 1. SCENE SETUP
 *    └─> Load shapes (spheres, planes) from JSON or generate randomly
 *
 * 2. CAMERA CONFIGURATION
 *    └─> Position camera, calculate field of view, set up viewport
 *
 * 3. PIXEL RENDERING (multithreaded)
 *    └─> For each pixel: cast rays → trace scene → compute color
 *
 * 4. IMAGE OUTPUT
 *    └─> Write final image to PNG file
 *
 * KEY ALGORITHMS USED:
 *
 * 1. **Perspective Camera Model**
 *    Simulates how a real camera sees the world with depth and perspective.
 *
 *    Components:
 *    - Camera Origin: The "eye" position in 3D space
 *    - Field of View (FOV): How wide the camera "sees" (like zoom on a camera)
 *    - Viewport: Virtual screen in 3D space that rays pass through
 *
 *    How it works:
 *    - Calculate viewport size based on FOV and aspect ratio
 *    - For each pixel, compute where it maps on the virtual viewport
 *    - Cast ray from camera through that viewport point
 *    - Objects further away appear smaller (perspective)
 *
 *    Math:
 *    viewportHeight = 2.0 × tan(FOV/2)
 *    viewportWidth = viewportHeight × aspectRatio
 *
 * 2. **Multithreaded Rendering (Work Division)**
 *    Speeds up rendering by using multiple CPU cores simultaneously.
 *
 *    Strategy: Horizontal Scanline Division
 *    - Divide image into horizontal strips (one per CPU thread)
 *    - Each thread renders its assigned rows independently
 *    - No shared data between threads → thread-safe without locks
 *
 *    Example with 4 threads on 1080 pixel height:
 *    Thread 0: rows    0 -  269
 *    Thread 1: rows  270 -  539
 *    Thread 2: rows  540 -  809
 *    Thread 3: rows  810 - 1079
 *
 *    Why this works:
 *    - Each thread writes to different rows → no race conditions
 *    - Balanced workload (each thread gets ~same number of pixels)
 *    - Near-linear speedup (4 threads ≈ 4× faster)
 *
 * 3. **Ray Generation for Each Pixel**
 *    Converts 2D pixel coordinates to 3D rays in world space.
 *
 *    Steps:
 *    a) Normalize pixel coords to [0, 1] range: u = x/(width-1), v = y/(height-1)
 *    b) Map to viewport: point = lowerLeft + horizontal*u + vertical*v
 *    c) Ray direction: normalize(point - cameraOrigin)
 *    d) Cast ray and trace through scene (handled by AntiAliasing class)
 *
 * HOW THE RENDERER WORKS (Complete Flow):
 *
 * Step 1: USER INPUT
 *         - Choice 1: Load scene from JSON file (predefined spheres)
 *         - Choice 2: Generate random spheres procedurally
 *
 * Step 2: SCENE CONSTRUCTION
 *         - Load/generate shapes (spheres)
 *         - Add ground plane with checkerboard pattern
 *         - Plane position adjusted based on sphere positions
 *
 * Step 3: CAMERA SETUP
 *         - Position: (width/2, 0, -2500) - centered, looking down +Z axis
 *         - FOV: 50 degrees (balanced between wide-angle and realistic)
 *         - Calculate viewport dimensions from FOV and aspect ratio
 *         - Compute viewport vectors (horizontal, vertical, lowerLeftCorner)
 *
 * Step 4: ANTI-ALIASING CONFIGURATION
 *         - Create AntiAliasing object with samplesPerAxis (default: 4)
 *         - This determines quality vs speed trade-off
 *
 * Step 5: MULTITHREADED RENDERING
 *         - Detect available CPU cores (hardware_concurrency)
 *         - Divide image height into chunks (one per thread)
 *         - Launch threads with lambda function capturing scene data
 *         - Each thread:
 *           * Iterates through its assigned rows
 *           * For each pixel in row:
 *             - Calls AntiAliasing::SamplePixel()
 *             - Gets anti-aliased color
 *             - Writes to image buffer
 *         - Wait for all threads to complete (join)
 *
 * Step 6: OUTPUT
 *         - Write image buffer to PNG file
 *         - Print elapsed rendering time
 *
 * COORDINATE SYSTEM:
 *
 * - X: Left to right (0 to width)
 * - Y: Top to bottom in screen space, but in world space higher Y = lower position
 * - Z: Camera looks toward +Z (into the scene)
 *
 * PERFORMANCE OPTIMIZATIONS:
 *
 * 1. Multithreading: Parallelizes pixel rendering across CPU cores
 * 2. Horizontal division: Cache-friendly memory access pattern
 * 3. Lambda capture by reference: Avoids copying large scene data
 * 4. Pre-computed camera vectors: Calculate once, reuse for all pixels
 * 5. Timer: Helps measure and optimize rendering performance
 *
 * THREAD SAFETY:
 *
 * - Each thread writes to DIFFERENT rows → no race conditions
 * - Image::SetPixel() doesn't need locks (different memory locations)
 * - Scene data is read-only after setup → safe to share across threads
 * - AntiAliasing and Ray objects created per-pixel → no sharing
 *
 * IMPORTANT PARAMETERS:
 *
 * - samplesPerAxis: 2-8 (quality vs speed)
 * - FOV: 40-70 degrees (narrow to wide view)
 * - Recursion depth: 5 (in Ray.cpp)
 * - Camera position: Affects perspective and visible objects
 */

#include <vector>
#include <memory>
#include <cmath>
#include <thread> // <-- 1. ADDED: Include for multithreading
#include <filesystem>
#include <random>
#include <iostream>

#include "json.hpp"
#include "Color.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "Plane.hpp"
#include "Shape.hpp"
#include "Vec3.hpp"
#include "Renderer.hpp"
#include "ShapeGenerator.hpp"
#include "AntiAliasing.hpp"
#include "SceneLoader.hpp"
#include "DNAgenerator.hpp"
#include "../include/Timer.hpp"

void render_scene(int width, int height, float screenZ, const char *outputFile)
{
    Image image(width, height);

    std::vector<std::unique_ptr<Shape>> scene;

    std::cout << "Choisir un mode:\n";
    std::cout << "1. Générer une scène JSON\n";
    std::cout << "2. Générer un nombre de sphères random\n";
    std::cout << "> ";

    int choice;
    std::cin >> choice;

    scene.push_back(std::make_unique<Plane>(
        Vec3(0, 200.0f, 0),
        Vec3(0, -1, 0),
        0.5f));

    try
    {
        if (choice == 1)
        {
            std::string outputPath = "../scene_boules.json";

            // charge la scène générée
            auto loadedScene = SceneLoader::LoadFromFile(outputPath);
            std::cout << "Loaded " << loadedScene.shapes.size() << " shapes.\n";

            // Keep your default plane, append loaded shapes
            for (auto &s : loadedScene.shapes)
            {
                scene.push_back(std::move(s));
            }

            // Add plane below JSON spheres (which are at Y=300 with radius up to 200)
            scene.push_back(std::make_unique<Plane>(
                Vec3(0, 550.0f, 0), // Below the lowest sphere point (Y=500)
                Vec3(0, -1, 0),
                0.5f));
        }
        else if (choice == 2)
        {
            int sphereCount;
            std::cout << "Combien de sphères veux-tu générer ? ";
            std::cin >> sphereCount;

            if (sphereCount <= 0)
            {
                std::cout << "Aucune sphère à générer, sortie.\n";
                return;
            }

            // Add spheres to the scene
            auto spheres = ShapeGenerator::Generate(sphereCount, width, height);
            for (auto &s : spheres)
                scene.push_back(std::move(s));

            // Add plane below random spheres (which are at Y=1 with radius 150)
            scene.push_back(std::make_unique<Plane>(
                Vec3(0, 200.0f, 0), // Below the lowest sphere point (Y=151)
                Vec3(0, -1, 0),
                0.5f));
        }
        else
        {
            std::cerr << "Invalid option.\n";
            return;
        }

        // Configuration caméra (at Y = 0, same level as spheres)
        Vec3 camOrigin = {width / 2.0f, 0.0f, -2500.0f};

        // Field of View - Ajuste entre 40° (peu de déformation) et 70° (vue plus large)
        float fovDegrees = 50.0f;
        float aspectRatio = (float)width / (float)height;

        // Calcul de la hauteur du plan image à distance 1.0
        float viewportHeight = 2.0f * std::tan((fovDegrees * M_PI / 180.0f) / 2.0f);
        float viewportWidth = viewportHeight * aspectRatio;

        // Vecteurs de la caméra (regardant vers +Z)
        Vec3 w = normalize(Vec3(0, 0, 1)); // Direction avant (vers la scène)
        Vec3 u = normalize(Vec3(1, 0, 0)); // Direction droite
        Vec3 v = normalize(Vec3(0, 1, 0)); // Direction haut (CORRIGÉ: positif au lieu de négatif)

        // Coin inférieur gauche du plan image virtuel
        Vec3 horizontal = u * viewportWidth;
        Vec3 vertical = v * viewportHeight;
        Vec3 lowerLeftCorner = camOrigin + w - horizontal * 0.5f - vertical * 0.5f;

        // ==================== ANTI-ALIASING CONFIGURATION ====================
        // Higher values = smoother edges but slower rendering
        // samplesPerAxis = 2 → 4 rays/pixel (2x2 grid)   - Fast, noticeable improvement
        // samplesPerAxis = 4 → 16 rays/pixel (4x4 grid)  - High quality, recommended
        // samplesPerAxis = 8 → 64 rays/pixel (8x8 grid)  - Ultra quality, very slow
        AntiAliasing antiAliasing(4);

        // ==================== Timer ====================
        Timer renderTimer;

        // ----- 2. REPLACED: Original loop is now multithreaded -----

        // Get number of available threads
        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0)
            numThreads = 2; // Fallback
        std::cout << "Rendu avec " << numThreads << " threads." << std::endl;

        std::vector<std::thread> threads;
        int chunkHeight = height / numThreads;

        // This lambda function contains the exact logic from your original loop
        //
        auto renderChunk = [&](int j_start, int j_end)
        {
            // Raytracing avec projection perspective uniforme
            for (int j = j_start; j < j_end; ++j) // Each thread works on a subset of 'j'
            {
                for (int i = 0; i < width; ++i)
                {
                    Color pixelColor = antiAliasing.SamplePixel(
                        i, j, width, height,
                        camOrigin, lowerLeftCorner, horizontal, vertical,
                        scene);

                    // SetPixel is thread-safe here because no two threads
                    // will ever write to the same 'j' row.
                    image.SetPixel(i, j, pixelColor);
                }
            }
        };

        // Launch threads
        for (int t = 0; t < numThreads; ++t)
        {
            int j_start = t * chunkHeight;
            // Ensure the last thread covers all remaining rows
            int j_end = (t == numThreads - 1) ? height : (t + 1) * chunkHeight;

            threads.emplace_back(renderChunk, j_start, j_end);
        }

        // Wait for all threads to finish
        for (auto &t : threads)
        {
            t.join();
        }
        // ----- END of threading changes -----

        image.WriteFile(outputFile);

        renderTimer.PrintElapsed("Temps de rendu");
    }

    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Working directory: " << std::filesystem::current_path() << "\n";
    }
}
