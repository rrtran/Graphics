#include <iostream>
#include <vector>
#include "include/sb7.h"
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
		"uniform mat4 mv_matrix;\n"
		"uniform mat4 proj_matrix;\n"
		"\n"
		"out VS_OUT\n"
		"{\n"
		"	vec4 color;\n"
		"} vs_out;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),\n"
		"									 vec4(-0.25, -0.25, 0.5, 1.0),\n"
		"									 vec4(0.25, 0.25, 0.5, 1.0));\n"
		"	gl_Position = proj_matrix * mv_matrix * position;\n"
		"	vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);\n"
		"}\n"
	};

	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"\n"
		"in VS_OUT\n"
		"{\n"
		"	vec4 color;\n"
		"} fs_in;\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	color = fs_in.color;\n"
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
		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
								  (float)cos(currentTime) * 0.5f + 0.5f, 
								  0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);

		glUseProgram(rendering_program);

		static const GLfloat vertex_positions[] =
		{
			 -0.25f,  0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			 0.25f, -0.25f, -0.25f,

			 0.25f, -0.25f, -0.25f,
			 0.25f,  0.25f, -0.25f,
			-0.25f,  0.25f, -0.25f,

			 0.25f, -0.25f, -0.25f,
			 0.25f, -0.25f,  0.25f,
			 0.25f,  0.25f, -0.25f,

			 0.25f, -0.25f,  0.25f,
			 0.25f,  0.25f,  0.25f,
			 0.25f,  0.25f, -0.25f,

			 0.25f, -0.25f,  0.25f,
			-0.25f, -0.25f,  0.25f,
			 0.25f,  0.25f,  0.25f,

			-0.25f, -0.25f,  0.25f,
			-0.25f,  0.25f,  0.25f,
			 0.25f,  0.25f,  0.25f,

			-0.25f, -0.25f,  0.25f,
			-0.25f, -0.25f, -0.25f,
			-0.25f,  0.25f,  0.25f,

			-0.25f, -0.25f, -0.25f,
			-0.25f,  0.25f, -0.25f,
			-0.25f,  0.25f,  0.25f,

			-0.25f, -0.25f,  0.25f,
			 0.25f, -0.25f,  0.25f,
			 0.25f, -0.25f, -0.25f,

			 0.25f, -0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			-0.25f, -0.25f,  0.25f,

			-0.25f,  0.25f, -0.25f,
			 0.25f,  0.25f, -0.25f,
			 0.25f,  0.25f,  0.25f,

			 0.25f,  0.25f,  0.25f,
			-0.25f,  0.25f,  0.25f,
			-0.25f,  0.25f, -0.25f
		};

		GLuint buffer;
		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, sizeof(vertex_positions), vertex_positions, GL_MAP_WRITE_BIT);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		
		void* ptr = glMapNamedBuffer(buffer, GL_WRITE_ONLY);
		memcpy(ptr, vertex_positions, sizeof(vertex_positions));
		glUnmapNamedBuffer(GL_ARRAY_BUFFER);

		GLuint vao;
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vmath::vec3));
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);
		glEnableVertexArrayAttrib(vao, 0);

		float f = (float)currentTime * (float)M_PI * 0.1f;
			vmath::mat4 mv_matrix =
			vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(glGetUniformLocation(rendering_program, "mv_matrix"), 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(glGetUniformLocation(rendering_program, "proj_matrix"), 1, GL_FALSE, proj_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		

		std::cout << currentTime << std::endl;
	}

	void startup()
	{
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
};

int main()
{
	my_application* app = new my_application();
	app->run(app);
	delete app;
	app = nullptr;
	return 0;
}