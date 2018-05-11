#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct BITMAPFILEHEADER{
    int8_t  b1, b2;
    int32_t bfSize;
    int32_t  bfReserved1;
    int32_t  bfReserved2;
    int32_t  padding;
    int32_t  bfOffBits;
};

struct BITMAPINFOHEADER{
    int32_t infoSize;
    int32_t width;
    int32_t depth;
    int16_t biPlanes;
    int16_t bits;
    int32_t biCompression;
    int32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    int32_t biClrUsed;
    int32_t biClrImportant;
};


struct PIXELDATA{
    uint8_t r;
    uint8_t b;
    uint8_t g;
};

int main() {
    int resize_count = 7;

    FILE * file;
    file = fopen("/home/mhnatyshyn/Programming_labs/Lab5/bmp.bmp","rb");

    BM bm;
    BM2 bm2;
    fread(&bm, 1, 14, file);
    fread(&bm2, 1, 40, file);
    
    int bytesPerLine;
    bytesPerLine = bm2.width * 3;
    if (bytesPerLine & 0x0003)
    {
        bytesPerLine |= 0x0003;
        ++bytesPerLine;
    }

    FILE * output;
    output = fopen("/home/mhnatyshyn/Programming_labs/Lab5/output.bmp","wb");

    PIXELDATA map[bm2.depth][bm2.width];
    PIXELDATA map2[bm2.depth * resize_count][bm2.width * resize_count];

    int new_bytesPerLine;

    new_bytesPerLine = bm2.width * resize_count * 3;
    if (new_bytesPerLine & 0x0003)
    {
        new_bytesPerLine |= 0x0003;
        ++new_bytesPerLine;
    }

    bm.bfSize=54+(long)new_bytesPerLine*bm2.depth * resize_count;

    const char null_byte = ' ';
    char garbage;
    int input_padding = bytesPerLine - bm2.width*3; // count of null byte at the end of each row

    //pixel map reading

    for (int y = bm2.depth-1; y >= 0; y--) {
        for (int x = 0; x < bm2.width; x++) {
            fread(&map[y][x],1,3,file);
        }
        if(input_padding != 0) fread(&garbage,1,input_padding,file);
    }

    //new matrix creating
    
    int p = 0;
    int s = 0;
    for (int i = 0; i < bm2.depth; ++i) {
        for (int l = 0; l < resize_count; ++l) {
            for (int j = 0; j < bm2.width; ++j) {
                for (int k = 0; k < resize_count; ++k) {
                    map2[p][s] = map[i][j];
                    s++;
                }
            }
            s = 0;
            p++;
        }
    }


    bm2.depth *= resize_count;
    bm2.width *= resize_count;


    fwrite(&bm, 1, 14, output);
    fwrite(&bm2, 1, 50, output);

    int output_padding = new_bytesPerLine - bm2.width*3; // count of null byte at the end of each row
    for (int y = bm2.depth-1; y >= 0; y--) {
        for (int x = 0; x < bm2.width; x++) {
            fwrite(&map2[y][x],1,3,output);
        }
        if(output_padding != 0) fwrite(&null_byte,1,output_padding,output);
    }
    
    fclose(output);
    fclose(file);

    return 0;
}
