#pragma once
/*!
	Dynamic Library class
*/
class DynamicLibrary
{
private:
	void* Handle;
public:
	DynamicLibrary();
	~DynamicLibrary();
	
	/*!
		Loads a Dynamic Library at a certain location
		\param FileName the Location to load the Dynamic Library from
		\return Whether the Library was loaded
	*/
	bool Load(const std::string &FileName);

	/*!
		Obtains the Function Address of a procedure
		\param ProcName the procedure
		\return the Procedure's Address
	*/
	void* GetAddressAt(const std::string &ProcName);
};
