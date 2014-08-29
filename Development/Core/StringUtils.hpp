#pragma once
/*!
	String Utilities
*/
class StringUtils
{
public:
	/*!
		Splits a String
		\param String the String to split
		\param Separator the Separator of the String
		\return the String Fragments
		\sa Join
	*/
	static std::vector<std::string> Split(const std::string &String, char Separator);
	/*!
		Joins a split String
		\param Separator the Separator to use when merging the Strings
		\param Fragments the String Fragments
		\return the merged string
		\sa Split
	*/
	static std::string Join(const std::string &Separator, const std::vector<std::string> &Fragments);

	/*!
		Converts a Hex string to a uint32
		\param str the String to convert
		\return the converted uint32
	*/
	static uint32 HexToInt(const std::string &str);

	/*!
		Converts a Hex string to a f32
		\param str the String to convert
		\return the converted f32
	*/
	static f32 HexToFloat(const std::string &str);

	/*!
		Converts a int32 to a String
		\param i the int32 to convert
		\param Hex whether to convert to a Hex string
		\return the converted string
	*/
	static std::string MakeIntString(const int32 &i, bool Hex = false);

	/*!
		Converts a uint32 to a String
		\param i the uint32 to convert
		\param Hex whether to convert to a Hex string
		\return the converted string
	*/
	static std::string MakeIntString(const uint32 &i, bool Hex = false);
    
	/*!
     Converts a int64 to a String
     \param i the int64 to convert
     \param Hex whether to convert to a Hex string
     \return the converted string
     */
	static std::string MakeIntString(const int64 &i, bool Hex = false);
    
	/*!
     Converts a uint64 to a String
     \param i the uint64 to convert
     \param Hex whether to convert to a Hex string
     \return the converted string
     */
	static std::string MakeIntString(const uint64 &i, bool Hex = false);

	/*!
		Converts a f32 to a String
		\param f the f32 to convert
		\return the converted string
	*/
	static std::string MakeFloatString(const f32 &f);

	/*!
		Converts a uint8 to a String
		\param u the uint8 to convert
		\return the converted string
	*/
	static std::string MakeByteString(const uint8 &u);

	/*!
		Strips a String from a character
		\param str the string to strip
		\param c the character to remove
		\return the stripped string
	*/
	static std::string Strip(const std::string &str, char c);

	/*!
		Uppercases a string
		\param str the string to uppercase
		\return the uppercased string
	*/
	static std::string ToUpperCase(const std::string &str);

	/*!
		Lowercases a string
		\param str the string to Lowercase
		\return the lowercased string
	*/
	static std::string ToLowerCase(const std::string &str);

	/*!
	*	Replaces a string with another string, within a string
	*	\param str the string to search
	*	\param Find the string to find
	*	\param Replace the string to use as replacement
	*	\return the filtered string
	*/
	static std::string Replace(const std::string &str, const std::string &Find, const std::string &Replace);

	/*!
	*	Gets the directory part of a directory string
	*	\param Path a slash (/) separated directory path (e.g. /User/Data/Files/Test.txt)
	*	\return the Directory name (e.g. /User/Data/Files/) or an empty string
	*/
	static std::string DirectoryName(const std::string &Path);

	/*!
	*	Gets the FileName part of a directory string
	*	\param Path a slash (/) separated directory path (e.g. /User/Data/Files/Test.txt)
	*	\return the File name (e.g. Test.txt) or an empty string
	*/
	static std::string FileName(const std::string &Path);
};
