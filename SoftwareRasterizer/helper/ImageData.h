#pragma once
#include <memory>

class ImageData
{
public:
    using ImagePtr = std::unique_ptr<unsigned char, void(*)(void*)>;

    // Keep only the declaration here, remove 'inline'
    static ImagePtr GetImageData(const char* filename, int& width, int& height, int& channels);
};
