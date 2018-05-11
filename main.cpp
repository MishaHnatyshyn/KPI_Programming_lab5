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

PIXELDATA get_interpolated_pixel(PIXELDATA Fx0, PIXELDATA Fx1, double x0, double x1, int x){
    PIXELDATA pixel;
    pixel.r = (int) floor(Fx0.r + ((Fx1.r - Fx0.r) / (x1 - x0)) * (x - x0));
    pixel.g = (int) floor(Fx0.g + ((Fx1.g - Fx0.g) / (x1 - x0)) * (x - x0));
    pixel.b = (int) floor(Fx0.b + ((Fx1.b - Fx0.b) / (x1 - x0)) * (x - x0));
    return pixel;
}

int byte_per_line(int width){
    int result;
    result = width * 3;
    if (result & 0x0003)
    {
        result |= 0x0003;
        ++result;
    }
    return result;
}


int main(int argc, char* argv[]) {
    char * input_path = argv[1];
    char * output_path = argv[2];
    double resize = atof(argv[3]);
    
    int resize_count = 7;

    FILE * file;
    file = fopen("/home/mhnatyshyn/Programming_labs/Lab5/bmp.bmp","rb");

    BITMAPFILEHEADER bm;
    BITMAPINFOHEADER bm2;

    fread(&bm, 1, 14, file);
    fread(&bm2, 1, 40, file);

    int bytesPerLine = byte_per_line(bm2.width);

    FILE * output;
    output = fopen("/home/mhnatyshyn/Programming_labs/Lab5/output.bmp","wb");

    int new_depth = (int)floor(bm2.depth*resize);
    int new_width = (int)floor(bm2.width*resize);

    PIXELDATA map[bm2.depth][bm2.width];
    PIXELDATA map2[bm2.depth][new_width];
    PIXELDATA map3[new_depth][new_width];

    int new_bytesPerLine = byte_per_line(bm2.width * resize_count);

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

    for (int y = 0; y < bm2.depth; y++) {
            int i = 0;
            for (int x = 0; x < bm2.width; x++) {
                double x0= x*resize;
                double x1= (x+1)*resize;
                if(x0 == i || x1 == i){
                    map2[y][i] = map[y][x];
                    i++;
                }
                while (i > x0 && i < x1) {
                    map2[y][i] = get_interpolated_pixel(map[y][x], map[y][x+1], x0, x1, i);
                    i++;
                }
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
