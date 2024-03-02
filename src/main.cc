#include <stdio.h>
#include <stdint.h>



#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"

#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core.hh"
#include "backend/render.hh"
#include "cv/cv.hh"
#include "settings.hh"
#include "backend/frameloader.hh"
#include "cv/compositor.hh"
#include "backend/input.hh"

#include "imgui.h"

struct TexturedPlaneMesh
{
	vector3 DLvertex;
	vector2 DLtexcoord;
	
	vector3 ULvertex;
	vector2 ULtexcoord;

	vector3 URvertex;
	vector2 URtexcoord;

	vector3 DRvertex;
	vector2 DRtexcoord;
};

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

Window window;

float cameraLookSpeed = 0.005f;
float cameraMoveSpeed = 1.5f;

glm::vec3 cameraPos;

float cameraPitch = 0.0f;
float cameraYaw = 0.0f;

bool running = true;
int main()
{
	InitWindow(&window);
	InitRenderer(&window);
	
	int frameNumber = 1;
	
	Texture texture; // opengl texture
	texture.Gen();
	
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

	InitPipeline();

	int64_t lastTicks = -1;

	while (running)
	{
		UpdateWindowEvents(&window);

		GUIBeginFrame();

		//ImGui::ShowDemoWindow();
		
		ImGui::Begin("Controls");

		bool frameStepped = ImGui::InputInt("Frame Number", &frameNumber);
		if (frameNumber < 1)
			frameNumber = 1;

		static bool isPlaying = false;
		if (isPlaying)
		{
			if (ImGui::Button("Pause"))
			{
				isPlaying = false;
			}
		}
		else {
			if (ImGui::Button("Play"))
			{
				isPlaying = true;
			}
		}


		static int thetaRefFrame1 = 7;
		static int thetaRefFrame2 = 1740;

		static float thetaRef1 = 0.0f + 1.57f;
		static float thetaRef2 = 207.345f + 1.57f;

		ImGui::InputInt("Frame 1", &thetaRefFrame1);
		if (ImGui::InputFloat("Theta 1", &thetaRef1))
		{
			thetaRefFrame1 = frameNumber;
		}

		ImGui::InputInt("Frame 2", &thetaRefFrame2);
		if (ImGui::InputFloat("Theta 2", &thetaRef2))
		{
			thetaRefFrame2 = frameNumber;
		}


		static float thetaOffset = 0.0f;
		ImGui::InputFloat("Theta Offset", &thetaOffset);
		
		float theta = lineMap((float)thetaRefFrame1, thetaRef1, (float)thetaRefFrame2, thetaRef2, (float)frameNumber);
		ImGui::InputFloat("Theta", &theta);
		float thetaMod = fmod(theta, PI * 2.0f / 5.0f);
		ImGui::InputFloat("ThetaM", &thetaMod);

		static float camRight = 60.0f;
		static float camForward = 57.0f;

		ImGui::End();


		vector2 coordRight = thetaToCoord(degToRad(-45.0f), 60.0f, 57.0f, thetaMod);
		vector2 coordLeft = thetaToCoord(degToRad(45.0f), 60.0f, 57.0f, thetaMod);

		vboData[0] = coordLeft.y;
		vboData[2] = coordLeft.x;

		vboData[5] = coordLeft.y;
		vboData[7] = coordLeft.x;

		vboData[10] = coordRight.y;
		vboData[12] = coordRight.x;

		vboData[15] = coordRight.y;
		vboData[17] = coordRight.x;

		vbo.Update(sizeof(vboData), vboData);


		if ((SDL_GetTicks64() - lastTicks > 10 && isPlaying) || (frameStepped))
		{

			// pull new frame
			{
				

				Image raw;
				Image processed;
				
				
				raw = loadImage(PATH, &frameNumber);
				printf("%04d.png, %d, %d\n", frameNumber, raw.width, raw.height);

				processed.width = raw.width;
				processed.height = raw.height;

				InitFrame(&processed);
				ZeroFrame(&processed);

				ProcessPipeline(raw, &processed);

				texture.Set(processed.width, processed.height, processed.data);

				freeImage(&raw);
				FreeFrame(&processed);

				if (!frameStepped)
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

		if (window.relativeMouse)
		{
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
		}

		

		movement = glm::rotate(
			glm::identity<glm::mat4>(),
			cameraYaw,
			glm::vec3 {0.0, 1.0, 0.0}
		) * movement;

		cameraPos.x += movement.x;
		cameraPos.y += movement.y;
		cameraPos.z += movement.z;

		

		

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

		

		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1i(glGetUniformLocation(shader.gl_id, "uTexture"), 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		GUIRenderFrame();

		SwapWindow(&window);
	}

	

	FreeShader(&shader);
	FreeRenderer(&window);
	FreeWindow(&window);
	FreePipeline();
	
    return 0;
}