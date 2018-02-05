#include "image_figure.h"

float * ImageFigure::vertexBuffer = NULL;
short * ImageFigure::orderBuffer = NULL;
int ImageFigure::instanceCount = 0;

ImageFigure::ImageFigure(std::string imageFilename)
{
	if (instanceCount <= 0)
	{
		initVertexBuffers();
	}

	instanceCount++;

	identifier = "img" + std::to_string(instanceCount) + "";
	srcImageFilename = imageFilename;
	srcImageWidth = 2048;
	srcImageHeight = 2048;

	texCoordBuffer = NULL;
	texSampler = 0;

	scaling = glm::vec2(1.0f, 1.0f);
	translation = glm::vec2(0.0f, 0.0f);
}

ImageFigure::~ImageFigure()
{
	instanceCount--;

	if (instanceCount <= 0)
	{
		deinitVertexBuffers();
	}

	delete texCoordBuffer; texCoordBuffer = NULL;
}

void ImageFigure::initVertexBuffers()
{
        float vertexBufSrc[12] = { 0.5f,  0.5f,  0.0f,
                     	          -0.5f,  0.5f,  0.0f,
                     	          -0.5f, -0.5f,  0.0f,
              		           0.5f, -0.5f,  0.0f};
	vertexBuffer = new float[12];
	std::memcpy(vertexBuffer, vertexBufSrc, 12 * sizeof(float));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	short orderBufSrc[6] = {0, 1, 2,
			        0, 2, 3};
	orderBuffer = new short[6];
	std::memcpy(orderBuffer, orderBufSrc, 6 * sizeof(short));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ImageFigure::deinitVertexBuffers()
{
	delete vertexBuffer; vertexBuffer = NULL;
	delete orderBuffer; orderBuffer = NULL;
}

std::string ImageFigure::getIdentifier() const
{
	return this->identifier;
}

void ImageFigure::setIdentifier(const std::string & newIdentifier)
{
	this->identifier = newIdentifier;
}


std::string ImageFigure::getImageFilename() const
{
	return this->srcImageFilename;
}


int ImageFigure::getImageWidth() const
{
	return this->srcImageWidth;
}

int ImageFigure::getImageHeight() const
{
	return this->srcImageHeight;
}

glm::vec2 ImageFigure::getImageDimensions() const
{
	return glm::vec2(this->srcImageWidth, this->srcImageHeight);
}


glm::vec2 ImageFigure::getScaling() const
{
	return this->scaling;
}

void ImageFigure::setScaling(const float newScalingX, const float newScalingY)
{
	this->scaling.x = newScalingX;
	this->scaling.y = newScalingY;
}

void ImageFigure::setScaling(const glm::vec2 & newScaling)
{
	this->scaling = newScaling;
}

void ImageFigure::scale(const float scalingFactorX, const float scalingFactorY)
{
	this->scaling.x *= scalingFactorX;
	this->scaling.y *= scalingFactorY;
}

void ImageFigure::scale(const glm::vec2 & scalingFactor)
{
	this->scaling.x *= scalingFactor.x;
	this->scaling.y *= scalingFactor.y;
}


glm::vec2 ImageFigure::getTranslation() const
{
	return this->translation;
}

void ImageFigure::setTranslation(const float newTranslationX, const float newTranslationY)
{
	this->translation.x = newTranslationX;
	this->translation.y = newTranslationY;
}

void ImageFigure::setTranslation(const glm::vec2 & newTranslation)
{
	this->translation = newTranslation;
}

void ImageFigure::translate(const float translationFactorX, const float translationFactorY)
{
	this->translation.x += translationFactorX;
	this->translation.y += translationFactorY;
}

void ImageFigure::translate(const glm::vec2 & translationFactor)
{
	this->translation.x += translationFactor.x;
	this->translation.y += translationFactor.y;
}

