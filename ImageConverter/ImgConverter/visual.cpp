#include "img_lib.h"
#include "visual.h"

using namespace std;

namespace visual {

    // Negate
    byte Negate(byte c) {
        int c_ = 255 - std::to_integer<int>(c);
        return static_cast<byte>(c_);
    }

    void NegateInplace(img_lib::Image & image) {
        int w, h;
        w = image.GetWidth();
        h = image.GetHeight();

        for (int y = 0; y < h; ++y) {
            img_lib::Color* line = image.GetLine(y);

            for (int x = 0; x < w; ++x) {
                line[x].r = Negate(line[x].r);
                line[x].g = Negate(line[x].g);;
                line[x].b = Negate(line[x].b);;
            }
        }
    }

    // vertical mirroring
    void VMirrInplace(img_lib::Image & image) {
        int w, h;
        w = image.GetWidth();
        h = image.GetHeight();

        for (int y = 0; y < h / 2; ++y) {
            int bottom = h - 1 - y;
            img_lib::Color* line = image.GetLine(y);
            img_lib::Color* bottom_line = image.GetLine(bottom);

            for (int x = 0; x < w; ++x) {
                std::swap(line[x], bottom_line[x]);
            }
        }
    }

    // horizontal mirroring
    void HMirrInplace(img_lib::Image & image) {
        int w, h;
        w = image.GetWidth();
        h = image.GetHeight();

        for (int y = 0; y < h; ++y) {
            img_lib::Color* line = image.GetLine(y);

            for (int x = 0; x < w / 2; ++x) {
                int right = w - 1 - x;
                if (x != right) {
                    std::swap(line[x], line[right]);
                }
            }
        }
    }

    // sobel's transformation
    int Sum(img_lib::Color c) {
        return to_integer<int>(c.r) + to_integer<int>(c.g) + to_integer<int>(c.b);
    }

    img_lib::Image Sobel(const img_lib::Image & image) {
        int w, h;
        w = image.GetWidth();
        h = image.GetHeight();

        img_lib::Image result(w, h, img_lib::Color::Black());
        for (int y = 1; y < h - 1; ++y) {
            const img_lib::Color* top = image.GetLine(y - 1);
            const img_lib::Color* center = image.GetLine(y);
            const img_lib::Color* bottom = image.GetLine(y + 1);

            for (int x = 1; x < w - 1; ++x) {
                const int tl = Sum(top[x - 1]);
                const int tc = Sum(top[x]);
                const int tr = Sum(top[x + 1]);
                const int cl = Sum(center[x - 1]);
                const int cr = Sum(center[x + 1]);
                const int bl = Sum(bottom[x - 1]);
                const int bc = Sum(bottom[x]);
                const int br = Sum(bottom[x + 1]);

                const int gx = -tl - 2 * tc - tr + bl + 2 * bc + br;
                const int gy = -tl - 2 * cl - bl + tr + 2 * cr + br;

                double sobel = std::clamp<double>(static_cast<double>(sqrt(gx * gx + gy * gy)), 0, 255);
                result.GetPixel(x, y).r = static_cast<byte>(sobel);
                result.GetPixel(x, y).g = static_cast<byte>(sobel);
                result.GetPixel(x, y).b = static_cast<byte>(sobel);
            }
        }

        return result;
    }

} // namespace visual