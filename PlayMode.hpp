#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <utility>

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

	void render_puzzle(glm::uvec2 const& drawable_size);
	void render_timer_and_score(glm::uvec2 const& drawable_size);
	void render_finish_screen(glm::uvec2 const& drawable_size);

	void level_finish(bool win);

	//----- game state -----
	int at_level = 0;
	int last_level = -1;
	int correct_x, correct_y;
	int mouse_x, mouse_y;
	float time_left = -1.0f;
	bool mouse_clicked = false;
	bool all_level_finished = false;
	int score = 0;

	//----- configurations -----
	int PUZZLE_HEIGHT = 5;
	int PUZZLE_WIDTH = 6;
	const float PUZZLE_DIST_X = 100.0f;
	const float PUZZLE_DIST_Y = 100.0f;
	const float LEVEL_TIME = 10.0f;
	//colors
	const glm::vec3 COLOR_NORMAL = glm::vec3(1.0f, 1.0f, 1.0f);
	const glm::vec3 COLOR_HIGHLIGHTED = glm::vec3(0.1f, 0.84f, 0.0f);
	//LEVELS
	std::vector< std::pair< std::string, std::string > > levels = {
		std::pair< std::string, std::string > ("口", "一"),
		std::pair< std::string, std::string > ("乱", "乩"),
		std::pair< std::string, std::string > ("忐", "忑"),
		std::pair< std::string, std::string > ("五", "丑"),
		std::pair< std::string, std::string > ("乒", "乓"),
		std::pair< std::string, std::string > ("鸟", "乌"),
		std::pair< std::string, std::string > ("糅", "煣"),
	};

	//------- text rendering -------
	FT_Library ft_library;
	FT_Face ft_face;
	hb_font_t* hb_font;
	hb_buffer_t* hb_buffer;

	GLuint VAO, VBO, program;

	//----- helper functions -----
	void render_text(std::string text, glm::vec2 pos, glm::uvec2 const& drawable_size, glm::vec3 color);
	bool mouse_on_this_character(glm::vec2 char_pos, glm::uvec2 const& drawable_size);
	bool click_on_correct(glm::uvec2 const& drawable_size);

	//----- for testing only -----
	std::string test_string = "String is here!";
	std::string test_string1 = "妈妈你好";
};
