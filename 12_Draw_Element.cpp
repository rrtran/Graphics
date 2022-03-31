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
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec3 normal;\n"
		"layout (location = 2) in vec2 tex_coord;\n"
		"layout (location = 4) in vec4 color;\n"
		"layout (location = 5) in int material_id;\n"
		"uniform mat4 mv_matrix;\n"
		"uniform mat4 proj_matrix;\n"
		"\n"
		"float clamp(float a, float b, float c, float d)\n"
		"{\n"
		"	return 1.0 / sqrt(a + b * d + c * d * d);\n"
		"}\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = proj_matrix * mv_matrix * position;\n"
		"}\n"
	};
	
	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
		"}\n"
	};
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	
	GLint log_length;
	const int BUFFER_SIZE = 255;
	GLchar log[255];
	glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
	glGetShaderInfoLog(vertex_shader, BUFFER_SIZE, NULL, log);
	if (log_length > 0)
		std::cout << "Vertex compilation error - Log length? " << log_length << ": " << log << std::endl;

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	log_length = 0;
	const int SHADER_LOG_BUFFER_SIZE = 255;
	GLchar shader_log[SHADER_LOG_BUFFER_SIZE];
	glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
	glGetShaderInfoLog(fragment_shader, SHADER_LOG_BUFFER_SIZE, NULL, shader_log);
	if (log_length > 0)
		std::cout << "Fragment compilation error - log length? " << log_length << ": " << shader_log << std::endl;

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	glLinkProgram(program);

	log_length = 0;
	const int PROGRAM_BUFFER_SIZE = 255;
	GLchar program_log[255];
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	glGetProgramInfoLog(program, PROGRAM_BUFFER_SIZE, NULL, program_log);
	if (log_length > 0)
		std::cout << "Link error - Log length? " << log_length << ": " << program_log << std::endl;

	GLint binary_size = 0;
	glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_size);
	unsigned char* program_binary = new unsigned char[binary_size];

	GLenum binary_format = GL_NONE;
	glGetProgramBinary(program, binary_size, NULL, &binary_format, program_binary);

	GLuint fresh_program_object;
	glProgramBinary(program, binary_format, program_binary, binary_size);

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

		static const GLfloat vertex_positions[] =
		{
			-0.25f, -0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,
			-0.25f, -0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f
		};

		static const GLushort vertex_indices[] =
		{
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			6, 0, 2,
			2, 4, 6,
			7, 5, 3,
			7, 3, 1
		};

		GLuint position_buffer;
		glGenBuffers(1, &position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		GLuint index_buffer;
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

		glUseProgram(rendering_program);
		float f = (float)currentTime * (float)M_PI * 0.1f;
		vmath::mat4 mv_matrix =
			vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::translate(sinf(2.1f * f) * 0.5f,
							 cosf(1.7f * f) * 0.5f,
							 sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
			vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(glGetUniformLocation(rendering_program, "mv_matrix"), 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(glGetUniformLocation(rendering_program, "proj_matrix"), 1, GL_FALSE, proj_matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		glDeleteBuffers(1, &position_buffer);
		glDeleteBuffers(1, &index_buffer);
		
	}

	void startup()
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		rendering_program = compile_shaders();
		glBindProgramPipeline(rendering_program);
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

		int data[2];
		glGetIntegerv(GL_POINT_SIZE_RANGE, data);
		std::cout << "Min size: " << data[0] << std::endl;
		std::cout << "Max size: " << data[1] << std::endl;
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