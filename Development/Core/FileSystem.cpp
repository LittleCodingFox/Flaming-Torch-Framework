#include "FlamingCore.hpp"
#include <FileWatcher/FileWatcher.h>

#if FLPLATFORM_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	define VC_LEAN_AND_MEAN
#	include <windows.h>
#	include <direct.h>
#	include <ShlObj.h>
#	include <Commdlg.h>
#	undef CreateDirectory
#	undef CopyFile
#	define GetCWD _getcwd
#	include <windirent/dirent.h>
#else
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <dirent.h>
#	include <unistd.h>
#	include <cerrno>
#	define GetCWD getcwd
#	define _mkdir(n) mkdir(n, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH)
#	define _rmdir(n) rmdir(n)
#endif

#if FLPLATFORM_ANDROID
#include <SFML/System/Android/Activity.hpp>
#include <SFML/System/Android/ResourceStream.hpp>
#include <android/log.h>
#endif

namespace FlamingTorch
{
#	define TAGMANAGER "PackageFileSystemManager"
#	define TAGPATH "Path"
#	define TAGUTILS "FileSystemUtils"
#	define TAGWATCHER "FileSystemWatcher"
#	define STREAM_COPY_BUFFER_SIZE 1048576 //1MB

	std::string Path::PathSeparator = FLPLATFORM_UNIVERSAL_PATHSPLITSTRING;
	std::string Path::PlatformPathSeparator = FLPLATFORM_PATHSPLITSTRING;

#if !FLPLATFORM_ANDROID
	FW::FileWatcher GlobalFileWatcher;
	FileSystemWatcher FileSystemWatcher::Instance;

	class FileSystemWatcherCallback : public FW::FileWatchListener
	{
	public:
		void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action)
		{
			uint32 ActionID = 0;

			switch(action)
			{
            case FW::Actions::Add:
				ActionID = FileSystemWatcherAction::Added;

				break;

			case FW::Actions::Delete:
				ActionID = FileSystemWatcherAction::Deleted;

				break;

			case FW::Actions::Modified:
				ActionID = FileSystemWatcherAction::Modified;

				break;
			};

			FileSystemWatcher::Instance.OnAction(dir, filename, ActionID);
		};
	};
#endif

	Path::Path()
	{
	};

	Path::Path(const std::string &_Directory, const std::string &_BaseName)
	{
		std::string PathName = Normalize(_Directory + PathSeparator + _BaseName);

		int32 DirectorySeparator = PathName.rfind(Path::PathSeparator);

		if(DirectorySeparator == -1)
		{
			BaseName = PathName;
		}
		else
		{
			Directory = PathName.substr(0, DirectorySeparator + 1);

			if(DirectorySeparator + 1 < (int32)PathName.length())
				BaseName = PathName.substr(DirectorySeparator + 1);
		};
	};

	Path::Path(const std::string &_PathName)
	{
		std::string PathName = Normalize(_PathName);

		int32 DirectorySeparator = PathName.rfind(Path::PathSeparator);

		if(DirectorySeparator == -1)
		{
			BaseName = PathName;
		}
		else
		{
			Directory = PathName.substr(0, DirectorySeparator + 1);

			if(DirectorySeparator + 1 < (int32)PathName.length())
				BaseName = PathName.substr(DirectorySeparator + 1);
		};
	};

	std::string Path::FullPath() const
	{
		return Directory + BaseName;
	};

	std::string Path::Extension() const
	{
		int32 ExtensionIndex = BaseName.rfind('.');

		if(ExtensionIndex == std::string::npos)
			return "";

		return BaseName.substr(ExtensionIndex + 1);
	};

	Path Path::ChangeExtension(const std::string &NewExtension) const
	{
		int32 Index = BaseName.rfind('.');

		if(Index != std::string::npos)
		{
			return Path(Directory, BaseName.substr(0, Index + 1) + NewExtension);
		};

		return *this;
	};

	Path Path::StripExtension() const
	{
		int32 Index = BaseName.rfind('.');

		if(Index != std::string::npos)
		{
			return Path(Directory, BaseName.substr(0, Index));
		};

		return *this;
	};

	std::string Path::Normalize(const std::string &PathName)
	{
		return StringUtils::Replace(StringUtils::Replace(StringUtils::Replace(PathName, "\\", Path::PathSeparator), "/", Path::PathSeparator),
			Path::PathSeparator + Path::PathSeparator, Path::PathSeparator);
	};

	bool FileSystemUtils::RemoveFile(const std::string Name)
	{
		return unlink(Name.c_str()) != -1;
	};

	bool FileSystemUtils::CopyFile(const std::string &From, const std::string &To)
	{
		FileStream In, Out;

		return In.Open(From, StreamFlags::Read) && Out.Open(To, StreamFlags::Write) && In.CopyTo(&Out);
	};

	std::string FileSystemUtils::OpenFileDialog(const std::string &Title, const std::string &Extension, const std::string &Filter)
	{
#if FLPLATFORM_WINDOWS
		static std::vector<char> FileNameBuffer(MAX_PATH);

		memset(&FileNameBuffer[0], 0, FileNameBuffer.size());

		OPENFILENAMEA Config;

		memset(&Config, 0, sizeof(Config));

		Config.lStructSize = sizeof(Config);
		Config.lpstrDefExt = (char *)Extension.c_str();
		Config.nMaxFile = MAX_PATH;
		Config.lpstrFilter = (char *)Filter.c_str();
		Config.lpstrTitle = (char *)Title.c_str();
		Config.lpstrFile = &FileNameBuffer[0];
		Config.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

#if USE_GRAPHICS
		Renderer *TheRenderer = RendererManager::Instance.ActiveRenderer();

		if(TheRenderer)
		{
			Config.hwndOwner = (HWND)TheRenderer->WindowHandle();
		};
#endif

		if(Config.hwndOwner == NULL)
		{
			Config.hwndOwner = GetDesktopWindow();
		};

		if(GetOpenFileNameA(&Config))
		{
			return Config.lpstrFile;
		}
		else
		{
			Log::Instance.LogDebug(TAGUTILS, "Failed to OpenFileDialog: 0x%04x", CommDlgExtendedError());
		};
#endif

		return std::string();
	};

	std::string FileSystemUtils::SaveFileDialog(const std::string &Title, const std::string &Extension, const std::string &Filter)
	{
#if FLPLATFORM_WINDOWS
		static std::vector<char> FileNameBuffer(MAX_PATH);

		memset(&FileNameBuffer[0], 0, FileNameBuffer.size());

		OPENFILENAMEA Config;

		memset(&Config, 0, sizeof(Config));

		Config.lStructSize = sizeof(Config);
		Config.lpstrDefExt = (char *)Extension.c_str();
		Config.nMaxFile = MAX_PATH;
		Config.lpstrFilter = (char *)Filter.c_str();
		Config.lpstrTitle = (char *)Title.c_str();
		Config.lpstrFile = &FileNameBuffer[0];
		Config.Flags = OFN_OVERWRITEPROMPT;

#if USE_GRAPHICS
		Renderer *TheRenderer = RendererManager::Instance.ActiveRenderer();

		if(TheRenderer)
		{
			Config.hwndOwner = (HWND)TheRenderer->WindowHandle();
		};
#endif

		if(Config.hwndOwner == NULL)
		{
			Config.hwndOwner = GetDesktopWindow();
		};

		if(GetSaveFileNameA(&Config))
		{
			return Config.lpstrFile;
		}
		else
		{
			Log::Instance.LogDebug("FileSystemUtils", "Failed to SaveFileDialog: 0x%04x", CommDlgExtendedError());
		};
#endif

		return std::string();
	};

	std::vector<std::string> FileSystemUtils::ScanDirectory(const std::string &Directory,
		const std::string &Extension, bool Recursive)
	{
		std::vector<std::string> Files;

		Log::Instance.LogDebug(TAGUTILS, "Scanning directory '%s' (Recursive: %s)", Directory.c_str(), Recursive ? "YES" : "NO");

		DIR *Root = opendir (Directory.c_str());

		if(Root == NULL)
		{
			Log::Instance.LogDebug(TAGUTILS, "Unable to opendir directory '%s'", Directory.c_str());

			return Files;
		};

		dirent *Entry = readdir(Root);

		if(Entry == NULL)
		{
			Log::Instance.LogErr(TAGUTILS, "Error while reading directory: %d", errno);

			closedir(Root);

			return Files;
		};

		while(Entry != NULL)
		{
			std::string FileName(Entry->d_name);

			if(FileName != "." && FileName != "..")
			{
				if(Entry->d_type == DT_DIR)
				{
					if(Recursive)
					{
						std::vector<std::string> t(ScanDirectory(Directory + std::string("/") +
							FileName, Extension, Recursive));

						Files.insert(Files.end(), t.begin(), t.end());
					};
				}
				else if(Entry->d_type == DT_REG)
				{
					if(Extension.length() == 0 || Extension == "*" || FileName.rfind(Extension) == FileName.length() - Extension.length())
						Files.push_back(Directory + std::string("/") + FileName);
				};
			};

			Entry = readdir(Root);
		};

		closedir(Root);

		return Files;
	};

	bool FileSystemUtils::DirectoryExists(const std::string &Directory)
	{
		DIR *Root = opendir(Path(Directory).FullPath().c_str());

		if(Root)
		{
			closedir(Root);

			return true;
		};

		return false;
	};

    bool FileSystemUtils::CopyDirectory(const std::string &_From, const std::string &_To, bool Recursive)
    {
		std::string From = Path(_From).FullPath(), To = Path(_To).FullPath();

		if(From == To)
            return false;

		FileSystemUtils::CreateDirectory(To);
        
		std::vector<std::string> Directories = FileSystemUtils::GetAllDirectories(From);

		for(uint32 i = 0; i < Directories.size(); i++)
		{
			std::string TargetDirectory = To + "/" + Directories[i].substr(From.length());

			FileSystemUtils::CreateDirectory(TargetDirectory);
		};
		
		std::vector<std::string> Files = ScanDirectory(From, "*", Recursive);
        
        FileStream In, Out;
        
        for(uint32 i = 0; i < Files.size(); i++)
        {
            if(!In.Open(Files[i], StreamFlags::Read))
            {
                Log::Instance.LogErr(TAGUTILS, "Unable to copy a file '%s' while copying a directory", Files[i].c_str());
                
                continue;
            };
            
            std::string OutName = Files[i].substr(Files[i].find(From) + From.length() + 1);
            std::string OutFileName = To + "/" + OutName;
            
            if(!Out.Open(OutFileName, StreamFlags::Write))
            {
				std::string DirectoryName = Path(OutFileName).Directory;
                
                std::vector<std::string> MissingDirectories = StringUtils::Split(DirectoryName.substr(To.length() + 1), '/');
                
                DirectoryName = To;
                
                for(uint32 j = 0; j < MissingDirectories.size(); j++)
                {
                    DirectoryName += "/" + MissingDirectories[j];
                    
                    CreateDirectory(DirectoryName);
                };
                
                if(!Out.Open(OutFileName, StreamFlags::Write))
                {
                    Log::Instance.LogErr(TAGUTILS, "Unable to copy a file '%s' while copying a directory: Cannot open '%s' for writing",
                                         Files[i].c_str(), OutFileName.c_str());
                    
                    continue;
                };
            };
            
            if(!In.CopyTo(&Out))
            {
                FileSystemUtils::RemoveFile(OutFileName);
                
                Log::Instance.LogErr(TAGUTILS, "Unable to copy a file '%s' while copying a directory: Cannot copy to '%s'",
                                     Files[i].c_str(), OutFileName.c_str());
                
                continue;
            };
        };
        
        return true;
    };

	bool FileSystemUtils::CreateDirectory(const std::string &_Directory)
	{
		std::string Directory = Path(_Directory).FullPath();

        bool result = _mkdir(Directory.c_str()) == 0;
        
        if(!result)
		{
			int32 Errno = errno;

			if(Errno != EEXIST)
			{
				Log::Instance.LogWarn(TAGUTILS, "Failed to create directory '%s': %d", Directory.c_str(), Errno);
			};
		};
        
        return result;
	};

	bool FileSystemUtils::CreateDirectoryRecursive(const std::string &Directory)
	{
		std::vector<std::string> DirectoryPieces = StringUtils::Split(Path(Directory).FullPath(), '/');

		std::stringstream str;

		for(uint32 i = 0; i < DirectoryPieces.size(); i++)
		{
			str << (i > 0 ? "/" : "") << DirectoryPieces[i];

			if(_mkdir(str.str().c_str()) != 0 && errno != EEXIST && (i != 0 || str.str().find(':') != std::string::npos))
				return false;
		};

		return true;
	};
    
    bool FileSystemUtils::DeleteDirectory(const std::string &Directory)
    {
		DIR *Root = opendir (Path(Directory).FullPath().c_str());
        
        if(Root == NULL)
        {
            Log::Instance.LogDebug(TAGUTILS, "Failed to remove directory %s", Directory.c_str());
            
            return false;
        };
        
        dirent *Entry = readdir(Root);
        
        while(Entry != NULL)
        {
            std::string FileName(Entry->d_name);
            
            if(FileName != "." && FileName != "..")
            {
                if(Entry->d_type == DT_DIR)
                {
					if(!DeleteDirectory(Path(Directory + "/" + FileName).FullPath()))
                    {
                        Log::Instance.LogDebug(TAGUTILS, "Failed to remove subdirectory %s/%s", Directory.c_str(), FileName.c_str());

						closedir(Root);

                        return false;
                    };
                }
                else if(Entry->d_type == DT_REG)
                {
					if(!FileSystemUtils::RemoveFile(Path(Directory + "/" + FileName).FullPath()))
                    {
                        Log::Instance.LogDebug(TAGUTILS, "Failed to remove file %s/%s", Directory.c_str(), FileName.c_str());

						closedir(Root);
                        
                        return false;
                    };
                };
            };
            
            Entry = readdir(Root);
        };
        
        closedir(Root);
        
        return _rmdir (Directory.c_str()) == 0;
    };
    
    std::vector<std::string> FileSystemUtils::GetAllDirectories(const std::string &Directory)
    {
        std::vector<std::string> Out;
        
        DIR *Root = opendir (Directory.c_str());
        
        if(Root == NULL)
        {
            Log::Instance.LogErr(TAGUTILS, "Failed to open Directory '%s' for reading!", Directory.c_str());
            
            return Out;
        };
        
        dirent *Entry = readdir(Root);
        
        while(Entry != NULL)
        {
            std::string FileName(Entry->d_name);
            
            if(FileName[0] != '.')
            {
                if(Entry->d_type == DT_DIR)
                {
                    Out.push_back(Directory + "/" + FileName);
                };
            };
            
            Entry = readdir(Root);
        };
        
        closedir(Root);
        
        return Out;
    };

#if FLPLATFORM_MACOSX
	const char *OSXResourcesDirectory();
	const char *OSXPreferredStorageDirectory();
#endif
	
	std::string ActualDirectory, ActualResourcesDirectory, ActualStorageDirectory;
	
	const std::string &FileSystemUtils::ActiveDirectory()
	{
		if(ActualDirectory.length() == 0)
		{
			ActualDirectory.resize(1024);

			if(NULL == GetCWD(&ActualDirectory[0], 1024))
			{
				ActualDirectory.clear();
			}
			else
			{
				ActualDirectory.resize(strlen(ActualDirectory.c_str()));
				ActualDirectory += "/";
			};

			ActualDirectory = Path(ActualDirectory).FullPath();
		};
		
		return ActualDirectory;
	};

	//Platform-specific resources directory (used mainly on OSX)
	const std::string &FileSystemUtils::ResourcesDirectory()
	{
		if(ActualResourcesDirectory.length() == 0)
#if FLPLATFORM_MACOSX
		{
			ActualResourcesDirectory = OSXResourcesDirectory();
			ActualResourcesDirectory.resize(strlen(ActualResourcesDirectory.c_str()));
		};
#endif

		if(ActualResourcesDirectory.length() == 0)
		{
			ActualResourcesDirectory.resize(1024);

			if(NULL == GetCWD(&ActualResourcesDirectory[0], 1024))
			{
				ActualResourcesDirectory.clear();
			}
			else
			{
				ActualResourcesDirectory.resize(strlen(ActualResourcesDirectory.c_str()));
				ActualResourcesDirectory += "/";
			};

			ActualResourcesDirectory = Path(ActualResourcesDirectory).FullPath();
		};
		
		return ActualResourcesDirectory;
	};

	const std::string &FileSystemUtils::PreferredStorageDirectory()
	{
		if(ActualStorageDirectory.length() == 0)
		{
#if SANDBOX_BUILD
#	if FLPLATFORM_ANDROID
			ActualStorageDirectory = sf::priv::getActivity(NULL, false)->activity->externalDataPath;
			FileSystemUtils::CreateDirectoryRecursive(ActualStorageDirectory.c_str());
#	else
			ActualStorageDirectory = ResourcesDirectory();
#	endif
#else
#	if FLPLATFORM_WINDOWS
			ActualStorageDirectory.resize(1024);

			if(FAILED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, &ActualStorageDirectory[0])))
			{
				ActualStorageDirectory.clear();
			}
			else
			{
				ActualStorageDirectory.resize(strlen(ActualStorageDirectory.c_str()));
			};
            
			ActualStorageDirectory += std::string("/My Games");

#	elif FLPLATFORM_MACOSX
			ActualStorageDirectory = OSXPreferredStorageDirectory();
			
			ActualStorageDirectory += "/Games";
#	elif FLPLATFORM_ANDROID
			ActualStorageDirectory = sf::priv::getActivity(NULL, false)->activity->externalDataPath;
#	endif
			FileSystemUtils::CreateDirectoryRecursive(ActualStorageDirectory.c_str());
#endif

			if(Log::Instance.FolderName.length())
			{
				ActualStorageDirectory += "/" + Log::Instance.FolderName;
				ActualStorageDirectory = Path(ActualStorageDirectory).FullPath();

				FileSystemUtils::CreateDirectory(ActualStorageDirectory.c_str());
			}
			else if(GameInterface::Instance.Get())
			{
				ActualStorageDirectory += "/" + GameInterface::Instance->GameName() + "_files";
				ActualStorageDirectory = Path(ActualStorageDirectory).FullPath();

				FileSystemUtils::CreateDirectory(ActualStorageDirectory.c_str());
			};
		};

		return ActualStorageDirectory;
	};

	std::string Stream::AsString()
	{
		if(Length() == 0)
			return std::string();

		std::string str;
		str.resize((uint32)Length());

		AsBuffer(&str[0], (uint32)Length());

		//Remove all extra 0's
		for(int32 i = str.length() - 1; i >= 0; i--)
		{
			if(str[i] != '\0')
			{
				str.resize(i + 1);

				break;
			};
		};

		return str;
	};

	MemoryStream::MemoryStream() : _Position(0) {};

	uint64 MemoryStream::Length() const
	{
		return Data.size();
	};

	uint64 MemoryStream::Position() const
	{
		return _Position;
	};

	bool MemoryStream::Seek(uint64 Position)
	{
		if(Position >= Data.size())
			return false;

		_Position = Position;

		return true;
	};

	bool MemoryStream::Write(const void *Data, uint32 ElementSize, uint32 Length)
	{
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(ElementSize, "Invalid Element Length!");
		FLASSERT(Length, "Invalid Length!");

		if(!Data || ElementSize == 0  || Length == 0)
			return false;

		uint32 ActualLength = ElementSize * Length;

		if(_Position + ActualLength > this->Data.size())
		{
			this->Data.resize((uint32)_Position + ActualLength);
		};
		
		memcpy(&this->Data[(uint32)_Position], Data, ActualLength);

		if(Processor)
			Processor->Encode(&this->Data[(uint32)_Position], ActualLength);

		_Position += ActualLength;

		return true;
	};

	bool MemoryStream::Read(void *Data, uint32 ElementSize, uint32 Length)
	{
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(ElementSize, "Invalid Element Length!");
		FLASSERT(Length, "Invalid Length!");

		if(!Data || ElementSize == 0  || Length == 0)
			return false;

		uint32 ActualLength = ElementSize * Length;

		if(_Position + ActualLength > this->Data.size())
			return false;

		memcpy(Data, &this->Data[(uint32)_Position], ActualLength);

		if(Processor)
			Processor->Decode(Data, ActualLength);

		_Position += ActualLength;

		return true;
	};

	void MemoryStream::AsBuffer(void *Data, uint32 Length)
	{
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(Length, "Invalid Length!");

		if(!Data || Length == 0)
			return;

		uint32 Count = (this->Data.size() - (uint32)_Position < Length ? this->Data.size() - (uint32)_Position : Length);

		memcpy(Data, &this->Data[(uint32)_Position], Count);

		if(Processor)
			Processor->Decode(Data, Count);

		_Position += Count;
	};

	const void *MemoryStream::Get() const
	{
		FLASSERT(!Processor, "Unable to decode the memory stream when using Get()!");

		return Data.size() ? &Data[0] : NULL;
	};

	void MemoryStream::Clear()
	{
		_Position = 0;
		Data.clear();
	};

	ConstantMemoryStream::ConstantMemoryStream(const void *Ptr, uint32 __Length) : Data((uint8 *)Ptr), _Length(__Length), _Position(0) {};

	uint64 ConstantMemoryStream::Length() const
	{
		return _Length;
	};

	uint64 ConstantMemoryStream::Position() const
	{
		return _Position;
	};

	bool ConstantMemoryStream::Seek(uint64 Position)
	{
		if(Position > _Length)
			return false;

		_Position = Position;

		return true;
	};

	bool ConstantMemoryStream::Write(const void *Data, uint32 ElementSize, uint32 Length)
	{
		return false;
	};

	bool ConstantMemoryStream::Read(void *Data, uint32 ElementSize, uint32 Length)
	{
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(ElementSize, "Invalid Element Length!");
		FLASSERT(Length, "Invalid Length!");

		if(!Data || ElementSize == 0  || Length == 0)
			return false;

		uint32 ActualLength = ElementSize * Length;

		if(_Position + ActualLength > _Length)
			return false;

		memcpy(Data, this->Data + _Position, ActualLength);

		if(Processor)
			Processor->Decode(Data, ActualLength);

		_Position += ActualLength;

		return true;
	};

	void ConstantMemoryStream::AsBuffer(void *Data, uint32 Length)
	{
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(Length, "Invalid Length!");

		if(!Data || Length == 0)
			return;

		uint32 Count = (_Length - (uint32)_Position < Length ? _Length - (uint32)_Position : Length);

		memcpy(Data, &this->Data[(uint32)_Position], Count);

		if(Processor)
			Processor->Decode(Data, Count);

		_Position += Count;
	};

	const void *ConstantMemoryStream::Get() const
	{
		FLASSERT(!Processor, "Unable to decode the constant memory stream when using Get()!");

		return Data;
	};

	FileStream::FileStream() : Handle(NULL), _Length(0), _Position(0), IsBasic(false) {};

	FileStream::~FileStream()
	{
		Close(); 
	};

	bool FileStream::Open(const std::string &_name, uint8 Flags)
	{
		FLASSERT(_name.length(), "Invalid Filename!");

		std::string name = Path(_name).FullPath();

		Close();

		char Mode[4] = {
			'\0', '\0', '\0', '\0'
		};

		if(Flags & StreamFlags::Read)
		{
			Mode[0] = 'r';
		}
		else if(Flags & StreamFlags::Write)
		{
			Mode[0] = 'w';
		}
		else
		{
			FLASSERT(0, "Invalid Stream Flags!");
		};

		if(Flags & StreamFlags::Text)
		{
			Mode[1] = 't';
		}
		else
		{
			Mode[1] = 'b';
		};

		if(!Handle)
		{
#ifdef FLPLATFORM_WINDOWS
			fopen_s((FILE**) &Handle, name.c_str(), Mode);
#else
			Handle = fopen(name.c_str(), Mode);
#endif
		};

		_Length = 0;
		_Position = 0;

		if(!Handle)
		{
#if FLPLATFORM_ANDROID
			if(Flags & StreamFlags::Write)
				return false;

			if(name[0] == '/')
			{
				name = name.substr(1);
			};

			//__android_log_print(ANDROID_LOG_DEBUG, "Core", "Loading asset '%s'", name.c_str());
			sf::priv::ResourceStream *resource = new sf::priv::ResourceStream(name);

			if(!resource->getSize())
			{
				delete resource;
				//GIF Hack Workaround
				//__android_log_print(ANDROID_LOG_DEBUG, "Core", "Loading asset '%s.gif' (GIF Workaround)", name.c_str());
				resource = new sf::priv::ResourceStream(name + ".gif");

				if(!resource->getSize())
				{
					__android_log_print(ANDROID_LOG_DEBUG, "Core", "Asset '%s' not found", name.c_str());

					delete resource;

					return false;
				};
			};

			_Length = resource->getSize();
			Handle = resource;
			IsBasic = false;

			return true;
#else
			return false;
#endif
		};

		if(Flags & StreamFlags::Read)
		{
			fseek((FILE*)Handle, 0, SEEK_END);

#ifdef FLPLATFORM_WINDOWS
			_Length = _ftelli64((FILE*)Handle);
#elif FLPLATFORM_MACOSX || ANDROID
			_Length = ftell((FILE*)Handle);
#else
			_Length = ftello64((FILE*)Handle);
#endif
			
			fseek((FILE*)Handle, 0, SEEK_SET);
		};

		IsBasic = true;

		return true;
	};

	void FileStream::Close()
	{
		if(Handle)
		{
#if FLPLATFORM_ANDROID
			if(!IsBasic)
			{
				delete (sf::priv::ResourceStream *)Handle;
			}
			else
			{
				fclose((FILE*)Handle);
			};
#else
			fclose((FILE*)Handle);
#endif

			Handle = NULL;
		};

		_Length = 0;
		_Position = 0;
		IsBasic = true;
	};

	uint64 FileStream::Length() const
	{
		return _Length;
	};

	uint64 FileStream::Position() const
	{
		return _Position;
	};

	bool FileStream::Seek(uint64 Position)
	{
		FLASSERT(Handle, "Invalid File Handle!");

		if(Position == this->_Position)
			return true;

		if(_Length != 0)
			FLASSERT(Position <= _Length, "Invalid Position!");

#ifdef FLPLATFORM_WINDOWS
		int64 Result = _fseeki64((FILE *)Handle, Position, SEEK_SET);
#elif FLPLATFORM_MACOSX
		int64 Result = fseek((FILE *)Handle, Position, SEEK_SET);
#elif FLPLATFORM_ANDROID
		int64 Result = 0;

		if(!IsBasic)
		{
			Result = ((sf::priv::ResourceStream *)Handle)->seek(Position);
		}
		else
		{
			Result = fseek((FILE *)Handle, Position, SEEK_SET);
		};
#else
		int64 Result = fseeko64((FILE *)Handle, Position, SEEK_SET);
#endif

		if(Result == -1)
			return false;

		this->_Position = Position;

		return true;
	};

	bool FileStream::Write(const void *Data, uint32 ElementSize, uint32 Length)
	{
		FLASSERT(Handle, "Invalid File Handle!");
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(ElementSize, "Invalid Element Length!");
		FLASSERT(Length, "Invalid Length!");

		if(!Handle || !Data || ElementSize == 0 || Length == 0)
			return false;

#if FLPLATFORM_ANDROID
		if(!IsBasic)
			return false;
#endif

		if(Processor)
		{
			static std::vector<uint8> Buffer;
			uint32 ActualLength = ElementSize * Length;
			Buffer.resize(ActualLength);

			memcpy(&Buffer[0], Data, ActualLength);

			Processor->Encode(&Buffer[0], ActualLength);

			if(fwrite(&Buffer[0], ElementSize, Length, (FILE*)Handle) != Length)
				return false;
		}
		else
		{
			if(fwrite(Data, ElementSize, Length, (FILE*)Handle) != Length)
				return false;
		}

		fflush((FILE*)Handle);

		_Position += ElementSize * Length;

		return true;
	};

	bool FileStream::Read(void *Data, uint32 ElementSize, uint32 Length)
	{
		FLASSERT(Handle, "Invalid File Handle!");
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(ElementSize, "Invalid Element Length!");
		FLASSERT(Length, "Invalid Length!");

		if(!Handle || !Data || ElementSize == 0 || Length == 0)
			return false;

		if(_Position + ElementSize * Length > this->_Length)
			return false;

#if FLPLATFORM_ANDROID
		if(!IsBasic)
		{
			if(((sf::priv::ResourceStream *)Handle)->read(Data, ElementSize * Length) != ElementSize * Length)
				return false;
		}
		else
		{
			if(fread(Data, ElementSize, Length, (FILE*)Handle) != Length)
				return false;
		};
#else
		if(fread(Data, ElementSize, Length, (FILE*)Handle) != Length)
			return false;
#endif

		if(Processor)
			Processor->Decode(Data, ElementSize * Length);

		_Position += ElementSize * Length;

		return true;
	};

	void FileStream::AsBuffer(void *Data, uint32 Length)
	{
		FLASSERT(Data, "Invalid Data!");
		FLASSERT(Length, "Invalid Length!");

		if(!Data || Length == 0)
			return;

		Read(Data, sizeof(uint8), Length);
	};

	bool Stream::CopyTo(Stream *Out)
	{
		FLASSERT(Out, "Invalid Stream!");

		if(!Out)
			return false;

		std::vector<uint8> Buffer(STREAM_COPY_BUFFER_SIZE);

		for(uint64 Offset = Position(); Offset < Length();)
		{
			if(Length() - Position() < STREAM_COPY_BUFFER_SIZE)
				Buffer.resize((uint32)(Length() - Position()));

			SFLASSERT(Read2<uint8>(&Buffer[0], Buffer.size()));

			if(Processor)
				Processor->Encode(&Buffer[0], Buffer.size());

			SFLASSERT(Out->Write2<uint8>(&Buffer[0], Buffer.size()));

			Offset += Buffer.size();
		};

		return true;
	};

	bool Stream::ReadFromStream(Stream *Target, uint32 Length)
	{
		FLASSERT(Target, "Invalid Stream!");

		if(!Target)
			return false;

		if(Target->Position() + Length > Target->Length())
			return false;

		std::vector<uint8> Buffer(Length);

		SFLASSERT(Target->Read2<uint8>(&Buffer[0], Length));
		SFLASSERT(Write2<uint8>(&Buffer[0], Length));

		return true;
	};

	bool Stream::WriteToStream(Stream *Target, uint32 _Length)
	{
		FLASSERT(Target, "Invalid Stream!");

		if(!Target)
			return false;

		if(Position() + _Length > Length())
			return false;

		std::vector<uint8> Buffer(_Length);

		SFLASSERT(Read2<uint8>(&Buffer[0], _Length));

		//Don't encode again since the Target might have its own encoder/decoder

		SFLASSERT(Target->Write2<uint8>(&Buffer[0], _Length));

		return true;
	};

	bool PackageFileSystemManager::Package::Serialize(Stream *Out)
	{
		uint64 CurrentOffset = 0;
		uint64 StartOffset = 0;

		sf::Lock Lock(FileAccessMutex);

		std::vector<FileEntry *> EntryList;

		for(EntryMap::iterator it = Entries.begin(); it != Entries.end(); it++)
		{
			for(FileMap::iterator fit = it->second.begin(); fit != it->second.end(); fit++)
			{
				if(fit->second.Get() == NULL || fit->second->Input.Get() == NULL)
					continue;

				EntryList.push_back(fit->second.Get());
			};
		};

		StartOffset += sizeof(VersionType) + //Version
			sizeof(uint32) + //Length
			sizeof(StringID) + //CRC
			sizeof(uint32); //Count

		for(uint32 i = 0; i < EntryList.size(); i++)
		{
			StartOffset += sizeof(StringID) + //DirectoryID
				sizeof(uint8) + //NameLength
				sizeof(uint8) * EntryList[i]->Name.length() + //Entry Name
				sizeof(uint8) + //DirectoryLength
				sizeof(uint8) * EntryList[i]->DirectoryName.length() + //Directory Name
				sizeof(uint64[2]); //Offset + Length
		};

		MemoryStream HeaderStream;

		uint32 Count = EntryList.size();
		SFLASSERT(HeaderStream.Write2<uint32>(&Count));

		for(uint32 i = 0; i < EntryList.size(); i++)
		{
			EntryList[i]->Offset = StartOffset;
			EntryList[i]->Length = EntryList[i]->Input->Length();
			EntryList[i]->Input->Seek(0);

			SFLASSERT(HeaderStream.Write2<StringID>(&EntryList[i]->DirectoryID));

			uint8 DirectoryNameLength = EntryList[i]->DirectoryName.length();
			SFLASSERT(HeaderStream.Write2<uint8>(&DirectoryNameLength));

			if(DirectoryNameLength)
				SFLASSERT(HeaderStream.Write2<char>(EntryList[i]->DirectoryName.c_str(), DirectoryNameLength));

			uint8 NameLength = EntryList[i]->Name.length();
			SFLASSERT(HeaderStream.Write2<uint8>(&NameLength));

			if(NameLength)
				SFLASSERT(HeaderStream.Write2<char>(EntryList[i]->Name.c_str(), NameLength));

			SFLASSERT(HeaderStream.Write2<uint64>(&EntryList[i]->Offset));
			SFLASSERT(HeaderStream.Write2<uint64>(&EntryList[i]->Length));

			StartOffset += EntryList[i]->Length;
		};

		//Start by writing the header
		VersionType Version = CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR);
		uint32 Length = (uint32)HeaderStream.Length();
		StringID CRC = CRC32::Instance.CRC((uint8 *)HeaderStream.Get(), (uint32)HeaderStream.Length());

		SFLASSERT(Out->Write2<VersionType>(&Version));
		SFLASSERT(Out->Write2<uint32>(&Length));
		SFLASSERT(Out->Write2<StringID>(&CRC));
		SFLASSERT(Out->Write2<uint8>((uint8 *)HeaderStream.Get(), (uint32)HeaderStream.Length()));

		if(!EntryList.size())
		{
			Log::Instance.LogWarn(TAGMANAGER, "Serializing a package with no Entries!");

			return true;
		};
		
		for(uint32 i = 0; i < EntryList.size(); i++)
		{
			if(!EntryList[i]->Input->CopyTo(Out))
			{
				Log::Instance.LogErr(TAGMANAGER, "While serializing: Unable to serialize Entry '%d/%s'!", EntryList[i]->DirectoryID,
					EntryList[i]->Name.c_str());

				return false;
			};
		};

		return true;
	};

	bool PackageFileSystemManager::Package::DeSerialize(Stream *In)
	{
		sf::Lock Lock(FileAccessMutex);

		OriginalOffset = In->Position();

		Entries.clear();
		HasBeenTampered = false;

		VersionType Version = 0, TargetVersion = CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR);

		//All packages are now at least encrypted with the default XOR key, so add a processor if missing
		if(In->GetProcessor().Get() == NULL)
			In->SetProcessor(SuperSmartPointer<StreamProcessor>(new XORStreamProcessor()));

		SFLASSERT(In->Read2<VersionType>(&Version));

		if(Version != TargetVersion)
		{
			Log::Instance.LogErr(TAGMANAGER, "While deserializing: Invalid Version ID '%lld' (should be '%lld')",
				Version, TargetVersion);

			return false;
		};

		uint32 HeaderLength = 0;
		StringID CRC = 0;
		MemoryStream HeaderStream;

		SFLASSERT(In->Read2<uint32>(&HeaderLength));
		SFLASSERT(In->Read2<StringID>(&CRC));
		SFLASSERT(In->WriteToStream(&HeaderStream, HeaderLength));
		HeaderStream.Seek(0);

		StringID ActualCRC = CRC32::Instance.CRC((uint8 *)HeaderStream.Get(), (uint32)HeaderStream.Length());

		if(ActualCRC != CRC)
		{
			Log::Instance.LogErr(TAGMANAGER, "While deserializing: Invalid CRC '%08x' (should be '%08x')",
				ActualCRC, CRC);

			return false;
		};

		uint32 Count = 0;

		SFLASSERT(HeaderStream.Read2<uint32>(&Count));

		StringID DirectoryID, NameID;
		std::string NameStr, DirectoryNameStr;
		uint64 Offset, Length;

		for(uint32 i = 0; i < Count; i++)
		{
			SFLASSERT(HeaderStream.Read2<StringID>(&DirectoryID));

			uint8 DirectoryNameLength = 0;
			SFLASSERT(HeaderStream.Read2<uint8>(&DirectoryNameLength));

			DirectoryNameStr.resize(DirectoryNameLength);

			if(DirectoryNameLength)
				SFLASSERT(HeaderStream.Read2<char>(&DirectoryNameStr[0], DirectoryNameLength));

			uint8 NameLength = 0;
			SFLASSERT(HeaderStream.Read2<uint8>(&NameLength));

			NameStr.resize(NameLength);

			if(NameLength)
				SFLASSERT(HeaderStream.Read2<char>(&NameStr[0], NameLength));

			NameID = MakeStringID(NameStr);

			SFLASSERT(HeaderStream.Read2<uint64>(&Offset));
			SFLASSERT(HeaderStream.Read2<uint64>(&Length));

			SuperSmartPointer<FileEntry> Entry(new FileEntry());

			Entry->DirectoryID = DirectoryID;
			Entry->Name = NameStr;
			Entry->DirectoryName = DirectoryNameStr;
			Entry->Offset = Offset;
			Entry->Length = Length;

			Entries[DirectoryID][NameID] = Entry;
		};

		return true;
	};

	bool PackageFileSystemManager::Package::AddFile(const std::string &Directory, const std::string &Name, SuperSmartPointer<FileStream> In)
	{
		sf::Lock Lock(FileAccessMutex);

		StringID DirectoryID = MakeStringID(Directory);

		EntryMap::iterator eit = Entries.find(DirectoryID);
		StringID NameID = MakeStringID(Name.c_str());

		if(eit == Entries.end() || eit->second.find(NameID) == eit->second.end())
		{
			SuperSmartPointer<FileEntry> &Entry = Entries[DirectoryID][NameID];

			Entry.Dispose();
			Entry.Reset(new FileEntry());

			Entry->DirectoryID = DirectoryID;
			Entry->DirectoryName = Directory;
			Entry->Name = Name;
			Entry->Offset = 0;
			Entry->Length = In->Length();
			Entry->Input = In;
			Entries[DirectoryID][NameID] = Entry;

			HasBeenTampered = true;

			return true;
		};

		return false;
	};

	bool PackageFileSystemManager::Package::RemoveFile(const std::string &Directory, const std::string &Name)
	{
		sf::Lock Lock(FileAccessMutex);

		StringID DirectoryID = MakeStringID(Directory);

		EntryMap::iterator eit = Entries.find(DirectoryID);
		StringID NameID = MakeStringID(Name.c_str());

		if(eit == Entries.end())
			return false;

		HasBeenTampered = true;

		eit->second.erase(NameID);

		return true;
	};

	PackageFileSystemManager::Package::~Package()
	{
		PackageFileSystemManager::Instance.ClearPackageData(this);

		PackageStream.Dispose();
		Entries.clear();
	};

	bool PackageFileSystemManager::Package::FromStream(SuperSmartPointer<Stream> Stream)
	{
		PackageStream.Dispose();
		PackageStream = Stream;

		return DeSerialize(Stream.Get());
	};

	uint64 PackageFileSystemManager::PackageStream::Length() const
	{
		return LengthValue;
	};

	uint64 PackageFileSystemManager::PackageStream::Position() const
	{
		return PositionValue;
	};

	bool PackageFileSystemManager::PackageStream::Seek(uint64 Position)
	{
		FLASSERT(Source, "Invalid Source!");

		if(!Source)
			return false;

		if(Position > LengthValue)
			return false;

		if(Source->Seek(StartOffset + Position))
		{
			PositionValue = Position;

			return true;
		};

		return false;
	};

	//Package files can't write to
	bool PackageFileSystemManager::PackageStream::Write(const void *Data, uint32 ElementSize, uint32 Length)
	{
		return false;
	};

	bool PackageFileSystemManager::PackageStream::Read(void *Data, uint32 ElementSize, uint32 Length)
	{
		FLASSERT(Source, "Invalid Source!");

		if(!Source)
			return false;

		if(ElementSize * Length + PositionValue > LengthValue)
			return false;

		if(Source->Position() != StartOffset + PositionValue && !Source->Seek(StartOffset + PositionValue))
			return false;

		if(Source->Read(Data, ElementSize, Length))
		{
			PositionValue += ElementSize * Length;

			return true;
		};

		return false;
	};

	void PackageFileSystemManager::PackageStream::AsBuffer(void *Data, uint32 Length)
	{
		FLASSERT(Source, "Invalid Source!");

		if(!Source)
			return;

		//We can't do AsBuffer by default, so simply read the data in.

		if(PositionValue + Length > LengthValue)
			return;

		bool Result = Read(Data, sizeof(uint8), Length);

		if(!Result)
		{
			Log::Instance.LogErr(TAGMANAGER, "Unable to read '%d' bytes!", Length);
		};
	};

	PackageFileSystemManager PackageFileSystemManager::Instance;

	void PackageFileSystemManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAGMANAGER, "Initializing Package Filesystem...");
	};

	void PackageFileSystemManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo(TAGMANAGER, "Terminating Package Filesystem...");

		for(PackageMap::iterator it = Packages.begin(); it != Packages.end(); it++)
		{
			it->second.Dispose();
		};
	};

	void PackageFileSystemManager::Update(uint32 Priority)
	{
		PROFILE("PackageFileSystemManager::Update", StatTypes::Game);

		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();
	};

	SuperSmartPointer<PackageFileSystemManager::Package> PackageFileSystemManager::NewPackage()
	{
		FLASSERT(WasStarted, "PackageFileSystem Subsystem not started!");

		if(!WasStarted)
		{
			Log::Instance.LogErr(TAGMANAGER, "While calling NewPackage: Subsystem was not inited!");

			return SuperSmartPointer<Package>();
		};

		return SuperSmartPointer<Package>(new Package());
	};

	bool PackageFileSystemManager::AddPackage(StringID ID, SuperSmartPointer<Stream> PackageStream)
	{
		FLASSERT(WasStarted, "PackageFileSystem Subsystem not started!");

		if(Packages.find(ID) != Packages.end())
		{
			Log::Instance.LogWarn(TAGMANAGER, "Attempted to add a package '%08x' that already exists!", ID);

			return false;
		};

		SuperSmartPointer<Package> ThePackage(new Package());

		if(!ThePackage->FromStream(PackageStream))
		{
			Log::Instance.LogErr(TAGMANAGER, "Package '%08x' failed to be deserialized!", ID);

			return false;
		};

		Packages[ID] = ThePackage;

		for(Package::EntryMap::iterator it = ThePackage->Entries.begin(); it != ThePackage->Entries.end(); it++)
		{
			for(Package::FileMap::iterator fit = it->second.begin(); fit != it->second.end(); fit++)
			{
				StringID NameID = MakeStringID(fit->second->Name);

				EntryMap::iterator mit = Files.find(it->first);

				if(mit != Files.end() && mit->second.find(NameID) != mit->second.end())
				{
					Log::Instance.LogWarn(TAGMANAGER, "While adding Package '%08x': Overriding file %08x/%08x",
						ID, it->first, NameID);
				};

				std::pair<Package *, Package::FileEntry *> Pair;

				Pair.first = ThePackage.Get();
				Pair.second = fit->second;

				Files[it->first][NameID] = Pair;
			};
		};

		return true;
	};

	bool PackageFileSystemManager::RemovePackage(StringID ID)
	{
		PackageMap::iterator it = Packages.find(ID);

		if(it == Packages.end())
			return false;

		it->second.Dispose();

		Packages.erase(it);

		return true;
	};
	
	SuperSmartPointer<PackageFileSystemManager::Package> PackageFileSystemManager::GetPackage(StringID ID)
	{
		PackageMap::iterator it = Packages.find(ID);

		if(it == Packages.end() || it->second.Get() == NULL)
			return SuperSmartPointer<Package>();

		return it->second;
	};

	void PackageFileSystemManager::ClearPackageData(Package *p)
	{
		for(EntryMap::iterator it = Files.begin(); it != Files.end(); it++)
		{
			for(FileMap::iterator fit = it->second.begin(); fit != it->second.end(); fit++)
			{
				if(fit->second.first == p)
				{
					it->second.erase(fit);
					fit = it->second.begin();

					if(fit == it->second.end())
						break;
				};
			};
		};
	};

	SuperSmartPointer<Stream> PackageFileSystemManager::GetFile(StringID Directory, StringID Name)
	{
		EntryMap::iterator it = Files.find(Directory);

		if(it == Files.end())
			return SuperSmartPointer<Stream>();

		FileMap::iterator fit = it->second.find(Name);

		if(fit == it->second.end())
			return SuperSmartPointer<Stream>();

		sf::Lock Lock(fit->second.first->FileAccessMutex);

		PackageFileSystemManager::PackageStream *PStream = new PackageFileSystemManager::PackageStream();

		PStream->Source = fit->second.first->PackageStream;
		PStream->StartOffset = fit->second.second->Offset;
		PStream->LengthValue = fit->second.second->Length;
		PStream->PositionValue = 0;

		return SuperSmartPointer<Stream>(PStream);
	};

	std::vector<std::string> PackageFileSystemManager::FindDirectories(const std::string &DirectoryBasePath)
	{
		std::vector<std::string> Out;

		for(EntryMap::iterator it = Files.begin(); it != Files.end(); it++)
		{
			for(FileMap::iterator fit = it->second.begin(); fit != it->second.end(); fit++)
			{
				const std::string &DirectoryName = fit->second.second->DirectoryName;

				if(DirectoryName.find(DirectoryBasePath) == 0)
				{
					bool Found = false;

					for(uint32 i = 0; i < Out.size(); i++)
					{
						if(Out[i] == DirectoryName)
						{
							Found = true;

							break;
						};
					};

					if(!Found)
						Out.push_back(DirectoryName);
				};
			};
		};

		return Out;
	};

	std::vector<std::pair<std::string, std::string> > PackageFileSystemManager::FindFiles(const std::string &Prefix, const std::string &Suffix,
		const std::string &Extension, const std::string &StartingDirectory)
	{
		std::vector<std::pair<std::string, std::string> > Out;
		std::string BaseFileName, FileExtension;

		for(EntryMap::iterator it = Files.begin(); it != Files.end(); it++)
		{
			for(FileMap::iterator fit = it->second.begin(); fit != it->second.end(); fit++)
			{
				const std::string &DirectoryName = fit->second.second->DirectoryName;

				if(DirectoryName.find(StartingDirectory) != 0)
					continue;

				int32 ExtensionIndex = fit->second.second->Name.rfind('.');

				BaseFileName = ExtensionIndex != std::string::npos ? fit->second.second->Name.substr(0, ExtensionIndex) :
					fit->second.second->Name;

				if(ExtensionIndex != std::string::npos)
				{
					FileExtension = fit->second.second->Name.substr(ExtensionIndex + 1);
				};

				if(Extension.length() && FileExtension != Extension)
					continue;

				if((Prefix.length() && BaseFileName.find(Prefix) == std::string::npos) ||
					(Suffix.length() && BaseFileName.find(Suffix) == std::string::npos))
					continue;

				Out.push_back(std::pair<std::string, std::string>(fit->second.second->DirectoryName, fit->second.second->Name));
			};
		};

		return Out;
	};

#if !FLPLATFORM_ANDROID
	void FileSystemWatcher::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAGWATCHER, "Initializing FileSystem Watcher...");
	};

	void FileSystemWatcher::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo(TAGWATCHER, "Terminating FileSystem Watcher...");
	};

	void FileSystemWatcher::Update(uint32 Priority)
	{
		PROFILE("FileSystemWatcherManager::Update", StatTypes::Game);

		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		GlobalFileWatcher.update();
	};

	bool FileSystemWatcher::WatchDirectory(const std::string &Path)
	{
		FLASSERT(WasStarted, "FileSystem Watcher not Started!");

		try
		{
			GlobalFileWatcher.addWatch(Path, new FileSystemWatcherCallback());
		}
		catch(std::exception &)
		{
			return false;
		};

		return true;
	};
#endif
};
