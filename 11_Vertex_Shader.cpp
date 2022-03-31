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
		"\n"
		"float clamp(float a, float b, float c, float d)\n"
		"{\n"
		"	return 1.0 / sqrt(a + b * d + c * d * d);\n"
		"}\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = position;\n"
		"	gl_PointSize = clamp(0, 1, 0, position.z);\n"
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
		glUseProgram(rendering_program);

		typedef struct VERTEX_t
		{
			vmath::vec4 position;
			vmath::vec3 normal;
			vmath::vec2 tex_coord;
			GLubyte color[3];
			int material_id;
		} VERTEX;

		VERTEX_t v;
		v.position = { 0.0f, 0.0f, 0.5, 1.0f };
		v.normal = { 1.0f, 0.0f, 0.0f };
		v.tex_coord = { 1.0f, 0.0f };
		v.color[0] = 1;
		v.color[1] = 2;
		v.color[2] = 3;
		v.material_id = 1;

		glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, offsetof(VERTEX, position));
		glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(VERTEX, normal));
		glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(VERTEX, tex_coord));
		glVertexAttribFormat(4, 3, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(VERTEX, color));
		glVertexAttribFormat(5, 1, GL_INT, GL_TRUE, offsetof(VERTEX, material_id));

		glVertexAttribBinding(0, 0);
		glVertexAttribBinding(1, 0);
		glVertexAttribBinding(2, 0);
		glVertexAttribBinding(4, 0);
		glVertexAttribBinding(5, 0);

		GLuint buffer;
		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, sizeof(VERTEX_t), &v, 0);
		glBindVertexBuffer(0, buffer, 0, sizeof(VERTEX_t));
		glEnableVertexArrayAttrib(vertex_array_object, 0);

		glDrawArrays(GL_POINTS, 0, 1);
		glDeleteBuffers(1, &buffer);
	}

	void startup()
	{
		glPointSize(20.0f);
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

		glEnable(GL_PROGRAM_POINT_SIZE);
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