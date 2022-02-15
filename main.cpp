#include "include/sb7.h"
#include "include/vmath.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>

using namespace std;
using namespace vmath;

GLuint compile_shaders()
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint tess_control_shader;
	GLuint tess_evaluation_shader;
	GLuint geometry_shader;
	GLuint compute_shader;
	GLuint program;

	static const GLchar * vertex_shader_source[] =
	{
		"#version 450 core													 \n"
		"																	 \n"
		"layout (location = 0) in vec4 offset; 								 \n"
		"layout (location = 1) in vec4 color;								 \n"
		"																	 \n"
		"out VS_OUT															 \n"
		"{																	 \n"
		"	vec4 color;														 \n"
		"} vs_out;															 \n"
		"																	 \n"
		"void main()														 \n"
		"{																	 \n"
		"	const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),    \n"
		"								   	 vec4(-0.25, -0.25, 0.5, 1.0),	 \n"
		"									 vec4( 0.25,  0.25, 0.5, 1.0));	 \n"
		"	const vec4 colors[] = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),			 \n"
		"								  vec4(0.0, 1.0, 0.0, 1.0),			 \n"
		"								  vec4(0.0, 0.0, 1.0, 1.0));		 \n"
		"																	 \n"
		"	//Index into our array using gl_VertexID						 \n"	
		"	gl_Position = vertices[gl_VertexID] + offset;					 \n"
		"																	 \n"
		"	vs_out.color = colors[gl_VertexID];								 \n"
		"}																	 \n"
	};

	static const GLchar * tess_control_shader_source[] =
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

	static const GLchar * tess_evaluation_shader_source[] =
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

class my_application : public sb7::application
{
public:
	void render(double currentTime)
	{
		// { red, green, blue, alpha }
		static const GLfloat color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
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