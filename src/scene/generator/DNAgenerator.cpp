#include "DNAgenerator.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "../../libs/json.hpp"

using json = nlohmann::json;

namespace {

inline double round2(double v) { return std::round(v * 100.0) / 100.0; }

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

inline void lerp3(const float a[3], const float b[3], float t, float out[3]) {
    out[0] = lerp(a[0], b[0], t);
    out[1] = lerp(a[1], b[1], t);
    out[2] = lerp(a[2], b[2], t);
}

} // namespace

namespace SceneGenerator {

void GenerateDNAAiryCentered(
    const std::string& outputPath,
    int  width,
    int  height,
    int  countPairs,
    float radius,
    float stepY,
    float angleStep,
    bool  addBridges,
    float sphereRadius,
    float bridgeRadius
) {
    // --- couleurs (dégradés doux) ---
    const float blueStart[3] = {0.18f, 0.55f, 0.95f};
    const float blueEnd  [3] = {0.30f, 0.80f, 1.00f};
    const float pinkStart[3] = {1.00f, 0.35f, 0.65f};
    const float pinkEnd  [3] = {1.00f, 0.60f, 0.85f};
    const float bridgeCol[3] = {0.60f, 0.60f, 0.70f};

    // --- centrage vertical ---
    // hauteur totale occupée par les paires
    const float totalHeight = (countPairs > 1) ? (countPairs - 1) * stepY : 0.0f;
    // on centre dans [0..height] = startY .. startY+totalHeight
    const float startY = std::max(0.0f, (height - totalHeight) * 0.5f);

    // --- caméra (légèrement reculée et au milieu) ---
    const float camX = width * 0.5f;
    const float camY = height * 0.5f; // au centre vertical de l'image
    const float camZ = -1000.0f;
    const float screenZ = 900.0f;

    // --- centre horizontal & profondeur ---
    const float centerX = width  * 0.5f;
    const float centerZ = 0.0f;

    json scene;
    scene["name"] = "DNA Helix Airy - Centered";
    scene["image"] = {
        {"width",  width},
        {"height", height},
        {"background", {0.05, 0.05, 0.08}}
    };
    scene["camera"] = {
        {"position", {camX, camY, camZ}},
        {"screen_z", screenZ}
    };

    json shapes = json::array();

    // Sol (damier dans ton renderer)
    shapes.push_back({
        {"type", "plane"},
        {"point",  {0, height, 0}},
        {"normal", {0, -1, 0}},
        {"color",  {0.25, 0.25, 0.30}}
    });

    // Génération de l'hélice double
    for (int i = 0; i < countPairs; ++i) {
        const float tNorm = (countPairs > 1) ? float(i) / float(countPairs - 1) : 0.0f;
        const float angle = i * angleStep;
        const float y     = startY + i * stepY;

        // ----- brin bleu -----
        const double x1 = centerX + radius * std::cos(angle);
        const double z1 = centerZ + radius * std::sin(angle);

        float blue[3]; lerp3(blueStart, blueEnd, tNorm, blue);
        shapes.push_back({
            {"type", "sphere"},
            {"position", { round2(x1), round2(y), round2(z1) }},
            {"radius", sphereRadius},
            {"color",  { round2(blue[0]), round2(blue[1]), round2(blue[2]) }}
        });

        // ----- brin rose (opposé de phase) -----
        const double x2 = centerX + radius * std::cos(angle + M_PI);
        const double z2 = centerZ + radius * std::sin(angle + M_PI);

        float pink[3]; lerp3(pinkStart, pinkEnd, tNorm, pink);
        shapes.push_back({
            {"type", "sphere"},
            {"position", { round2(x2), round2(y), round2(z2) }},
            {"radius", sphereRadius},
            {"color",  { round2(pink[0]), round2(pink[1]), round2(pink[2]) }}
        });

        // ----- ponts ADN -----
        if (addBridges) {
            const double xb = (x1 + x2) * 0.5;
            const double zb = (z1 + z2) * 0.5;
            shapes.push_back({
                {"type", "sphere"},
                {"position", { round2(xb), round2(y), round2(zb) }},
                {"radius", bridgeRadius},
                {"color",  { bridgeCol[0], bridgeCol[1], bridgeCol[2] }}
            });
        }
    }

    scene["shapes"] = std::move(shapes);

    // Écriture du JSON
    std::ofstream f(outputPath);
    if (!f.is_open()) {
        std::cerr << " J'arrive pas à ouvrir ton " << outputPath << " pour écriture.\n";
        return;
    }
    f << std::setw(2) << scene;
    std::cout << " ADN scene générée dans " << outputPath << "\n";
}

}
