#include "TextTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load< TextTextureProgram > text_texture_program(LoadTagEarly);

TextTextureProgram::TextTextureProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"in vec4 Position;\n"
		"in vec2 TexCoord;\n"
		"out vec2 texCoord;\n"
		"void main() {\n"
		"	gl_Position = OBJECT_TO_CLIP * vec4(Position.xy, 0.0, 1.0);\n"
		"	texCoord = Position.zw;\n"
		"}\n"
	,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
        "uniform vec3 TextColor;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
        "   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(TEX, texCoord).r);"
		"	fragColor = vec4(TextColor, 1.0) * sampled;\n"
		"}\n"
	);

	//look up the locations of vertex attributes:
	Position_vec4 = glGetAttribLocation(program, "Position");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");

	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");
    TextColor_3f = glGetUniformLocation(program, "TextColor");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

TextTextureProgram::~TextTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}

