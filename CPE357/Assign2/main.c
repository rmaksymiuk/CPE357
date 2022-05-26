#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>

typedef unsigned short WORD; 
typedef unsigned int DWORD; 
typedef unsigned int LONG; 
typedef unsigned char BYTE;


struct BITMAPFILEHEADER 
 { 
 WORD bfType;  //specifies the file type 
 DWORD bfSize;  //specifies the size in bytes of the bitmap file 
 WORD bfReserved1;  //reserved; must be 0 
 WORD bfReserved2;  //reserved; must be 0 
 DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to 

 }; 
struct BITMAPINFOHEADER 
 { 
 DWORD biSize;  //specifies the number of bytes required by the struct 
 LONG biWidth;  //specifies width in pixels 
 LONG biHeight;  //species height in pixels 
 WORD biPlanes; //specifies the number of color planes, must be 1 
 WORD biBitCount; //specifies the number of bit per pixel 
 DWORD biCompression;//spcifies the type of compression 
 DWORD biSizeImage;  //size of image in bytes 
 LONG biXPelsPerMeter;  //number of pixels per meter in x axis 
 LONG biYPelsPerMeter;  //number of pixels per meter in y axis 
 DWORD biClrUsed;  //number of colors used by th ebitmap 
 DWORD biClrImportant;  //number of colors that are important 
 }; 




BYTE get_red(BYTE* pixelArray, int x, int y, int wb)
{
    return pixelArray[y*wb + x*3 +2];
}

BYTE get_blue(BYTE* pixelArray, int x, int y, int wb)
{
    return pixelArray[y*wb + x*3 +0];
}

BYTE get_green(BYTE* pixelArray, int x, int y, int wb)
{
    return pixelArray[y*wb + x*3 +1];
}

float get_color_bilinear(float left, float right, float dx)
{
    float redResult = left * (1-dx) + right * dx;
    return redResult;
}


void calculations(struct BITMAPINFOHEADER infoHeader, struct BITMAPINFOHEADER infoHeader2, BYTE* pixelArray, BYTE* pixelArray2, BYTE* pixelArrayResult, float ratio)
{
     float dx, dy = 0.0;
        dx = (float)infoHeader2.biWidth/infoHeader.biWidth;
        dy = (float)infoHeader2.biHeight/infoHeader.biHeight;

        int wb = infoHeader.biWidth * 3;
        if(wb%4 != 0)
            wb += 4 - wb%4;

        int wb2 = infoHeader2.biWidth * 3;
        if(wb2%4 != 0)
            wb2 += 4 - wb2%4;


        for(int y = 0; y < infoHeader.biHeight; y++)
        {
            for (int x = 0; x < infoHeader.biWidth; x++)
            {
                //red color
                BYTE red_leftUpper, red_rightUpper, red_leftLower, red_rightLower;
                float redLeft, redRight, redResult;
                //green color
                BYTE  green_leftUpper, green_rightUpper, green_leftLower, green_rightLower;
                float greenLeft, greenRight, greenResult;
                //blue color
                BYTE  blue_leftUpper, blue_rightUpper, blue_leftLower, blue_rightLower;
                float blueLeft, blueRight, blueResult;
                //r,g,b of the bigger image
                BYTE r1, g1, b1;


                //getting pixels from the big image
                b1 = get_blue(pixelArray, x, y, wb); 
                g1 = get_green(pixelArray, x, y, wb);
                r1 = get_red(pixelArray, x, y, wb);

                //getting pixled from the smaller image: infoHeader2: get_pixel()
                blue_leftLower = get_blue(pixelArray2, x, y, wb2); 
                green_leftLower =get_green(pixelArray2, x, y, wb2);
                red_leftLower = get_red(pixelArray2, x, y, wb2);

                blue_rightLower =get_blue(pixelArray2, x+1, y, wb2); 
                green_rightLower =get_green(pixelArray2, x+1, y, wb2);
                red_rightLower = get_blue(pixelArray2, +1, y, wb2);

                blue_leftUpper = get_blue(pixelArray2, x, y+1, wb2); 
                green_leftUpper = get_green(pixelArray2, x, y+1, wb2); 
                red_leftUpper = get_red(pixelArray2, x, y+1, wb2); 

                blue_rightUpper = get_blue(pixelArray2, x+1, y+1, wb2); 
                green_rightUpper = get_green(pixelArray2, x+1, y+1, wb2); 
                red_rightUpper = get_red(pixelArray2, x+1, y+1, wb2); 

                //Interpolation
                
                redLeft = red_leftUpper * (1-dy) + red_leftLower * dy;
                redRight = red_rightUpper * (1-dy) + red_rightLower * dy;
                redResult = get_color_bilinear(redLeft, redRight, dx);

                greenLeft = green_leftUpper * (1-dy) + green_leftLower * dy;
                greenRight = green_rightUpper * (1-dy) + green_rightLower * dy;
                greenResult = get_color_bilinear(greenLeft, greenRight, dx);


                blueLeft = blue_leftUpper * (1-dy) + blue_leftLower * dy;
                blueRight = blue_rightUpper * (1-dy) + blue_rightLower * dy;
                blueResult = get_color_bilinear(blueLeft, blueRight, dx);

    
                pixelArrayResult[y*wb + x*3 +0] = b1* ratio + blueResult*(1 - ratio);
                pixelArrayResult[y*wb + x*3 +1] = g1* ratio + greenResult*(1 - ratio);
                pixelArrayResult[y*wb + x*3 +2] = r1* ratio + redResult*(1 - ratio);
            }
        }
}
int main(int argc, char* argv[])
{
    if(argc >5 || argc < 5)
    {
        printf("Not enough or too many arguments were provided\nCorrect format is:\n");
        printf("[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n");
        printf("e.g. ./main lion.bmp wolf.bmp 0.5 newFile.bmp\n\n");
        return 1;
    }
    float ratio = atof(argv[3]);
    FILE *file = fopen(argv[1], "rb");
    struct BITMAPFILEHEADER fileHeader;
    fread(&fileHeader.bfType, 2, 1, file);
    fread(&fileHeader.bfSize, 4, 1, file);
    fread(&fileHeader.bfReserved1, 2, 1, file);
    fread(&fileHeader.bfReserved2, 2, 1, file);
    fread(&fileHeader.bfOffBits, 4, 1, file);
    struct BITMAPINFOHEADER infoHeader;
    fread(&infoHeader.biSize, 4,1,file);
    fread(&infoHeader.biWidth, 4,1,file);
    fread(&infoHeader.biHeight, 4,1,file);
    fread(&infoHeader.biPlanes, 2,1,file);
    fread(&infoHeader.biBitCount, 2,1,file);
    fread(&infoHeader.biCompression, 4,1,file);
    fread(&infoHeader.biSizeImage, 4,1,file);
    fread(&infoHeader.biXPelsPerMeter, 4,1,file);
    fread(&infoHeader.biYPelsPerMeter, 4,1,file);
    fread(&infoHeader.biClrUsed, 4,1,file);
    fread(&infoHeader.biClrImportant, 4,1,file);

    //allocating memory based on the infoHeader.biSizeImage
    BYTE* pixelArray = (BYTE*)malloc(infoHeader.biSizeImage);
    //reading the data from the image
    for( int i = 0; i < infoHeader.biSizeImage; i++)
    {
        fread(&pixelArray[i], infoHeader.biSizeImage,1,file);
    }

    fclose(file);



    FILE *file2 = fopen(argv[2], "rb");
    struct BITMAPFILEHEADER fileHeader2;
    fread(&fileHeader2.bfType, 2, 1, file2);
    fread(&fileHeader2.bfSize, 4, 1, file2);
    fread(&fileHeader2.bfReserved1, 2, 1, file2);
    fread(&fileHeader2.bfReserved2, 2, 1, file2);
    fread(&fileHeader2.bfOffBits, 4, 1, file2);
    struct BITMAPINFOHEADER infoHeader2;
    fread(&infoHeader2.biSize, 4,1,file2);
    fread(&infoHeader2.biWidth, 4,1,file2);
    fread(&infoHeader2.biHeight, 4,1,file2);
    fread(&infoHeader2.biPlanes, 2,1,file2);
    fread(&infoHeader2.biBitCount, 2,1,file2);
    fread(&infoHeader2.biCompression, 4,1,file2);
    fread(&infoHeader2.biSizeImage, 4,1,file2);
    fread(&infoHeader2.biXPelsPerMeter, 4,1,file2);
    fread(&infoHeader2.biYPelsPerMeter, 4,1,file2);
    fread(&infoHeader2.biClrUsed, 4,1,file2);
    fread(&infoHeader2.biClrImportant, 4,1,file2);

    //allocating memory based on the infoHeader.biSizeImage
    BYTE* pixelArray2 = (BYTE*)malloc(infoHeader2.biSizeImage);
    //reading the data from the image
    for( int i = 0; i < infoHeader2.biSizeImage; i++)
    {
        fread(&pixelArray2[i], infoHeader2.biSizeImage,1,file2);
    }    

    fclose(file2);

    BYTE* pixelArrayResult = (BYTE*)malloc(infoHeader.biSizeImage);
    BYTE* pixelArrayResult2 = (BYTE*)malloc(infoHeader2.biSizeImage);
    //Image1: pixelArray1
    //Image2: pixelArray2
    //both images are the same size
    if(infoHeader.biWidth == infoHeader2.biWidth)
    {
        int wb = infoHeader.biWidth * 3;
        if(wb%4 != 0)
            wb += 4 - wb%4;

        for(int y = 0; y < infoHeader.biHeight; y++)
        {
            for (int x = 0; x < infoHeader.biWidth; x++)
            {
                BYTE r1, g1, b1, r2, g2, b2;
                BYTE* RGB = (BYTE*)malloc(3);
                b1 = pixelArray[y*wb + x*3 +0];
                g1 = pixelArray[y*wb + x*3 +1];
                r1= pixelArray[y*wb + x*3 +2];

                b2 = pixelArray2[y*wb + x*3 +0];
                g2 = pixelArray2[y*wb + x*3 +1];
                r2= pixelArray2[y*wb + x*3 +2];

                
                //Do blending here
                RGB[0] = b1* ratio + b2*(1- ratio);
                RGB[1] = g1* ratio + g2*(1- ratio);;
                RGB[2] = r1* ratio + r2*(1- ratio);;
                
                

                pixelArrayResult[y*wb + x*3 +0] = RGB[0];
                pixelArrayResult[y*wb + x*3 +1] = RGB[1];
                pixelArrayResult[y*wb + x*3 +2] = RGB[2];
            }
        }
    }
    //first image is bigger
    else if(infoHeader.biWidth > infoHeader2.biWidth)
    {
        calculations(infoHeader, infoHeader2, pixelArray, pixelArray2, pixelArrayResult, ratio);
        
    }
    //second image is bigger
    else if(infoHeader.biWidth < infoHeader2.biWidth)
    {
       calculations(infoHeader2, infoHeader, pixelArray2, pixelArray, pixelArrayResult2, ratio);
    }

    FILE *newFile = fopen(argv[4], "wb");
    fwrite(&fileHeader.bfType, 2, 1, newFile);
    fwrite(&fileHeader.bfSize, 4, 1, newFile);
    fwrite(&fileHeader.bfReserved1, 2, 1, newFile);
    fwrite(&fileHeader.bfReserved2, 2, 1, newFile);
    fwrite(&fileHeader.bfOffBits, 4, 1, newFile);
    fwrite(&infoHeader, sizeof(infoHeader), 1, newFile);
    fwrite(pixelArrayResult, infoHeader.biSizeImage, 1, newFile);
    fclose(newFile);


    free(pixelArray);
    free(pixelArray2);
    free(pixelArrayResult);
    free(pixelArrayResult2);

    return 0;
}
