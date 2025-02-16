#include <img_lib.h>
#include <bmp_image.h>
#include <jpeg_image.h>
#include <ppm_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

#include "visual.h"

using namespace std;


enum Format {
    JPEG,
    PPM,
    BMP,
    UNKNOWN
};



class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

namespace FormatInterfaces {

class PPM : public ImageFormatInterface {

public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SavePPM(file, image);
    }


    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadPPM(file);
    }
};


class JPEG : public ImageFormatInterface {

public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveJPEG(file, image);
    }


    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadJPEG(file);
    }
};


class BMP : public ImageFormatInterface {

public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveBMP(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadBMP(file);
    }
};

} // namespace FormatInterfaces


Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }


    return Format::UNKNOWN;
}

const ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
    const Format format = GetFormatByExtension(path);

    static const FormatInterfaces::PPM ppm_Interface;
    static const FormatInterfaces::JPEG jpeg_Interface;
    static const FormatInterfaces::BMP bmp_Interface;

    switch (format) {
        case Format::UNKNOWN:
            return nullptr;

        case Format::PPM:
            return &ppm_Interface;

        case Format::JPEG:
            return &jpeg_Interface;

        case Format::BMP: 
            return &bmp_Interface;
    }

    return nullptr;
}


int main() {

    cout << "Welcome to the Image Converter!" << endl;
    cout << "Supported formats: JPG, BMP, PPM." << endl;
    cout << "Please follow the prompts to convert your images." << endl;

    img_lib::Path executable_path = filesystem::absolute(filesystem::current_path());

    string input_filename, output_filename;

    cout << "Enter input filename in current directory (e.g., file.jpg): ";
    cin >> input_filename;
    cout << "Enter output filename in current directory (e.g., file.bmp): ";
    cin >> output_filename;
    cout << endl;
    
    img_lib::Path input_path = executable_path / input_filename;
    img_lib::Path output_path = executable_path / output_filename;

    if (input_path.empty() || output_path.empty()) {
        cerr << "Incorrect name <in_file> <out_file>"sv << endl;
        return 1;
        system("pause");
    }
    

    const ImageFormatInterface* in_format_interface = GetFormatInterface(input_path);
    if (!in_format_interface) {
        cerr << "Unknown format of the input file"sv << endl;
        system("pause"); 
        return 2;
    }

    const ImageFormatInterface* out_format_interface = GetFormatInterface(output_path);
    if (!out_format_interface) {
        cerr << "Unknown format of the output file"sv << endl;
        system("pause");
        return 3;
    }

    img_lib::Image image = in_format_interface->LoadImage(input_path);

    if (!image) {
        cerr << "Loading failed"sv << endl;
        system("pause");
        return 4;
    }

    int edit_variant = 0;
    vector<int> edit_variants;

    cout << "You may select one or more options to edit the image. They will procede the order you provide." << endl; 
    cout << "Enter the number of the option(s) to select it, or enter -1 to finish." << endl;
    cout << "0 - Nothing" << endl;
    cout << "1 - Negate" << endl;
    cout << "2 - Vertical mirroring" << endl;
    cout << "3 - Horizontal mirroring" << endl;
    cout << "4 - Sobel's transformation" << endl;

    while (true) {
        cin >> edit_variant;
        if (edit_variant == -1) break;
        if (edit_variant < 0 || edit_variant > 4) {
            cout << "Invalid option. Please try again." << endl;
            continue;
        }
        edit_variants.push_back(edit_variant);
    }

    for (int variant : edit_variants) {
        switch (variant) {
            case 1:
                visual::NegateInplace(image);
                break;
            case 2:
                visual::VMirrInplace(image);
                break;
            case 3:
                visual::HMirrInplace(image);
                break;
            case 4:
                img_lib::Image image_sobel = visual::Sobel(image);
                image = image_sobel;
                break;
        }
    }

if (!out_format_interface->SaveImage(output_path, image)) {
    cerr << "Saving failed"sv << endl;
    return 5;
}

cout << endl;
cout << "Successfully converted and saved to " << output_filename << endl;
    system("pause");
    return 0;
}
