#pragma once

/*!
	The Shader Types
*/
namespace ShaderType
{
	enum
	{
		Vertex = 0, //!<Vertex Shader
		Fragment, //!<Fragment Shader
		Geometry //!<Geometry Shader
	};
};

/*!
	A Shader class
*/
class Shader
{
private:
	uint32 VertexHandle, FragmentHandle, GeometryHandle, ProgramHandle;
public:
	Shader();
	~Shader();

	/*!
		Compiles a Shader from its code
		\param Type one of ShaderType::*
		\param Data the Shader Code
		\param DataLength the Length of the Shader Code
		\param Log the Shader compilation log (Optional)
		\return Whether the shader was successfully compiled
		\sa ShaderType
	*/
	bool CompileShader(uint32 Type, void* Data, 
		uint32 DataLength, std::string *Log = NULL);

	/*!
		Links the Shader
		\param Log the Shader linkage log (Optional)
		\return Whether the Shader was successfully linked
	*/
	bool LinkShader(std::string *Log = NULL);

	/*!
		Gets an Uniform location
		\param Name the Uniform's name
		\return the Uniform's Location
		\sa GetAttribute
	*/
	int32 GetUniform(const char *Name);

	/*!
		Gets an Attribute location
		\param Name the Attribute's name
		\return the Attribute's Location
		\sa GetUniform
	*/
	int32 GetAttribute(const char *Name);
	void UniformFloat(int32 Uniform, f32 value);
	void UniformVector2(int32 Uniform, const Vector2 &value);
	void UniformVector3(int32 Uniform, const Vector3 &value);
	void UniformVector4(int32 Uniform, const Vector4 &value);
	void UniformFloatArray(int32 Uniform, uint32 Length, f32 *Ptr);
	void UniformVector2Array(int32 Uniform, uint32 Length, Vector2 *Ptr);
	void UniformVector3Array(int32 Uniform, uint32 Length, Vector3 *Ptr);
	void UniformVector4Array(int32 Uniform, uint32 Length, Vector4 *Ptr);
	void UniformMat3(int32 Uniform, f32 *value, uint32 Count);
	void UniformMat4(int32 Uniform, Matrix4x4 *value, uint32 Count);
	void UniformInt(int32 Uniform, int32 value);
	void UniformInt2(int32 Uniform, int32 v1, int32 v2);
	void UniformInt3(int32 Uniform, int32 v1, int32 v2, int32 v3);
	void UniformInt4(int32 Uniform, int32 v1, int32 v2, int32 v3, int32 v4);
	void UniformIntArray(int32 Uniform, uint32 Length, int32 *Ptr);
	void UniformInt2Array(int32 Uniform, uint32 Length, int32 *Ptr);
	void UniformInt3Array(int32 Uniform, uint32 Length, int32 *Ptr);
	void UniformInt4Array(int32 Uniform, uint32 Length, int32 *Ptr);

	/*!
		Destroys the Shader
	*/
	void Destroy();

	/*!
		Activates this Shader for rendering
	*/
	void Activate();

	/*!
		Deactivates this Shader
	*/
	void Deactivate();
};
