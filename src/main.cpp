#include "version.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H



static FT_Library library;
static FT_Face face;
static std::string message = "Hello, World!";


int app(int argc, char** argv) {
    if(FT_Init_FreeType(&library)) {
        std::cout << "Error: Freetype Library didn't load\n";
        return -1;
    }

    if(FT_New_Face(library, "font/open_sans_regular.ttf", 0, &face)) {
        std::cout << "Error: Failed to load font.\n";
        return -1;
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return 0;
}

int main(int argc, char** argv) {
    return app(argc, argv);
}