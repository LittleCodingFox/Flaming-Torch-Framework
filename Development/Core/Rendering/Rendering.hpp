#pragma once
#if USE_GRAPHICS

#
#ifdef CreateFont
#	undef CreateFont
#endif

class Input;
class Texture;

#define MAX_VERTEX_CACHE_SIZE 20000

/*!
*	Blending Modes
*/
namespace BlendingMode
{
	enum BlendingMode
	{
		None = 0, //!<No Blending
		Alpha //!<Alpha Blending
	};
}

/*!
*	Vertex Element Type
*/
namespace VertexElementType
{
	enum VertexElementType
	{
		Position, //!<Position of the vertex
		TexCoord, //!<Texture Coordinate of the vertex
		Color, //!<Color of the vertex
		Normal //!<Normal of the vertex
	};
}

/*!
*	Vertex Element Data Type
*/
namespace VertexElementDataType
{
	enum VertexElementDataType
	{
		Float, //!<Single Float
		Float2, //!<Two Floats (Vector2)
		Float3, //!<Three Floats (Vector3)
		Float4, //!<Four Floats (Vector4)
		Count //!<Total data types (do not use)
	};
}

/*!
*	Vertex Details Modes
*/
namespace VertexDetailsMode
{
	enum VertexDetailsMode
	{
		Mixed = 0, //!<Vertices are mixed (e.g., vertices are in format (pos, tex, normal, pos, tex, normal)
		Lists //!<Vertices are lists (e.g., vertices are in format (pos, pos, pos, tex, tex, tex, norma, normal, normal)
	};
}

/*!
*	Vertex Element Descriptor
*/
struct VertexElementDescriptor
{
	uint32 Offset; //!<Byte Offset
	uint8 Type; //!<One of VertexElementType::*
	uint8 DataType; //!<One of VertexElementDataType::*
};

/*!
*	Position and Color Vertex Structure
*/
struct PosCol
{
	Vector3 Position;
	Vector4 Color;
};

/*!
*	PosCol Vertex Format
*/
static VertexElementDescriptor PosColFormat[] = {
	{ 0, VertexElementType::Position, VertexElementDataType::Float3 },
	{ sizeof(Vector3), VertexElementType::Color, VertexElementDataType::Float4 }
};

/*!
*	Position, TexCoord and Color Vertex Structure
*/
struct PosTexCol
{
	Vector2 Position, TexCoord;
	Vector4 Color;
};

/*!
*	PosTexCol Vertex Format
*/
static VertexElementDescriptor PosTexColFormat[] = {
	{ 0, VertexElementType::Position, VertexElementDataType::Float2 },
	{ sizeof(Vector2), VertexElementType::TexCoord, VertexElementDataType::Float2 },
	{ sizeof(Vector2[2]), VertexElementType::Color, VertexElementDataType::Float4 }
};

/*!
*	Renderer Vertex Modes namespace
*/
namespace VertexModes
{
	enum VertexModes
	{
		Triangles, //!<Triangle list
		Lines, //!<Line list
		Points, //!<Point list
		Count //!<Total Vertex Modes (do not use)
	};
}

/*!
*	Renderer Window Styles
*/
namespace RendererWindowStyle
{
	enum RendererWindowStyle
	{
		Popup = 0, //!<Window is a popup with no border or title bar
		FullScreen, //!<Window is a fullscreen window
		Default //!<Default Window with a border and title bar
	};
}

/*!
*	Renderer Buffers namespace
*/
namespace RendererBuffer
{
	enum RendererBuffer
	{
		Color = FLAGVALUE(0), //!<Color Buffer
		Depth = FLAGVALUE(1), //!<Depth Buffer
		Stencil = FLAGVALUE(2) //!<Stencil Buffer
	};
}

extern uint8 VertexBufferDataElementSizes[VertexElementDataType::Count];
extern uint32 VertexBufferDataElementCount[VertexElementDataType::Count];

class IRendererImplementation;

#include "RendererEvent.hpp"
#include "RendererFrameStats.hpp"
#include "RendererDisplayMode.hpp"
#include "RendererCapabilities.hpp"
#include "Renderer.hpp"
#include "IRendererImplementation.hpp"

#if USE_SDL_RENDERER
#	include "Implementations/SDLRenderer.hpp"
#	undef DEFAULT_RENDERER_IMPLEMENTATION
#	define DEFAULT_RENDERER_IMPLEMENTATION SDLRendererImplementation
#endif

#if USE_SFML_RENDERER
#	include "Implementations/SFMLRenderer.hpp"
#	undef DEFAULT_RENDERER_IMPLEMENTATION
#	define DEFAULT_RENDERER_IMPLEMENTATION SFMLRendererImplementation
#endif

#ifndef DEFAULT_RENDERER_IMPLEMENTATION
#	define DEFAULT_RENDERER_IMPLEMENTATION NULLRendererImplementation
#endif

#include "Implementations/NULLRenderer.hpp"

#endif
