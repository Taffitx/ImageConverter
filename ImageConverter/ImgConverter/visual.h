#pragma once

#include "img_lib.h"

#include <algorithm>
#include <cmath>


namespace visual {

    // Negate
    std::byte Negate(std::byte c);
    void NegateInplace(img_lib::Image& image);

    // vertical mirroring
    void VMirrInplace(img_lib::Image& image);

    // horizontal mirroring
    void HMirrInplace(img_lib::Image& image);

    // sobel's transformation
    int Sum(img_lib::Color c);
    img_lib::Image Sobel(const img_lib::Image& image);

} // namespace visual