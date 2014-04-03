#ifndef _ImageIO_h_
#define _ImageIO_h_


#define RGB_MODEL 0
#define RGBA_MODEL 1


#include "Image.h"
#include <string.h>
#include <stdlib.h>

class ImageIO  {
protected:
	//the file name of the image to be written or read
	char* fileName;
public:
	ImageIO(char* fileName){
		this->fileName = fileName;
	}
	~ImageIO(void){
	}

	/**
		This method should be implemented by the extending classes - it will load an image from the given file.
		The imageModel indicates whether the image will be an RGB or RGBA image.
	*/
	virtual Image* loadFromFile(int imageModel = RGB_MODEL) = 0;
	/**
		This method should be implemented by the extending classes - it will save an image in a specific file format.
	*/
	virtual void writeToFile(Image* img) = 0;
};

#endif

