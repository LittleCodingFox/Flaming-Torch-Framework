#pragma once
#ifdef CreateDirectory
#	undef CreateDirectory
#endif
#
#ifdef CopyFile
#	undef CopyFile
#endif

/*!
*	FileSystem Path Utiity class
*/
class Path
{
public:
	std::string Directory, BaseName;

	Path();
	Path(const std::string &PathName);
	Path(const std::string &Directory, const std::string &BaseName);

	/*!
	*	Gets the combined full path as a string
	*/
	std::string FullPath() const;

	/*!
	*	Gets the extension (if any) of the path
	*/
	std::string Extension() const;

	/*!
	*	Generates a Path with a modified extension based on this path
	*	\param NewExtension the new extension
	*/
	Path ChangeExtension(const std::string &NewExtension) const;

	/*!
	*	Generates a Path without an extension
	*/
	Path StripExtension() const;

	/*!
	*	Normalizes a Path by removing all path separators and replacing them with the default path separator
	*/
	static std::string Normalize(const std::string &PathName);

	/*!
	*	Default Path Separator
	*/
	static std::string PathSeparator;

	/*!
	*	Platform-specific Path Separator
	*/
	static std::string PlatformPathSeparator;
};

/*!
*	FileSystem Utility Class
*/
class FileSystemUtils
{
public:
	/*!
	*	Enumerate all files with a certain extension
	*	\param Directory the directory to start scanning from
	*	\param Extension the extension, without the *. portion (e.g., instead of "*.txt" it should be "txt")
	*	\param Recursive whether it should recursively scan subdirectories
	*/
	static std::vector<std::string> ScanDirectory(const std::string &Directory,
		const std::string &Extension, bool Recursive = false);
    
    /*!
    *   Enumerate all directories within a directory (non-recursive)
    *   \param Directory the directory to scan
    */
    static std::vector<std::string> GetAllDirectories(const std::string &Directory);

	/*!
	*	Creates a directory
	*	\param Directory the directory path
	*/
	static bool CreateDirectory(const std::string &Directory);

	/*!
	*	Creates a directory recursively (tries to create all path pieces)
	*	\param Directory the directory path
	*/
	static bool CreateDirectoryRecursive(const std::string &Directory);
    
    /*!
    *   Copies a directory and its contents
    *   \param From the directory to copy from
    *   \param To the directory to copy to
    *   \param Recursive whether to copy all subdirectories too
    */
    static bool CopyDirectory(const std::string &From, const std::string &To, bool Recursive = false);

	/*!
	*	Checks whether a directory exists
	*	\param Directory the directory to check
	*/
	static bool DirectoryExists(const std::string &Directory);
    
    /*!
    *   Deletes a directory
    *	\param Directory the directory path
    */
    static bool DeleteDirectory(const std::string &Directory);

	/*!
	*	The current working directory
	*/
	static const std::string &ActiveDirectory();

	/*!
	*	The resources directory
	*/
	static const std::string &ResourcesDirectory();

	/*!
	*	The preferred storage directory
	*/
	static const std::string &PreferredStorageDirectory();

	/*!
	*	Creates an Open File Dialog
	*	\param Title the title of the dialog
	*	\param Extension the extension to use, without the *. portion (e.g., instead of "*.txt" it should be "txt")
	*	\param Filter the filter to use in the form of Name\0Filter\0Repeat (e.g., "Text Files\0*.txt" or "Text Files\0*.txt\0RTF Files\0*.rtf")
	*/
	static std::string OpenFileDialog(const std::string &Title, const std::string &Extension, const std::string &Filter);

	/*!
	*	Creates a Save File Dialog
	*	\param Title the title of the dialog
	*	\param Extension the extension to use, without the *. portion (e.g., instead of "*.txt" it should be "txt")
	*	\param Filter the filter to use in the form of Name\0Filter\0Repeat (e.g., "Text Files\0*.txt" or "Text Files\0*.txt\0RTF Files\0*.rtf")
	*/
	static std::string SaveFileDialog(const std::string &Title, const std::string &Extension, const std::string &Filter);

	/*!
	*	Removes a file from a storage device
	*	\param Name the filename with path to remove
	*/
	static bool RemoveFile(const std::string Name);

	/*!
	*	Copies a file
	*	\param From the file to copy
	*	\param To where to store the file
	*/
	static bool CopyFile(const std::string &From, const std::string &To);
};

/*!
*	Stream Access Flags
*/
namespace StreamFlags
{
	enum StreamFlags
	{
		Write = FLAGVALUE(1), //!<Write Mode
		Read = FLAGVALUE(2), //!<Read Mode
		Text = FLAGVALUE(3) //!<Text Mode
	};
};

/*!
*	Stream Processor
*	Processes Stream Data while it is read or written
*/
class StreamProcessor 
{
public:
	StreamProcessor(const std::string &NativeType) {};
	virtual ~StreamProcessor() {};

	/*!
	*	Encodes data
	*	\param Data the data to encode
	*	\param Length the data length
	*/
	virtual void Encode(void *Data, uint32 Length) = 0;

	/*!
	*	Decodes data
	*	\param Data the data to decode
	*	\param Length the data length
	*/
	virtual void Decode(void *Data, uint32 Length) = 0;
};

/*!
*	XOR-Based Stream Processor
*/
class XORStreamProcessor : public StreamProcessor
{
public:
	/*!
	*	XOR Key
	*/
	uint8 Key;

	XORStreamProcessor() : StreamProcessor("XORStreamProcessor"), Key((uint8)'FL') {};

	void Encode(void *Data, uint32 Length)
	{
		uint8 *Ptr = (uint8 *)Data;

		for(uint32 i = 0; i < Length; i++)
		{
			Ptr[i] ^= Key;
		};
	};

	void Decode(void *Data, uint32 Length)
	{
		uint8 *Ptr = (uint8 *)Data;

		for(uint32 i = 0; i < Length; i++)
		{
			Ptr[i] ^= Key;
		};
	};
};

/*!
*	Data Stream Class
*/
class Stream 
{
protected:
	SuperSmartPointer<StreamProcessor> Processor;
public:
	virtual ~Stream() {};
	/*!
	*	\return the size of the stream
	*/
	virtual uint64 Length() const = 0;
	/*!
	*	\return the position in the stream
	*/
	virtual uint64 Position() const = 0;
	/*!
	*	Moves the stream's position
	*	\param Position the new Position
	*	\return whether we were able to move
	*/
	virtual bool Seek(uint64 Position) = 0;
	/*!
	*	Writes data to a stream
	*	\param Data the data to write
	*	\param ElementSize the Size of an element in the data as bytes
	*	\param Length the amount of elements to write
	*/
	virtual bool Write(const void *Data, uint32 ElementSize, uint32 Length) = 0;
	/*!
	*	Reads data from a stream
	*	\param Data the data to read
	*	\param ElementSize the Size of an element in the data as bytes
	*	\param Length the amount of elements to read
	*/
	virtual bool Read(void *Data, uint32 ElementSize, uint32 Length) = 0;
	/*!
	*	Gets a part of the stream as a buffer
	*	\param Data the data to read from
	*	\param Length the amount of bytes to read
	*/
	virtual void AsBuffer(void *Data, uint32 Length) = 0;
	/*!
	*	\return the stream as a String
	*/
	std::string AsString();

	/*!
	*	Copies a Stream to another Stream
	*	\param Out the stream to write to
	*/
	bool CopyTo(Stream *Out);

	/*!
	*	Sets a new stream processor to this stream
	*	\param Processor the processor to set up
	*/
	void SetProcessor(SuperSmartPointer<StreamProcessor> Processor)
	{
		this->Processor = Processor;
	};

	/*!
	*	Gets our active stream processor
	*/
	SuperSmartPointer<StreamProcessor> GetProcessor()
	{
		return Processor;
	};

	/*!
	*	Template-specialized Write method
	*	\param Data the data to write
	*	\param Length how many data elements to write
	*	\sa Stream::Write
	*/
	template<typename type> bool Write2(const type *Data, uint32 Length = 1)
	{
		return Write(Data, sizeof(type), Length);
	};

	/*!
	*	Template-specialized Read method
	*	\param Data the data to read
	*	\param Length how many data elements to read
	*	\sa Stream::Read
	*/
	template<typename type> bool Read2(type *Data, uint32 Length = 1)
	{
		return Read(Data, sizeof(type), Length);
	};

	/*!
	*	Writes data from another stream into this stream
	*	\param Target the stream to read from
	*	\param Length the amount to read from in bytes
	*/
	bool ReadFromStream(Stream *Target, uint32 Length);

	/*!
	*	Writes data from this stream into another stream
	*	\param Target the stream to write to
	*	\param Length the amount to write to in bytes
	*/
	bool WriteToStream(Stream *Target, uint32 Length);
};

/*!
*	A stream made of RAM data
*/
class MemoryStream : public Stream
{
private:
	std::vector<uint8> Data;
	uint64 _Position;
public:
	MemoryStream();
	uint64 Length() const;
	uint64 Position() const;
	bool Seek(uint64 Position);
	bool Write(const void *Data, uint32 ElementSize, uint32 Length);
	bool Read(void *Data, uint32 ElementSize, uint32 Length);
	void AsBuffer(void *Data, uint32 Length);
	/*!
	*	\return the memory stored inside
	*/
	const void *Get() const;
	/*!
	*	Clears the data within
	*/
	void Clear();
};

/*!
*	An immutable Memory Stream
*/
class ConstantMemoryStream : public Stream
{
private:
	uint8 *Data;
	uint64 _Position;
	uint32 _Length;
public:
	ConstantMemoryStream(const void *Ptr, uint32 _Length);
	uint64 Length() const;
	uint64 Position() const;
	bool Seek(uint64 Position);
	bool Write(const void *Data, uint32 ElementSize, uint32 Length);
	bool Read(void *Data, uint32 ElementSize, uint32 Length);
	void AsBuffer(void *Data, uint32 Length);
	/*!
	*	\return the memory stored inside
	*/
	const void *Get() const;
};

/*!
*	A stream from a file
*/
class FileStream : public Stream
{
private:
	void *Handle;
	uint64 _Length, _Position;
	bool IsBasic;
public:
	FileStream();
	~FileStream();
	/*!
	*	tries to open a file
	*	\param Name the File Name
	*	\param Flags one or several of StreamFlags::*
	*/
	bool Open(const std::string &Name, uint8 Flags);
	/*!
	*	Close this file stream
	*/
	void Close();
	uint64 Length() const;
	uint64 Position() const;
	bool Seek(uint64 Position);
	bool Write(const void *Data, uint32 ElementSize, uint32 Length);
	bool Read(void *Data, uint32 ElementSize, uint32 Length);
	void AsBuffer(void *Data, uint32 Length);
};

/*!
*	Manager of package filesystems
*/
class PackageFileSystemManager : public SubSystem
{
public:
	class Package
	{
	public:
		class FileEntry
		{
		public:
			StringID DirectoryID;
			std::string Name, DirectoryName;
			uint64 Length, Offset;
			//Used for reading the file's data
			SuperSmartPointer<FileStream> Input;
		};

		typedef std::map<StringID, SuperSmartPointer<FileEntry> > FileMap;
		typedef std::map<StringID, FileMap> EntryMap;

		sf::Mutex FileAccessMutex;
		bool HasBeenTampered;
	public:
		EntryMap Entries;

		SuperSmartPointer<Stream> PackageStream;
		uint64 OriginalOffset;

		Package() : HasBeenTampered(false), OriginalOffset(0) {};
		~Package();

		bool Serialize(Stream *Out);
		//When deserializing, the stream must ALWAYS be "alive" until the package is destroyed
		bool DeSerialize(Stream *In);

		bool FromStream(SuperSmartPointer<Stream> Stream);

		//Requires the FileStream to be at position 0 and can't be accessed until we serialize
		bool AddFile(const std::string &Directory, const std::string &Name, SuperSmartPointer<FileStream> In);
		bool RemoveFile(const std::string &Directory, const std::string &Name);
	};

	class PackageStream : public Stream
	{
		friend class Package;
	public:
		SuperSmartPointer<Stream> Source;
		uint64 StartOffset, PositionValue, LengthValue;

		PackageStream() : StartOffset(0), PositionValue(0), LengthValue(0) {};
		uint64 Length() const;
		uint64 Position() const;
		bool Seek(uint64 Position);
		bool Write(const void *Data, uint32 ElementSize, uint32 Length);
		bool Read(void *Data, uint32 ElementSize, uint32 Length);
		void AsBuffer(void *Data, uint32 Length);
	};
private:
	friend class Package;
	PackageFileSystemManager() : SubSystem(PACKAGE_FILESYSTEM_PRIORITY) {};

	typedef std::map<StringID, std::pair<Package *, Package::FileEntry *> > FileMap;
	typedef std::map<StringID, FileMap> EntryMap;
	EntryMap Files;

	typedef std::map<StringID, SuperSmartPointer<Package> > PackageMap;
	PackageMap Packages;

	void ClearPackageData(Package *p);
public:
	static PackageFileSystemManager Instance;

	/*!
	*	Creates a new empty package
	*/
	SuperSmartPointer<Package> NewPackage();

	/*!
	*	Loads a package into this manager
	*	\param ID the ID of the package
	*	\param PackageStream a Stream containing the package
	*	\sa MakeStringID
	*/
	bool AddPackage(StringID ID, SuperSmartPointer<Stream> PackageStream);
	/*!
	*	Removes a package from this manager
	*	\param ID the ID of the package
	*	\sa MakeStringID
	*/
	bool RemovePackage(StringID ID);

	/*!
	*	Gets a package from this manager
	*	\param ID the ID of the package
	*	\sa MakeStringID
	*/
	SuperSmartPointer<Package> GetPackage(StringID ID);

	/*!
	*	Gets a file stream
	*	\param Directory the directory as a StringID
	*	\param Name the filename as the StringID
	*	\sa MakeStringID
	*/
	SuperSmartPointer<Stream> GetFile(StringID Directory, StringID Name);

	/*!
	*	Finds directories within a base
	*	\param DirectoryBasePath the starting directory
	*/
	std::vector<std::string> FindDirectories(const std::string &DirectoryBasePath);
	/*!
	*	Finds Files within a starting directory, prefix, suffix, and extension
	*	\param Prefix the file's prefix (may be "")
	*	\param Suffix the file's suffix (may be "")
	*	\param Extension the file's extension (without a ".")
	*	\param StartingDirectory the starting directory to search
	*/
	std::vector<std::pair<std::string, std::string> > FindFiles(const std::string &Prefix, const std::string &Suffix = "",
		const std::string &Extension = "", const std::string &StartingDirectory = "");

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
};

#if !FLPLATFORM_ANDROID
namespace FileSystemWatcherAction
{
	enum FileSystemWatcherAction
	{
		Added, //!<File was added
		Deleted, //!<File was deleted
		Modified //!<File was modified
	};
};

/*!
*	FileSystem Watching Class
*	Provides notifications when files are modified in some way within a directory
*/
class FileSystemWatcher : public SubSystem
{
public:
	static FileSystemWatcher Instance;

	//!<Directory, FileName, Action
	Signal3<const std::string &, const std::string &, uint32> OnAction;

	FileSystemWatcher() : SubSystem(FILESYSTEM_WATCHER_PRIORITY) {};

	/*!
	*	Watches a directory for changes
	*	\param Path the directory to watch
	*	\return whether it was able to watch that directory
	*/
	bool WatchDirectory(const std::string &Path);

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
};
#endif

