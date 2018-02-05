#ifndef __IMAGE_FIGURE_H__
#define __IMAGE_FIGURE_H__

#include <string>
#include <cstring>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <GL/glew.h>

class ImageFigure
{

	public:

		ImageFigure(std::string imageFilename);
		~ImageFigure();

		static void initVertexBuffers();
		static void deinitVertexBuffers();

		std::string getIdentifier() const;
		void setIdentifier(const std::string & newIdentifier);

		std::string getImageFilename() const;
		int getImageWidth() const;
		int getImageHeight() const;
		glm::vec2 getImageDimensions() const;
		
		glm::vec2 getScaling() const;
		void setScaling(const float newScalingX, const float newScalingY);
		void setScaling(const glm::vec2 & newScaling);
		void scale(const float scalingFactorX, const float scalingFactorY);
		void scale(const glm::vec2 & scalingFactor);

		glm::vec2 getTranslation() const;
		void setTranslation(const float newTranslationX, const float newTranslationY);
		void setTranslation(const glm::vec2 & newTranslation);
		void translate(const float translationFactorX, const float translationFactorY);
		void translate(const glm::vec2 & translationFactor);


	private:

		static float * vertexBuffer;
		static short * orderBuffer;
		static int instanceCount;

		std::string identifier;
		std::string srcImageFilename;
		int srcImageWidth, srcImageHeight;

		float * texCoordBuffer;
		short texSampler;

		glm::vec2 scaling;
		glm::vec2 translation;
};

#endif
