#pragma once

#include "vec/Vec3.hpp"
#include "color/Color.hpp"
#include "shape/Shape.hpp"
#include <vector>
#include <memory>

/**
 * Représente un rayon lumineux dans l'espace 3D pour le raytracing.
 * Un rayon est défini par une origine et une direction, permettant de
 * calculer les intersections avec les objets de la scène.
 */
class Ray
{
public:
  /**
   * Construit un rayon avec une origine et une direction données.
   * @param origin Point de départ du rayon dans l'espace 3D
   * @param direction Vecteur directionnel du rayon (devrait être normalisé)
   */
  Ray(const Vec3 &origin, const Vec3 &direction);

  /**
   * Lance le rayon à travers la scène et calcule la couleur résultante.
   * Parcourt toutes les formes, trouve la plus proche intersectée,
   * et retourne sa couleur avec ombrage. Retourne noir si aucune intersection.
   * @param scene Liste des formes présentes dans la scène
   * @return Couleur du pixel résultant du lancer de rayon
   */
  Color TraceScene(const std::vector<std::unique_ptr<Shape>> &scene) const;

  /**
   * Retourne le point d'origine du rayon.
   * @return Position de départ du rayon
   */
  Vec3 GetOrigin() const { return _origin; }

  /**
   * Retourne la direction du rayon.
   * @return Vecteur directionnel du rayon
   */
  Vec3 GetDirection() const { return _direction; }

  /**
   * Calcule un point le long du rayon à une distance t de l'origine.
   * Formule : P(t) = origin + t * direction
   * @param t Distance paramétrique le long du rayon
   * @return Point 3D situé à la distance t sur le rayon
   */
  Vec3 PointAt(float t) const { return _origin + _direction * t; }

private:
  Vec3 _origin;    // Point de départ du rayon
  Vec3 _direction; // Direction du rayon (vecteur unitaire)
};
