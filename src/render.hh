#pragma once

#include "core.hh"

char* LoadString(const char* path);

enum ShaderType
{
	SHADER_FRAGMENT = 0,
	SHADER_VERTEX,
};

struct Shader
{
    u32 gl_id;
	Shader AddStage(ShaderType type, char* source);
	Shader Link();
	void Bind();
};

Shader CreateShader();
void FreeShader(Shader* shader);

typedef u32 BufferFlags;
enum {
	BUFFER_NONE   = 0x0,
	BUFFER_VERTEX = 0x1,
	BUFFER_INDEX  = 0x2,
};

struct Buffer {
	u32         gl_id;
	u32         size;
	BufferFlags flags;
	void Bind();
};

Buffer CreateBuffer(u32 size, void* data, BufferFlags flags);
void FreeBuffer(Buffer* buffer);

struct Texture {
	u32 gl_id;
	void Gen();
	void Bind();
	void Set(u32 width, u32 height, unsigned char* data);
	void Free();
};
