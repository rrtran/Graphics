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
		"layout (location = 4) in vec2 tc;\n"
		"\n"
		"out VS_OUT\n"
		"{\n"
		"	vec2 tc;\n"
		"} vs_out;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = position;\n"
		"	vs_out.tc = tc;\n"
		"}\n"
	};

	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"\n"
		"layout (binding = 0) uniform sampler2D tex_object;\n"
		"\n"
		"in VS_OUT\n"
		"{\n"
		"	vec2 tc;\n"
		"} fs_in;\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	color = texture(tex_object, fs_in.tc * vec2(3.0, 1.0));\n"
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

	// Create program, attach shaders to it, and link it
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	// delete the shaders as the program has them now
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
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
		const GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);

		const GLfloat position[6][4] = {
			{ -1.0f, -1.0f, 0.5f, 1.0f },
			{ 1.0f, -1.0f, 0.5f, 1.0f },
			{ 1.0f, 1.0f, 0.5f, 1.0f },
			{ 0.0f, 1.0f, 0.5f, 1.0f },
			{ -1.0f, 1.0f, 0.5f, 1.0f },
			{ -1.0f, -1.0f, 0.5f, 1.0f }
		};
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, sizeof(position), &position, 0);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		GLuint vao;
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLint position_index = glGetAttribLocation(rendering_program, "position");
		glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vmath::vec4));
		glVertexArrayAttribFormat(vao, position_index, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, position_index, 0);
		glEnableVertexArrayAttrib(vao, position_index);

		const GLfloat tc[6][2] = {
			{ 1.0f, 0.0f },
			{ 0.5f, 0.5f },
			{ 0.25f, 0.25f },
			{ 1.0f, 0.0f },
			{ 0.5f, 0.5f },
			{ 0.25f, 0.25f }
		};
		GLuint buffer2;
		glCreateBuffers(1, &buffer2);
		glNamedBufferStorage(buffer2, sizeof(tc), &tc, 0);
		glBindBuffer(GL_ARRAY_BUFFER, buffer2);

		GLint tc_index = glGetAttribLocation(rendering_program, "tc");
		glVertexArrayVertexBuffer(vao, 1, buffer2, 0, sizeof(vmath::vec2));
		glVertexArrayAttribFormat(vao, tc_index, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, tc_index, 1);
		glEnableVertexAttribArray(tc_index);

		glUseProgram(rendering_program);

		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, 1, GL_RGBA32F, 256, 256);
		glBindTexture(GL_TEXTURE_2D, texture);

		float* data = new float[256 * 256 * 4];
		generate_texture(data, 256, 256);
		glTextureSubImage2D(texture, 0, 0, 0, 256, 256, GL_RGBA, GL_FLOAT, data);
		delete[] data;

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, texture);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void startup()
	{
		glPointSize(20.0f);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		rendering_program = compile_shaders();
		glCreateVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);
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
	GLuint buffer3;
};

int main()
{
	my_application* app = new my_application();
	app->run(app);
	delete app;
	app = nullptr;
	return 0;
}