#ifndef RTTNW_UTIL
#define RTTNW_UTIL

#include "rttnw.h"
// This contains the very high level expression of what's going on

void rttnw::create_window()
{
	if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8);

	// this is how you query the screen resolution
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0, &dm);

	// pulling these out because I'm going to try to span the whole screen with
	// the window, in a way that's flexible on different resolution screens
	int total_screen_width = dm.w;
	int total_screen_height = dm.h;

	cout << "creating window.....................";

	// window = SDL_CreateWindow( "OpenGL Window", 0, 0, total_screen_width, total_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS );
	window = SDL_CreateWindow( "OpenGL Window", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE );
  	SDL_ShowWindow(window);

	cout << "done." << endl;


	cout << "setting up OpenGL context...........";
	// OpenGL 4.3 + GLSL version 430
	const char* glsl_version = "#version 430";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	GLcontext = SDL_GL_CreateContext( window );

	SDL_GL_MakeCurrent(window, GLcontext);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	// SDL_GL_SetSwapInterval(0); // explicitly disable vsync

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
	}

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);

    glPointSize(3.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // void cast prevents unused variable warning

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, GLcontext);
	ImGui_ImplOpenGL3_Init(glsl_version);

	clear_color = ImVec4(75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.5f); // initial value for clear color

	// really excited by the fact imgui has an hsv picker to set this
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear( GL_COLOR_BUFFER_BIT );
	SDL_GL_SwapWindow( window );

	cout << "done." << endl << endl;

	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text]                   = ImVec4(0.64f, 0.37f, 0.37f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.49f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.17f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.18f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.35f, 0.00f, 0.03f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


    accumulated_samples.resize(WIDTH);

    for(auto& x : accumulated_samples)
    {
        x.resize(HEIGHT);
    }


	 const auto aspect_ratio = 1.0 / 1.0;
    const int image_width = 600;
    // const int image_height = static_cast<int>(image_width / aspect_ratio);

	    point3 lookfrom;
	    point3 lookat;
	    vec3 vup(0,1,0);
	    auto vfov = 40.0;
	    auto aperture = 0.0;
	    auto dist_to_focus = 10.0;
	    background = color(0,0,0);

	    switch (8) {
	        case 1:
	            world = random_scene();
	            lookfrom = point3(13,2,3);
	            lookat = point3(0,0,0);
	            vfov = 20.0;
	            background = color(0.70, 0.80, 1.00);
	            break;

	        case 2:
	            world = two_spheres();
	            lookfrom = point3(13,2,3);
	            lookat = point3(0,0,0);
	            vfov = 20.0;
	            background = color(0.70, 0.80, 1.00);
	            break;

	        case 3:
	            world = two_perlin_spheres();
	            lookfrom = point3(13,2,3);
	            lookat = point3(0,0,0);
	            vfov = 20.0;
	            background = color(0.70, 0.80, 1.00);
	            break;

	        case 4:
	            world = earth();
	            lookfrom = point3(0,0,12);
	            lookat = point3(0,0,0);
	            vfov = 20.0;
	            background = color(0.70, 0.80, 1.00);
	            break;

	        case 5:
	            world = simple_light();
	            lookfrom = point3(26,3,6);
	            lookat = point3(0,2,0);
	            vfov = 20.0;
	            break;

	        default:
	        case 6:
	            world = cornell_box();
	            lookfrom = point3(278, 278, -800);
	            lookat = point3(278, 278, 0);
	            vfov = 40.0;
	            break;

	        case 7:
	            world = cornell_balls();
	            lookfrom = point3(278, 278, -800);
	            lookat = point3(278, 278, 0);
	            vfov = 40.0;
	            break;

	        case 8:
	            world = cornell_smoke();
	            lookfrom = point3(278, 278, -800);
	            lookat = point3(278, 278, 0);
	            vfov = 40.0;
	            break;

	        case 9:
	            world = cornell_final();
	            lookfrom = point3(278, 278, -800);
	            lookat = point3(278, 278, 0);
	            vfov = 40.0;
	            break;

	        case 10:
	            world = final_scene();
	            lookfrom = point3(478, 278, -600);
	            lookat = point3(278, 278, 0);
	            vfov = 40.0;
	            break;
	    }

    cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
}


void rttnw::gl_setup()
{
	// some info on your current platform
	const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
	const GLubyte *version = glGetString( GL_VERSION );		// version as a string
	printf( "Renderer: %s\n", renderer );
	printf( "OpenGL version supported %s\n\n\n", version );



    // create the shader for the triangles to cover the screen
    display_shader = Shader("resources/code/shaders/blit.vs.glsl", "resources/code/shaders/blit.fs.glsl").Program;

    // set up the points for the display
    //  A---------------B
    //  |          .    |
    //  |       .       |
    //  |    .          |
    //  |               |
    //  C---------------D

    // diagonal runs from C to B
    //  A is -1, 1
    //  B is  1, 1
    //  C is -1,-1
    //  D is  1,-1
    std::vector<glm::vec3> points;

    points.clear();
    points.push_back(glm::vec3(-1, 1, 0.5));  //A
    points.push_back(glm::vec3(-1,-1, 0.5));  //C
    points.push_back(glm::vec3( 1, 1, 0.5));  //B

    points.push_back(glm::vec3( 1, 1, 0.5));  //B
    points.push_back(glm::vec3(-1,-1, 0.5));  //C
    points.push_back(glm::vec3( 1,-1, 0.5));  //D

    // vao, vbo
    cout << "  setting up vao, vbo for display geometry...........";
    glGenVertexArrays( 1, &display_vao );
    glBindVertexArray( display_vao );

    glGenBuffers( 1, &display_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, display_vbo );
    cout << "done." << endl;

    // buffer the data
    cout << "  buffering vertex data..............................";
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * points.size(), &points[0]);
    cout << "done." << endl;

    // set up attributes
    cout << "  setting up attributes in display shader............";
    GLuint points_attrib = glGetAttribLocation(display_shader, "vPosition");
    glEnableVertexAttribArray(points_attrib);
    glVertexAttribPointer(points_attrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (static_cast<const char*>(0) + (0)));
    cout << "done." << endl;


    // create the image textures
    glGenTextures(1, &display_texture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, display_texture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    // compile the compute shader to do the raycasting

    // ...



}


static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void rttnw::draw_everything()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io; // void cast prevents unused variable warning
    //get the screen dimensions and pass in as uniforms


	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);   // from hsv picker
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                     // clear the background

	// draw the stuff on the GPU

    // start a timer
    auto start = std::chrono::high_resolution_clock::now();

    int num_threads = 16;

    // launch all the threads
    std::thread t0(&rttnw::one_thread_sample, this, 0, num_threads);
    std::thread t1(&rttnw::one_thread_sample, this, 1, num_threads);
    std::thread t2(&rttnw::one_thread_sample, this, 2, num_threads);
    std::thread t3(&rttnw::one_thread_sample, this, 3, num_threads);
    std::thread t4(&rttnw::one_thread_sample, this, 4, num_threads);
    std::thread t5(&rttnw::one_thread_sample, this, 5, num_threads);
    std::thread t6(&rttnw::one_thread_sample, this, 6, num_threads);
    std::thread t7(&rttnw::one_thread_sample, this, 7, num_threads);
    std::thread t8(&rttnw::one_thread_sample, this, 8, num_threads);
    std::thread t9(&rttnw::one_thread_sample, this, 9, num_threads);
    std::thread t10(&rttnw::one_thread_sample, this, 10, num_threads);
    std::thread t11(&rttnw::one_thread_sample, this, 11, num_threads);
    std::thread t12(&rttnw::one_thread_sample, this, 12, num_threads);
    std::thread t13(&rttnw::one_thread_sample, this, 13, num_threads);
    std::thread t14(&rttnw::one_thread_sample, this, 14, num_threads);
    std::thread t15(&rttnw::one_thread_sample, this, 15, num_threads);


    // join all the threads
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    t11.join();
    t12.join();
    t13.join();
    t14.join();
    t15.join();


    // increment the sample count
    sample_count++;

    // stop that timer, add its value to the total time
    auto end = std::chrono::high_resolution_clock::now();
    int time_in_milliseconds = (int)std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count();
    total_time += time_in_milliseconds;

	cout << "sample took " << time_in_milliseconds << "ms" << endl;

    // start a new timer, to see how long it took to send the data to the GPU
    start = std::chrono::high_resolution_clock::now();


    if(send_tex)
    {
        std::vector<unsigned char> tex_data;
        tex_data.resize(0);

        for(unsigned int y = 0; y < HEIGHT; y++)
        {	for(unsigned int x = 0; x < WIDTH; x++)
            {
				auto r = accumulated_samples[x][y].x/static_cast<double>(sample_count);
				auto g = accumulated_samples[x][y].y/static_cast<double>(sample_count);
				auto b = accumulated_samples[x][y].z/static_cast<double>(sample_count);

				// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
				if (r != r) r = 0.0;
				if (g != g) g = 0.0;
				if (b != b) b = 0.0;

				// // gamma-correct for gamma=2.0. (sqrt, aka pow(blah, -2))
				// r = sqrt(r);
				// g = sqrt(g);
				// b = sqrt(b);

				//more flexible gamma correction
				r = pow(r, gamma_factor);
				g = pow(g, gamma_factor);
				b = pow(b, gamma_factor);

                tex_data.push_back(static_cast<int>(256 * clamp(r, 0.0, 0.999)));
                tex_data.push_back(static_cast<int>(256 * clamp(g, 0.0, 0.999)));
                tex_data.push_back(static_cast<int>(256 * clamp(b, 0.0, 0.999)));
                tex_data.push_back(static_cast<unsigned char>(255));
            }
        }

        // buffer the averaged data to the GPU
        glBindTexture(GL_TEXTURE_2D, display_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex_data[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // stop the timer
    end = std::chrono::high_resolution_clock::now();
    int time_buffering = (int)std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count();
    total_time += time_buffering;

    // texture display
    glUseProgram(display_shader);
    glBindVertexArray( display_vao );
    glBindBuffer( GL_ARRAY_BUFFER, display_vbo );

    glDrawArrays( GL_TRIANGLES, 0, 6 );


	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	// show the demo window
	// static bool show_demo_window = true;
	// if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

	// do my own window
	ImGui::SetNextWindowPos(ImVec2(10,10));
	ImGui::SetNextWindowSize(ImVec2(300, 250));
	ImGui::Begin("Controls", NULL, 0);

    //do the other widgets
    ImGui::Text("How many samples do you want?");

    ImGui::InputInt(" ", &num_samples);
    ImGui::SameLine(); HelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");
    ImGui::Text("%i samples have been completed", sample_count);
	ImGui::Text(" ");

	ImGui::SliderFloat(" Gamma ", &gamma_factor, 0.0f, 2.0f, "%.3f");

    ImGui::Text(" ");
    ImGui::Checkbox("Send to GPU each sample: ", &send_tex);

    ImGui::Text(" ");
    ImGui::Text("Previous sample took:        %*i ms", 9, time_in_milliseconds);
    ImGui::Text("Averaging/GPU buffering took:%*i ms", 9, time_buffering);
    ImGui::Text("Total elapsed:              %*li ms", 10, total_time);



	ImGui::End();
	ImGui::Render();

	glViewport(0,0,(int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());   // put imgui data into the framebuffer

	SDL_GL_SwapWindow(window);			// swap the double buffers

	// handle events

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		if (event.type == SDL_QUIT)
			pquit = true;

		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
			pquit = true;

		if ((event.type == SDL_KEYUP  && event.key.keysym.sym == SDLK_ESCAPE) || (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1)) //x1 is browser back on the mouse
			pquit = true;
        if((event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE))
            send_tex = !send_tex;
    }
}


color rttnw::ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

void rttnw::one_thread_sample(int thread_index, int thread_count)
{
    long unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine engine{seed};
    std::uniform_real_distribution<double> distribution{0, 1};

    // do a sample for all the pixels - this takes time
    for(auto & x : accumulated_samples)
    {
        for(auto & y : x)
        {
            int x_coord = &x - &accumulated_samples[0];
            int y_coord = &y - &x[0];

            if(x_coord % thread_count == thread_index)
            {
                double x_fl = (static_cast<double>(x_coord) + distribution(engine))/(static_cast<double>(WIDTH-1));
                double y_fl = (static_cast<double>(y_coord) + distribution(engine))/(static_cast<double>(HEIGHT-1));

                ray r = cam.get_ray(x_fl, y_fl);

                // figure out the color, put it in 'sample'
                color sample = ray_color(r, background, world, max_depth);

                // push it onto the vector of samples for this pixel
                y += glm::dvec3(sample.x(), sample.y(), sample.z());
            }
        }
    }
}

void rttnw::quit()
{
  //shutdown everything
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  //destroy window
  SDL_GL_DeleteContext(GLcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  //average the samples and create your output using LodePNG
    std::vector<unsigned char> tex_data;
    tex_data.resize(0);             //zero it out




    // iterate through the samples per pixel and compute the average color
    for(int y = HEIGHT-1; y >= 0; y--)         // iterating through y
    {   for(int x = 0; x < WIDTH; x++)         // iterating through x
        {
			auto r = accumulated_samples[x][y].x/static_cast<double>(sample_count);
			auto g = accumulated_samples[x][y].y/static_cast<double>(sample_count);
			auto b = accumulated_samples[x][y].z/static_cast<double>(sample_count);

			// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
			if (r != r) r = 0.0;
			if (g != g) g = 0.0;
			if (b != b) b = 0.0;

			// // gamma-correct for gamma=2.0. (sqrt)
			// r = sqrt(r);
			// g = sqrt(g);
			// b = sqrt(b);

			//more flexible gamma correction
			r = pow(r, gamma_factor);
			g = pow(g, gamma_factor);
			b = pow(b, gamma_factor);

			tex_data.push_back(static_cast<int>(256 * clamp(r, 0.0, 0.999)));
			tex_data.push_back(static_cast<int>(256 * clamp(g, 0.0, 0.999)));
			tex_data.push_back(static_cast<int>(256 * clamp(b, 0.0, 0.999)));
			tex_data.push_back(static_cast<unsigned char>(255));
        }
    }


    unsigned width, height;

    width = WIDTH;
    height = HEIGHT;

    /* tex_data.resize(4*WIDTH*HEIGHT); */
    std::string filename = std::string("save.png");

    unsigned error = lodepng::encode(filename.c_str(), tex_data, width, height);

    if(error) std::cout << "decode error during save(\" "+ filename +" \") " << error << ": " << lodepng_error_text(error) << std::endl;




    cout << "goodbye." << endl;
}

#endif
