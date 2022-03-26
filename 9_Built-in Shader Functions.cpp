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
		"vec4[4] functionThatReturnsArray()\n"
		"{\n"
		"	vec4[4] foo = {vec4(0.0, 0.0, 0.0, 1.0), vec4(0.25, 0.0, 0.0, 1.0), vec4(0.5, 0.0, 0.0, 1.0), vec4(0.75, 0.0, 0.0, 1.0) };\n"
		"\n"
		"	return foo;\n"
		"}\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	float foo[5];\n"
		"	ivec2 bar[13];\n"
		"	dmat3 baz[29];\n"
		"\n"
		"	float[5] foo2;\n"
		"	ivec2[13] bar2;\n"
		"	dmat3[29] baz2;\n"
		"\n"
		"	float[6] var = float[6](1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);\n"
		"	float var2[6] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };\n"
		"\n"
		"	struct foo3\n"
		"	{\n"
		"		int a;\n"
		"		vec2 b;\n"
		"		mat4 c;\n"
		"	};\n"
		"\n"
		"	struct bar3\n"
		"	{\n"
		"		vec3 a;\n"
		"		foo3[7] b;\n"
		"	};\n"
		"\n"
		"	bar3[29] baz3;\n"
		"\n"
		"	float a[10];\n"
		"	float b[a.length()];\n"
		"	mat4 c;\n"
		"	float d = float(c.length());\n"
		"	int e = c[0].length();\n"
		"\n"
		"	for (int i = 0; i < a.length(); i++)\n"
		"	{\n"
		"		b[i] = a[i];\n"
		"	}\n"
		"\n"
		"	float a2[10];\n"
		"	float b2[10][2];\n"
		"	float c2[10][2][5];\n"
		"	b2[1].length();\n"
		"	c2[3].length();\n"
		"	c2[3][1].length();\n"
		"\n"
		"	vec2 v1 = vec2(-0.5f, 0.0f);\n"
		"	vec4 v2 = vec4(v1, 0.0f, 1.0f);\n"
		"	mat4 m;\n"
		"	m[0][0] = 1.0f;\n"
		"	m[0][1] = 2.0f;\n"
		"	m[1][0] = 3.0f;\n"
		"	m[1][1] = 4.0f;\n"
		"\n"
		"	mat4 m4;\n"
		"	m4[0][0] = 0.25;\n"
		"	m4[0][1] = 0.25;\n"
		"	m4[1][0] = 0.25;\n"
		"	m4[1][1] = 0.25;\n"
		"\n"
		"	mat4 m2 = m * m4;\n"
		"	vec4 v3 = {1.0f, 1.0f, 1.0f, 1.0f};\n"
		"	vec4 v4 = {1.0f, 1.0f, 1.0f, 0.0f};\n"
		"	bvec4 v5 = equal(v3, v4);\n"
		"	float f = distance(v3, v4);\n"
		"	vec4 v6 = normalize(v3);\n"
		"	mat4 m3 = matrixCompMult(m, m4);\n"
		"	float g = -1.0f;\n"
		"	float g2 = abs(g);\n"
		"	float g3 = sign(g);\n"
		"	float h = 0.1f;\n"
		"	float h2 = ceil(h);\n"
		"	float h3 = trunc(h);\n"
		"	float h4 = round(h);\n"
		"	float h5 = roundEven(h);"
		"	float i = 0.5f;\n"
		"	float i2 = roundEven(i);\n"
		"	float j = 100.5f;\n"
		"	float j2 = fract(j);\n"
		"	float k = 10.5f;\n"
		"	float k2 = mod(k, 3.0);\n"
		"	float l2;\n"
		"	float l = modf(0.5f, l2);\n"
		"	float n = 0.0f;\n"
		"	float n2 = 1.0f;\n"
		"	float n3 = min(n, n2);\n"
		"	float n4 = max(n, n2);\n"
		"	vec4 v7 = {-0.5, 0.0, 0.0, 1.0}; \n"
		"	vec4 v8 = abs(v7);\n"
		"	vec4 v9 = {0.2, 0.7, 0.0, 1.0f};\n"
		"	vec4 v10 = round(v9);\n"
		"	vec4 v11 = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
		"	vec4 v12 = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"	float o = 1.0;\n"
		"	mix(v11, v12, 0);\n"
		"	step(v11, v12);\n"
		"	vec4 v13 = vec4(0.5, 0.5, 0.5, 1.0);\n"
		"	smoothstep(v11, v12, v13);\n"
		"	vec4 v14 = fma(v11, v12, v13);\n"
		"	uint p4 = uint(pow(2, 32)) - 1;\n"
		"	uint p = 0;\n"
		"	uint p2 = 1;\n"
		"	uint p3;\n"
		"	uaddCarry(p4, p2, p3);\n"
		"	uint p5;\n"
		"	usubBorrow(p, p2, p5);\n"
		"	gl_Position = vec4(p5, 0.0f, 0.0f, 1.0f);\n"
		"}\n"
	};
	 
	static const GLchar* fragment_shader_source[]
	{
		"#version 450 core\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec3 foo = vec3(0.5f);\n"
		"	vec3 bar = vec3(foo);\n"
		"	vec4 baz = vec4(1.0, 2.0, 3.0, 4.0);\n"
		"	vec4 bat = vec4(foo, 1.0f);\n"
		"	color = bat;\n"
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