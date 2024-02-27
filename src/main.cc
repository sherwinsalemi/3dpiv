#include <stdio.h>
#include <stdint.h>



#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"

#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core.hh"
#include "render.hh"
#include "cv.hh"
#include "settings.hh"
#include "frameloader.hh"
#include "composite.hh"
#include "input.hh"

float vboData[] = {
	-100.0f, -100.0f, 1.0f, 0.0f, 1.0f,
	-100.0f, 100.0f, 1.0f, 0.0f, 0.0f,
	100.0f, 100.0f, 1.0f, 1.0f, 0.0f,
	100.0f, -100.0f, 1.0f, 1.0f, 1.0f
};

u32 iboData[] = {
	0, 1, 2,
	0, 2, 3
};


Renderer r;

bool relativeMouse = true;

float cameraLookSpeed = 0.005f;
float cameraMoveSpeed = 1.5f;

glm::vec3 cameraPos;

float cameraPitch = 0.0f;
float cameraYaw = 0.0f;

int main()
{
	InitRenderer(&r);
	SDL_SetRelativeMouseMode((SDL_bool)true);
	
	int frameNumber = 1;

	u32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	
	Texture texture; // opengl texture
	texture.Gen();
	
	
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


	vector2 pixelPoint = thetaToCoord(-0.26f, 60.0f, 57.0f, 0.17f);
	printf("Calc pos: %f, %f\n", pixelPoint.x, pixelPoint.y);
	

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
			case SDL_KEYDOWN:
				if (e.key.repeat == 0)
				{
					Input::SetKeyDown(e.key.keysym.sym);
				}
				else
				{
					Input::SetKeyDownRepeat(e.key.keysym.sym);
				}
				//Logger::Log(std::to_string((int)e.key.keysym.sym) + " pressed");
				break;
			case SDL_KEYUP:
				Input::SetKeyUp(e.key.keysym.sym);
				//Logger::Log(std::to_string((int)e.key.keysym.sym) + " released");
				break;
			case SDL_MOUSEMOTION:
				MousePos mousePos;

				mousePos.x = e.motion.x;
				mousePos.y = e.motion.y;
				mousePos.relativeX = e.motion.xrel;
				mousePos.relativeY = e.motion.yrel;

				if (relativeMouse)//if (mouseRelative)
				{
					int x,y;
					SDL_GetWindowSize(r.window, &x, &y);
					SDL_WarpMouseInWindow(r.window, x/2, y/2);
				} else {
					mousePos.relativeX = 0;
					mousePos.relativeY = 0;
				}

				Input::SetMouseMovement(mousePos);
				

				break;
			case SDL_MOUSEBUTTONDOWN:
				Input::SetMouseButton((MouseButton)e.button.button, true);

				break;
			case SDL_MOUSEBUTTONUP:
				Input::SetMouseButton((MouseButton)e.button.button, false);
				
				break;
			}
		}

		if (SDL_GetTicks64() - lastTicks > 10)
		{
			// pull new frame
			{
				

				Image raw;
				Image processed;
				

				raw = loadImage(PATH, frameNumber);
				printf("%04d.png, %d, %d\n", frameNumber, raw.width, raw.height);

				processed.width = raw.width;
				processed.height = raw.height;

				initFrame(&processed);
				zeroFrame(&processed);

				processFrame(raw, &processed);

				texture.Set(processed.width, processed.height, processed.data);

				freeImage(&raw);
				freeFrame(&processed);

				frameNumber++;
			}

			lastTicks = SDL_GetTicks64();
		}


		shader.Bind();


		// controls

		MousePos pos = Input::GetMouseMovement();

		cameraYaw -= pos.relativeX * cameraLookSpeed;
		cameraPitch -= pos.relativeY * cameraLookSpeed;

		glm::vec4 movement {0.0, 0.0, 0.0, 0.0};

		if (Input::KeyboardCheck(SDLK_w))
		{
			movement.z -= cameraMoveSpeed;
		}
		if (Input::KeyboardCheck(SDLK_a))
		{
			movement.x -= cameraMoveSpeed;
		}
		if (Input::KeyboardCheck(SDLK_s))
		{
			movement.z += cameraMoveSpeed;
		}
		if (Input::KeyboardCheck(SDLK_d))
		{
			movement.x += cameraMoveSpeed;
		}
		if (Input::KeyboardCheck(SDLK_LSHIFT))
		{
			movement.y -= cameraMoveSpeed;
		}
		if (Input::KeyboardCheck(SDLK_SPACE))
		{
			movement.y += cameraMoveSpeed;
		}

		

		movement = glm::rotate(
			glm::identity<glm::mat4>(),
			cameraYaw,
			glm::vec3 {0.0, 1.0, 0.0}
		) * movement;

		cameraPos.x += movement.x;
		cameraPos.y += movement.y;
		cameraPos.z += movement.z;

		if (Input::KeyboardCheckPressed(SDLK_ESCAPE))
		{
			SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
			relativeMouse = SDL_GetRelativeMouseMode();
		}

		Input::Update();

		

		glm::mat4 camera = glm::inverse(
			
				glm::rotate(
					glm::rotate(
						glm::translate(
							glm::identity<glm::mat4>(),
							glm::vec3 {cameraPos.x, cameraPos.y, cameraPos.z}),
						cameraYaw, glm::vec3 {0.0, 1.0 ,0.0}
					),
					cameraPitch,
					glm::vec3 {1.0, 0.0, 0.0}
				)
		 	
		);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1920.0f / 1080.0f, 0.01f, 5000.0f) * camera;

		glUniformMatrix4fv(
			glGetUniformLocation(shader.gl_id, "matrix"),
			1,
			false,
			&projection[0][0]
		);

		glClearColor(0.1, 0.1, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1i(glGetUniformLocation(shader.gl_id, "uTexture"), 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(r.window);

		
	}

	FreeShader(&shader);
	FreeRenderer(&r);

	freePipeline();
	
    return 0;
}