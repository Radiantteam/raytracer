#pragma once

#include <iostream>
#include <vector>
#include "Color.hpp"

class Image
{
private:
    unsigned int width = 1920;
    unsigned int height = 1080;
    std::vector<Color> buffer;

public:
    Image(unsigned int w, unsigned int h);
    Image(unsigned int w, unsigned int h, Color c);
    ~Image();

    void SetPixel(unsigned int x, unsigned int y, Color color);
    Color GetPixel(unsigned int x, unsigned int y);

    int GetWidth();
    int GetHeight();

    void WriteFile(const char *filename) const;
};
