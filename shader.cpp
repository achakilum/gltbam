#include "shader.h"

Shader::Shader()
{
	glGetError(); // Reset GL error flag

	// Source code for shaders
	std::stringstream vertexShaderSourceStream;
	vertexShaderSourceStream << "" <<
                "attribute vec3 aPosition;" <<
                "attribute vec2 aTexCoord;" <<
                "" <<
                "uniform mat4 uPMatrix;" <<
                "uniform mat4 uVMatrix;" <<
                "uniform mat4 uMMatrix;" <<
                "" <<
                "varying vec2 vTexCoord;" <<
                "" <<
                "void main(void)" <<
                "{" <<
                "   vTexCoord = aTexCoord;" <<
                "" <<
                "   gl_Position = uPMatrix * uVMatrix * uMMatrix * vec4(aPosition, 1.0);" <<
                "}" <<
                "";
	std::string vertexShaderSourceString = vertexShaderSourceStream.str();
	const GLchar * vertexShaderSourceGLString = (GLchar *) vertexShaderSourceString.c_str();

	std::stringstream fragmentShaderSourceStream;
	fragmentShaderSourceStream << "" <<
                "uniform sampler2D uTextureSampler;" <<
                "uniform vec4 uColor;" <<
                "" <<
                "varying vec2 vTexCoord;" <<
                "" <<
                "void main(void)" <<
                "{" <<
                "   vec4 texColor = texture2D(uTextureSampler, vTexCoord);" <<
                "   vec4 plainColor = uColor;" <<
                "" <<
                "   gl_FragColor = texColor;" <<
                "}" <<
                "";
	std::string fragmentShaderSourceString = fragmentShaderSourceStream.str();
	const GLchar * fragmentShaderSourceGLString = (GLchar *) fragmentShaderSourceString.c_str();

	// Compile shaders
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSourceGLString, NULL);
	glCompileShader(vertexShader);

	GLint vertexShaderCompileSuccess = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompileSuccess);

	if (vertexShaderCompileSuccess == GL_FALSE)
	{
		GLint errorLogLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &errorLogLength);
		
		GLchar * errorLog = new GLchar[errorLogLength];
		glGetShaderInfoLog(vertexShader, errorLogLength, &errorLogLength, &errorLog[0]);
		std::cerr << "Vertex shader error: " << errorLog << std::endl;
		delete errorLog;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceGLString, NULL);
	glCompileShader(fragmentShader);

	GLint fragmentShaderCompileSuccess = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompileSuccess);

	if (fragmentShaderCompileSuccess == GL_FALSE)
	{
		GLint errorLogLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &errorLogLength);
		
		GLchar * errorLog = new GLchar[errorLogLength];
		glGetShaderInfoLog(fragmentShader, errorLogLength, &errorLogLength, &errorLog[0]);
		std::cerr << "Fragment shader error: " << errorLog << std::endl;
		delete errorLog;
	}

	// Link shader program and enable program
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint programLinkSuccess = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &programLinkSuccess);
	
	if (programLinkSuccess == GL_FALSE)
	{
		GLint errorLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLogLength);
		
		GLchar * errorLog = new GLchar[errorLogLength];
		glGetProgramInfoLog(program, errorLogLength, &errorLogLength, &errorLog[0]);
		std::cerr << "Shader program error: " << errorLog << std::endl;
		delete errorLog; 
	}

        aPosition = glGetAttribLocation(program, "aPosition");
        aTexCoord = glGetAttribLocation(program, "aTexCoord");

        uPMatrix = glGetUniformLocation(program, "uPMatrix");
        uVMatrix = glGetUniformLocation(program, "uVMatrix");
        uMMatrix = glGetUniformLocation(program, "uMMatrix");

        uColor = glGetUniformLocation(program, "uColor");
        uTextureSampler = glGetUniformLocation(program, "uTextureSampler");
}

Shader::~Shader()
{
	this->deactivate();

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glDeleteProgram(program);
}

GLint Shader::getAttrib(Shader::Attrib attribName)
{
        GLint attrib = 0;

        switch(attribName)
        {
            case Attrib::POSITION:
                attrib = aPosition;
                break;
            case Attrib::TEX_COORD:
                attrib = aTexCoord;
                break;
            default:
                attrib = -1;
                break;
        }

        return attrib;
}

GLint Shader::getUniform(Shader::Uniform uniformName)
{
        GLint uniform = 0;

        switch(uniformName)
        {
            case Uniform::P_MATRIX:
                uniform = uPMatrix;
                break;
            case Uniform::V_MATRIX:
                uniform = uVMatrix;
                break;
            case Uniform::M_MATRIX:
                uniform = uMMatrix;
                break;
            case Uniform::COLOR:
                uniform = uColor;
                break;
            case Uniform::TEXTURE_SAMPLER:
                uniform = uTextureSampler;
                break;
            default:
                uniform = -1;
                break;
        }

        return uniform;
}

GLuint Shader::getProgramProperty(Shader::ProgramProperty programPropertyName)
{
        int programProperty = 0;

        switch(programPropertyName)
        {
            case ProgramProperty::VERTEX_SHADER:
                programProperty = vertexShader;
                break;
            case ProgramProperty::FRAGMENT_SHADER:
                programProperty = fragmentShader;
                break;
            case ProgramProperty::PROGRAM:
                programProperty = program;
                break;
            default:
                programProperty = -1;
                break;
        }

        return programProperty;
}

void Shader::activate()
{
	glUseProgram(program);

        glEnableVertexAttribArray(aPosition);
        glEnableVertexAttribArray(aTexCoord);
}

void Shader::deactivate()
{
	glDisableVertexAttribArray(aPosition);
	glDisableVertexAttribArray(aTexCoord);

	glUseProgram(0);
}
