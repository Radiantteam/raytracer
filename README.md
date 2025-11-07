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
## Construction image docker
```bash
docker build -t raytracer_image -f Dockerfile .
```

## Lancement du la commande via le conteneur
```bash
docker run --rm -it -v "$(pwd)/output:/output" raytracer_image
```

