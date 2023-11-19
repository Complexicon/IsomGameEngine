#define GLAD_GL_IMPLEMENTATION
#include "../glad.hpp"
#include "OpenGL.h"
#include "windows.h"

#include <iostream>

static void* GetAnyGLFuncAddress(const char* name) {
	void* p = (void*)wglGetProcAddress(name);
	if(p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void*)GetProcAddress(module, name);
	}

	return p;
}

const char* vertexShader = R"glsl(
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in uint a_TexIndex;
layout(location = 4) in uint a_AlphaTexIndex;

out vec4 v_Color;
out vec2 v_TexCoord;
flat out uint v_TexIndex;
flat out uint v_AlphaTexIndex;

void main() {
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_AlphaTexIndex = a_AlphaTexIndex;
	gl_Position = vec4(a_Position, 1.0);
}

)glsl";

const char* fragShader = R"glsl(
#version 450 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in uint v_TexIndex;
flat in uint v_AlphaTexIndex;

layout (binding = 0) uniform sampler2D u_Texture[16];

void main() {
	vec4 actualTex = texture(u_Texture[int(v_TexIndex)], v_TexCoord);
	vec4 alphaTex = texture(u_Texture[int(v_AlphaTexIndex)], v_TexCoord);
	vec4 textureMasked = vec4(actualTex.r, actualTex.g, actualTex.b, alphaTex.a);
	color = textureMasked * v_Color;
}

)glsl";

	static void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{

		std::cout << severity << message << std::endl; 
		
	}

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
	switch(type) {
	case ShaderDataType::Float: return GL_FLOAT;
	case ShaderDataType::Float2: return GL_FLOAT;
	case ShaderDataType::Float3: return GL_FLOAT;
	case ShaderDataType::Float4: return GL_FLOAT;
	case ShaderDataType::Mat3: return GL_FLOAT;
	case ShaderDataType::Mat4: return GL_FLOAT;
	case ShaderDataType::Int: return GL_INT;
	case ShaderDataType::Int2: return GL_INT;
	case ShaderDataType::Int3: return GL_INT;
	case ShaderDataType::Int4: return GL_INT;
	case ShaderDataType::Bool: return GL_BOOL;
	}

	return 0;
}

static void AddVertexBuffer(uint32_t vao, uint32_t vbo, const BufferLayout& layout) {

	uint32_t m_VertexBufferIndex = 0;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	for(const auto& element : layout) {
		switch(element.Type) {
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4: {
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(),
								  ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE,
								  layout.GetStride(), (const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool: {
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribIPointer(m_VertexBufferIndex, element.GetComponentCount(),
								   ShaderDataTypeToOpenGLBaseType(element.Type), layout.GetStride(),
								   (const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4: {
			uint8_t count = element.GetComponentCount();
			for(uint8_t i = 0; i < count; i++) {
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex, count, ShaderDataTypeToOpenGLBaseType(element.Type),
									  element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(),
									  (const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(m_VertexBufferIndex, 1);
				m_VertexBufferIndex++;
			}
			break;
		}
		}
	}
}

uint32_t UploadTexture(iV2D size, vector<uint8_t> bitmap, uint32_t format = GL_RGBA, uint32_t layout = GL_UNSIGNED_INT_8_8_8_8) {
	uint32_t texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, bitmap.data());
	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texID;
}

OpenGL::OpenGL(Win32Window* owner) {
	this->owner = owner;
	owner->OnResize = [&]() { this->shouldResizeNextTick = true; };

	glDeviceContext = GetDC(owner->windowHandle);
	PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
								 1,
								 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
								 PFD_TYPE_RGBA,
								 32,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 PFD_MAIN_PLANE,
								 0,
								 0,
								 0,
								 0};

	int pf = 0;
	if(!(pf = ChoosePixelFormat(glDeviceContext, &pfd))) throw runtime_error("failed to set pixel format for opengl device");
	SetPixelFormat(glDeviceContext, pf, &pfd);

	if(!(glRenderContext = wglCreateContext(glDeviceContext))) throw runtime_error("failed to create opengl device");
	wglMakeCurrent(glDeviceContext, glRenderContext);

	gladLoadGL((GLADloadfunc)GetAnyGLFuncAddress);

	// compile shaders

	int success;
	char infoLog[512];

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

		glEnable(GL_LINE_SMOOTH);

	uint32_t quadVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(quadVertexShader, 1, &vertexShader, nullptr);
	glCompileShader(quadVertexShader);

	glGetShaderiv(quadVertexShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(quadVertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	uint32_t quadFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(quadFragmentShader, 1, &fragShader, nullptr);
	glCompileShader(quadFragmentShader);

	glGetShaderiv(quadFragmentShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(quadFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	quadShader = glCreateProgram();
	glAttachShader(quadShader, quadVertexShader);
	glAttachShader(quadShader, quadFragmentShader);
	glLinkProgram(quadShader);

	glGetProgramiv(quadShader, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(quadShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Create Vertex Buffer Object

	glCreateBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_QUADS * 4 * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);

	// create and bind buffers for quads
	BufferLayout quadLayout = {{ShaderDataType::Float3, "a_Position"},
							   {ShaderDataType::Float4, "a_Color"},
							   {ShaderDataType::Float2, "a_TexCoord"},
							   {ShaderDataType::Int, "a_TexIndex"},
							   {ShaderDataType::Int, "a_AlphaTexIndex"}};


	glCreateVertexArrays(1, &quadVAO);

	// Create Index Buffer

	array<uint32_t, MAX_QUADS * 6> quadIndicies;

	uint32_t offset = 0;
	for(uint32_t i = 0; i < MAX_QUADS; i += 6) {
		quadIndicies[i + 0] = offset + 0;
		quadIndicies[i + 1] = offset + 1;
		quadIndicies[i + 2] = offset + 2;

		quadIndicies[i + 3] = offset + 2;
		quadIndicies[i + 4] = offset + 3;
		quadIndicies[i + 5] = offset + 0;

		offset += 4;
	}

	glCreateBuffers(1, &quadIBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadIBO);
	glBufferData(GL_ARRAY_BUFFER, quadIndicies.size() * sizeof(uint32_t), quadIndicies.data(), GL_STATIC_DRAW);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28, 0);
	// glEnableVertexAttribArray(0); // pos

	// glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 24, (void*)12);
	// glEnableVertexAttribArray(1); // color

	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)28);
	// glEnableVertexAttribArray(2); // texture coords

	// glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 36, (void*)36);
	// glEnableVertexAttribArray(3); // texture slot

	AddVertexBuffer(quadVAO, quadVBO, quadLayout);

	vector<uint8_t> bitmap;

	bitmap.reserve(4);

	bitmap.push_back(0xff);
	bitmap.push_back(0xff);
	bitmap.push_back(0xff);
	bitmap.push_back(0xff);

	textureSlots[0] = UploadTexture({1, 1}, bitmap);

	wglMakeCurrent(nullptr, nullptr);
}

glm::vec3 OpenGL::ScreenToRelative(float x, float y, float zOffset) {
	return { (x / owner->w) * 2 - 1 , -((y / owner->h) * 2 - 1), (float)layer + zOffset };
}

OpenGL::~OpenGL() {}

void OpenGL::Clear(const Color& color) {
	fV4D glColor = color;
	glClearColor(glColor.a, glColor.b, glColor.c, glColor.d);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::DrawAt(float x, float y, const struct LineTo& line) {}

void OpenGL::DrawAt(float x, float y, const struct Rectangle& rect) {

	glm::vec4 color;
	fV4D tmp = rect.color;

	color.r = tmp.a;
	color.g = tmp.b;
	color.b = tmp.c;
	color.a = tmp.d;

	if(rect.fill) {
		quadVerticies[quadVertexCount++] = { ScreenToRelative(x, y), color, glm::vec2{ 0, 0 }, 0, 0};
		quadVerticies[quadVertexCount++] = { ScreenToRelative(x + rect.size.x, y), color, glm::vec2{ 0, 0 }, 0, 0};
		quadVerticies[quadVertexCount++] = { ScreenToRelative(x + rect.size.x, y + rect.size.y), color, glm::vec2{ 0, 0 }, 0, 0};
		quadVerticies[quadVertexCount++] = { ScreenToRelative(x, y + rect.size.y), color, glm::vec2{ 0, 0 }, 0, 0};
	} else {

	}
}

void OpenGL::DrawAt(float x, float y, const struct Sprite& sprite) {}

void OpenGL::DrawAt(float x, float y, const struct Text2D& text) {

	uint32_t textureSlot;
	uint32_t textureID;

	if(fontTextures.find(text.font) == fontTextures.end()) {

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		textureID = UploadTexture(text.font->size, text.font->bitmap, GL_ALPHA, GL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		fontTextures[text.font] = textureID;
	} else {
		textureID = fontTextures[text.font];
	}

	textureSlot = texturesInUse;
	textureSlots[texturesInUse++] = textureID;

	uint32_t offsetX = 0;

	glm::vec4 color;
	fV4D tmp = text.color;

	color.r = tmp.a;
	color.g = tmp.b;
	color.b = tmp.c;
	color.a = tmp.d;

	auto& bitmapSz = text.font->size;

	for(char c : text.text) {


		if(c == '\n') {
			offsetX = 0;
			y += text.font->ascender + text.font->lineGap;
			continue;
		}

		auto& glyph = text.font->glyphs[c];

		if(glyph.size.x == 0 || glyph.size.y == 0) {
			offsetX += glyph.advance;
			continue;
		}

		float yPos = y + (text.font->ascender + glyph.bearing.y);
		float xPos = x + offsetX + glyph.bearing.x;

		quadVerticies[quadVertexCount++] = {
			ScreenToRelative(xPos, yPos), color,
			glm::vec2{(float)glyph.texCoords.x / bitmapSz.x, (float)glyph.texCoords.y / bitmapSz.y}, 0, textureSlot};

		quadVerticies[quadVertexCount++] = {
			ScreenToRelative(xPos + glyph.size.x, yPos), color,
			glm::vec2{(float)(glyph.texCoords.x + glyph.size.x) / bitmapSz.x, (float)glyph.texCoords.y / bitmapSz.y},
			0, textureSlot};

		quadVerticies[quadVertexCount++] = {
			ScreenToRelative(xPos + glyph.size.x, yPos + glyph.size.y),
			color,
			glm::vec2{(float)(glyph.texCoords.x + glyph.size.x) / bitmapSz.x, (float)(glyph.texCoords.y + glyph.size.y) / bitmapSz.y},
			0, textureSlot
		};

		quadVerticies[quadVertexCount++] = {
			ScreenToRelative(xPos, yPos + glyph.size.y),
			color,
			glm::vec2{(float)glyph.texCoords.x / bitmapSz.x, (float)(glyph.texCoords.y + glyph.size.y) / bitmapSz.y},
			0, textureSlot};

		offsetX += glyph.advance;

		zOffset += LAYER_OFFSET;

		quadCount += 1;
	}

	int xxx = 0;
}

Texture* OpenGL::LoadSpriteFromFile(string filePath) { return nullptr; }

Texture* OpenGL::LoadSpriteFromBitmap(const vector<uint8_t>& bytes) { return nullptr; }

void OpenGL::BeginDraw() {
	wglMakeCurrent(glDeviceContext, glRenderContext);

	quadVertexCount = 0;
	quadCount = 0;
	texturesInUse = 1;

	// quadVerticies[quadVertexCount++] = {
	// 	{ -0.5, -0.5, 0.0 },
	// 	{0.0,0.0,0.0,0.0},
	// 	{0.0, 0.0},
	// 	0.0
	// };

	// quadVerticies[quadVertexCount++] = {
	// 	{ 0.5, -0.5, 0.0 },
	// 	{0.0,0.0,0.0,0.0},
	// 	{0.0, 0.0},
	// 	0.0
	// };

	// 	quadVerticies[quadVertexCount++] = {
	// 	{ 0.5, 0.5, 0.0 },
	// 	{0.0,0.0,0.0,0.0},
	// 	{0.0, 0.0},
	// 	0.0
	// };

	// quadVerticies[quadVertexCount++] = {
	// 	{ -0.5, 0.5, 0.0 },
	// 	{0.0,0.0,0.0,0.0},
	// 	{0.0, 0.0},
	// 	0.0
	// };

	// quadCount++;

}

bool OpenGL::EndDraw() {

	for(int i = 0; i < texturesInUse; i++) { glBindTextureUnit(i, textureSlots[i]); }

	if(quadCount) {

		glUseProgram(quadShader);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, quadVertexCount * sizeof(QuadVertex), quadVerticies.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBindVertexArray(quadVAO);

		glDrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_INT, 0);
	}
// snip


	// unsigned int VAO;
	// glGenVertexArrays(1, &VAO);  

	// auto top = ScreenToRelative(owner->w/2, 0);
	// auto left = ScreenToRelative(0,owner->h);
	// auto right = ScreenToRelative(owner->w,owner->h);

	// float vertices[] = {
	// 	top.x, top.y, top.z,// 0,0,0,0, 0,0, 0,
	// 	left.x, left.y, left.z, //0,0,0,0, 0,0, 0,
	// 	right.x, right.y, right.z,// 0,0,0,0, 0,0, 0,
	// };  

	// unsigned int VBO;
	// glGenBuffers(1, &VBO);
	// glBindVertexArray(VAO);
	// // 2. copy our vertices array in a buffer for OpenGL to use
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// // 3. then set our vertex attributes pointers
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray(0); 

	// glUseProgram(quadShader);
	// glDrawArrays(GL_TRIANGLES, 0, 3);

// snip

	glFlush();
	SwapBuffers(glDeviceContext);

	if(shouldResizeNextTick) { glViewport(0, 0, owner->w, owner->h); }

	return true;
}
