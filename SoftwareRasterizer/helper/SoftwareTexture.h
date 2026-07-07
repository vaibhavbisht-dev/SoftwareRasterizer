#pragma once
#include "ImageData.h" // Includes your smart pointer code

class SoftwareTexture
{
private:
    ImageData::ImagePtr m_PixelData; // Keeps the memory alive!
    int m_Width = 0;
    int m_Height = 0;
    int m_Channels = 0;

public:
    SoftwareTexture(const char* filepath) : m_PixelData(nullptr, [](void*) {}) {
        /*
         * V-FLIP CONVENTION NOTE:
         * We enforce stbi_set_flip_vertically_on_load(true) during loading.
         * This aligns the image data with a standard graphics coordinate system
         * where the UV origin (0,0) is firmly anchored at the BOTTOM-LEFT.
         */
        try {
            m_PixelData = ImageData::GetImageData(filepath, m_Width, m_Height, m_Channels);
        }
        catch (const std::runtime_error& e) {
            // Print the exact error message to the console or output window
            std::cerr << "Image Loading Failed: " << e.what() << std::endl;

            // Fallback: Assign a missing-texture color (e.g., solid magenta) to prevent rendering crashes
            m_PixelData = nullptr;
            m_Width = 0;
            m_Height = 0;
        }
    }

    // A fast inline sampler helper function for your rasterizer loops
    // Takes normalized UV coordinates (0.0 to 1.0) and returns a pixel index or color
    void Sample(float u, float v, uint8_t& r, uint8_t& g, uint8_t& b) const {
        if (!m_PixelData) {
            r = 255;
            g = 0;
            b = 255;
            return;
        }

        // 1. Clamp UVs to avoid out-of-bounds crashes
        u = std::max(0.0f, std::min(u, 1.0f));
        v = std::max(0.0f, std::min(v, 1.0f));

        // 2. Map UV mapping spaces to absolute pixel locations
        int x = static_cast<int>(u * (m_Width - 1));
        int y = static_cast<int>(v * (m_Height - 1));

        // 3. Find the starting byte index in the row-major array (Hardcoded to 4 channels)
        int index = (y * m_Width + x) * 4;

        // 4. Extract data using the underlying raw pointer
        const unsigned char* rawBytes = m_PixelData.get();
        r = rawBytes[index];
        g = rawBytes[index + 1];
        b = rawBytes[index + 2];
        // Alpha channel (rawBytes[index + 3]) is loaded but skipped here per function signature
    }

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
};