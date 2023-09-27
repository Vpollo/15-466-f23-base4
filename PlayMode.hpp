#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include <hb.h>
#include <hb-ft.h>
#include <freetype/freetype.h>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//void render_puzzle(glm::uvec2 const& drawable_size, std::string const& correct_char, std::string const& wrong_char);
	void render_text(std::string text, glm::vec2 pos, glm::uvec2 const& drawable_size, glm::vec3 color);

	//----- game state -----


	//------- text rendering --------
	FT_Library ft_library;
	FT_Face ft_face;
	hb_font_t* hb_font;
	hb_buffer_t* hb_buffer;

	GLuint VAO, VBO, program;

	std::string test_string = "String is here!";
	std::string test_string1 = "妈妈你好";
};
