#include <iostream>
#include <iomanip>
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
		"const int NUM_OF_VERTICES = 4;\n"
		"\n"
		"layout (binding = 3, offset = 0) uniform atomic_uint area;\n"
		"\n"
		"struct vertex\n"
		"{\n"
		"	vec4 position;\n"
		"	vec4 color;\n"
		"};\n"
		"\n"
		"layout (binding = 2, std430) buffer my_vertices\n"
		"{\n"
		"	vertex vertices[NUM_OF_VERTICES];\n"
		"};\n"
		"\n"
		"struct my_structure\n"
		"{\n"
		"	vec4 potato;\n"
		"	float pea;\n"
		"	float carrot;\n"
		"};\n"
		"\n"
		"layout (binding = 0, std430) buffer my_storage_block\n"
		"{\n"
		"	vec4 foo;\n"
		"	vec4 bar;\n"
		"	my_structure veggies;\n"
		"	float baz[24];\n"
		"};\n"
		"\n"
		"out VS_OUT\n"
		"{\n"
		"	vec4 color;\n"
		"} vs_out; \n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = vertices[gl_VertexID].position; \n"
		"	atomicCounterIncrement(area);\n"
		"	memoryBarrierAtomicCounter();\n"
		"	gl_Position[0] += atomicCounter(area) * 0.00001f;\n"
		"	vs_out.color = vertices[gl_VertexID].color;\n"
		"}\n"
	};

	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"\n"
		"layout (binding = 3) uniform area_block\n"
		"{\n"
		"	uint counter_value;\n"
		"};\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"uniform float max_area;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	float brightness = clamp(float(counter_value) / max_area, 0.0, 1.0);\n"
		"	color = vec4(brightness, brightness, brightness, 1.0);\n"
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
		//const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
		//						  (float)cos(currentTime) * 0.5f + 0.5f, 
		//						  0.0f, 1.0f };
		const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);

		glUseProgram(rendering_program);

		GLfloat vColor[3][4] = { { 1.0f, 0.0f, 0.0f, 1.0f },
								 { 0.0f, 1.0f, 0.0f, 1.0f },
								 { 0.0f, 0.0f, 1.0f, 1.0f } };

		
		glUniform1f(glGetUniformLocation(rendering_program, "max_area"), 20000.0f);

		struct my_structure
		{
			float potato[4];
			float pea;
			float carrot;
		};
		
		struct my_storage_block
		{
			float foo[4];
			float bar[4];
			my_structure veggies;
			float baz[24];
		};

		my_storage_block s =
		{
			{-0.5f, 0.0f, 0.0f, 1.0f},
			{-0.5f, 0.0f, 0.0f, 0.0f},
			{
				{0.5f, 0.0f, 0.0f, 1.0f},
				0.5f,
				-0.5f
			},
			{
				1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
				7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f,
				13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f,
				19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f
			}
		};
		GLfloat position[4] = { 0.5f, 0.0f, 0.0f, 1.0f };

		struct vertex
		{
			GLfloat position[4];
			GLfloat color[4];
		};

		vertex v1 = {
			{-0.5f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f}
		};

		vertex v2 = {
			{0.5f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f}
		};

		vertex v3 = {
			{0.5f, 0.5f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f}
		};

		vertex v4 = {
			{-0.5f, 0.5f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f}
		};

		struct my_vertices
		{
			vertex vertices[4];
		};

		my_vertices v;
		v.vertices[0] = v1;
		v.vertices[1] = v2;
		v.vertices[2] = v3;
		v.vertices[3] = v4;

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
		glNamedBufferStorage(buffer, sizeof(my_storage_block), &s, GL_DYNAMIC_STORAGE_BIT);

		GLuint buffer2;
		glGenBuffers(1, &buffer2);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer2);
		glNamedBufferStorage(buffer2, sizeof(my_vertices), &v, GL_DYNAMIC_STORAGE_BIT);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		std::cout << std::setprecision(2);

		glDrawArrays(GL_POINTS, 0, 4);


		GLuint* ptr = (GLuint*)glMapNamedBufferRange(buffer3, 0, sizeof(GLuint), GL_MAP_READ_BIT);
		unsigned int data = ptr[0];
		glUnmapNamedBuffer(buffer3);
		std::cout << data << std::endl;

		//std::cout << currentTime << std::endl;
	}

	void startup()
	{
		glPointSize(20.0f);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		rendering_program = compile_shaders();
		glCreateVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);

		
		glGenBuffers(1, &buffer3);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer3);

		static const GLuint zero = 0;
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, 16 * sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, buffer3);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer3);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, buffer3);
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