#pragma once

#include "image/Image.hpp"

class Shape {
public:
    virtual ~Shape() = default; // Virtual destructor for polymorphism
    virtual void Draw(Image& img) const = 0; // Pure virtual function
};