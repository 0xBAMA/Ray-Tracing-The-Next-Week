#ifndef RTTNW
#define RTTNW

#include "includes.h"
#include "book_code.h"


class rttnw
{
public:

	rttnw();
	~rttnw();

private:

	SDL_Window * window;
	SDL_GLContext GLcontext;

	ImVec4 clear_color;

	GLuint display_shader;
	GLuint display_vao;
	GLuint display_vbo;
	GLuint display_texture;



	const int max_depth = 50;
	bool send_tex = true;

	hittable_list world;
	camera cam;



	void create_window();
	void gl_setup();
	void draw_everything();


	void one_thread_sample(int index, int count);
	glm::dvec3 ray_color(const ray& r, const hittable& world, int depth);

	std::vector<std::vector<glm::dvec3>> accumulated_samples;


	bool pquit;
	void quit();
};

#endif
