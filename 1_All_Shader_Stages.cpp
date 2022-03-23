#include "include/sb7.h"
#include "include/vmath.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>

using namespace vmath;
using namespace std;

GLuint compile_shaders()
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint tess_control_shader;
	GLuint tess_evaluation_shader;
	GLuint geometry_shader;
	GLuint compute_shader;
	GLuint program;

	static const GLchar* vertex_shader_source[] =
	{
		"#version 450 core													 \n"
		"																	 \n"
		"layout (location = 0) in vec4 position; 							 \n"
		"layout (location = 1) in vec4 color;								 \n"
		"layout (location = 10) uniform float fTime;						 \n"
		"layout (location = 11) uniform int iIndex;							 \n"
		"layout (location = 12) uniform vec4 vColorValue;					 \n"
		"layout (location = 13) uniform bool bSomeFlag;						 \n"
		"																	 \n"
		"uniform vec4 vColor;												 \n"
		"uniform vec4 vColor2;												 \n"
		"layout(std140) uniform TransformBlock {							 \n"
		"	float scale;													 \n"
		"	vec3 translation;												 \n"
		"	float rotation[3];												 \n"
		"	mat4 projection_matrix;											 \n"
		"} transform;														 \n"
		"																	 \n"
		"																	 \n"
		"																	 \n"
		"out VS_OUT															 \n"
		"{																	 \n"
		"	vec4 color;														 \n"
		"} vs_out;															 \n"
		"																	 \n"
		"void main()														 \n"
		"{																	 \n"
		"	mat4 m1 = {vec4(1.0f, 0.0f, 0.0f, 0.0f),						 \n"
		"					 vec4(0.0f, 1.0f, 0.0f, 0.0f),					 \n"
		"					 vec4(0.0f, 0.0f, 1.0f, 0.0f),					 \n"
		"					 vec4(0.0f, 0.0f, 0.0f, 1.0f)};		   			 \n"
		"	mat4 m2 = {vec4(1.0f, 0.0f, 0.0f, 0.0f),						 \n"
		"			   vec4(0.0f, 1.0f, 0.0f, 0.5f),						 \n"
		"			   vec4(0.0f, 0.0f, 1.0f, 0.0f),						 \n"
		"			   vec4(0.0f, 0.0f, 0.0f, 1.0f)};						 \n"
		"	mat4 m3 = { vec4(1.0f, 0.0f, 0.0f, 0.0f),						 \n"
		"				vec4(0.0f, cos(radians(45)), sin(radians(45)), 0.0f),\n"
		"				vec4(0.0f, -sin(radians(45)), cos(radians(45)), 0.0f),\n"
		"				vec4(0.0f, 0.0f, 0.0f, 1.0f) };						 \n"
		"	float x = 90;													\n"
		"	mat4 m4 = {	vec4(cos(radians(x)), -sin(radians(x)), 0.0f, 0.0f),\n"
		"				vec4(sin(radians(x)), cos(radians(x)), 0.0f, 0.0f),\n"
		"				vec4(0.0f, 0.0f, 1.0f, 0.0f),						 \n"
		"				vec4(0.0f, 0.0f, 0.0f, 1.0f)};						 \n"
		"	mat4 m5 = { vec4(0.5f, 0.0f, 0.0f, 0.0f),						 \n"
		"				vec4(0.0f, 0.5f, 0.0f, 0.0f),						 \n"
		"				vec4(0.0f, 0.0f, 0.5f, 0.0f),						 \n"
		"				vec4(0.0f, 0.0f, 0.0f, 1.0f)};						 \n"
		"	const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),    \n"
		"								   	 vec4(-0.25, -0.25, 0.5, 1.0),	 \n"
		"									 vec4( 0.25, 0.25, 0.5, 1.0));	 \n"
		"	for (int i = 0; i < vertices.length(); i++) {					 \n"
		"		vertices[i] = vertices[i] * m5 * m4 * m2;					 \n"
		"	}																 \n"
		"	const vec4 colors[] = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),			 \n"
		"								  vec4(0.0, 1.0, 0.0, 1.0),			 \n"
		"								  vec4(0.0, 0.0, 1.0, 1.0));		 \n"
		"																	 \n"
		"	//Index into our array using gl_VertexID						 \n"
		"	gl_Position = position * m5 * m2;								 \n"
		"	float n = fTime;												 \n"
		"	vColor;															 \n"
		"	vColor2;														 \n"
		"																	 \n"
		"	vs_out.color = colors[gl_VertexID];								 \n"
		"}																	 \n"
	};

	static const GLchar* tess_control_shader_source[] =
	{
		"#version 450 core									\n"
		"													\n"
		"in VS_OUT											\n"
		"{													\n"
		"	vec4 color; // Send color to the next stage		\n"
		"} tcs_in[];										\n"
		"													\n"
		"out TCS_OUT										\n"
		"{													\n"
		"	vec4 color;										\n"
		"} tcs_out[];										\n"
		"layout (vertices = 3) out;							\n"
		"													\n"
		"void main()										\n"
		"{													\n"
		"	// Only if I am invocation 0...					\n"
		"	if (gl_InvocationID == 0)						\n"
		"	{												\n"
		"		gl_TessLevelInner[0] = 5.0;					\n"
		"		gl_TessLevelOuter[0] = 5.0;					\n"
		"		gl_TessLevelOuter[1] = 5.0;					\n"
		"		gl_TessLevelOuter[2] = 5.0;					\n"
		"	}												\n"
		"	tcs_out[gl_InvocationID].color =				\n"
		"			tcs_in[gl_InvocationID].color;			\n"
		"													\n"
		"	// Everybody copies their input to their output	\n"
		"	gl_out[gl_InvocationID].gl_Position				\n"
		"		= gl_in[gl_InvocationID].gl_Position;		\n"
		"}													\n"
	};

	static const GLchar* tess_evaluation_shader_source[] =
	{
		"#version 450 core											\n"
		"															\n"
		"in TCS_OUT													\n"
		"{															\n"
		"	vec4 color;												\n"
		"} tes_in[];												\n"
		"															\n"
		"out TES_OUT												\n"
		"{															\n"
		"	vec4 color;												\n"
		"} tes_out;													\n"
		"															\n"
		"layout (triangles, equal_spacing, cw) in;					\n"
		"															\n"
		"void main()												\n"
		"{															\n"
		"	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +	\n"
		"				   gl_TessCoord.y * gl_in[1].gl_Position +  \n"
		"				   gl_TessCoord.z * gl_in[2].gl_Position);	\n"
		"		tes_out.color = gl_TessCoord.x * tes_in[0].color +  \n"
		"						gl_TessCoord.y * tes_in[1].color +  \n"
		"						gl_TessCoord.z * tes_in[2].color;   \n"
		"}															\n"
	};

	static const GLchar* geometry_shader_source[] =
	{
		"#version 450 core									\n"
		"													\n"
		"in TES_OUT											\n"
		"{													\n"
		"	vec4 color; // Send color to the next stage		\n"
		"} gs_in[];											\n"
		"													\n"
		"out GEO_OUT										\n"
		"{													\n"
		"	vec4 color;										\n"
		"} gs_out;											\n"
		"													\n"
		"layout (triangles) in;								\n"
		"layout (triangle_strip, max_vertices = 3) out;		\n"
		"													\n"
		"void main()										\n"
		"{													\n"
		"	for (int i = 0; i < gl_in.length(); i++)		\n"
		"	{												\n"
		"		gl_Position = gl_in[i].gl_Position;			\n"
		"		gs_out.color = gs_in[i].color;\n"
		"       EmitVertex();								\n"
		"	}												\n"
		"}													\n"
	};

	// Source code for fragment shader
	static const GLchar* fragment_shader_source[] =
	{
		"#version 450 core														  \n"
		"																		  \n"
		"in GEO_OUT																  \n"
		"{																		  \n"
		"	vec4 color; // Send color to the next stage							  \n"
		"} fs_in;																  \n"
		"																		  \n"
		"out vec4 color;														  \n"
		"																		  \n"
		"void main()															  \n"
		"{																		  \n"
		"//	color =	vec4(sin(gl_FragCoord.x * 0.25) * 0.5 + 0.5,				  \n"
		"//				 cos(gl_FragCoord.y * 0.25) * 0.5 + 0.5,				  \n"
		"//				 sin(gl_FragCoord.x * 0.15) * cos(gl_FragCoord.y * 0.15), \n"
		"//				 1.0);													  \n"
		"	color = fs_in.color;												  \n"
		"}																		  \n"
	};

	static const GLchar* compute_shader_source[] =
	{
		"#version 450 core\n"
		"\n"
		"layout (local_size_x = 32, local_size_y = 32) in; \n"
		"\n"
		"void main() \n"
		"{\n"
		"	// Do nothing\n"
		"}\n"
	};

	// Create and compile vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	GLint isCompiled = 0;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE)	cout << "vertex shader compiled" << endl;
	else
	{
		GLint maxLength = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

		for (int i = 0; i < errorLog.size(); i++) {
			cout << errorLog[i];
		}
		cout << endl;

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(vertex_shader); // Don't leak the shader.
	}

	// Create and compile tessellation control shader
	tess_control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tess_control_shader, 1, tess_control_shader_source, NULL);
	glCompileShader(tess_control_shader);

	isCompiled = 0;
	glGetShaderiv(tess_control_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE) cout << "Tessellation control shader compiled" << endl;

	// Create and compile tessellation evaluation shader
	tess_evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tess_evaluation_shader, 1, tess_evaluation_shader_source, NULL);
	glCompileShader(tess_evaluation_shader);

	isCompiled = 0;
	glGetShaderiv(tess_evaluation_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE) cout << "Tessellation evaluation shader compiled" << endl;

	// Create and compile geometry shader
	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader, 1, geometry_shader_source, NULL);
	glCompileShader(geometry_shader);

	isCompiled = 0;
	glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE) cout << "Geomtry shader compiled" << endl;
	else
	{
		GLint maxLength = 0;
		glGetShaderiv(geometry_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(geometry_shader, maxLength, &maxLength, &errorLog[0]);

		for (int i = 0; i < errorLog.size(); i++) {
			cout << errorLog[i];
		}
		cout << endl;

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(geometry_shader); // Don't leak the shader.
	}

	// Create and compile fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	isCompiled = 0;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE) cout << "fragment shader compiled" << endl;

	compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader, 1, compute_shader_source, NULL);
	glCompileShader(compute_shader);

	isCompiled = 0;
	glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE) cout << "compute shader is compiled" << endl;

	// Create program, attach shaders to it, and link it
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, tess_control_shader);
	glAttachShader(program, tess_evaluation_shader);
	glAttachShader(program, geometry_shader);
	glAttachShader(program, fragment_shader);
	//glAttachShader(program, compute_shader);
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_TRUE)
	{
		cout << "Program is linked" << endl;
	}
	else
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		cout << "Link error: ";
		for (int i = 0; i < infoLog.size(); i++) {
			cout << infoLog[i];
		}

		// The program is useless now. So delete it.
		glDeleteProgram(program);

		// Provide the infolog in whatever manner you deem best.
		// Exit with failure.
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(tess_control_shader);
	glDeleteShader(tess_evaluation_shader);
	glDeleteShader(fragment_shader);

	return program;
}

void runVariousVectorAndMatrixComputations()
{
	vec3 vVertex1(0.0f, 0.0f, 1.0f);
	vec4 vVertex2 = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	vec4 vVertex3(vVertex1, 1.0f);

	vec3 vVerts[] = { vec3(-0.5f, 0.0f, 0.0f), vec3(0.5f, 0.0f, 0.0f), vec3(0.0f, 0.5f, 0.0f) };

	vec3 a(1.0f, 2.0f, 3.0f);
	vec3 b(4.0f, 5.0f, 6.0f);
	vec3 c;

	cout << "vector addition" << endl;
	cout << "---------------" << endl;
	c = a + b;
	printf("(1.0f, 2.0f, 3.0f) + (4.0f, 5.0f, 6.0f) = (%.1f, %.1f, %.1f)\n", c[0], c[1], c[2]);
	cout << endl;

	cout << "vector subtraction" << endl;
	cout << "------------------" << endl;
	c = a - b;
	printf("(1.0f, 2.0f, 3.0f) - (4.0f, 5.0f, 6.0f) = (%.1f, %.1f, %.1f)\n", c[0], c[1], c[2]);
	cout << endl;

	cout << "shorthand addition" << endl;
	cout << "------------------" << endl;
	c += b;
	printf("(-3.0f, -3.0f, -3.0f) + (4.0f, 5.0f, 6.0f) = (%.1f, %.1f, %.1f)\n", c[0], c[1], c[2]);
	cout << endl;

	cout << "unary negation" << endl;
	cout << "--------------" << endl;
	c = -c;
	printf("-1 * (1.0f, 2.0f, 3.0f) = (%.1f, %.1f, %.1f)\n", c[0], c[1], c[2]);
	cout << endl;

	float j = dot(a, b);
	float d = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	cout << "dot product" << endl;
	cout << "-----------" << endl;
	printf("(1.0f, 2.0f, 3.0f) * (4.0f, 5.0f, 6.0f) = %.1f\n", d);
	printf("dot((1.0f, 2.0f, 3.0f), (4.0f, 5.0f, 6.0f)) = %.1f\n", j);
	cout << endl;

	cout << "angle between 2 unit vectors" << endl;
	cout << "----------------------------" << endl;
	vec3 e(1.0f, 0.0f, 0.0f);
	vec3 f(0.0f, 1.0f, 0.0f);
	float g = angle(e, f);
	cout << "Angle between vector a and b (in radians): " << g << endl;
	cout << endl;

	cout << "cross product" << endl;
	cout << "-------------" << endl;
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
	printf("(1.0f, 2.0f, 3.0f) X (4.0f, 5.0f, 6.0f) = (%.1f, %.1f, %.1f)\n", c[0], c[1], c[2]);
	c = cross(a, b);
	printf("cross(a, b) = (%.1f, %.1f, %.1f)\n", c[0], c[1], c[2]);
	cout << endl;

	cout << "magnitude" << endl;
	cout << "---------" << endl;
	float h = sqrt(pow(c[0], 2.0f) + pow(c[1], 2.0f) + pow(c[2], 2.0f));
	float i = length(c);
	printf("sqrt((-3.0f)^2 + (6.0f)^2 + (-3.0f)^ 2) = %f\n", h);
	printf("length((-3.0f, 6.0f, -3.0f)) = %f\n", i);
	cout << endl;

	cout << "Reflection" << endl;
	cout << "----------" << endl;
	vec3 k(1.0f, 0.0f, -1.0f); // Incoming unit-length vector
	vec3 l(0.0f, 0.0f, 1.0f); // Surface normal unit-length vector
	vec3 m = k - 2.0f * dot(l, k) * l; // Reflected unit-length vector
	vec3 n = reflect(k, l);
	printf("(1.0f, 0.0f, -1.0f) - (dot(2 * (0.0f, 0.0f, 1.0f), (1.0f, 0.0f, -1.0f)) * (0.0f, 0.0f, 1.0f) = (%.1f, %.1f, %.1f)\n", m[0], m[1], m[2]);
	printf("reflect((1.0f, 0.0f, -1.0f), (0.0f, 0.0f, 1.0f)) = (%.1f, %.1f, %.1f)\n", n[0], n[1], n[2]);
	cout << endl;

	cout << "Refraction" << endl;
	cout << "----------" << endl;
	float p = 1.0f; // Refractive index
	float o = 1 - pow(p, 2.0f) * (1 - pow(dot(l, k), 2.0f));
	vec3 q;
	if (o < 0.0f) {
		q = vec3(0.0f, 0.0f, 0.0f);
		printf("1 - (0.5f)^2 * (1 - dot((0.0f, 0.0f, 1.0f), (1.0f, 0.0f, 1.0f))^2 < 0.0: (%f, %f, %f)\n", q[0], q[1], q[2]);
	}
	else {
		q = p * k - (p * dot(l, k) + sqrt(o)) * l;
		printf("1 - (0.5f)^2 * (1 - dot((0.0f, 0.0f, 1.0f), (1.0f, 0.0f, 1.0f))^2 >= 0.0: (%f, %f, %f)\n", q[0], q[1], q[2]);
	}

	vec3 r = refract(k, l, p);
	printf("refract((1.0f, 0.0f, -1.0f), (0.0f, 0.0f, 1.0f) = (%f, %f, %f)\n", r[0], r[1], r[2]);
	cout << endl;

	mat2 m1;
	mat3 m2;
	mat4 m3;

	GLfloat matrix1[16];
	GLfloat matrix2[4][4];

	float A00 = 1.0f;
	float A01 = 2.0f;
	float A02 = 3.0f;
	float A03 = 4.0f;
	float A10 = 5.0f;
	float A11 = 6.0f;
	float A12 = 7.0f;
	float A13 = 8.0f;
	float A20 = 9.0f;
	float A21 = 10.0f;
	float A22 = 11.0f;
	float A23 = 12.0f;
	float A30 = 13.0f;
	float A31 = 14.0f;
	float A32 = 15.0f;
	float A33 = 16.0f;

	static const float A[] =
	{
		A00, A01, A02, A03, A10, A11, A12, A13,
		A20, A21, A22, A23, A30, A31, A32, A33
	};

	static const float B[] =
	{
		A00, A10, A20, A30, A01, A11, A21, A31,
		A20, A21, A22, A23, A30, A31, A32, A33
	};

	mat4 m4 = { vec4(1.0f, 2.0f, 3.0f, 4.0f),
			   vec4(5.0f, 6.0f, 7.0f, 8.0f),
			   vec4(9.0f, 10.0f, 11.0f, 12.0f),
			   vec4(13.0f, 14.0f, 15.0f, 16.0f) };
	mat4 m5 = { vec4(1.0f, 2.0f, 3.0f, 4.0f),
			   vec4(5.0f, 6.0f, 7.0f, 8.0f),
			   vec4(9.0f, 10.0f, 11.0f, 12.0f),
			   vec4(13.0f, 14.0f, 15.0f, 16.0f) };

	vec4 v1(1.0f, 1.0f, 1.0f, 1.0f);

	vec4 v2 = (v1 * m4) * m5;
	vec4 v3 = v1 * (m4 * m5);

	cout << "Multiplying vector by matrices" << endl;
	cout << "------------------------------" << endl;
	cout << "m4" << endl;
	cout << "--" << endl;
	for (int i = 0; i < m4.width(); i++) {
		for (int j = 0; j < m4.height(); j++) {
			printf("%.1f ", m4[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "m5" << endl;
	cout << "--" << endl;
	for (int i = 0; i < m5.width(); i++) {
		for (int j = 0; j < m5.height(); j++) {
			printf("%.1f ", m5[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "v1" << endl;
	cout << "--" << endl;
	for (int i = 0; i < v1.size(); i++) {
		printf("%.1f ", v1[i]);
	}
	cout << endl;

	cout << "(v1 * m4) * m5" << endl;
	cout << "--------------" << endl;
	for (int i = 0; i < v2.size(); i++) {
		printf("%.1f ", v2[i]);
	}
	cout << endl;

	cout << "v1 * (m4 * m5)" << endl;
	cout << "--------------" << endl;
	for (int i = 0; i < v3.size(); i++) {
		printf("%.1f ", v3[i]);
	}
	cout << endl;

	cout << "Matix is not commutative" << endl;
	cout << "------------------------" << endl;
	m5 = m5.transpose();
	mat4 m6 = m4 * m5;
	mat4 m7 = m5 * m4;

	cout << "m4" << endl;
	cout << "--" << endl;
	for (int i = 0; i < m4.width(); i++) {
		for (int j = 0; j < m4.height(); j++) {
			printf("%.1f ", m4[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "m5" << endl;
	cout << "--" << endl;

	for (int i = 0; i < m5.width(); i++) {
		for (int j = 0; j < m5.height(); j++) {
			printf("%.1f ", m5[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "m4 * m5" << endl;
	cout << "-------" << endl;
	for (int i = 0; i < m6.width(); i++) {
		for (int j = 0; j < m6.height(); j++) {
			printf("%.1f ", m6[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "m5 * m4" << endl;
	cout << "-------" << endl;
	for (int i = 0; i < m7.width(); i++) {
		for (int j = 0; j < m7.height(); j++) {
			printf("%.1f ", m7[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "Identity matrix" << endl;
	cout << "---------------" << endl;
	mat4 m8 = { vec4(1.0f, 0.0f, 0.0f, 0.0f),
			   vec4(0.0f, 1.0f, 0.0f, 0.0f),
			   vec4(0.0f, 0.0f, 1.0f, 0.0f),
			   vec4(0.0f, 0.0f, 0.0f, 1.0f) };
	mat4 m9 = mat4::identity();

	cout << "m8" << endl;
	cout << "--" << endl;
	for (int i = 0; i < m8.width(); i++)
	{
		for (int j = 0; j < m8.height(); j++) {
			printf("%.1f ", m8[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "m9" << endl;
	cout << "--" << endl;
	for (int i = 0; i < m9.width(); i++)
	{
		for (int j = 0; j < m9.width(); j++) {
			printf("%.1f ", m9[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	vec4 v4(0.1f, 0.2f, 0.3f, 1.0f);
	cout << "v4" << endl;
	cout << "--" << endl;
	for (int i = 0; i < v4.size(); i++) {
		printf("%.1f ", v4[i]);
	}
	cout << endl;

	vec4 v4_transformed = v4 * m8;
	cout << "v4 * m8" << endl;
	cout << "-------" << endl;
	for (int i = 0; i < v4_transformed.size(); i++) {
		printf("%.1f ", v4_transformed[i]);
	}
	cout << endl;

	cout << "Translation Matrix" << endl;
	cout << "------------------" << endl;
	mat4 m10 = { vec4(1.0f, 0.0f, 0.0f, 0.0f),
				 vec4(0.0f, 1.0f, 0.0f, 0.5f),
				 vec4(0.0f, 0.0f, 1.0f, 0.0f),
				 vec4(0.0f, 0.0f, 0.0f, 1.0f) };
	mat4 m11 = translate(vec3(0.0f, 0.5f, 0.0f));

	vec4 v5[3] = { vec4(0.25, -0.25, 0.5, 1.0),
				 vec4(-0.25, -0.25, 0.5, 1.0),
				 vec4(0.25, 0.25, 0.5, 1.0) };
	vec4 v6[3];
	for (int i = 0; i < 3; i++) {
		v6[i] = v5[i] * m10;
	}

	cout << "m10" << endl;
	cout << "---" << endl;
	for (int i = 0; i < m10.width(); i++) {
		for (int j = 0; j < m10.height(); j++) {
			printf("%.1f ", m10[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "m11" << endl;
	cout << "---" << endl;
	for (int i = 0; i < m11.width(); i++) {
		for (int j = 0; j < m11.height(); j++) {
			printf("%.1f ", m11[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "Vertices of a right triangle" << endl;
	cout << "----------------------------" << endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < v5[i].size(); j++) {
			printf("%.2f ", v5[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "Translated vertices" << endl;
	cout << "-------------------" << endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < v6[i].size(); j++) {
			printf("%.2f ", v6[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "Rotation matrix" << endl;
	cout << "---------------" << endl;
	mat4 m12 = { vec4(1.0f, 0.0f, 0.0f, 0.0f),
				vec4(0.0f, cos(radians(90.0f)), sin(radians(90.0f)), 0.0),
				vec4(0.0f, -sin(radians(90.0f)), cos(radians(90.0f)), 0.0),
				vec4(0.0f, 0.0f, 0.0f, 1.0f) };
	cout << "m12" << endl;
	cout << "---" << endl;
	for (int i = 0; i < m12.width(); i++) {
		for (int j = 0; j < m12.height(); j++) {
			printf("%.1f ", m12[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	vec4 v7[3];
	for (int i = 0; i < 3; i++) {
		v7[i] = v5[i] * m12;
	}

	cout << "Rotated vertices" << endl;
	cout << "----------------" << endl;
	cout << "v7" << endl;
	cout << "--" << endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < v7[i].size(); j++) {
			printf("%.2f ", v7[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	cout << "Scaling Matrix" << endl;
	cout << "--------------" << endl;
	mat4 m13 = { vec4(0.5f, 0.0f, 0.0f, 0.0f),
				 vec4(0.0f, 0.5f, 0.0f, 0.0f),
				 vec4(0.0f, 0.0f, 0.5f, 0.0f),
				 vec4(0.0f, 0.0f, 0.0f, 1.0f) };
	cout << "m13" << endl;
	cout << "---" << endl;
	for (int i = 0; i < m13.width(); i++) {
		for (int j = 0; j < m13.height(); j++) {
			printf("% .1f ", m13[i][j]);
		}
		cout << endl;
	}
	cout << endl;

	vec4 v8(1.0f, 1.0f, 1.0f, 1.0f);
	cout << "v8" << endl;
	cout << "--" << endl;
	for (int i = 0; i < v8.size(); i++) {
		printf("%.1f ", v8[i]);
	}
	cout << endl;

	vec4 v9;
	v9 = v8 * m13;
	cout << "v9" << endl;
	cout << "--" << endl;
	for (int i = 0; i < v9.size(); i++) {
		printf("%.1f ", v9[i]);
	}
	cout << endl;
}

class my_application : public sb7::application
{
public:
	void render(double currentTime)
	{
		// { red, green, blue, alpha }
		static const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);

		// Use the program object we created earlier for rendering
		glUseProgram(rendering_program);

		GLfloat attrib[] = { (float)sin(currentTime) * 0.5f,
							 (float)cos(currentTime) * 0.6f,
							 0.0f, 0.0f };

		GLfloat attrib2[] = { (float)sin(currentTime) * 0.5f + 0.5f,
							  (float)cos(currentTime) * 0.5f + 0.5f,
							  0.0f, 0.0f };

		glVertexAttrib4fv(0, attrib);
		glVertexAttrib4fv(1, attrib2);
		glUniform1f(10, 45.2f);
		glUniform1i(11, 42);
		glUniform4f(12, 1.0f, 0.0f, 0.0f, 1.0f);
		glUniform1i(13, GL_FALSE);

		// cout << glGetUniformLocation(rendering_program, "fTime") << endl;
		//cout << glGetUniformLocation(rendering_program, "vColor") << endl;
		
		GLfloat vColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// glUniform4fv(iColorLocation, 1, vColor);

		struct vertex {
			float x;
			float y;
			float z;
			float w;

			float r;
			float g;
			float b;
			float w2;
		};
		
		GLuint vao;
		GLuint buffer;

		static const vertex vertices[] = { {0.25, -0.25, 0.5, 1.0, 1.0, 0.0, 0.0, 1.0},
										   {-0.25, -0.25, 0.5, 1.0, 0.0, 1.0, 0.0, 1.0},
										   {0.25, 0.25, 0.5, 1.0, 0.0, 0.0, 1.0, 1.0} };

		// Create the vertex array object
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		// Allocate and initialize a buffer object
		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, sizeof(vertices), vertices, 0);

		// Set up two vertex attributes - first positions
		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
		glEnableVertexArrayAttrib(vao, 0);

		// Now Colors
		glVertexArrayAttribBinding(vao, 1, 0);
		glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
		glEnableVertexAttribArray(1);

		// Finally, bind our one and only buffer to the vertex array object
		glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vertex));
		
		//cout << "Uniform TransformBlock Index: " << glGetUniformBlockIndex(rendering_program, "TransformBlock") << endl;

		static const GLchar* uniformNames[4] =
		{
			"TransformBlock.scale",
			"TransformBlock.translation",
			"TransformBlock.rotation",
			"TransformBlock.projection_matrix",
		};
		GLuint uniformIndices[4];

		glGetUniformIndices(rendering_program, 4, uniformNames, uniformIndices);

		GLint uniformOffsets[4];
		GLint arrayStrides[4];
		GLint matrixStrides[4];
		glGetActiveUniformsiv(rendering_program, 4, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
		glGetActiveUniformsiv(rendering_program, 4, uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
		glGetActiveUniformsiv(rendering_program, 4, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);
		
		// Allocate some memory for our buffer (don't forget to free it later)
		unsigned char* buffer2 = (unsigned char*)malloc(4096);
		*((float*)(buffer2 + uniformOffsets[0])) = 3.0f;

		((float*)(buffer2 + uniformOffsets[1]))[0] = 1.0f;
		((float*)(buffer2 + uniformOffsets[1]))[1] = 2.0f;
		((float*)(buffer2 + uniformOffsets[1]))[2] = 3.0f;

		const GLfloat rotations[] = { 30.0f, 40.0f, 60.0f };
		unsigned int offset = uniformOffsets[2];
		for (int n = 0; n < 3; n++) {
			*((float*)(buffer2 + offset)) = rotations[n];
			offset += arrayStrides[2];
		}

		const GLfloat matrix[] = {
			1.0f, 2.0f, 3.0f, 4.0f,
			9.0f, 8.0f, 7.0f, 6.0f,
			2.0f, 4.0f, 6.0f, 8.0f,
			1.0f, 3.0f, 5.0f, 7.0f
		};

		for (int i = 0; i < 4; i++) {
			GLuint offset = uniformOffsets[3] + matrixStrides[3] * i;
			for (int j = 0; j < 4; j++) {
				*((float*)(buffer2 + offset)) = matrix[i * 4 + j];
				offset += sizeof(GLfloat);
			}
		}

		GLint transformBlockIndex = glGetUniformBlockIndex(rendering_program, "TransformBlock");
		glUniformBlockBinding(rendering_program, transformBlockIndex, 2);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, *buffer2);
		// GLuint buffer[2];
		// GLuint vao;

		/*static const GLfloat positions[] = { 0.25, -0.25, 0.5, 1.0,
											 -0.25, -0.25, 0.5, 1.0,
											 0.25, 0.25, 0.5, 1.0 };
		static const GLfloat colors[] = { 1.0, 0.0, 0.0, 1.0,			
										  0.0, 1.0, 0.0, 1.0,
										  0.0, 0.0, 1.0, 1.0 };*/

		//// Create the vertex array object
		//glCreateVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//// Get create two buffers
		//glCreateBuffers(2, &buffer[0]);

		//// Initialize the first buffer
		//glNamedBufferStorage(buffer[0], sizeof(positions), positions, 0);

		//// Bind it to the vertex array - offset zero, stride = sizeof(vec3)
		//glVertexArrayVertexBuffer(vao, 0, buffer[0], 0, sizeof(vmath::vec4));

		//// Tell OpenGL what the format of the attribute is
		//glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);

		//// Tell OpenGL which vertex buffer binding to use for this attribute
		//glVertexArrayAttribBinding(vao, 0, 0);

		//// Enable the attribute
		//glEnableVertexArrayAttrib(vao, 0);

		// Perform similar initialization for the second buffer
		//glNamedBufferStorage(buffer[1], sizeof(colors), colors, 0);
		//glVertexArrayVertexBuffer(vao, 1, buffer[1], 0, sizeof(vmath::vec4));
		//glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
		//glVertexArrayAttribBinding(vao, 1, 1);
		//glEnableVertexAttribArray(1);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		// Draw one triangle
		glDrawArrays(GL_PATCHES, 0, 3);
		GLint numOfExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numOfExtensions);
	}

	void startup()
	{
		rendering_program = compile_shaders();
		glCreateVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		runVariousVectorAndMatrixComputations();
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
};

int main()
{
	my_application* app = new my_application();                                 \
		app->run(app);
	delete app;
	return 0;
}

//int main()
//{
//	my_application* app = new my_application();
//	app->run(app);
//	delete app;
//	return 0;
//}