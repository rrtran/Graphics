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
		"layout (binding = 0, std140) uniform TransformBlock\n"
		"{\n"
		"	float scale;\n"
		"	vec3 translation;\n"
		"	float rotation[3];\n"
		"	mat4 projection_matrix;\n"
		"} transform;\n"
		"\n"
		"layout (std140) uniform ManuallyLaidOutBlock\n"
		"{\n"
		"	layout (offset = 8) vec2	bar;\n"
		"	layout (offset = 32) vec4	foo;\n"
		"	layout (offset = 48) vec3	baz;\n"
		"} myBlock;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = vec4(0.5, transform.scale, 0.0, 1.0); \n"
		"}\n"
	};

	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"\n"
		"layout (location = 0) uniform vec4 vColor;\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	color = vColor;\n"
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

		GLfloat vColor[3][4] = { { 1.0f, 0.0f, 0.0f, 1.0f },
								 { 0.0f, 1.0f, 0.0f, 1.0f },
								 { 0.0f, 0.0f, 1.0f, 1.0f } };

		glUniform4fv(0, 1, vColor[(int)currentTime % 3]);

		static const GLchar* uniformNames[4] =
		{
			"TransformBlock.scale",
			"TransformBlock.translation",
			"TransformBlock.rotation",
			"TransformBlock.projection_matrix"
		};

		GLuint uniformIndices[4];

		glGetUniformIndices(rendering_program, 4, uniformNames, uniformIndices);
		GLint uniformOffsets[4];
		GLint arrayStrides[4];
		GLint matrixStrides[4];
		glGetActiveUniformsiv(rendering_program, 4, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
		glGetActiveUniformsiv(rendering_program, 4, uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
		glGetActiveUniformsiv(rendering_program, 4, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);
		
		glDrawArrays(GL_POINTS, 0, 1);

		std::cout << "TransFormBlock.scale index: " << uniformOffsets[0] << std::endl;
		std::cout << "TransformBlock.translation index: " << uniformOffsets[1] << std::endl;
		std::cout << "TransformBlock.rotation index: " << uniformOffsets[2] << std::endl;
		std::cout << "TransformBlock.proj_matrix index: " << uniformOffsets[3] << std::endl;
		unsigned char* buffer = (unsigned char*)malloc(139);
		*((float*)(buffer + uniformOffsets[0])) = 0.5f;

		((float*)(buffer + uniformOffsets[1]))[0] = 1.0f;
		((float*)(buffer + uniformOffsets[1]))[1] = 2.0f;
		((float*)(buffer + uniformOffsets[1]))[2] = 3.0f;


		const GLfloat rotations[] = { 30.0f, 40.0f, 60.0f };
		unsigned int offset = uniformOffsets[2];
		for (int n = 0; n < 3; n++) {
			*((float*)(buffer + offset)) = rotations[n];
			offset += arrayStrides[2];
		}

		const GLfloat matrix[] =
		{
			1.0f, 2.0f, 3.0f, 4.0f,
			9.0f, 8.0f, 7.0f, 6.0f,
			2.0f, 4.0f, 6.0f, 8.0f,
			1.0f, 3.0f, 5.0f, 7.0f
		};

		for (int i = 0; i < 4; i++) {
			GLuint offset = uniformOffsets[3] + matrixStrides[3] * i;
			for (int j = 0; j < 4; j++) {
				*((float*)(buffer + offset)) = matrix[i * 4 + j];
				offset += sizeof(GLfloat);
			}
		}

		// +-------------------------------------------------------------------------------------+
		// |0.5f     | | | | |  |  |  |  |  |  | |1.0f       |2.0f       |3.0f       |  |  |  |  |
		// +-------------------------------------------------------------------------------------+
		//  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
		//
		// +---------------------------------------------------------------------------------------------+
		// |30.0f										  |40.0f										 |
		// +---------------------------------------------------------------------------------------------+
		// 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
		//
		// +---------------------------------------------------------------------------------------------+
		// |60.0f										  |1.0f       |2.0f       |3.0f       |4.0f		 |
		// +---------------------------------------------------------------------------------------------+
		// 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
		//
		// +--------------------------------------------------------------------------------------------------------------------------+
		// |9.0f	  |8.0f			  |7.0f			  |6.0f           |2.0f           |4.0f			  | 6.0f		  |8.0f			  |
		// +--------------------------------------------------------------------------------------------------------------------------+
		// 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127
		//
		// +--------------------------------------------------------------+
		// |1.0f     	  |3.0f			  |5.0f			  |7.0f           |
		// +--------------------------------------------------------------+
		// 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
		GLuint buffer2;
		glGenBuffers(1, &buffer2);
		glBindBuffer(GL_ARRAY_BUFFER, buffer2);
		glNamedBufferStorage(buffer2, sizeof(buffer), buffer, GL_MAP_WRITE_BIT);


		GLuint transformBlock_index = glGetUniformBlockIndex(rendering_program, "TransformBlock");
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffer2);


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