#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "TextTextureProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

PlayMode::PlayMode() {
	//https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	FT_Init_FreeType(&ft_library);
	FT_New_Face(ft_library, data_path("NotoSansSC-Medium.ttf").c_str(), 0, &ft_face);
	FT_Set_Char_Size(ft_face, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0);

    hb_font = hb_ft_font_create(ft_face, NULL);
	hb_buffer = hb_buffer_create();

	std::srand(static_cast< unsigned int >(std::time(nullptr)));
	time_left = LEVEL_TIME;
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_r) {
			restart();
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		//https://wiki.libsdl.org/SDL2/SDL_GetMouseState
		SDL_GetMouseState(&mouse_x, &mouse_y);
		return true;
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		mouse_clicked = true;
	}

	return false;
}

void PlayMode::update(float elapsed) {
	// std::cout << mouse_x << ", " << mouse_y << "\n";
	if (!all_level_finished) {
		time_left -= elapsed;
		if (time_left < 0.0f) {
			level_finish(false);
		}
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (mouse_clicked) {
		if (click_on_correct(drawable_size)) {
			level_finish(true);
		} else {
			level_finish(false);
		}
	}

	if (!all_level_finished) {
		render_puzzle(drawable_size);
		render_timer_and_score(drawable_size);
	} else {
		render_finish_screen(drawable_size);
	}

	mouse_clicked = false;
}

void PlayMode::level_finish(bool win) {
	if (win) {
		score++;
	}

	at_level++;
	if (at_level == levels.size()) {
		all_level_finished = true;
	} else {
		time_left = LEVEL_TIME;
	}
}

void PlayMode::render_puzzle(glm::uvec2 const& drawable_size) {
	//get random int in range: https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
	//init a new correct pos for puzzle if new level is loaded
	if (last_level != at_level) {
		correct_x = std::rand() % PUZZLE_WIDTH;
		correct_y = std::rand() % PUZZLE_HEIGHT;
	}

	float start_x = (drawable_size.x - PUZZLE_DIST_X * PUZZLE_WIDTH) / 2.0f + FONT_SIZE;
	float start_y = (drawable_size.y - PUZZLE_DIST_Y * PUZZLE_HEIGHT) / 2.0f + FONT_SIZE - drawable_size.y * 0.1f;

	//render the grid of characters to the bottom center of screen
	for (size_t x = 0; x < PUZZLE_WIDTH; x++) {
		for (size_t y = 0; y < PUZZLE_HEIGHT; y++) {
			glm::vec2 pos = glm::vec2(start_x + x * PUZZLE_DIST_X, start_y + y * PUZZLE_DIST_Y);
			glm::vec3 color = mouse_on_this_character(pos, drawable_size) ? COLOR_HIGHLIGHTED : COLOR_NORMAL;
			if (x == correct_x && y == correct_y) {
				render_text(levels[at_level].first, pos, drawable_size, color);
			} else {
				render_text(levels[at_level].second, pos, drawable_size, color);
			}
		}
	}

	last_level = at_level;
}

void PlayMode::render_timer_and_score(glm::uvec2 const& drawable_size) {
	//convert time left to string with 2 digit precision
	//https://stackoverflow.com/questions/29200635/convert-float-to-string-with-precision-number-of-decimal-digits-specified
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << time_left;
	std::string time_left_s = stream.str();
	glm::vec2 pos = glm::vec2(FONT_SIZE, drawable_size.y - FONT_SIZE*2);
	render_text("Time Left: " + time_left_s, pos, drawable_size, COLOR_NORMAL);

	pos = glm::vec2(drawable_size.x - FONT_SIZE*5, drawable_size.y - FONT_SIZE*2);
	render_text("Score: " + std::to_string(score), pos, drawable_size, COLOR_NORMAL);
}

void PlayMode::render_finish_screen(glm::uvec2 const& drawable_size){
	std::string txt = "Your Final Score: " + std::to_string(score);
	glm::vec2 pos = glm::vec2(FONT_SIZE, drawable_size.y - FONT_SIZE*2);
	render_text(txt, pos, drawable_size, COLOR_NORMAL);

	txt = "Press R to Retry";
	pos.y -= FONT_SIZE + 30.0f;
	render_text(txt, pos, drawable_size, COLOR_NORMAL);
}

void PlayMode::restart() {
	at_level = 0;
	last_level = -1;
	time_left = LEVEL_TIME;
	all_level_finished = false;
	score = 0;
}

bool PlayMode::mouse_on_this_character(glm::vec2 char_pos, glm::uvec2 const& drawable_size) {
	const float TOLERANCE = 15.0f;
	float x_min = char_pos.x - TOLERANCE;
	float x_max = char_pos.x + FONT_SIZE + TOLERANCE;
	float y_min = char_pos.y - TOLERANCE;
	float y_max = char_pos.y + FONT_SIZE + TOLERANCE;

	int mouse_y_veiwport = drawable_size.y - mouse_y;
	if (mouse_x >= x_min && mouse_x <= x_max 
	    && mouse_y_veiwport >= y_min && mouse_y_veiwport <= y_max) {
		return true;
	}

	return false;
}

bool PlayMode::click_on_correct(glm::uvec2 const& drawable_size) {
	float start_x = (drawable_size.x - PUZZLE_DIST_X * PUZZLE_WIDTH) / 2.0f + FONT_SIZE;
	float start_y = (drawable_size.y - PUZZLE_DIST_Y * PUZZLE_HEIGHT) / 2.0f + FONT_SIZE - drawable_size.y * 0.1f;

	for (size_t x = 0; x < PUZZLE_WIDTH; x++) {
		for (size_t y = 0; y < PUZZLE_HEIGHT; y++) {
			glm::vec2 pos = glm::vec2(start_x + x * PUZZLE_DIST_X, start_y + y * PUZZLE_DIST_Y);
			if (mouse_on_this_character(pos, drawable_size)) {
				if (x == correct_x && y == correct_y) return true;
				else return false;
			}
		}
	}

	return false;
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

		//https://stackoverflow.com/questions/42554090/c-opengl-memory-leak
		glDeleteTextures(1, &texture);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//prevent memory leaks https://stackoverflow.com/questions/42554090/c-opengl-memory-leak
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}