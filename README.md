# Raytracer

## Prérequis
Assurez-vous d'avoir installé :
- CMake (3.20)
- Un compilateur C++ compatible C++23

## Génération du projet
```bash
cmake -S . -B build
```

## Compilation
```bash
cmake --build build -j
```

## Exécution des tests unitaires
```bash
cd build && ctest --output-on-failure
```