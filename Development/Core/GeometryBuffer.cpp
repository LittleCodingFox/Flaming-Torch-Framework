#include "FlamingCore.hpp"
#if USE_GRAPHICS
namespace FlamingTorch
{
	uint32 GBCToGL[3] = {
		GL_STREAM_DRAW,
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW
	};

	uint32 GBMToGL[3] = {
		GL_WRITE_ONLY,
		GL_READ_ONLY,
		GL_READ_WRITE
	};

	uint32 GBTToGL[2] = {
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER
	};

	uint32 GBIDTToGL[4] = {
		0,
		sizeof(uint8),
		sizeof(uint16),
		sizeof(uint32)
	};

	uint32 GBGDTToGL[12] = {
		sizeof(uint8),
		sizeof(uint8[2]),
		sizeof(uint8[3]),
		sizeof(uint8[4]),
		sizeof(uint16),
		sizeof(uint16[2]),
		sizeof(uint16[3]),
		sizeof(uint16[4]),
		sizeof(f32),
		sizeof(f32[2]),
		sizeof(f32[3]),
		sizeof(f32[4]),
	};

	uint32 GBDTECToGL[12] = {
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4,
	};

	uint32 GBDTETToGL[12] = {
		GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, 
		GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, 
		GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, 
	};

	uint32 GBIDTETToGL[4] = {
		0,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_SHORT,
		GL_UNSIGNED_INT
	};

#define BUFFER_OFFSET(x) ((char *)NULL + x)

	GeometryBuffer::GeometryBuffer() : ID(0), Type(GeometryBufferType::None), IndexFormat(GeometryBufferIndexDataType::None)
	{
	};

	GeometryBuffer::~GeometryBuffer()
	{
		Destroy();
	};

	bool GeometryBuffer::Initialize(const void* GeometryData, uint32 Length, GeometryBufferType::GeometryBufferType Type,
		GeometryElement *Elements, uint32 ElementCount, GeometryBufferCreation::GeometryBufferCreation CreationFlag,
		GeometryBufferIndexDataType::GeometryBufferIndexDataType IndexFormat)
	{
		Destroy();
		SFLASSERT(Length);
		SFLASSERT((Type == GeometryBufferType::Index && IndexFormat != GeometryBufferIndexDataType::None) ||
			(Type == GeometryBufferType::Attribute && Elements && ElementCount));

		this->Type = Type;
		this->IndexFormat = IndexFormat;

		glGenBuffers(1, (GLuint*)&ID);
		glBindBuffer(GBTToGL[Type], ID);
		glBufferData(GBTToGL[Type], Length, GeometryData, GBCToGL[CreationFlag]);
		glBindBuffer(GBTToGL[Type], 0);

		if(glGetError() == GL_OUT_OF_MEMORY)
		{
			Destroy();

			return false;
		};

		VertexSize = 0;

		if(ElementCount)
		{
			this->Elements.resize(ElementCount);
			memcpy(&this->Elements[0], Elements, sizeof(GeometryElement) * ElementCount);

			for(uint32 i = 0; i < this->Elements.size(); i++)
			{
				VertexSize += GBGDTToGL[this->Elements[i].DataType];
			};
		};

		if(Type == GeometryBufferType::Index)
		{
			int32 size = GBIDTToGL[IndexFormat];
			IndexCount = Length / size;
		};

		return true;
	};

	void GeometryBuffer::Destroy()
	{
		if(glIsBuffer(ID))
		{
			glBindBuffer(GBTToGL[Type], 0);
			glDeleteBuffers(1, (const GLuint*)&ID);
		};

		ID = 0;
		Type = GeometryBufferType::None;
		IndexFormat = GeometryBufferIndexDataType::None;
	};

	void* GeometryBuffer::Map(GeometryBufferMapping::GeometryBufferMapping Usage)
	{
		if(glIsBuffer(ID))
		{
			glBindBuffer(GBTToGL[Type], ID);
			void* Map = glMapBuffer(GBTToGL[Type], GBMToGL[Usage]);

			if(!Map)
			{
				glGetBufferPointerv(GBTToGL[Type], GL_BUFFER_MAP_POINTER, &Map);

				if(!Map)
				{
					return NULL;
				};

				return Map;
			};

			return Map;
		};

		return NULL;
	};

	void GeometryBuffer::Unmap()
	{
		glUnmapBuffer(GBTToGL[Type]);
	};

	uint32 GeometryBuffer::GetVertexSize()
	{
		return VertexSize;
	};

	uint32 GeometryBuffer::GetIndexCount()
	{
		return IndexCount;
	};

	void FillGBInfo(Shader *TheShader, SuperSmartPointer<std::vector<GeometryElement> > &Elements)
	{
		std::vector<GeometryElement> &Ref = *Elements.Get();

		for(uint32 i = 0; i < Ref.size(); i++)
		{
			switch(Ref[i].AttributeHandle)
			{
			case GeometryElementBuiltinHandle::Position:
				Ref[i].AttributeHandle = TheShader->GetAttribute("VertexPosition");

				break;
			case GeometryElementBuiltinHandle::TexCoord0:
			case GeometryElementBuiltinHandle::TexCoord1:
			case GeometryElementBuiltinHandle::TexCoord2:
			case GeometryElementBuiltinHandle::TexCoord3:
			case GeometryElementBuiltinHandle::TexCoord4:
			case GeometryElementBuiltinHandle::TexCoord5:
			case GeometryElementBuiltinHandle::TexCoord6:
			case GeometryElementBuiltinHandle::TexCoord7:
			case GeometryElementBuiltinHandle::TexCoord8:
			case GeometryElementBuiltinHandle::TexCoord9:
			case GeometryElementBuiltinHandle::TexCoord10:
			case GeometryElementBuiltinHandle::TexCoord11:
			case GeometryElementBuiltinHandle::TexCoord12:
			case GeometryElementBuiltinHandle::TexCoord13:
			case GeometryElementBuiltinHandle::TexCoord14:
			case GeometryElementBuiltinHandle::TexCoord15:
				{
					std::stringstream str;
					str << "VertexTexCoord" << Ref[i].AttributeHandle - GeometryElementBuiltinHandle::TexCoord0;

					Ref[i].AttributeHandle = TheShader->GetAttribute(str.str().c_str());
				};

				break;
			case GeometryElementBuiltinHandle::Normal:
				Ref[i].AttributeHandle = TheShader->GetAttribute("VertexNormal");

				break;
			case GeometryElementBuiltinHandle::Tangent:
				Ref[i].AttributeHandle = TheShader->GetAttribute("VertexTangent");

				break;
			case GeometryElementBuiltinHandle::BiTangent:
				Ref[i].AttributeHandle = TheShader->GetAttribute("VertexBiTangent");

				break;
			case GeometryElementBuiltinHandle::Color:
				Ref[i].AttributeHandle = TheShader->GetAttribute("VertexColor");

				break;
			};
		};

		GLCHECK();
	};

	void GeometryBuffer::Bind()
	{
		if(ID == 0 || !glIsBuffer(ID))
			return;

		glBindBuffer(Type == GeometryBufferType::Index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER, ID);

		for(uint32 i = 0; i < Elements.size(); i++)
		{
			int32 AttribHandle = Elements[i].AttributeHandle;

			if(AttribHandle == -1 || AttribHandle > 15)
				continue;

			switch(Elements[i].DataType)
			{
			case GeometryElementDataType::Skinning:
				break;
			default:
				glEnableVertexAttribArray(AttribHandle);
				GLCHECK();

				glVertexAttribPointer(AttribHandle, GBDTECToGL[Elements[i].DataType], GBDTETToGL[Elements[i].DataType],
					GL_FALSE, VertexSize, BUFFER_OFFSET(Elements[i].Offset));
				GLCHECK();

				break;
			};
		};
	};

	void GeometryBuffer::Unbind()
	{
		for(uint32 i = 0; i < Elements.size(); i++)
		{
			int32 AttribHandle = Elements[i].AttributeHandle;

			if(AttribHandle == -1 || AttribHandle > 15)
				continue;

			switch(Elements[i].DataType)
			{
			case GeometryElementDataType::Skinning:
				break;

			default:
				glDisableVertexAttribArray(AttribHandle);
				GLCHECK();
			};
		};

		glBindBuffer(Type == GeometryBufferType::Index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER, 0);
	};
};
#endif
