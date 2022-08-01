#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <unordered_map>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include "stb_truetype.h"
// TODO: #include opengl stufff

///* Created By: Justin Meiners (2013) */
// #include <stdio.h>
// #include <stdlib.h>
//
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */
//
// #define STB_TRUETYPE_IMPLEMENTATION
// #include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

// int main(int argc, const char * argv[])
// {
//     /* load font file */
//     long size;
//     unsigned char* fontBuffer;
//
//     FILE* fontFile = fopen("font/cmunrm.ttf", "rb");
//     fseek(fontFile, 0, SEEK_END);
//     size = ftell(fontFile); /* how long is the file ? */
//     fseek(fontFile, 0, SEEK_SET); /* reset */
//
//     fontBuffer = malloc(size);
//
//     fread(fontBuffer, size, 1, fontFile);
//     fclose(fontFile);
//
//     /* prepare font */
//     stbtt_fontinfo info;
//     if (!stbtt_InitFont(&info, fontBuffer, 0))
//     {
//         printf("failed\n");
//     }
//
//     int b_w = 512; /* bitmap width */
//     int b_h = 128; /* bitmap height */
//     int l_h = 64; /* line height */
//
//     /* create a bitmap for the phrase */
//     unsigned char* bitmap = calloc(b_w * b_h, sizeof(unsigned char));
//
//     /* calculate font scaling */
//     float scale = stbtt_ScaleForPixelHeight(&info, l_h);
//
//     char* word = "the quick brown fox";
//
//     int x = 0;
//
//     int ascent, descent, lineGap;
//     stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
//
//     ascent = roundf(ascent * scale);
//     descent = roundf(descent * scale);
//
//     int i;
//     for (i = 0; i < strlen(word); ++i)
//     {
//         /* how wide is this character */
//         int ax;
//     int lsb;
//         stbtt_GetCodepointHMetrics(&info, word[i], &ax, &lsb);
//         /* (Note that each Codepoint call has an alternative Glyph version which caches the work required to lookup the character word[i].) */
//
//         /* get bounding box for character (may be offset to account for chars that dip above or below the line) */
//         int c_x1, c_y1, c_x2, c_y2;
//         stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
//
//         /* compute y (different characters have different heights) */
//         int y = ascent + c_y1;
//
//         /* render character (stride and offset is important here) */
//         int byteOffset = x + roundf(lsb * scale) + (y * b_w);
//         stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);
//
//         /* advance x */
//         x += roundf(ax * scale);
//
//         /* add kerning */
//         int kern;
//         kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
//         x += roundf(kern * scale);
//     }
//
//     /* save out a 1 channel image */
//     stbi_write_png("out.png", b_w, b_h, 1, bitmap, b_w);
//
//     /*
//      Note that this example writes each character directly into the target image buffer.
//      The "right thing" to do for fonts that have overlapping characters is
//      MakeCodepointBitmap to a temporary buffer and then alpha blend that onto the target image.
//      See the stb_truetype.h header for more info.
//     */
//
//     free(fontBuffer);
//     free(bitmap);
//
//     return 0;
// }

struct Text
{
    // OpenGL
    int _vao;
    int _vbo;
    int _ebo; //maybe?
    int _triangle_n;
    std::vector<float> _vbo_vec;

//    struct GlyphMetrics
//    {
//        // stride
//        // position
//        // texture ?
//    };
//
//    struct GlyphParams
//    {
//        float x, y, w, h;
//    };

    // Font
//    std::unordered_map<char, GlyphMetrics> metrics_reference;
    unsigned char* fontBuffer;
    stbtt_fontinfo info;
    float scale;
    int ascent, descent, lineGap;
    
    // TODO: fix
    int b_w = 512; /* bitmap width */
    int b_h = 128; /* bitmap height */
    int l_h = 64; /* line height */
    
    Text()
    {
        // Load font
        // TODO: embed file
        long size;

        FILE* fontFile = fopen("font/cmunrm.ttf", "rb");
        fseek(fontFile, 0, SEEK_END);
        size = ftell(fontFile); /* how long is the file ? */
        fseek(fontFile, 0, SEEK_SET); /* reset */

        fontBuffer = (unsigned char *) malloc(size);

        fread(fontBuffer, size, 1, fontFile);
        fclose(fontFile);
        
        // Font info
        if (!stbtt_InitFont(&info, fontBuffer, 0))
        {
         printf("failed\n");
        }
        
        // Setting the buffers information
        /*
        glVertexAttribPointer();
        */

        /* calculate font scaling */
        scale = stbtt_ScaleForPixelHeight(&info, l_h);
        
//        // Load the map of characters
//        for (int i = 0 ; i < 256 ; i++)
//        {
//            // TODO: CharMetrics gen_metrics = FreeType(i);
//            metrics_reference[i] = gen_metrics;
//        }
        
        //     /* create a bitmap for the phrase */
        //     unsigned char* bitmap = calloc(b_w * b_h, sizeof(unsigned char));
        //
        
        //     char* word = "the quick brown fox";
        //
        //     int x = 0;
        //
        stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

        ascent = roundf(ascent * scale);
        descent = roundf(descent * scale);
    }
    
    ~Text()
    {
        free(fontBuffer);
    }

    // x and y are the upper right coordinates of the text.
    void rightAligned(float x, float y, std::string text)
    {
        // Generate the vbo and stuff
//        GlyphParams p;
//        p.x = x;
//        p.y = y;

        // reverse iterator?
        //for (char& c : text)
        for(std::string::iterator it = text.begin(); it != text.end(); ++it)
        {
            char c = *it;
            //do_things_with(*it);
//            GlyphMetrics m = metrics_reference[c];
//            p.x -= m.stride; // TODO stride?
//            p.y = y - (m.height - m.baseline)/2.0f;
//
//            p.w = m.width;
//            p.h = m.height;
//
//            trianglePairGlyph(p);
            
            /* how wide is this character */
            int ax;
            int lsb;
            stbtt_GetCodepointHMetrics(&info, c, &ax, &lsb);
            /* (Note that each Codepoint call has an alternative Glyph version which caches the work required to lookup the character c.) */

            /* get bounding box for character (may be offset to account for chars that dip above or below the line) */
            int c_x1, c_y1, c_x2, c_y2;
            stbtt_GetCodepointBitmapBox(&info, c, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

            /* compute y (different characters have different heights) */
            int y = ascent + c_y1;

            /* render character (stride and offset is important here) */
            int byteOffset = x + roundf(lsb * scale) + (y * b_w);
            stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, c);

            /* advance x */
            x += roundf(ax * scale);

            /* add kerning */
            int kern;
            kern = stbtt_GetCodepointKernAdvance(&info, c, *(it + 1)); //TODO: check if iterator reached end
            x += roundf(kern * scale);

        }
    }

    // TODO: leftaligned

    /*
        Assembles a triangle pair and sets the parameters
        such as texture coordinates, coordinates and so on.
    */
    void trianglePairGlyph(GlyphParams p)
    {
        glm::vec3 ul, ll, ur, lr; // Vertices

        // TODO: check
        ul = glm::vec3(p.x, p.y, 0);
        ll = glm::vec3(p.x, p.y + p.h, 0);
        ur = glm::vec3(p.x + p.w, p.y, 0);
        lr = glm::vec3(p.x + p.w, p.y + p.h, 0);

        // ebo ??

        // Upper triangle
        pushVertexAttrib(ur, ul, ll);
        // TODO: other informations as well.

        // Lower triangle
        pushVertexAttrib(ur, lr, ll);
    }

    void render() // Should we call the render function here?
    {
        // TODO: glBindVertexArray(_vao);
        // TODO: glDrawTriangles(_vao, GL_TRIANGLE, _triangle_n * 3);
    }
}

#endif