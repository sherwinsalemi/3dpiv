#include "render.hh"

#include "../core.hh"
#include "../settings.hh"

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"

#include "SDL/SDL.h"
#include "SDL/SDL_events.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

#include <assert.h>

u32 vao;

char* LoadString(const char* path)
{
	FILE* f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = (char*)malloc(size + 1);
	buf[size] = 0;
	fread(buf, 1, size, f);
	return buf;
}

void InitRenderer(Window* r)
{
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 6 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );

	r->context = SDL_GL_CreateContext(r->window);

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



	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& m_io = ImGui::GetIO(); (void)m_io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(r->window, r->context);
	ImGui_ImplOpenGL3_Init();

	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void SwapWindow(Window* r)
{
	SDL_GL_SwapWindow(r->window);
}

void FreeRenderer(Window* r)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(r->context);
}



void GUIBeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void GUIRenderFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




Shader CreateShader()
{
	Shader s;
	s.gl_id = glCreateProgram();

	return s;
}

Shader Shader::AddStage(ShaderType type, char* source)
{
	u32 id = 0;

	switch (type)
	{
	case SHADER_FRAGMENT:
		id = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case SHADER_VERTEX:
		id = glCreateShader(GL_VERTEX_SHADER);
		break;
	}

	glShaderSource(id, 1, &source, NULL);

	glCompileShader(id);

	i32 status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		i32 len = 0;
		char msg[1025];
		glGetShaderInfoLog(id, 1024, &len, msg);
		msg[len] = '\0';

		switch (type)
		{
		case SHADER_FRAGMENT:
			printf("Pixel shader compile failed: %s\n", msg);
			break;
		case SHADER_VERTEX:
			printf("Vertex shader compile failed: %s\n", msg);
			break;
		}
	}

	glAttachShader(this->gl_id, id);
	glDeleteShader(id);

	return *this;
}

Shader Shader::Link()
{
	glLinkProgram(this->gl_id);

	return *this;
}

void Shader::Bind()
{
	glUseProgram(this->gl_id);
}

void FreeShader(Shader* buffer)
{
	glDeleteProgram(buffer->gl_id);
}

void Buffer::Bind()
{
	switch (this->flags)
	{
	case BUFFER_VERTEX:
		glBindBuffer(GL_ARRAY_BUFFER, this->gl_id);
		printf("Vertex bound %u\n", this->gl_id);
		break;
	case BUFFER_INDEX:
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->gl_id);
		break;
	default:
		printf("Warning: Attempted to bind buffer with no case\n");
		break;
	}
}

void Buffer::Update(u32 size, void* data)
{
	glNamedBufferSubData(this->gl_id, 0, size, data);	
}

Buffer CreateBuffer(u32 size, void* data, BufferFlags flags) {
	Buffer buffer = {
		.size = size,
		.flags = flags
	};
	glCreateBuffers(1, &buffer.gl_id);

	printf("%u ID created\n", buffer.gl_id);

	assert((flags & BUFFER_VERTEX) != 0 || (flags & BUFFER_INDEX) != 0);

	glNamedBufferStorage(buffer.gl_id, size, data, GL_DYNAMIC_STORAGE_BIT);	

	return buffer;
}

void FreeBuffer(Buffer* buffer)
{
	glDeleteBuffers(1, &buffer->gl_id);
}

void Texture::Gen()
{
	glGenTextures(1, &this->gl_id);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gl_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Set(u32 width, u32 height, unsigned char* data)
{
	glBindTexture(GL_TEXTURE_2D, this->gl_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Free()
{
	glDeleteTextures(1, &this->gl_id);
}
