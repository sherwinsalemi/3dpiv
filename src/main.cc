#include <stdio.h>
#include <stdint.h>



#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core.hh"
#include "render.hh"
#include "cv.hh"
#include "settings.hh"

float vboData[] = {
	-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 1.0f, 1.0f
};

u32 iboData[] = {
	0, 1, 2,
	0, 2, 3
};

float testArrowVBO[] = {
	-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 1.0f, 1.0f
};

SDL_Window* window;
SDL_GLContext context;

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("CV", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 6 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );

	
	SDL_GLContext context;
	context = SDL_GL_CreateContext(window);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		printf("Failed to load GLAD\n");
	}

	SDL_Log("Vendor   : %s", glGetString(GL_VENDOR));
    SDL_Log("Renderer : %s", glGetString(GL_RENDERER));
    SDL_Log("Version  : %s", glGetString(GL_VERSION));
    SDL_Log("GLSL     : %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    int maj;
    int min;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
    SDL_Log("Context  : %d.%d", maj, min);

    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);
    SDL_Log("Context  : %d.%d", maj, min);

	int frameNumber = 57;

	int width,height,channels;
	char fileName[32];
	unsigned char* img;

	u32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	img = stbi_load("res/phone/%04d.png", &width, &height, &channels, 3);
	Texture texture;
	texture.Gen();
	texture.Set(width, height, img);
	stbi_image_free(img);
	
	printf("1\n");

	char* fragmentSource = LoadString("fragment.glsl");
	char* vertexSource = LoadString("vertex.glsl");

	Shader shader = CreateShader()
		.AddStage(SHADER_FRAGMENT, fragmentSource)
		.AddStage(SHADER_VERTEX, vertexSource)
		.Link();

	Buffer vbo = CreateBuffer(sizeof(vboData), vboData, BUFFER_VERTEX);
	Buffer ibo = CreateBuffer(sizeof(iboData), iboData, BUFFER_INDEX);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	vbo.Bind();
	ibo.Bind();
	shader.Bind();
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		5*sizeof(float),
		(void*)0);

	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		5*sizeof(float),
		(void*)(3*sizeof(float)));

	initPipeline();
	

	int64_t lastTicks = -1;
	bool running = true;
	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
			running = false;
				break;
			}
		}

		if (SDL_GetTicks64() - lastTicks > 100)
		{
			// pull new frame
			{
				frameNumber++;
				
				sprintf(fileName, "res/phone/%04d.png", frameNumber);
				
				//printf("Loaded %s\n", fileName);
				img = stbi_load(fileName, &width, &height, &channels, 3);
				printf("%04d.png, %d, %d\n", frameNumber, width, height);

				Image raw;
				Image processed;

				raw.width = width;
				raw.height = height;
				processed.width = width;
				processed.height = height;

				initFrame(&processed);
				zeroFrame(&processed);

				copyFrame(img, &raw);

				processFrame(raw, &processed);

				texture.Set(width, height, processed.data);
				stbi_image_free(img);

				freeFrame(&raw);
				freeFrame(&processed);

				
			}

			lastTicks = SDL_GetTicks64();
		}


		shader.Bind();

		glClearColor(0.1, 0.1, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1i(glGetUniformLocation(shader.gl_id, "uTexture"), 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}

	freePipeline();
	FreeShader(&shader);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
    return 0;
}