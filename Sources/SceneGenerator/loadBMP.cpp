#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include "loadBMP.h"

unsigned char header[54]; // Each BMP file begins by a 54-bytes header
unsigned int dataPos;     // Position in the file where the actual data begins
// unsigned int width, height;
unsigned int imageSize;   // = width*height*3

unsigned char * loadBMP(const char * imagepath, unsigned int& width, unsigned int& height) {

  // Open the file
  FILE * file = NULL;
  fopen_s( &file, imagepath, "rb" );

  if (!file){printf("Image could not be opened\n"); return 0;}
  
  if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
    printf("Not a correct BMP file\n");
    return false;
  }
  
  if ( header[0]!='B' || header[1]!='M' ){
    printf("Not a correct BMP file\n");
    return 0;
  }
  
  // Read ints from the byte array
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);
  
  // Some BMP files are misformatted, guess missing information
  if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos==0)      dataPos=54; // The BMP header is done that way
  
  // Actual RGB data
  unsigned char * data;
  
  // Create a buffer
  data = new unsigned char [imageSize];

  // Read the actual data from the file into the buffer
  fread(data,1,imageSize,file);

  //Everything is in memory now, the file can be closed
  fclose(file);
  
  return data;

}

unsigned char floatToByte( const float color ) {
	return (unsigned char)( color * 255 );
}

void savePPM( const char* pFilePath, unsigned int width, unsigned int height, const float* pPixels ) {
	FILE * file = NULL;
	fopen_s( &file, pFilePath, "wb" );
	if ( !file ) {
		printf( "Image could not be opened\n" ); 
	}

	fprintf( file, "P6\n%u %u\n255\n", width, height );

	for ( unsigned int i = 0; i < width* height; i++ ) {
		unsigned char rgb[3];
		unsigned char color = floatToByte( *pPixels );
		memset( rgb, color, 3 );
		fwrite( rgb, 1, 3, file );
		pPixels++;
	}

	fclose( file );
}
