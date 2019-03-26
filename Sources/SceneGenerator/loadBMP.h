// Data read from the header of the BMP file


unsigned char * loadBMP(const char * imagepath, unsigned int& width, unsigned int& height); 

void savePPM( const char* pFilePath, unsigned int width, unsigned int height, const float* pPixels );