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
	-101.0f, -100.0f, 1.0f, 0.0f, 1.0f,
	-102.0f, 100.0f, 1.0f, 0.0f, 0.0f,
	103.0f, 100.0f, 1.0f, 1.0f, 0.0f,
	104.0f, -100.0f, 1.0f, 1.0f, 1.0f
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

	printf("da\n");
	
	char* fragmentSource = LoadString("fragment.glsl");
	char* vertexSource = LoadString("vertex.glsl");

	Shader shader = CreateShader()
		.AddStage(SHADER_FRAGMENT, fragmentSource)
		.AddStage(SHADER_VERTEX, vertexSource)
		.Link();

	Buffer vbo = CreateBuffer(sizeof(vboData), vboData, BUFFER_VERTEX);
	Buffer ibo = CreateBuffer(sizeof(iboData), iboData, BUFFER_INDEX);

	GLenum errZ;
	while((errZ = glGetError()) != GL_NO_ERROR)
	{
	// Process/log the error.
		printf("GLZ error %u\n", errZ);
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLenum errD;
	while((errD = glGetError()) != GL_NO_ERROR)
	{
	// Process/log the error.
		printf("GLD error %u\n", errD);
	}
	//vbo.Bind();
	vbo.Bind();
	ibo.Bind();
	shader.Bind();

	GLenum errE;
	while((errE = glGetError()) != GL_NO_ERROR)
	{
	// Process/log the error.
		printf("GLD error %u\n", errE);
	}

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

	GLenum errB;
	while((errB = glGetError()) != GL_NO_ERROR)
	{
	// Process/log the error.
		printf("GLB error %u\n", errB);
	}


	InitPipeline();

	printf("da\n");

	int64_t lastTicks = -1;

	static int thetaRefFrame1 = 7;
	static int thetaRefFrame2 = 1740;

	static float thetaRef1 = 0.0f + 1.57f;
	static float thetaRef2 = 207.345f + 1.57f;

	// ImGui::InputInt("Frame 1", &thetaRefFrame1);
	// if (ImGui::InputFloat("Theta 1", &thetaRef1))
	// {
	// 	thetaRefFrame1 = frameNumber;
	// }

	// ImGui::InputInt("Frame 2", &thetaRefFrame2);
	// if (ImGui::InputFloat("Theta 2", &thetaRef2))
	// {
	// 	thetaRefFrame2 = frameNumber;
	// }
	
	// float theta = lineMap((float)thetaRefFrame1, thetaRef1, (float)thetaRefFrame2, thetaRef2, (float)frameNumber);
	// ImGui::InputFloat("Theta", &theta);
	// float thetaMod = fmod(theta, PI * 2.0f);
	// ImGui::InputFloat("ThetaM", &thetaMod);

	// ImGui::End();


	// vector2 coordRight = thetaToCoord(degToRad(-45.0f), 60.0f, 57.0f, thetaMod);
	// vector2 coordLeft = thetaToCoord(degToRad(45.0f), 60.0f, 57.0f, thetaMod);

	printf("da\n");
	// Populate data structure
	Image planes[19];
	TexturedPlaneMesh bufferData[19];
	Buffer buffer[19];

	// float iVboData[] = {
	// 	coordLeft.y, -100.0f, coordLeft.x, 0.0f, 1.0f,
	// 	coordLeft.y, 100.0f, coordLeft.x, 0.0f, 0.0f,
	// 	coordRight.y, 100.0f, coordRight.x, 1.0f, 0.0f,
	// 	coordRight.y, -100.0f, coordRight.x, 1.0f, 1.0f
	// };

	

	for (int i = 0; i < 20; i++)
	{
		int iFrame = 2 + i;
		float theta = lineMap((float)thetaRefFrame1, thetaRef1, (float)thetaRefFrame2, thetaRef2, (float)iFrame);
		float thetaMod = fmod(theta, PI * 2.0f);

		vector2 coordRight = thetaToCoord(degToRad(-45.0f), 60.0f, 57.0f, thetaMod);
		vector2 coordLeft = thetaToCoord(degToRad(45.0f), 60.0f, 57.0f, thetaMod);
		
		planes[i] = loadImage(PATH, &iFrame);
		printf("%d, theta %f\n", iFrame, thetaMod);

		
		bufferData[i] = {
			coordLeft.y, -100.0f, coordLeft.x, 0.0f, 1.0f,
			coordLeft.y, 100.0f, coordLeft.x, 0.0f, 0.0f,
			coordRight.y, 100.0f, coordRight.x, 1.0f, 0.0f,
			coordRight.y, -100.0f, coordRight.x, 1.0f, 1.0f
		};
		

		//printf("%f, %f, %f\n", ((float*)&bufferData[i])[0], ((float*)&bufferData[i])[1],((float*)&bufferData[i])[2]);

		//buffer[i] = CreateBuffer(sizeof(TexturedPlaneMesh), (float*)&bufferData[i], BUFFER_VERTEX);
		buffer[i] = CreateBuffer(sizeof(vboData), &bufferData[i], BUFFER_VERTEX);
		
		printf("Bufcreate\n");

		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
		{
		// Process/log the error.
			printf("GL1 error %u\n", err);
		}
	}

	printf("da\n");
	
	

	while (running)
	{
		UpdateWindowEvents(&window);

		GUIBeginFrame();

		//ImGui::ShowDemoWindow();
		
		//ImGui::Begin("Controls");

		// bool frameStepped = ImGui::InputInt("Frame Number", &frameNumber);
		// if (frameNumber < 1)
		// 	frameNumber = 1;
		
		// static bool isPlaying = false;
		// if (isPlaying)
		// {
		// 	if (ImGui::Button("Pause"))
		// 	{
		// 		isPlaying = false;
		// 	}
		// }
		// else {
		// 	if (ImGui::Button("Play"))
		// 	{
		// 		isPlaying = true;
		// 	}
		// }
	
		// if ((SDL_GetTicks64() - lastTicks > 10 && isPlaying) || (frameStepped))
		// {

		// 	// pull new frame
		// 	{
				

		// 		Image raw;
		// 		Image processed;
				
				
		// 		raw = loadImage(PATH, &frameNumber);
		// 		printf("%04d.png, %d, %d\n", frameNumber, raw.width, raw.height);

		// 		processed.width = raw.width;
		// 		processed.height = raw.height;

		// 		InitFrame(&processed);
		// 		ZeroFrame(&processed);

		// 		ProcessPipeline(raw, &processed);

		// 		texture.Set(processed.width, processed.height, processed.data);

		// 		freeImage(&raw);
		// 		FreeFrame(&processed);

		// 		if (!frameStepped)
		// 			frameNumber++;
		// 	}

		// 	lastTicks = SDL_GetTicks64();
		// }

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
		for (int i = 0; i < 20; i++)
		{	

			texture.Set(planes[i].width, planes[i].height, planes[i].data);

			ibo.Bind();
			buffer[i].Bind();
			



			GLenum err;
			while((err = glGetError()) != GL_NO_ERROR)
			{
			// Process/log the error.
				printf("GL2 error %u\n", err);
			}

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

			GLenum err7;
			while((err7 = glGetError()) != GL_NO_ERROR)
			{
			// Process/log the error.
				printf("GL3 error %u\n", err7);
			}

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			printf("Drawed %d\n", i);
		}
		

		

		GUIRenderFrame();

		SwapWindow(&window);
	}

	

	FreeShader(&shader);
	FreeRenderer(&window);
	FreeWindow(&window);
	FreePipeline();
	
    return 0;
}