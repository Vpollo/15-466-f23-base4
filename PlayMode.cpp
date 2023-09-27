#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "TextTextureProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <iostream>

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

PlayMode::PlayMode() {
	//https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	FT_Init_FreeType(&ft_library);
	FT_New_Face(ft_library, data_path("NotoSansSC-Medium.ttf").c_str(), 0, &ft_face);
	FT_Set_Char_Size(ft_face, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0);

    hb_font = hb_ft_font_create(ft_face, NULL);
	hb_buffer = hb_buffer_create();
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	return false;
}

void PlayMode::update(float elapsed) {

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	render_text(test_string1, glm::vec2(100.0f, 100.0f), drawable_size, glm::vec3(1.0f, 1.0f, 1.0f));
	render_text("妈妈再见", glm::vec2(800.0f, 500.0f), drawable_size, glm::vec3(0.0f, 1.0f, 1.0f));
}

void PlayMode::render_text(std::string text, glm::vec2 pos, glm::uvec2 const& drawable_size, glm::vec3 color) {

	hb_buffer_clear_contents(hb_buffer);
	//code from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	//all hb related code is the same source
	hb_buffer_add_utf8(hb_buffer, text.c_str(), -1, 0, -1);
	hb_buffer_guess_segment_properties(hb_buffer);

	hb_shape(hb_font, hb_buffer, NULL, 0);

	//Get glyph information and positions out of the buffer.
	uint32_t len = hb_buffer_get_length(hb_buffer);
	hb_glyph_info_t* glyph_infos = hb_buffer_get_glyph_infos(hb_buffer, NULL);
	hb_glyph_position_t* glyph_positions = hb_buffer_get_glyph_positions(hb_buffer, NULL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	//copied from https://learnopengl.com/In-Practice/Text-Rendering
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(text_texture_program->program);
	glUniform3f(text_texture_program->TextColor_3f, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	glm::mat4 projection = glm::ortho(0.0f, (float)drawable_size.x, 0.0f, (float)drawable_size.y);
	glUniformMatrix4fv(text_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, &projection[0][0]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (uint32_t i = 0; i < len; i++) {
		//https://freetype.org/freetype2/docs/tutorial/step1.html
		hb_codepoint_t glyph_index = glyph_infos[i].codepoint;
		FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);

		//code from https://learnopengl.com/In-Practice/Text-Rendering
		//same with VBO below
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                ft_face->glyph->bitmap.width,
                ft_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                ft_face->glyph->bitmap.buffer
            );
		//set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float w = static_cast<float>(ft_face->glyph->bitmap.width);
        float h = static_cast<float>(ft_face->glyph->bitmap.rows);

		float xpos = pos.x + ft_face->glyph->bitmap_left;
        float ypos = pos.y + ft_face->glyph->bitmap_top - h;

        //update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
		
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos += glm::vec2(glyph_positions[i].x_advance >> 6, glyph_positions[i].y_advance >> 6); // bitshift by 6 to get value in pixels in both x and y (2^6 = 64)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}