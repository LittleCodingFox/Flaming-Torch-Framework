#pragma once

typedef uint32 StringID;

/*!
*	Makes a StringID from a string
*	\param Name the name of the String ID
*/
StringID FLAMING_API MakeStringID(const std::string &Name);
/*!
*	Gets the String from a StringID
*	\param ID the StringID
*/
FLAMING_API const std::string &GetStringIDString(StringID ID);
