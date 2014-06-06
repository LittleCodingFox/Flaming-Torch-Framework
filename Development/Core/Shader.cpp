#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	Shader::Shader() : VertexHandle(0), FragmentHandle(0), GeometryHandle(0), ProgramHandle(0)
	{
	};

	Shader::~Shader()
	{
		Destroy();
	};

	void Shader::Destroy()
	{
		GLCHECK();

		if(glIsShader(VertexHandle))
		{
			GLCHECK();

			if(glIsProgram(ProgramHandle))
			{
				GLCHECK();

				glDetachShader(ProgramHandle, VertexHandle);
			};

			GLCHECK();

			glDeleteShader(VertexHandle);

			GLCHECK();
		};

		GLCHECK();

		if(glIsShader(FragmentHandle))
		{
			GLCHECK();

			if(glIsProgram(FragmentHandle))
			{
				GLCHECK();

				glDetachShader(ProgramHandle, FragmentHandle);
			};

			GLCHECK();

			glDeleteShader(FragmentHandle);
		};

		GLCHECK();

		if(glIsShader(GeometryHandle))
		{
			GLCHECK();

			if(glIsProgram(GeometryHandle))
			{
				GLCHECK();

				glDetachShader(ProgramHandle, GeometryHandle);
			};

			GLCHECK();

			glDeleteShader(GeometryHandle);
		};

		GLCHECK();

		glDeleteProgram(ProgramHandle);

		GLCHECK();

		VertexHandle = FragmentHandle = GeometryHandle = ProgramHandle = 0;
	};

	bool Shader::CompileShader(uint32 Type, void* Data, 
		uint32 DataLength, std::string *Log)
	{
		if(Data == NULL || DataLength == 0)
		{
			if(Log)
			{
				*Log = std::string("Invalid Data or DataLength");
			};

			return false;
		};

		if(Type == ShaderType::Geometry && !glewIsSupported("GL_EXT_geometry_shader4"))
		{
			if(Log)
			{
				*Log = std::string("Geometry Shaders are unsupported");
			};

			return false;
		};

		GLCHECK();

		if(glIsProgram(ProgramHandle))
			Destroy();

		GLCHECK();

		int32 _Status = 0;

		switch(Type)
		{
		case ShaderType::Vertex:
			if(glIsShader(VertexHandle))
			{
				GLCHECK();

				glDeleteShader(VertexHandle);

				GLCHECK();
			};

			VertexHandle = glCreateShader(GL_VERTEX_SHADER);

			GLCHECK();

			glShaderSource(VertexHandle, 1, (const GLchar**)&Data, (const GLint*)&DataLength);

			GLCHECK();

			glCompileShader(VertexHandle);

			GLCHECK();

			glGetShaderiv(VertexHandle, GL_COMPILE_STATUS, (GLint *)&_Status);

			GLCHECK();

			if(_Status != 1)
			{
				std::vector<int8> InfoLog(102401);
				InfoLog[InfoLog.size() - 1] = '\0';

				int dummy;

				GLCHECK();

				glGetShaderInfoLog(VertexHandle, 102400, &dummy, (GLchar *)&InfoLog[0]);

				GLCHECK();

				glDeleteShader(VertexHandle);

				GLCHECK();

				VertexHandle = 0;

				if(Log)
				{
					*Log = std::string((char *)&InfoLog[0]);
				};

				return false;
			};

			return true;
			break;
		case ShaderType::Fragment:
			if(glIsShader(FragmentHandle))
			{
				GLCHECK();

				glDeleteShader(FragmentHandle);
			};

			GLCHECK();

			FragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);

			GLCHECK();

			glShaderSource(FragmentHandle, 1, (const GLchar**)&Data, (const GLint*)&DataLength);

			GLCHECK();

			glCompileShader(FragmentHandle);

			GLCHECK();

			glGetShaderiv(FragmentHandle, GL_COMPILE_STATUS, (GLint *)&_Status);

			GLCHECK();

			if(_Status != 1)
			{
				std::vector<int8> InfoLog(102401);
				InfoLog[InfoLog.size() - 1] = '\0';

				int dummy;

				GLCHECK();

				glGetShaderInfoLog(FragmentHandle, 102400, &dummy, (GLchar *)&InfoLog[0]);

				GLCHECK();

				glDeleteShader(FragmentHandle);

				GLCHECK();

				FragmentHandle = 0;

				if(Log)
				{
					*Log = std::string((char *)&InfoLog[0]);
				};

				return false;
			};

			return true;
			break;
		case ShaderType::Geometry:
			if(glIsShader(GeometryHandle))
			{
				GLCHECK();

				glDeleteShader(GeometryHandle);

				GLCHECK();
			};

			GeometryHandle = glCreateShader(GL_GEOMETRY_SHADER_EXT);

			GLCHECK();

			glShaderSource(GeometryHandle, 1, (const GLchar**)&Data, (const GLint*)&DataLength);

			GLCHECK();

			glCompileShader(GeometryHandle);

			GLCHECK();

			glGetShaderiv(GeometryHandle, GL_COMPILE_STATUS, (GLint *)&_Status);

			GLCHECK();

			if(_Status != 1)
			{
				std::vector<int8> InfoLog(102401);
				InfoLog[InfoLog.size() - 1] = '\0';

				int dummy;

				GLCHECK();

				glGetShaderInfoLog(GeometryHandle, 102400, &dummy, (GLchar *)&InfoLog[0]);

				GLCHECK();

				glDeleteShader(GeometryHandle);

				GLCHECK();

				GeometryHandle = 0;

				if(Log)
				{
					*Log = std::string((char *)&InfoLog[0]);
				};

				return false;
			};

			return true;
			break;
		};
		return false;
	};

	bool Shader::LinkShader(std::string *Log)
	{
		GLCHECK();

		if(!glIsShader(VertexHandle))
		{
			if(Log)
			{
				*Log = std::string("Vertex Shader Missing");
			};

			return false;
		};

		GLCHECK();

		if(glIsProgram(ProgramHandle))
		{
			if(Log)
			{
				*Log = std::string("Shader already linked");
			};

			return false;
		};

		GLCHECK();

		ProgramHandle = glCreateProgram();

		GLCHECK();

		glAttachShader(ProgramHandle, VertexHandle);

		GLCHECK();

		if(glIsShader(FragmentHandle))
		{
			GLCHECK();

			glAttachShader(ProgramHandle, FragmentHandle);

			GLCHECK();
		};

		GLCHECK();

		if(glIsShader(GeometryHandle))
		{
			GLCHECK();

			glAttachShader(ProgramHandle, GeometryHandle);

			GLCHECK();
		};

		GLCHECK();

		glLinkProgram(ProgramHandle);

		GLCHECK();

		int32 _Status = 0;

		GLCHECK();

		glGetProgramiv(ProgramHandle, GL_LINK_STATUS, (GLint *)&_Status);

		GLCHECK();

		if(_Status != 1)
		{
			std::vector<int8> InfoLog(102401);
			InfoLog[InfoLog.size() - 1] = '\0';

			int dummy;
			glGetProgramInfoLog(ProgramHandle, 102400, &dummy, (GLchar *)&InfoLog[0]);

			GLCHECK();

			glDeleteShader(ProgramHandle);

			GLCHECK();

			Destroy();

			if(Log)
			{
				*Log = std::string((char *)&InfoLog[0]);
			};

			return false;
		};

		return true;
	};

	int32 Shader::GetUniform(const char *Name)
	{
		GLCHECK();

		if(!glIsProgram(ProgramHandle))
			return -1;

		GLCHECK();

		int32 ID = glGetUniformLocation(ProgramHandle, Name);

		GLCHECK();

		if(ID == -1)
			Log::Instance.LogWarn("Shader", "Shader Uniform not found: '%s'.", Name);

		return ID;
	};

	void Shader::UniformFloat(int32 Uniform, f32 value)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform1f(Uniform, value);

		GLCHECK();
	};

	void Shader::UniformVector2(int32 Uniform, const Vector2 &value)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform2f(Uniform, value.x, value.y);

		GLCHECK();
	};

	void Shader::UniformVector3(int32 Uniform, const Vector3 &value)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform3f(Uniform, value.x, value.y, value.z);

		GLCHECK();
	};

	void Shader::UniformVector4(int32 Uniform, const Vector4 &value)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform4f(Uniform, value.x, value.y, value.z, value.w);

		GLCHECK();
	};

	void Shader::UniformFloatArray(int32 Uniform, uint32 Length, f32 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform1fv(Uniform, Length, Ptr);

		GLCHECK();
	};

	void Shader::UniformVector2Array(int32 Uniform, uint32 Length, Vector2 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform2fv(Uniform, Length, (float*)Ptr);

		GLCHECK();
	};

	void Shader::UniformVector3Array(int32 Uniform, uint32 Length, Vector3 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform3fv(Uniform, Length, (float*)Ptr);

		GLCHECK();
	};

	void Shader::UniformVector4Array(int32 Uniform, uint32 Length, Vector4 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform4fv(Uniform, Length, (float*)Ptr);

		GLCHECK();
	};

	void Shader::UniformMat3(int32 Uniform, f32 *value, uint32 Count)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniformMatrix3fv(Uniform, Count, false, value);

		GLCHECK();
	};

	void Shader::UniformMat4(int32 Uniform, Matrix4x4 *value, uint32 Count)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniformMatrix4fv(Uniform, Count, false, (const GLfloat*)value);

		GLCHECK();
	};

	void Shader::UniformInt(int32 Uniform, int32 value)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform1i(Uniform, value);

		GLCHECK();
	};

	void Shader::UniformInt2(int32 Uniform, int32 v1, int32 v2)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform2i(Uniform, v1, v2);

		GLCHECK();
	};

	void Shader::UniformInt3(int32 Uniform, int32 v1, int32 v2, int32 v3)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform3i(Uniform, v1, v2, v3);

		GLCHECK();
	};

	void Shader::UniformInt4(int32 Uniform, int32 v1, int32 v2, int32 v3, int32 v4)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform4i(Uniform, v1, v2, v3, v4);

		GLCHECK();
	};

	void Shader::UniformIntArray(int32 Uniform, uint32 Length, int32 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform1iv(Uniform, Length, (GLint *)Ptr);

		GLCHECK();
	};

	void Shader::UniformInt2Array(int32 Uniform, uint32 Length, int32 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform2iv(Uniform, Length, (GLint *)Ptr);

		GLCHECK();
	};

	void Shader::UniformInt3Array(int32 Uniform, uint32 Length, int32 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUniform3iv(Uniform, Length, (GLint *)Ptr);

		GLCHECK();
	};

	void Shader::UniformInt4Array(int32 Uniform, uint32 Length, int32 *Ptr)
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));
		int CurrentProgram = 0;

		GLCHECK();

		glGetIntegerv(GL_CURRENT_PROGRAM, &CurrentProgram);

		GLCHECK();

		glUseProgram(ProgramHandle);

		GLCHECK();

		glUniform4iv(Uniform, Length, (GLint *)Ptr);

		GLCHECK();

		glUseProgram(CurrentProgram);

		GLCHECK();
	};

	void Shader::Activate()
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUseProgram(ProgramHandle);

		GLCHECK();
	};

	void Shader::Deactivate()
	{
		GLCHECK();

		SVOIDFLASSERT(glIsProgram(ProgramHandle));

		GLCHECK();

		glUseProgram(0);

		GLCHECK();
	};

	int32 Shader::GetAttribute(const char *Name)
	{
		GLCHECK();

		if(!glIsProgram(ProgramHandle))
			return -1;

		GLCHECK();

		int32 ID = glGetAttribLocation(ProgramHandle, Name);

		GLCHECK();

		if(ID == -1)
			Log::Instance.LogWarn("Shader", "Shader Attribute not found: '%s'.", Name);

		return ID;
	};
#endif
};
