#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include "ImageData.h"
#include <string>
#include <stdexcept>

ImageData::ImagePtr ImageData::GetImageData(const char* filename, int& width, int& height, int& channels)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data)
    {
        throw std::runtime_error("Failed to load image: " + std::string(filename) +
            " | Reason: " + std::string(stbi_failure_reason()));
    }

    // Update channels reference so caller knows we are processing as 4 channels
    channels = 4;

    return ImagePtr(data, [](void* p) { stbi_image_free(p); });
}
