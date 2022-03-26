#include <iostream>
#include <iomanip>
#include <vector>
#include "include/sb7.h"
#include "include/sb7ktx.h"
#include "include/vmath.h"

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar* vertex_shader_source[] =
	{
		"#version 450 core\n"
		"\n"
		"layout (location = 0) in vec4 v1;\n"
		"layout (location = 2) in vec4 v2;\n"
		"in vec4 v3;\n"
		"out vec4 v4;\n"
		"out VS_OUT\n"
		"{\n"
		"	vec4 x;\n"
		"} vs_out;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	v4 = v3 + vs_out.x;\n"
		"	gl_Position = v1 + v2 + v3;\n"
		"}\n"
	};
	
	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"\n"
		"out vec4 color;\n"
		"layout (location = 2) out ivec2 data;\n"
		"out float extra;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	color = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
		"	data = ivec2(0, 0);\n"
		"	extra = 0.0f;\n"
		"}\n"
	};

	// create and compile vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	// Create and compile fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	GLint is_compiled;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);

	GLint log_length;
	glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
	GLchar* log = new GLchar[log_length];
	glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
	std::cout << "compiled? " << is_compiled << std::endl;
	if (log_length > 0)
		std::cout << "log length? " << log_length << ": " << log << std::endl;
	delete[] log;
	log = nullptr;

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint is_linked;
	glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	log = new GLchar[log_length];
	glGetProgramInfoLog(program, log_length, NULL, log);
	std::cout << "linked? " << is_linked << std::endl;
	if (log_length > 0)
		std::cout << "log length? " << log_length << ": " << log << std::endl;
	delete[] log;
	log = nullptr;

	// delete the shaders as the program has them now
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}

std::string type_to_name(GLint n)
{
	std::string result;
	if (n == 35666) result = "vec4";
	else if (n == 35667) result = "ivec2";
	else if (n == 5126) result = "float";
	return result;
}

void generate_texture(float* data, int w, int h)
{
	int size = w * h * 4;
	for (int i = 0; i < size; i++) {
		int n = i % 64;
		if (n < 16) {
			data[i] = 0.0f;
		}
		else if (n < 32)
		{
			data[i] == 0.25f;
		}
		else if (n < 48) 
		{
			data[i] = 0.5f;
		}
		else {
			data[i] = 0.75f;
		}
		
	}
}

class my_application : public sb7::application {
public:
	void onResize(int w, int h)
	{
		sb7::application::onResize(w, h);

		float aspect = (float)w / (float)h;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

	void render(double currentTime) 
	{
		//const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
		//						  (float)cos(currentTime) * 0.5f + 0.5f, 
		//						  0.0f, 1.0f };
		const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);
		glUseProgram(rendering_program);

		glDrawArrays(GL_POINTS, 0, 1);
	}

	void startup()
	{
		glPointSize(20.0f);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		rendering_program = compile_shaders();
		glCreateVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);
		
		GLint outputs;
		glGetProgramInterfaceiv(rendering_program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &outputs);
		std::cout << "Total outputs: " << outputs << std::endl;
		static const GLenum props[] = { GL_TYPE, GL_LOCATION };

		GLint i;
		GLint params[2];
		GLchar name[64];
		const char* type_name;

		for (int i = 0; i < outputs; i++)
		{
			glGetProgramResourceName(rendering_program, GL_PROGRAM_OUTPUT, i, sizeof(name), NULL, name);
			glGetProgramResourceiv(rendering_program, GL_PROGRAM_OUTPUT, i, sizeof(props), props, sizeof(params), NULL, params);
			type_name = type_to_name(params[0]).c_str();
			printf("Index %d: %s %s @ location %d.\n", i, type_name, name, params[1]);
			//printf("Index %d: %d %s @ location %d.\n", i, params[0], name, params[1]);
		}
	}

	void shutdown()
	{
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vertex_array_object);
	}

private:
	GLuint rendering_program;
	GLuint vertex_array_object;
	vmath::mat4 proj_matrix;
};

int main()
{
	my_application* app = new my_application();
	app->run(app);
	delete app;
	app = nullptr;
	return 0;
}