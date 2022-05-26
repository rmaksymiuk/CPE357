#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

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

// void calculate(BYTE* pixelArray, struct BITMAPINFOHEADER infoHeader, char* argv[])
// {
//     //Perfoming the calculations
//     for( int i = 0; i < infoHeader.biSizeImage; i++)
//     {
//         float floatNum = (float)pixelArray[i];
//         floatNum = floatNum/255;
//         double temp = pow(floatNum, atof(argv[3]));
//         temp *= 255;
//         //printf("Before: %d\t After: %f\n", pixelArray[i], temp);
//         pixelArray[i] = (BYTE)temp;
//        // printf("%d\n", pixelArray[i]);

//     }
// }


int main(int argc, char* argv[])
{

    int pid = fork();

    if(pid == 0)
    {
        //child
        printf("Child!");
        return 0 ;
    }
    //parent
    wait(0);
    printf("Parent!");
    // FILE *file = fopen(argv[1], "rb");
    // struct BITMAPFILEHEADER fileHeader;
    // fread(&fileHeader.bfType, 2, 1, file);
    // fread(&fileHeader.bfSize, 4, 1, file);
    // fread(&fileHeader.bfReserved1, 2, 1, file);
    // fread(&fileHeader.bfReserved2, 2, 1, file);
    // fread(&fileHeader.bfOffBits, 4, 1, file);
    // struct BITMAPINFOHEADER infoHeader;
    // fread(&infoHeader.biSize, 4,1,file);
    // fread(&infoHeader.biWidth, 4,1,file);
    // fread(&infoHeader.biHeight, 4,1,file);
    // fread(&infoHeader.biPlanes, 2,1,file);
    // fread(&infoHeader.biBitCount, 2,1,file);
    // fread(&infoHeader.biCompression, 4,1,file);
    // fread(&infoHeader.biSizeImage, 4,1,file);
    // fread(&infoHeader.biXPelsPerMeter, 4,1,file);
    // fread(&infoHeader.biYPelsPerMeter, 4,1,file);
    // fread(&infoHeader.biClrUsed, 4,1,file);
    // fread(&infoHeader.biClrImportant, 4,1,file);
    
    // //allocating memory based on the infoHeader.biSizeImage
    // BYTE* pixelArray = (BYTE*)sbrk(infoHeader.biSizeImage);

    // //reading the data from the image
    // for( int i = 0; i < infoHeader.biSizeImage; i++)
    // {
    //     fread(&pixelArray[i], infoHeader.biSizeImage,1,file);
    // }

    // //Calculations
    // calculate(pixelArray, infoHeader, argv);
    
    // //writing to the new file
    // FILE *newFile = fopen(argv[2], "wb");
    // fwrite(&fileHeader.bfType, 2, 1, newFile);
    //  fwrite(&fileHeader.bfSize, 4, 1, newFile);
    //  fwrite(&fileHeader.bfReserved1, 2, 1, newFile);
    //  fwrite(&fileHeader.bfReserved2, 2, 1, newFile);
    //  fwrite(&fileHeader.bfOffBits, 4, 1, newFile);
    //  fwrite(&infoHeader, sizeof(infoHeader), 1, newFile);
    //  fwrite(pixelArray, infoHeader.biSizeImage, 1, newFile);
    //  fclose(newFile);
    // fclose(file);


    return 0;
}
