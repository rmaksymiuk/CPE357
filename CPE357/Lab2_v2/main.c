#include <stdio.h>
#include <unistd.h>
#include <math.h>

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

int main()
{
    
    FILE *file = fopen("lion.bmp", "rb");
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

    printf("%d\n", fileHeader.bfSize);
    printf("%d\n", infoHeader.biSizeImage);
    
    //allocating memory based on the infoHeader.biSizeImage
    BYTE* pixelArray = (BYTE*)sbrk(infoHeader.biSizeImage);
    pixelArray = (BYTE*)sbrk(0);
    
    //reading the data from the image
    for( int i = 0; i < infoHeader.biSizeImage; i++)
    {
        fread(&pixelArray[i], infoHeader.biSizeImage,1,file);
    }
    //Perfoming the calculations
    for( int i = 0; i < infoHeader.biSizeImage; i++)
    {
        //double temp = pow(((float)pixelArray[i]/255), 3.2);
        //pixelArray[i] = (BYTE)temp;

    }

    //writing to the new file
    // FILE *newFile = fopen("newFile.bmp", "wb");
    // fwrite(&fileHeader.bfType, 2, 1, newFile);
    // fwrite(&fileHeader.bfSize, 4, 1, newFile);
    // fwrite(&fileHeader.bfReserved1, 2, 1, newFile);
    // fwrite(&fileHeader.bfReserved2, 2, 1, newFile);
    // fwrite(&fileHeader.bfOffBits, 4, 1, newFile);

    // fwrite(&infoHeader.biSize, 4,1,newFile);
    // fwrite(&infoHeader.biWidth, 4,1,newFile);
    // fwrite(&infoHeader.biHeight, 4,1,newFile);
    // fwrite(&infoHeader.biPlanes, 2,1,newFile);
    // fwrite(&infoHeader.biBitCount, 2,1,newFile);
    // fwrite(&infoHeader.biCompression, 4,1,newFile);
    // fwrite(pixelArray, 4,1,newFile);
    // fwrite(&infoHeader.biXPelsPerMeter, 4,1,newFile);
    // fwrite(&infoHeader.biYPelsPerMeter, 4,1,newFile);
    // fwrite(&infoHeader.biClrUsed, 4,1,newFile);
    // fwrite(&infoHeader.biClrImportant, 4,1,newFile);

    // fclose(newFile);
    fclose(file);


    return 0;
}
