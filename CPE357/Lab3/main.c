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



//Lab3 function.
void colorCoding(BYTE* RGB, char* argv[])
{
    //Perfoming the calculations
    for( int i = 0; i < 3; i++)
    {
        float floatNum = (float)RGB[i];
        floatNum = floatNum/255;
        double temp;
        switch (i)
        {
            case 0:
                //blue
                temp = floatNum * atof(argv[4]);
                temp *= 255;
            
            case 1:
                //green
                temp = floatNum *atof(argv[3]);
                temp  *= 255;
            case 2:
                //red
                temp = floatNum * atof(argv[2]);
                temp  *= 255;
        }
        RGB[i] = (BYTE)temp;
       

    }
}


int main(int argc, char* argv[])
{
    struct timeval start, end, start_noFork, end_noFork;
    long duration, duration_noFork;

    //USING FORK while opening the file 
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
    BYTE* pixelArrayB = (BYTE*)mmap(NULL, infoHeader.biSizeImage, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //reading the data from the image
    for( int i = 0; i < infoHeader.biSizeImage; i++)
    {
        fread(&pixelArray[i], infoHeader.biSizeImage,1,file);
    }

    fclose(file);

    //start timing the program 
    gettimeofday(&start, NULL);
    int pid = fork();
    if(pid == 0)
    {
        //child work
        int wb = infoHeader.biWidth * 3;
        if(wb%4 != 0)
            wb += 4 - wb%4;

        for(int y = 0; y < infoHeader.biHeight/2; y++)
        {
            for (int x = 0; x < infoHeader.biWidth; x++)
            {
                BYTE r, g, b;
                BYTE* RGB = (BYTE*)malloc(3);
                b = pixelArray[y*wb + x*3 +0];
                g = pixelArray[y*wb + x*3 +1];
                r = pixelArray[y*wb + x*3 +2];

                RGB[0] = b;
                RGB[1] = g;
                RGB[2] = r;
                //Call color coding function
                colorCoding(RGB, argv);

                pixelArrayB[y*wb + x*3 +0] = RGB[0];
                pixelArrayB[y*wb + x*3 +1] = RGB[1];
                pixelArrayB[y*wb + x*3 +2] = RGB[2];
            }
        }
        return 0;
    }
    //parent
    int wb = infoHeader.biWidth * 3;
        if(wb%4 != 0)
            wb += 4 - wb%4;

    for(int y = infoHeader.biHeight/2; y < infoHeader.biHeight; y++)
        {
            for (int x = 0; x < infoHeader.biWidth; x++)
            {
                BYTE r, g, b;
                BYTE* RGB = (BYTE*)malloc(3);
                b = pixelArray[y*wb + x*3 +0];
                g = pixelArray[y*wb + x*3 +1];
                r = pixelArray[y*wb + x*3 +2];

                
                RGB[0] = b;
                RGB[1] = g;
                RGB[2] = r;
                //Call color coding function
                colorCoding(RGB, argv);

                pixelArrayB[y*wb + x*3 +0] = RGB[0];
                pixelArrayB[y*wb + x*3 +1] = RGB[1];
                pixelArrayB[y*wb + x*3 +2] = RGB[2];
            }
        }
    wait(0);
    gettimeofday(&end, NULL);
    duration = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    printf("The time it requires to change the color contrast with forking is: %ld microseconds\n", duration);

    //NOT USING FORK TO WRITE TO THE FILE
    BYTE* pixelArrayB_noFork = (BYTE*)mmap(NULL, infoHeader.biSizeImage, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    gettimeofday(&start_noFork, NULL);
        wb = infoHeader.biWidth * 3;
        if(wb%4 != 0)
            wb += 4 - wb%4;

        for(int y = 0; y < infoHeader.biHeight; y++)
        {
            for (int x = 0; x < infoHeader.biWidth; x++)
            {
                BYTE r, g, b;
                BYTE* RGB = (BYTE*)malloc(3);
                b = pixelArray[y*wb + x*3 +0];
                g = pixelArray[y*wb + x*3 +1];
                r = pixelArray[y*wb + x*3 +2];

                RGB[0] = b;
                RGB[1] = g;
                RGB[2] = r;
                //Call color coding function
                colorCoding(RGB, argv);

                pixelArrayB[y*wb + x*3 +0] = RGB[0];
                pixelArrayB[y*wb + x*3 +1] = RGB[1];
                pixelArrayB[y*wb + x*3 +2] = RGB[2];
            }
        }
    gettimeofday(&end_noFork, NULL);
    duration_noFork = 1000000 * (end_noFork.tv_sec - start_noFork.tv_sec) + (end_noFork.tv_usec - start_noFork.tv_usec);
    printf("The time it requires to change the color contrast without forking is: %ld microseconds\n", duration_noFork);

    //writing to the new file
    FILE *newFile = fopen(argv[5], "wb");
    fwrite(&fileHeader.bfType, 2, 1, newFile);
    fwrite(&fileHeader.bfSize, 4, 1, newFile);
    fwrite(&fileHeader.bfReserved1, 2, 1, newFile);
    fwrite(&fileHeader.bfReserved2, 2, 1, newFile);
    fwrite(&fileHeader.bfOffBits, 4, 1, newFile);
    fwrite(&infoHeader, sizeof(infoHeader), 1, newFile);
    fwrite(pixelArrayB, infoHeader.biSizeImage, 1, newFile);
    //fwrite(pixelArrayB_noFork, infoHeader.biSizeImage, 1, newFile);
    fclose(newFile);

    free(pixelArray);

    return 0;
}
