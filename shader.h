#ifndef __SHADER_H__
#define __SHADER_H__

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>

#include <GL/glew.h>

class Shader
{
	public:

		enum class Attrib { POSITION, TEX_COORD };
		enum class Uniform { P_MATRIX, V_MATRIX, M_MATRIX, COLOR, TEXTURE_SAMPLER };
		enum class ProgramProperty { VERTEX_SHADER, FRAGMENT_SHADER, PROGRAM };

		Shader();
		~Shader();
		
		GLint getAttrib(Attrib attribName);
		GLint getUniform(Uniform uniformName);
		GLuint getProgramProperty(ProgramProperty programPropertyName);

		void activate();
		void deactivate();

	private:

		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint program;

		GLint aPosition;
		GLint aTexCoord;

		GLint uPMatrix;
		GLint uVMatrix;
		GLint uMMatrix;

		GLint uColor;
		GLint uTextureSampler;
};

#endif
