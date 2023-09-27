#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>
#include <hb.h>
#include <hb-ft.h>
#include <freetype/freetype.h>

//mode to play the Chinese character game
struct TextMode : Mode {
    TextMode();
    virtual ~TextMode();

    //functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

    //------- game state --------



    //------- text rendering --------
	FT_Library ft_library;
	FT_Face ft_face;
	hb_font_t* hb_font;
	hb_buffer_t* hb_buffer;
};
