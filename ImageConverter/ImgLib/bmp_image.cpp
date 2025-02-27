#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>
#include <iostream>

using namespace std;

namespace img_lib {

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    BitmapFileHeader() = default;
    BitmapFileHeader(int width, int height) {
        bfSize = GetBMPStride(width) * height;
    }

    char bfType[2] = {'B', 'M'};
    uint32_t bfSize = {};
    uint32_t bfReserved = 0;
    uint32_t bfOffBits = 54; // равен размеру обоих частей заголовка - 54 байта
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    BitmapInfoHeader() = default;
    BitmapInfoHeader(int width, int height)
        : biWidth(width), biHeight(height) {
        biSizeImage = GetBMPStride(width) * height;
    }
    
    uint32_t biSize = 40; // размер второй части 40 байт
    int32_t biWidth = {};
    int32_t biHeight = {};
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = {};
    int32_t biXPelsPerMeter = 11811;
    int32_t biYPelsPerMeter = 11811;
    int32_t biClrUsed = 0;
    int32_t biClrImportant = 0x1000000;
}
PACKED_STRUCT_END

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    std::ofstream out(file, std::ios::binary);
    if (!out) {
        return false; 
    }    
    
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    const int stride = GetBMPStride(width);

    BitmapFileHeader file_header {width, height};
    file_header.bfSize = file_header.bfOffBits + (stride * height);
    
    BitmapInfoHeader info_header {width, height};
    info_header.biSizeImage = stride * height;

    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    std::vector<char> buff(stride);

    for (int y = height - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < width; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), stride);
    }

    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file) { 
    ifstream ifs(file, ios::binary);
    if (!ifs) {
        return {}; 
    }

    // Чтение заголовка BMP
    BitmapFileHeader file_header;
    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));

    // Проверка типа файла
    if (file_header.bfType[0] != 'B' || file_header.bfType[1] != 'M') {
        return {}; 
    }

    // Чтение информации о изображении
    BitmapInfoHeader info_header;
    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

    // Извлечение ширины и высоты изображения
    int w = info_header.biWidth;
    int h = info_header.biHeight;

    // Проверка на корректность высоты и ширины
    if (w <= 0 || h <= 0) {
        return {}; 
    }

    int stride = GetBMPStride(w); 
    Image result(stride / 3, h, Color::Black()); 
    std::vector<char> buff(w * 3); 

    for (int y = result.GetHeight() - 1; y >= 0; --y) { 
        Color* line = result.GetLine(y); 
        ifs.read(buff.data(), stride); 

        for (int x = 0; x < w; ++x) { 
            line[x].b = static_cast<byte>(buff[x * 3 + 0]); 
            line[x].g = static_cast<byte>(buff[x * 3 + 1]); 
            line[x].r = static_cast<byte>(buff[x * 3 + 2]); 
        } 
    } 

    return result; 
}

}  // namespace img_lib