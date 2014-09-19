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

std::string FLGameName()
{
	return TAG;
};

struct MapDirectoryInfo
{
    std::string From, To;
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
	Log::Instance.LogInfo(TAG, "Searching PackageData Directories...");
    
    FileStream ConfigStream;
    GenericConfig Configuration;
    
    std::vector<MapDirectoryInfo> MapDirectories;
	std::vector<std::string> ResourceDirectories;

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
        
        MapDirectoryInfo Info;
        Info.From = Pieces[0];
        Info.To = Pieces[1];
        
        MapDirectories.push_back(Info);
    };

	GenericConfig::Section &ResourceDirectoriesSection = Configuration.Sections["ResourceDirectories"];

	for(GenericConfig::Section::ValueMap::iterator it = ResourceDirectoriesSection.Values.begin(); it != ResourceDirectoriesSection.Values.end(); it++)
    {
		ResourceDirectories.push_back(it->second.Content);
	};

	std::string PackerPath = Configuration.GetString("Tools", "Packer", "../../Binaries/Packer/Release/Packer");
	std::string TiledConverterPath = Configuration.GetString("Tools", "TiledConverter", "../../Binaries/TiledConverter/Release/TiledConverter");
    
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
        
        Log::Instance.LogInfo(TAG, "...    Compiling Maps (if any)");
        
        for(uint32 j = 0; j < MapDirectories.size(); j++)
        {
            std::vector<std::string> MapFiles = FileSystemUtils::ScanDirectory(TargetDirectory + "/" + MapDirectories[j].From, "tmx");
            
            for(uint32 k = 0; k < MapFiles.size(); k++)
            {
                FileSystemUtils::CreateDirectory(TargetDirectory + "/" + MapDirectories[j].To);

				std::string ExePath = FileSystemUtils::ResourcesDirectory() + "/" + TiledConverterPath;
				std::string Parameters = "-dir \"" + TargetDirectory + "/" +  MapDirectories[j].To + "\"";

				for(uint32 l = 0; l < ResourceDirectories.size(); l++)
				{
					Parameters += " -resdir \"" + TargetDirectory + "/" + ResourceDirectories[l] + "\"";
				};
				
				Parameters += "\"" + MapFiles[k] + "\"";

				int32 ExitCode = CoreUtils::RunProgram(ExePath, Parameters, FileSystemUtils::ResourcesDirectory());

				if(0 != ExitCode)
				{
					Log::Instance.LogErr(TAG, "Unable to run TiledMap: Exit Code '%d'", ExitCode);

					continue;
				};
            };
        };

		for(uint32 j = 0; j < ResourceDirectories.size(); j++)
		{
			FileSystemUtils::DeleteDirectory(TargetDirectory + "/" + ResourceDirectories[j]);
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
    };
    
    Log::Instance.LogInfo(TAG, "... Deleting Temporary PackageData");
    
    FileSystemUtils::DeleteDirectory(FileSystemUtils::ResourcesDirectory() + "/PackageContent/PackageData/");

	DeInitSubsystems();

	return 0;
};
