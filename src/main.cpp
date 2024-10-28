#include "version.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>



static FT_Library library;
static FT_Face face;
static std::string font_face_path = "font/londrina_sketch_regular.ttf";
static int32_t font_size = 36;
static int32_t font_glyph_space = font_size / 8;
static int32_t font_space = font_size / 4;
static int32_t font_tab = font_size / 2;
static std::string message = "DSGE";
static std::string output_file_path = "hello.txt";
static int32_t max_glyph_size = 0; // This will hold the largest glyph size
static int32_t glyph_offset = 0;
// Used for the offset

struct Glyph {
    int32_t startX;
    int32_t startY;
    int32_t width;
    int32_t height;
    std::vector<uint8_t> data;
};


std::vector<Glyph> glyphs;

int app(int argc, char** argv) {
    if(FT_Init_FreeType(&library)) {
        std::cout << "Error: Freetype Library didn't load\n";
        return -1;
    }

    if(FT_New_Face(library, font_face_path.c_str() , 0, &face)) {
        std::cout << "Error: Failed to load font.\n";
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, font_size);

    // Generate Glyphs Table
    glyphs.resize(UCHAR_MAX);


     glyph_offset = font_size;

    for(uint8_t i = 0; i < glyphs.size(); i++) {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            std::cout << "Error: Failed to load glyph \n";
            continue;
        }

        glyphs[i].startX = face->glyph->bitmap_left;
        glyphs[i].startY = face->glyph->bitmap_top;
        glyphs[i].width = face->glyph->bitmap.width;
        glyphs[i].height = face->glyph->bitmap.rows;

        if(glyphs[i].height > max_glyph_size) {
            max_glyph_size = glyphs[i].height;
        }

        if((((font_size - max_glyph_size) + (font_size - glyphs[i].startY))) < glyph_offset && (i > 32 && i < 127)) {
            glyph_offset = ((font_size - max_glyph_size) + (font_size - glyphs[i].startY));
        }

        glyphs[i].data.resize(glyphs[i].width * glyphs[i].height);

        for(uint32_t y = 0; y < glyphs[i].height; y++) {
            for(uint32_t x = 0; x < glyphs[i].width; x++) {
                uint32_t j = y * glyphs[i].width + x;
                glyphs[i].data[j] = face->glyph->bitmap.buffer[j];
            }
        }
    }

    std::cout << "min: " << glyph_offset << ", max: " << max_glyph_size << "\n";
    
    // Compute Width and Height of string
    size_t width = 0;
    size_t height = font_size;
    size_t alignment = 0; // Make sure everything is aligned

    for(int i = 0; i < message.size(); i++) {

        width += glyphs[message[i]].width + font_glyph_space;
        
        if(glyphs[message[i]].height > height) {
            height = glyphs[message[i]].height;
        }

        if(message[i] == ' ') {
            width += font_size / 4;
        }

        std::cout << message[i] << "> startX: " << glyphs[message[i]].startX << ", startY: " << glyphs[message[i]].startY << " height - startY: " << (glyphs[message[i]].height - glyphs[message[i]].startY)<< "\n";


    }

    std::cout << "Message Size: " << width << ", " << height << "\n";

    // Generate Indencies
    std::vector<size_t> indencies;

    size_t current = 0;

    for(int i = 0; i < message.size(); i++) {
        indencies.push_back(current);
        std::cout << i << "> index: " << indencies[indencies.size() - 1] << "\n";
        current += glyphs[message[i]].width + font_glyph_space;

        if(message[i] == ' ') {
            current += font_size / 4; // Added Pixels to current...
        }
    }

    std::cout << "1. Hello, World" << "\n";

    // Create Buffer
    std::vector<uint8_t> buffers;
    buffers.resize(width * (height + 8));

    std::cout << "2. Hello, World" << "\n";
    // Render Buffer
    for(int i = 0; i < message.size(); i++) {
        if(message[i] == ' ') {
            continue;
        }

        int32_t startX = 0;
        int32_t startY = 0;
        int32_t gwidth = glyphs[message[i]].width;
        int32_t gheight = glyphs[message[i]].height;

        int32_t index = indencies[i];
        //uint32_t offset = ((max_glyph_size) - glyphs[message[i]].startY);
        int32_t offset = ((font_size - max_glyph_size) + (font_size - glyphs[message[i]].startY));

        std::cout << offset << "\n";

        for(int32_t y = startY; y < gheight; y++) {
            for(int32_t x = startX; x < gwidth; x++) {

                int32_t y_offset = y + offset;
                y_offset -= (glyph_offset);
                y_offset += 2;

                int32_t gindex = y * gwidth + x;
                int32_t bindex = y_offset * width + (x + index);

                if((x + index) < 0 || x > (width - 1) || (y + offset) < 0 || y > (height - 1)) {
                    continue;
                }

                buffers[bindex] = glyphs[message[i]].data[gindex];
            }
        }
    }

    std::cout << "3. Hello, World" << "\n";

    // Output to file
    std::ofstream out(output_file_path);

    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            uint32_t index = y * width + x;
            if(buffers[index] > 0) {
                out << "#";
                std::cout << "#";
            } else {
                out << " ";
                std::cout << " ";
            }
        }
        out << "\n";
        std::cout << "\n";
    }


    std::cout << "4. Hello, World" << "\n";

    out.close();
    std::cout << "5. Hello, World" << "\n";
    FT_Done_Face(face);
    std::cout << "6. Hello, World" << "\n";
    FT_Done_FreeType(library);
    std::cout << "7. Hello, World" << "\n";
    return 0;
}

int main(int argc, char** argv) {
    std::cout << VERSION_FULL_NAME << "\n";

    return app(argc, argv);
}