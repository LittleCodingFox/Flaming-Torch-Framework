////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2013 Jonathan De Wachter (dewachter.jonathan@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/System/Android/ResourceStream.hpp>
#include <SFML/System/Android/Activity.hpp>
#include <SFML/System/Lock.hpp>


namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
ResourceStream::ResourceStream(const std::string& filename) :
m_file (NULL)
{
    ActivityStates* states = getActivity(NULL);
    Lock(states->mutex);
    m_file = AAssetManager_open(states->activity->assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
}


////////////////////////////////////////////////////////////
ResourceStream::~ResourceStream()
{
	if(m_file)
	    AAsset_close(m_file);
}


////////////////////////////////////////////////////////////
Int64 ResourceStream::read(void *data, Int64 size)
{
    return m_file ? AAsset_read(m_file, data, size) : 0;
}


////////////////////////////////////////////////////////////
Int64 ResourceStream::seek(Int64 position)
{
    return m_file ? AAsset_seek(m_file, position, SEEK_SET) : 0;
}


////////////////////////////////////////////////////////////
Int64 ResourceStream::tell()
{
    return m_file ? getSize() - AAsset_getRemainingLength(m_file) : 0;
}


////////////////////////////////////////////////////////////
Int64 ResourceStream::getSize()
{
    return m_file ? AAsset_getLength(m_file) : 0;
}


} // namespace priv
} // namespace sf
