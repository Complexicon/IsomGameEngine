#pragma once
#include "engine/Renderer.h"
#include "Window.h"

#include "glm/glm.hpp"

struct QuadVertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	uint32_t TextureSlot = 0; // white
	uint32_t AlphaTextureSlot = 0; // white
};

// struct CircleVertex {
// 	glm::vec3 WorldPosition;
// 	glm::vec3 LocalPosition;
// 	glm::vec4 Color;
// 	float Thickness;
// 	float Fade;
// };

// struct LineVertex {
// 	glm::vec3 Position;
// 	glm::vec4 Color;
// };

enum class ShaderDataType { None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool };

static uint32_t ShaderDataTypeSize(ShaderDataType type) {
	switch(type) {
	case ShaderDataType::Float: return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;
	case ShaderDataType::Mat3: return 4 * 3 * 3;
	case ShaderDataType::Mat4: return 4 * 4 * 4;
	case ShaderDataType::Int: return 4;
	case ShaderDataType::Int2: return 4 * 2;
	case ShaderDataType::Int3: return 4 * 3;
	case ShaderDataType::Int4: return 4 * 4;
	case ShaderDataType::Bool: return 1;
	}

	return 0;
}

struct BufferElement {
	std::string Name;
	ShaderDataType Type;
	uint32_t Size;
	size_t Offset;
	bool Normalized;

	BufferElement() = default;

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

	uint32_t GetComponentCount() const {
		switch(Type) {
		case ShaderDataType::Float: return 1;
		case ShaderDataType::Float2: return 2;
		case ShaderDataType::Float3: return 3;
		case ShaderDataType::Float4: return 4;
		case ShaderDataType::Mat3: return 3; // 3* float3
		case ShaderDataType::Mat4: return 4; // 4* float4
		case ShaderDataType::Int: return 1;
		case ShaderDataType::Int2: return 2;
		case ShaderDataType::Int3: return 3;
		case ShaderDataType::Int4: return 4;
		case ShaderDataType::Bool: return 1;
		}

		return 0;
	}
};

class BufferLayout {
  public:
	BufferLayout() {}

	BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements) { CalculateOffsetsAndStride(); }

	uint32_t GetStride() const { return m_Stride; }
	const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

  private:
	void CalculateOffsetsAndStride() {
		size_t offset = 0;
		m_Stride = 0;
		for(auto& element : m_Elements) {
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

  private:
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0;
};

const uint32_t MAX_QUADS = 10000;
const float LAYER_OFFSET = 1.f / (MAX_QUADS*3);
const uint32_t MAX_TEXTURES = 16;

class OpenGL : public Renderer {
	Win32Window* owner;
	bool shouldResizeNextTick = false;
	HDC glDeviceContext;
	HGLRC glRenderContext;

	array<QuadVertex, MAX_QUADS * 4> quadVerticies;

	map<Font*, uint32_t> fontTextures;
	map<Texture*, uint32_t> textureAtlases;
	array<uint32_t, MAX_TEXTURES> textureSlots;

	uint32_t quadVertexCount = 0;
	uint32_t quadCount = 0;
	uint32_t texturesInUse = 1; // white

	uint32_t quadVBO;
	uint32_t quadIBO;
	uint32_t quadVAO;

	float zOffset = 0;

	uint32_t quadShader;

	glm::vec3 ScreenToRelative(float x, float y, float zOffset = 0);

  public:
	OpenGL(Win32Window* owner);

	~OpenGL();

	void Clear(const Color& color);

	void DrawAt(float x, float y, const struct LineTo& line);
	void DrawAt(float x, float y, const struct Rectangle& rect);
	void DrawAt(float x, float y, const struct Sprite& sprite);
	void DrawAt(float x, float y, const struct Text2D& text);

	Texture* LoadSpriteFromFile(string filePath);
	Texture* LoadSpriteFromBitmap(const vector<uint8_t>& bytes);

	void BeginDraw();
	bool EndDraw();
};