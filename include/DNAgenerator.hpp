#pragma once
#include <string>

namespace SceneGenerator {

// Génère une scène ADN "airy" centrée verticalement.
// - 2*countPairs sphères (bleu/rose) + ponts optionnels
// - positions arrondies à 2 décimales
// - centrage vertical calculé pour rentrer dans la hauteur donnée
void GenerateDNAAiryCentered(
    const std::string& outputPath,
    int  width        = 1920,
    int  height       = 1080,
    int  countPairs   = 300,   // 300 paires = 600 sphères
    float radius      = 250.0f,
    float stepY       = 15.0f,
    float angleStep   = 0.18f,
    bool  addBridges  = true,
    float sphereRadius= 35.0f,
    float bridgeRadius= 14.0f
);

} // namespace SceneGenerator
