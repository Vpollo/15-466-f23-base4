#include "TextMode.hpp"

#include "data_path.hpp"
#include "gl_errors.hpp"
#include "TextTextureProgram.hpp"

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

TextMode::TextMode() {
    //https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	FT_Init_FreeType(&ft_library);
	FT_New_Face(ft_library, data_path("NotoSansSC-Medium.ttf").c_str(), 0, &ft_face);
	FT_Set_Char_Size(ft_face, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0);

    hb_font = hb_ft_font_create(ft_face, NULL);
	hb_buffer = hb_buffer_create();
}

TextMode::~TextMode() {
}

bool TextMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size) {

}

void TextMode::update(float elapsed) {

}

void TextMode::draw(glm::uvec2 const &drawable_size) {

}