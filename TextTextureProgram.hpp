#include "GL.hpp"
#include "Load.hpp"

//adapted from base code (LitColorTexture)
//Shader program that draws transformed, lit, textured vertices tinted with vertex colors:
struct TextTextureProgram {
	TextTextureProgram();
	~TextTextureProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;
    GLuint TextColor_3f = -1U;

	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;

	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord
};

extern Load< TextTextureProgram > text_texture_program;