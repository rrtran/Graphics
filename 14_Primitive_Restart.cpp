#include <iostream>
#include "include/sb7.h"
#include "include/sb7ktx.h"
#include "include/vmath.h"

class my_application : public sb7::application {
public:
	// render - Specifies the background color and input data to the vertex shader
	// @param currentTime - program execution time in seconds starting at 0 seconds
	void render(double currentTime)
	{
		std::cout << currentTime << std::endl;

		// Define vertex data with duplicate vertices
		const GLfloat vertex_positions[108] =
		{
			-0.25f,  0.25f, -0.25f, // vertex 1
			-0.25f, -0.25f, -0.25f, // vertex 2
			 0.25f, -0.25f, -0.25f, // vertex 3

			 0.25f, -0.25f, -0.25f, // duplicate of vertex 3
			 0.25f,  0.25f, -0.25f,  // vertex 4
			-0.25f,  0.25f, -0.25f, // duplicate of vertex 1

			 0.25f, -0.25f, -0.25f, // duplicate of vertex 3
			 0.25f, -0.25f,  0.25f, // vertex 5
			 0.25f,  0.25f, -0.25f, // duplicate of vertex 4

			 0.25f, -0.25f,  0.25f, // duplicate of vertex 5
			 0.25f,  0.25f,  0.25f, // vertex 6
			 0.25f,  0.25f, -0.25f, // duplicate of vertex 4

			 0.25f, -0.25f,  0.25f, // duplicate of vertex 5
			-0.25f, -0.25f,  0.25f, // vertex 7
			 0.25f,  0.25f,  0.25f, // duplicate of vertex 6

			-0.25f, -0.25f,  0.25f, // duplicate of vertex 7
			-0.25f,  0.25f,  0.25f, // vertex 8
			 0.25f,  0.25f,  0.25f, // duplicate of vertex 6

			-0.25f, -0.25f,  0.25f, // duplicate of vertex 7
			-0.25f, -0.25f, -0.25f, // duplicate of vertex 2
			-0.25f,  0.25f,  0.25f, // duplicate of vertex 8

			-0.25f, -0.25f, -0.25f, // duplicate of vertex 2
			-0.25f,  0.25f, -0.25f, // duplicate of vertex 1
			-0.25f,  0.25f,  0.25f, // duplicate of vertex 8

			-0.25f, -0.25f,  0.25f, // duplicate of vertex 7
			 0.25f, -0.25f,  0.25f, // duplicate of vertex 5
			 0.25f, -0.25f, -0.25f, // duplicate of vertex 3

			 0.25f, -0.25f, -0.25f, // duplicate of vertex 3
			-0.25f, -0.25f, -0.25f, // duplicate of vertex 2
			-0.25f, -0.25f,  0.25f, // duplicate of vertex 7

			-0.25f,  0.25f, -0.25f, // duplicate of vertex 2
			 0.25f,  0.25f, -0.25f, // duplicate of vertex 4
			 0.25f,  0.25f,  0.25f, // duplicate of vertex 6

			 0.25f,  0.25f,  0.25f, // duplicate of vertex 6
			-0.25f,  0.25f,  0.25f, // duplicate of vertex 8
			-0.25f,  0.25f, -0.25f // duplicate of vertex 2
		};

		GLuint position_buffer;
		glGenBuffers(1, &position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		int position_index = glGetAttribLocation(program, "position");
		glEnableVertexArrayAttrib(vertex_array_object, position_index);

		// Define vertex data with unique vertices
		const GLfloat vertex_positions2[] = 
		{ 
			-0.25f, -0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,
			-0.25f, -0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
		};

		const GLushort vertex_indices[] =
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

		const int VERTEX_BUFFER_BINDING_INDEX_0 = 0;
		const int VERTEX_BUFFER_BINDING_INDEX_1 = 1;
		const int VERTEX_BUFFER_BINDING_INDEX_2 = 2;

		GLuint position2_buffer;
		glGenBuffers(1, &position2_buffer);
		glNamedBufferStorage(position2_buffer, sizeof(vertex_positions2), vertex_positions2, 0);
		int position2_index = glGetAttribLocation(program, "position2");
		glVertexArrayAttribFormat(vertex_array_object, position2_index, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayVertexBuffer(vertex_array_object, VERTEX_BUFFER_BINDING_INDEX_0, position2_buffer, 0, sizeof(vmath::vec3));
		glVertexArrayAttribBinding(vertex_array_object, position2_index, VERTEX_BUFFER_BINDING_INDEX_0);
		glEnableVertexArrayAttrib(vertex_array_object, position2_index);

		GLuint index_buffer;
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer); // Binding the index buffer to GL_ELEMENT_ARRAY_BUFFER somehow tells the glDrawElements() function to draw the unique vertices in the order specified in vertex_indices
		glNamedBufferData(index_buffer, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW); // Assign the vertex_indices to the GL_ARRAY_BUFFER

		const GLfloat vertex_positions3[] =
		{
			-0.25f, -0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, -0.25f
		};

		GLfloat vertex_positions4[] =
		{
			-1.0f, -1.0f, 0.0f,
			-0.5f, -1.0f, 0.0f,
			-1.0f, -0.5f, 0.0f,
			1.0f, 1.0f, 0.0f,
			0.5f, 1.0f, 0.0f,
			1.0f, 0.5f, 0.0f
		};

		GLuint positions4_buffer;
		glCreateBuffers(1, &positions4_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, positions4_buffer);
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertex_positions4), vertex_positions4, 0);
		int position4_index = glGetAttribLocation(program, "position4");
		glVertexArrayAttribFormat(vertex_array_object, position4_index, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayVertexBuffer(vertex_array_object, VERTEX_BUFFER_BINDING_INDEX_1, positions4_buffer, 0, sizeof(vmath::vec3));
		glVertexArrayAttribBinding(vertex_array_object, position4_index, VERTEX_BUFFER_BINDING_INDEX_1);
		glEnableVertexArrayAttrib(vertex_array_object, position4_index);

		GLfloat vertex_positions5[] =
		{
			-1.0f, 1.0f, 0.0f,		// v0
			-0.9375, 0.75f, 0.0f,	// v1
			-0.875f, 1.0f, 0.0f,	// v2
			-0.8125f, 0.75f, 0.0f,	// v3
			-0.75f, 1.0f, 0.0f,		// v4
			-0.6875f, 0.75f, 0.0f,	// v5
			-0.625f, 1.0f, 0.0f,	// v6
			-0.5625f, 0.75f, 0.0f,	// v7
			-0.5f, 1.0f, 0.0f,		// v8
			-0.4375f, 0.75f, 0.0f,	// v9
			-0.375f, 1.0f, 0.0f,	// v10
			-0.3125f, 0.75f, 0.0f,	// v11
			-0.25f, 1.0f, 0.0f,		// v12
			-0.1875f, 0.75f, 0.0f,	// v13
			-0.125f, 1.0f, 0.0f,	// v14
			-0.0625f, 0.75f, 0.0f,	// v15
			0.0f, 1.0f, 0.0f,		// v16
		};

		GLuint positions5_buffer;
		glCreateBuffers(1, &positions5_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, positions5_buffer);
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertex_positions5), vertex_positions5, 0);
		int position5_index = glGetAttribLocation(program, "position5");
		glVertexArrayAttribFormat(vertex_array_object, position5_index, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayVertexBuffer(vertex_array_object, VERTEX_BUFFER_BINDING_INDEX_2, positions5_buffer, 0, sizeof(vmath::vec3));
		glVertexArrayAttribBinding(vertex_array_object, position5_index, VERTEX_BUFFER_BINDING_INDEX_2);
		glEnableVertexArrayAttrib(vertex_array_object, position5_index);

		GLushort vertex_indices2[] = { 
			0, 1, 2,	// Draw a triangle
			1, 3, 2,	// Draw another triangle
			2, 3, 4,	// Draw another triangle
			3, 5, 4,	// ""
			4, 5, 6,	// ""
			5, 7, 6,	// ""
			6, 7, 8,	// ""
			7, 9, 8,	// ""
			8, 9, 10,	// ""
			9, 11, 10,	// ""
			10, 11, 12,	// ""
			11, 13, 12,	// ""
			12, 13, 14,	// ""
			13, 15, 14, // ""
			14, 15, 16	// Draw another triangle
		};

		GLuint index2_buffer;
		glCreateBuffers(1, &index2_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index2_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices2), vertex_indices2, GL_STATIC_DRAW);

		const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // #000000FF - black and opaque
		glClearBufferfv(GL_COLOR, 0, color);	// Clear the buffer leaving only a black-colored background
		glUseProgram(program);
		glEnable(GL_PRIMITIVE_RESTART); // Enable primitive restart
		glPrimitiveRestartIndex(8); // Specify the 8th index as the primitive restart index
		//glDrawArrays(GL_TRIANGLES, 0, 36); // Draw 36 vertices as defined in vertex_positions2
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0); // Draw 36 vertices by drawing the unique vertices in vertex_position2 in the order specified by vertex_indices
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLE_STRIP, 45, GL_UNSIGNED_SHORT, 0);

		glDeleteBuffers(1, &position_buffer);
		glDeleteBuffers(1, &position2_buffer);
		glDeleteBuffers(1, &positions4_buffer);
		glDeleteBuffers(1, &positions5_buffer);
		glDeleteBuffers(1, &index_buffer);
		glDeleteBuffers(1, &index2_buffer);
	}

	// startup - Define the source code for the vertex and fragment shaders, compile shaders, and link shader objects into a program object.
	//			 Creates a vertex array object and binds it to the current context.
	void startup()
	{
		// Define vertex shader source code
		static const GLchar* vertex_shader_source[] =
		{
			"#version 450 core\n"
			"\n"
			"layout (location = 0) in vec4 position;\n"
			"layout (location = 1) in vec4 position2;\n"
			"layout (location = 2) in vec4 position4;\n"
			"layout (location = 3) in vec4 position5;\n"
			"\n"
			"void main(void)\n"
			"{\n"
			"	gl_Position = position5;\n"
			"}\n"
		};

		// Define fragment shader source code
		static const GLchar* fragment_shader_source[]
		{
			"#version 450 core\n"
			"out vec4 color;\n"
			"void main(void)\n"
			"{\n"
			"	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
			"}\n"
		};

		// Compile vertex shader
		GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
		glCompileShader(vertex_shader);

		// Check vertex shader compilation status and output info if error
		GLint compile_status;
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
		if (compile_status == 0) // false
		{
			GLint log_length;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* vertex_shader_info_log = new GLchar[log_length];
			glGetShaderInfoLog(vertex_shader, log_length, NULL, vertex_shader_info_log);
			std::cout << "Vertex Shader INFO: " << vertex_shader_info_log << std::endl;
			delete[] vertex_shader_info_log;
			vertex_shader_info_log = nullptr;
		}

		// Compile fragment shader
		GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
		glCompileShader(fragment_shader);

		// Check fragment shader compilation status and output info if error
		GLint fragment_shader_compile_status;
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compile_status);
		if (fragment_shader_compile_status == 0)
		{
			GLint log_length;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* fragment_shader_info_log = new GLchar[log_length];
			glGetShaderInfoLog(fragment_shader, log_length, NULL, fragment_shader_info_log);
			std::cout << "Fragment Shader INFO: " << fragment_shader_info_log << std::endl;
			delete[] fragment_shader_info_log;
			fragment_shader_info_log = nullptr;
		}

		// Link vertex and fragment shader objects into a program
		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		// Check link status and output info if error
		GLint link_status;
		glGetProgramiv(program, GL_LINK_STATUS, &link_status);
		if (link_status == 0)
		{
			GLint log_length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* linked_program_info_log = new GLchar[log_length];
			glGetProgramInfoLog(program, log_length, NULL, linked_program_info_log);
			std::cout << "Linked program INFO: " << linked_program_info_log << std::endl;
			delete[] linked_program_info_log;
			linked_program_info_log = nullptr;
		}

		glCreateVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);	
	}

	// shutdown - Performs cleaning up in the memory
	void shutdown()
	{
		// Clean up the vertex array object and program object
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteProgram(program);
	}

private:
	GLuint program;
	GLuint vertex_array_object;
};

int main()
{
	// Run the application
	my_application* app = new my_application();
	app->run(app);
	delete app;
	app = nullptr;
	return 0;
}