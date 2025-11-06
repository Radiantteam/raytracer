#pragma once
#include <fstream>
#include <vector>
#include <memory>
#include "json.hpp"
#include "Shape.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cube.hpp"
#include "Vec3.hpp"
#include "Color.hpp"

using json = nlohmann::json;

class SceneLoader
{
public:
    struct SceneData
    {
        int width;
        int height;
        Color background;
        Vec3 cameraPos;
        float screenZ;
        std::vector<std::unique_ptr<Shape>> shapes;
    };

    static SceneData LoadFromFile(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Cannot open: " + filename);

        if (file.peek() == std::ifstream::traits_type::eof())
            throw std::runtime_error("File is empty: " + filename);

        json j;
        try {
            file >> j;
        } catch (const std::exception &e) {
            throw std::runtime_error("Invalid JSON in file: " + filename + " — " + e.what());
        }

        if (!j.contains("image") || !j.contains("camera") || !j.contains("shapes"))
            throw std::runtime_error("Missing required keys in JSON: " + filename);

        SceneData scene;

        // Parse image
        scene.width = j["image"]["width"];
        scene.height = j["image"]["height"];
        auto bg = j["image"]["background"];
        scene.background = Color(bg[0], bg[1], bg[2]);

        // Parse camera
        auto camPos = j["camera"]["position"];
        scene.cameraPos = Vec3{camPos[0], camPos[1], camPos[2]};
        scene.screenZ = j["camera"]["screen_z"];

        // Parse shapes
        scene.width = j["image"]["width"];
        scene.height = j["image"]["height"];

        auto background = j["image"]["background"];
        scene.background = Color(background[0], background[1], background[2]);

        auto cameraPos = j["camera"]["position"];
        scene.cameraPos = Vec3{camPos[0], camPos[1], camPos[2]};
        scene.screenZ = j["camera"]["screen_z"];

        for (const auto &shape : j["shapes"]) {
            std::string type = shape["type"];

            if (type == "sphere") {
                auto pos = shape["position"];
                auto col = shape["color"];
                scene.shapes.push_back(std::make_unique<Sphere>(
                    Vec3{pos[0], pos[1], pos[2]},
                    shape["radius"],
                    Color(col[0], col[1], col[2])));
            } else if (type == "cube") {
                auto pos = shape["position"];
                auto col = shape["color"];
                scene.shapes.push_back(std::make_unique<Cube>(
                    Vec3{pos[0], pos[1], pos[2]},
                    shape["size"],
                    Color(col[0], col[1], col[2])));
            }
            else {
                std::cerr << "Warning: Unknown shape type \"" << type << "\" in scene\n";
            }
        }

        return scene;
    }
};
