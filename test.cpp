#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "image_figure.h"
#include "gltbam.h"

Shader * shader;
glm::mat4 perspectiveMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

std::vector<float> vertexArr;
std::vector<short> orderArr;
std::vector<float> texCoordArr;

std::vector<GLuint> textureIds;
std::vector<std::string> imageTags;

int imageIdx;
int samplerId;

static void error_callback(int error, const char * desc);
static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);

void initializeShaders();
void initializeBuffers();

void deinitializeShaders();
void deinitializeBuffers();

void drawTextureSpaces();
void drawTexture(std::string tTag);

/**
 *	Handle GLFW errors
 */
static void error_callback(int error, const char * desc)
{
	std::cerr << "GLFW Error: " << desc << std::endl;
}

/**
 *	Handle keyboard commands
 */
static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		if (samplerId > 0 && samplerId <= 15) { samplerId--; };
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		if (samplerId >= 0 && samplerId < 15) { samplerId++; };
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if (imageIdx > 0 && imageIdx <= (imageTags.size() - 1)) { imageIdx--; };
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if (imageIdx >= 0 && imageIdx < (imageTags.size() - 1)) { imageIdx++; };
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void initializeShaders()
{
	samplerId = 0;

	perspectiveMatrix = glm::ortho(-21.0f / 18.0f, 21.0f / 18.0f, -0.5f, 0.5f);
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	shader = new Shader();
	shader->activate();
}

void deinitializeShaders()
{
	shader->deactivate();
	delete shader; 	shader = NULL;
}

void initializeBuffers()
{
	glGetError(); // Reset GL error flag

        float vertexArrSrc[] = { 0.5f,  0.5f,  0.0f,
				-0.5f,  0.5f,  0.0f,
				-0.5f, -0.5f,  0.0f,
				 0.5f, -0.5f,  0.0f};
	vertexArr.insert(vertexArr.end(), std::begin(vertexArrSrc), std::end(vertexArrSrc));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	short orderArrSrc[] = {0, 1, 2,
			       0, 2, 3};
	orderArr.insert(orderArr.end(), std::begin(orderArrSrc), std::end(orderArrSrc));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float texCoordArrSrc[] = {1.0f, 0.0f,
				  0.0f, 0.0f,
				  0.0f, 1.0f,
				  1.0f, 1.0f};
	texCoordArr.insert(texCoordArr.end(), std::begin(texCoordArrSrc), std::end(texCoordArrSrc));
}

void deinitializeBuffers()
{

}

void drawTextureSpaces()
{
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));

	glUniformMatrix4fv(shader->getUniform(Shader::Uniform::P_MATRIX), 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	glUniformMatrix4fv(shader->getUniform(Shader::Uniform::V_MATRIX), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shader->getUniform(Shader::Uniform::M_MATRIX), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform1i(shader->getUniform(Shader::Uniform::TEXTURE_SAMPLER), samplerId);
	glUniform4f(shader->getUniform(Shader::Uniform::COLOR), 0.0f, 0.0f, 1.0f, 1.0f);
	glVertexAttribPointer(shader->getAttrib(Shader::Attrib::POSITION), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vertexArr.data());
	glVertexAttribPointer(shader->getAttrib(Shader::Attrib::TEX_COORD), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), texCoordArr.data());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *) orderArr.data());
}

void drawTexture(std::string tTag)
{
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));

	glUniformMatrix4fv(shader->getUniform(Shader::Uniform::P_MATRIX), 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	glUniformMatrix4fv(shader->getUniform(Shader::Uniform::V_MATRIX), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shader->getUniform(Shader::Uniform::M_MATRIX), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform1i(shader->getUniform(Shader::Uniform::TEXTURE_SAMPLER), gltbam::tsampler(tTag));
	glUniform4f(shader->getUniform(Shader::Uniform::COLOR), 0.0f, 0.0f, 1.0f, 1.0f);
	glVertexAttribPointer(shader->getAttrib(Shader::Attrib::POSITION), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vertexArr.data());
	glVertexAttribPointer(shader->getAttrib(Shader::Attrib::TEX_COORD), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), gltbam::tcoords(tTag));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *) orderArr.data());
}

/**
 *	Main function
 */
int main(int argc, char ** argv)
{

	// GLFW error callback
	glfwSetErrorCallback(error_callback);
	
	// Initialize GLFW window with context and key callback
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow * window = glfwCreateWindow(1120, 480, "Test", NULL, NULL);

	if (!window)
	{
		std::cerr << "General Error: Failure to open GLFW window." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLEW context

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
	}


	// Print GL Version
	const GLubyte * glVersion = glGetString(GL_VERSION);
	if (glVersion)
	{
		std::cerr << glVersion << std::endl;
	}
	else
	{
		std::cerr << "GL Error: Could not find GL version" << std::endl;
	}

	// Before drawing
	initializeShaders();
	initializeBuffers();

	imageIdx = 0;
	imageTags.clear();

	gltbam::init();

	for (int i = 0; i < 10; i++)
	{
		std::string tTag = "";

		if (i % 3 == 0)
		{
			tTag = gltbam::talloc("./assets/mcr.jpg");
		}

		if (i % 3 == 1)
		{
			tTag = gltbam::talloc("./assets/mcg.jpg");
		}

		if (i % 3 == 2)
		{
			tTag = gltbam::talloc("./assets/mcb.jpg");
		}

		imageTags.push_back(tTag);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Drawing
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		drawTextureSpaces();
		drawTexture(imageTags[imageIdx]);

		glfwSwapBuffers(window);
		glfwPollEvents();	
	}

	// After drawing
	for (int i = 0; i < imageTags.size(); i++)
	{
		gltbam::tfree(imageTags[i]);
	}
	gltbam::deinit();

	deinitializeBuffers();
	deinitializeShaders();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
