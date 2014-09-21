#pragma once

namespace GeometryBufferType
{
	enum GeometryBufferType
	{
		Attribute,
		Index,
		None = 0xFF
	};
};

namespace GeometryBufferCreation
{
	enum GeometryBufferCreation
	{
		Stream = 0,
		Static,
		Dynamic
	};
};

namespace GeometryBufferMapping
{
	enum GeometryBufferMapping
	{
		Write = 0,
		Read,
		ReadAndWrite
	};
};

namespace GeometryBufferIndexDataType
{
	enum GeometryBufferIndexDataType
	{
		None = 0,
		Char,
		Short,
		Int
	};
};

namespace GeometryElementDataType
{
	enum GeometryElementDataType
	{
		Char = 0,
		Char2,
		Char3,
		Char4,
		Short,
		Short2,
		Short3,
		Short4,
		Float,
		Float2,
		Float3,
		Float4,
		Skinning
	};
};

namespace GeometryElementBuiltinHandle
{
	enum GeometryElementBuiltinHandle
	{
		Position = 0,
		TexCoord0,
		TexCoord1,
		TexCoord2,
		TexCoord3,
		TexCoord4,
		TexCoord5,
		TexCoord6,
		TexCoord7,
		TexCoord8,
		TexCoord9,
		TexCoord10,
		TexCoord11,
		TexCoord12,
		TexCoord13,
		TexCoord14,
		TexCoord15,
		Normal,
		Tangent,
		BiTangent,
		Color,
		Count
	};
};

struct GeometryElement
{
public:
	uint32 Offset;
	GeometryElementDataType::GeometryElementDataType DataType;
	int32 AttributeHandle;
};

class GeometryBuffer
{
	friend class RendererManager;
private:
	uint32 ID;
	GeometryBufferIndexDataType::GeometryBufferIndexDataType IndexFormat;
	GeometryBufferType::GeometryBufferType Type;
	std::vector<GeometryElement> Elements;
	uint32 VertexSize, IndexCount;
public:

	GeometryBuffer();
	~GeometryBuffer();

	/*!
	*	Initializes this geometry buffer
	*	\param GeometryData Data as collection of bytes
	*	\param Length the length of the buffer
	*	\param Type the type of geometry buffer
	*	\param Elements the format elements
	*	\param ElementCount the amount of format elements
	*	\param CreationFlag how to create the buffer
	*	\param IndexFormat the format of the index
	*/
	bool Initialize(const void* GeometryData, uint32 Length,
		GeometryBufferType::GeometryBufferType Type, GeometryElement *Elements,
		uint32 ElementCount, GeometryBufferCreation::GeometryBufferCreation CreationFlag,
		GeometryBufferIndexDataType::GeometryBufferIndexDataType IndexFormat = GeometryBufferIndexDataType::None);

	void Destroy();

	/*!
	*	Maps a buffer into modifiable memory
	*	\param Usage the way the memory will be handled
	*/
	void *Map(GeometryBufferMapping::GeometryBufferMapping Usage = GeometryBufferMapping::Read);

	/*!
	*	Finishes mapping a buffer of modifiable memory
	*/
	void Unmap();

	void Bind();
	void Unbind();
	uint32 GetVertexSize();
	uint32 GetIndexCount();
};
