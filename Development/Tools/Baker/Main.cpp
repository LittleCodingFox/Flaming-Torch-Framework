#include "FlamingCore.hpp"
using namespace FlamingTorch;

#include <stdio.h>
#
#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define TAG "Baker"
#
#if FLPLATFORM_WINDOWS
#	include <windows.h>
#	include <direct.h>
#	undef CreateDirectory
#endif

#ifdef CopyFile
#undef CopyFile
#endif

bool DontCleanup = false;

std::string FLGameName()
{
	return TAG;
};

int main(int argc, char **argv)
{
#if FLPLATFORM_WINDOWS
	EnableMinidumps(FLGameName().c_str(), CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
#endif

	Log::Instance.Register();

	Log::Instance.FolderName = FLGameName();

	InitSubsystems();

	Log::Instance.LogInfo(TAG, "Version %s starting up", CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());

	for(uint32 i = 1; i < (uint32)argc; i++)
	{
		if(std::string(argv[i]) == "-nocleanup")
		{
			DontCleanup = true;
		};
	};

	Log::Instance.LogInfo(TAG, "Searching PackageData Directories...");

    FileStream ConfigStream;
    GenericConfig Configuration;
    
    std::vector<std::pair<std::string, std::string> > MapDirectories, TexturePackDirectories;
	std::vector<std::string> ResourceDirectories;

	//Package Name => Directory List
	std::map<std::string, std::vector<std::string> > ValidResourceDirectories;

    if(!ConfigStream.Open(FileSystemUtils::ResourcesDirectory() + "/Baker.cfg", StreamFlags::Read | StreamFlags::Text) || !Configuration.DeSerialize(&ConfigStream))
    {
        Log::Instance.LogErr(TAG, "Failed to load Baker.cfg");
        
        DeInitSubsystems();
        
        return 1;
    };
    
    GenericConfig::Section &MapDirectoriesSection = Configuration.Sections["MapDirectories"];
    
    for(GenericConfig::Section::ValueMap::iterator it = MapDirectoriesSection.Values.begin(); it != MapDirectoriesSection.Values.end(); it++)
    {
        std::vector<std::string> Pieces = StringUtils::Split(it->second.Content, '|');
        
        if(Pieces.size() != 2)
            continue;
        
        Log::Instance.LogInfo(TAG, "Added Map Folder '%s' => '%s'", Pieces[0].c_str(), Pieces[1].c_str());
        
        MapDirectories.push_back(std::pair<std::string, std::string>(Pieces[0], Pieces[1]));
    };
    
    GenericConfig::Section &TexturePackingDirectoriesSection = Configuration.Sections["TexturePackingDirectories"];
    
    for(GenericConfig::Section::ValueMap::iterator it = TexturePackingDirectoriesSection.Values.begin(); it != TexturePackingDirectoriesSection.Values.end(); it++)
    {
        std::vector<std::string> Pieces = StringUtils::Split(it->second.Content, '|');
        
        if(Pieces.size() != 2)
            continue;
        
        Log::Instance.LogInfo(TAG, "Added Texture Packing Folder '%s' => '%s'", Pieces[0].c_str(), Pieces[1].c_str());
        
        TexturePackDirectories.push_back(std::pair<std::string, std::string>(Pieces[0], Pieces[1]));
    };

	GenericConfig::Section &ResourceDirectoriesSection = Configuration.Sections["ResourceDirectories"];

	for(GenericConfig::Section::ValueMap::iterator it = ResourceDirectoriesSection.Values.begin(); it != ResourceDirectoriesSection.Values.end(); it++)
    {
		ResourceDirectories.push_back(it->second.Content);
	};

	std::string PackerPath = Configuration.GetString("Tools", "Packer", "../../Binaries/Packer/Release/Packer");
	std::string TiledConverterPath = Configuration.GetString("Tools", "TiledConverter", "../../Binaries/TiledConverter/Release/TiledConverter");
	std::string TexturePackerPath = Configuration.GetString("Tools", "TexturePacker", "../../Binaries/TexturePacker/Release/TexturePacker");
	std::string AndroidPath = Configuration.GetString("Android", "ProjectPath", "Android");
    
    Log::Instance.LogInfo(TAG, "... Deleting Temporary PackageData");

    FileSystemUtils::DeleteDirectory(FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData/");
    
    std::vector<std::string> CopyDirectories = FileSystemUtils::GetAllDirectories(FileSystemUtils::ResourcesDirectory() + "/PackageContent/");
    
    FileSystemUtils::CreateDirectory(FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData/");
    
    Log::Instance.LogInfo(TAG, "... Copying '%d' Directories", CopyDirectories.size());
    
    for(uint32 i = 0; i < CopyDirectories.size(); i++)
    {
        std::string DirectoryName = CopyDirectories[i].substr((FileSystemUtils::ResourcesDirectory() + "/PackageContent/").length());
        std::string TargetDirectory = FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData/" + DirectoryName;
        
        Log::Instance.LogInfo(TAG, "... %s", DirectoryName.c_str());

        if(!FileSystemUtils::CopyDirectory(CopyDirectories[i], TargetDirectory, true))
        {
            FileSystemUtils::DeleteDirectory(FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData");
            
            Log::Instance.LogErr(TAG, "Unable to copy all data to Temporary PackageData Folder");
            
            DeInitSubsystems();

            return 1;
        };

		for(uint32 j = 0; j < ResourceDirectories.size(); j++)
		{
			std::string ResourceDirectoryName = Path(TargetDirectory + "/" + ResourceDirectories[j]).FullPath();

			if(FileSystemUtils::DirectoryExists(ResourceDirectoryName))
			{
				ValidResourceDirectories[DirectoryName].push_back(ResourceDirectoryName);
			};
		};
    };

	for(uint32 i = 0; i < CopyDirectories.size(); i++)
	{
        std::string DirectoryName = CopyDirectories[i].substr((FileSystemUtils::ResourcesDirectory() + "/PackageContent/").length());
        std::string TargetDirectory = FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData/" + DirectoryName;

		std::vector<std::string> PrioritizedResourceDirectories;

		{
			std::map<std::string, std::vector<std::string> >::iterator it = ValidResourceDirectories.find(DirectoryName);

			if(it != ValidResourceDirectories.end())
			{
				for(uint32 l = 0; l < it->second.size(); l++)
				{
					PrioritizedResourceDirectories.push_back(it->second[l]);
				};
			};

			for(it = ValidResourceDirectories.begin(); it != ValidResourceDirectories.end(); it++)
			{
				if(it->first == DirectoryName)
					continue;

				for(uint32 l = 0; l < it->second.size(); l++)
				{
					PrioritizedResourceDirectories.push_back(it->second[l]);
				};
			};
		};
        
        Log::Instance.LogInfo(TAG, "...    Compiling Maps (if any)");
        
        for(uint32 j = 0; j < MapDirectories.size(); j++)
        {
            std::vector<std::string> MapFiles = FileSystemUtils::ScanDirectory(TargetDirectory + "/" + MapDirectories[j].first, "tmx");
			FileSystemUtils::CreateDirectory(TargetDirectory + "/" + MapDirectories[j].second);
            
            for(uint32 k = 0; k < MapFiles.size(); k++)
            {
				Log::Instance.LogInfo(TAG, "...    Converting Map '%s'", Path(MapFiles[k]).BaseName.c_str());

				std::string ExePath = FileSystemUtils::ResourcesDirectory() + "/" + TiledConverterPath;
				std::string Parameters = "-dir \"" + TargetDirectory + "/" +  MapDirectories[j].second + "\"";

				for(uint32 l = 0; l < PrioritizedResourceDirectories.size(); l++)
				{
					Parameters += " -resdir \"" + PrioritizedResourceDirectories[l] + "\"";
				};
				
				Parameters += " \"" + MapFiles[k] + "\"";

				int32 ExitCode = CoreUtils::RunProgram(ExePath, Parameters, FileSystemUtils::ResourcesDirectory());

				if(0 != ExitCode)
				{
					Log::Instance.LogErr(TAG, "Unable to run TiledMap: Exit Code '%d'", ExitCode);

					continue;
				};
            };
        };
        
        Log::Instance.LogInfo(TAG, "...    Packing Textures (if any)");

		for(uint32 j = 0; j < TexturePackDirectories.size(); j++)
		{
            std::vector<std::string> TexturePackFiles = FileSystemUtils::ScanDirectory(TargetDirectory + "/" + TexturePackDirectories[j].first, "cfg");

			if(TexturePackFiles.size() == 0)
			{
				Log::Instance.LogInfo(TAG, "...    No Config files found at '%s'", TexturePackDirectories[j].first.c_str());

				continue;
			};

			FileSystemUtils::CreateDirectoryRecursive(TargetDirectory + "/" + TexturePackDirectories[j].second);
            
			for(uint32 k = 0; k < TexturePackFiles.size(); k++)
            {
				Log::Instance.LogInfo(TAG, "...    Texture Packing '%s'", Path(TexturePackFiles[k]).BaseName.c_str());

				std::string ExePath = FileSystemUtils::ResourcesDirectory() + "/" + TexturePackerPath;
				std::string Parameters = "-dir \"" + Path(TargetDirectory + "/" +  TexturePackDirectories[j].second).FullPath() + "\"";

				for(uint32 l = 0; l < PrioritizedResourceDirectories.size(); l++)
				{
					Parameters += " -resdir \"" + PrioritizedResourceDirectories[l] + "\"";
				};
				
				Parameters += " \"" + TexturePackFiles[k] + "\"";

				int32 ExitCode = CoreUtils::RunProgram(ExePath, Parameters, FileSystemUtils::ResourcesDirectory());

				if(0 != ExitCode)
				{
					Log::Instance.LogErr(TAG, "Unable to run TexturePacker: Exit Code '%d'", ExitCode);

					continue;
				};
			};
		};

		for(uint32 j = 0; j < ValidResourceDirectories[DirectoryName].size(); j++)
		{
			Log::Instance.LogInfo(TAG, "...    Deleting Resource Directory '%s'", Path(ValidResourceDirectories[DirectoryName][j]).BaseName.c_str());

			FileSystemUtils::DeleteDirectory(Path(ValidResourceDirectories[DirectoryName][j]).FullPath());
		};
        
        Log::Instance.LogInfo(TAG, "...    Packing...");

		std::string ExePath = FileSystemUtils::ResourcesDirectory() + "/" + PackerPath;
		std::string Parameters = "-dir \"" + TargetDirectory + "\" \"\" -out \"Content/" + DirectoryName + ".package\"";

		int32 ExitCode = CoreUtils::RunProgram(ExePath, Parameters, FileSystemUtils::ResourcesDirectory());

		if(0 != ExitCode)
		{
			Log::Instance.LogErr(TAG, "Unable to run Packer: Exit Code '%d'", ExitCode);

			continue;
		};

        Log::Instance.LogInfo(TAG, "...    Processing for Android...");

		std::string FinalAndroidPath = Path(FileSystemUtils::ResourcesDirectory() + "/" + AndroidPath).FullPath();

		if(!FileSystemUtils::DirectoryExists(FinalAndroidPath))
		{
			Log::Instance.LogWarn(TAG, "Android Path '%s' not found!", AndroidPath.c_str());
		}
		else
		{
			std::string AssetsPath = FinalAndroidPath + "/assets/";

			if(!FileSystemUtils::DirectoryExists(AssetsPath) && !FileSystemUtils::CreateDirectory(AssetsPath))
			{
				Log::Instance.LogWarn(TAG, "Android: Unable to create Assets folder");

				continue;
			};

			AssetsPath += "/Content/";

			if(!FileSystemUtils::DirectoryExists(AssetsPath) && !FileSystemUtils::CreateDirectory(AssetsPath))
			{
				Log::Instance.LogWarn(TAG, "Android: Unable to create Content folder");

				continue;
			};

			FileStream OutStream, InStream;

			if(!OutStream.Open(Path(AssetsPath + "/" + DirectoryName + ".package.gif").FullPath(), StreamFlags::Write) ||
				!InStream.Open(Path(FileSystemUtils::ResourcesDirectory() + "/Content/" + DirectoryName + ".package").FullPath(), StreamFlags::Read) || !InStream.CopyTo(&OutStream))
			{
				Log::Instance.LogErr(TAG, "Android: Unable to create copy package '%s'", DirectoryName.c_str());

				continue;
			};
		};
	};
    
    Log::Instance.LogInfo(TAG, "... Deleting Temporary PackageData");
    
	if(!DontCleanup)
	{
	    FileSystemUtils::DeleteDirectory(FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData/");
	};

	DeInitSubsystems();

	return 0;
};
