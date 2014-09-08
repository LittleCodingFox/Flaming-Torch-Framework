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

std::string FLGameName()
{
	return TAG;
};

struct MapDirectoryInfo
{
    std::string From, To, WorkingDirectory;
};

int main(int argc, char **argv)
{
	Log::Instance.Register();

	Log::Instance.FolderName = FLGameName();

	InitSubsystems();

	Log::Instance.LogInfo(TAG, "Version %s starting up", CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
	Log::Instance.LogInfo(TAG, "Searching PackageData Directories...");
    
    FileStream ConfigStream;
    GenericConfig Configuration;
    
    std::vector<MapDirectoryInfo> MapDirectories;
    
    if(!ConfigStream.Open(DirectoryInfo::ResourcesDirectory() + "/Baker.cfg", StreamFlags::Read | StreamFlags::Text) || !Configuration.DeSerialize(&ConfigStream))
    {
        Log::Instance.LogErr(TAG, "Failed to load Baker.cfg");
        
        DeInitSubsystems();
        
        return 1;
    };
    
    GenericConfig::Section &MapDirectoriesSection = Configuration.Sections["MapDirectories"];
    
    for(GenericConfig::Section::ValueMap::iterator it = MapDirectoriesSection.Values.begin(); it != MapDirectoriesSection.Values.end(); it++)
    {
        std::vector<std::string> Pieces = StringUtils::Split(it->second.Content, '|');
        
        if(Pieces.size() != 3)
            continue;
        
        Log::Instance.LogInfo(TAG, "Added Map Folder '%s' => '%s' ('%s')", Pieces[0].c_str(), Pieces[1].c_str(), Pieces[2].c_str());
        
        MapDirectoryInfo Info;
        Info.From = Pieces[0];
        Info.To = Pieces[1];
        Info.WorkingDirectory = Pieces[2];
        
        MapDirectories.push_back(Info);
    };

	std::string PackerPath = Configuration.GetString("Tools", "Packer", "../../Binaries/Packer/Release/Packer");
	std::string TiledConverterPath = Configuration.GetString("Tools", "TiledConverter", "../../Binaries/TiledConverter/Release/TiledConverter");
    
    Log::Instance.LogInfo(TAG, "... Deleting Temporary PackageData");

    DirectoryInfo::DeleteDirectory(DirectoryInfo::ResourcesDirectory() + "/PackageContent/PackageData/");
    
    std::vector<std::string> CopyDirectories = DirectoryInfo::GetAllDirectories(DirectoryInfo::ResourcesDirectory() + "/PackageContent/");
    
    DirectoryInfo::CreateDirectory(DirectoryInfo::ResourcesDirectory() + "/PackageContent/PackageData/");
    
    Log::Instance.LogInfo(TAG, "... Copying '%d' Directories", CopyDirectories.size());
    
    for(uint32 i = 0; i < CopyDirectories.size(); i++)
    {
        std::string DirectoryName = CopyDirectories[i].substr((DirectoryInfo::ResourcesDirectory() + "/PackageContent/").length());
        std::string TargetDirectory = DirectoryInfo::ResourcesDirectory() + "/PackageContent/PackageData/" + DirectoryName;
        
        Log::Instance.LogInfo(TAG, "... %s", DirectoryName.c_str());

        if(!DirectoryInfo::CopyDirectory(CopyDirectories[i], TargetDirectory, true))
        {
            DirectoryInfo::DeleteDirectory(DirectoryInfo::ResourcesDirectory() + "/PackageContent/PackageData");
            
            Log::Instance.LogErr(TAG, "Unable to copy all data to Temporary PackageData Folder");
            
            DeInitSubsystems();

            return 1;
        };
        
        Log::Instance.LogInfo(TAG, "...    Compiling Maps (if any)");
        
        for(uint32 j = 0; j < MapDirectories.size(); j++)
        {
            std::vector<std::string> MapFiles = DirectoryInfo::ScanDirectory(TargetDirectory + "/" + MapDirectories[j].From, "tmx");
            
            for(uint32 k = 0; k < MapFiles.size(); k++)
            {
				std::string WorkingDirectory = TargetDirectory + "/" + MapDirectories[j].WorkingDirectory;

				WorkingDirectory = StringUtils::Replace(WorkingDirectory, "//", "/");

                DirectoryInfo::CreateDirectory(TargetDirectory + "/" + MapDirectories[j].To);
                
                if(chdir(WorkingDirectory.c_str()) != 0)
				{
					Log::Instance.LogWarn(TAG, "Unable to change to map working directory '%s': Errno is %d", WorkingDirectory.c_str(), errno);

					continue;
				};
                
                //TODO: Do this the "right way"

				std::string ExePath = DirectoryInfo::ResourcesDirectory() + "/" + TiledConverterPath;
				std::string Parameters = "-dir \"" + TargetDirectory + "/" +  MapDirectories[j].To + "\" \"" +
                    MapFiles[k] + "\"";

				int32 ExitCode = CoreUtils::RunProgram(ExePath, Parameters, DirectoryInfo::ResourcesDirectory());

				if(0 != ExitCode)
				{
					Log::Instance.LogErr(TAG, "Unable to run TiledMap: Exit Code '%d'", ExitCode);

					continue;
				};
                
                if(chdir(DirectoryInfo::ResourcesDirectory().c_str()) != 0)
                {
                    Log::Instance.LogErr(TAG, "Unable to restore working directory while compiling maps!");
                    
                    DeInitSubsystems();
                    
                    return 1;
                };
            };
        };
        
        //TODO: Do this the "right way"
        
        Log::Instance.LogInfo(TAG, "...    Packing...");

		std::string ExePath = DirectoryInfo::ResourcesDirectory() + "/" + PackerPath;
		std::string Parameters = "-dir \"" + TargetDirectory + "\" \"\" -out \"Content/" + DirectoryName + ".package\"";

		int32 ExitCode = CoreUtils::RunProgram(ExePath, Parameters, DirectoryInfo::ResourcesDirectory());

		if(0 != ExitCode)
		{
			Log::Instance.LogErr(TAG, "Unable to run Packer: Exit Code '%d'", ExitCode);

			continue;
		};
    };
    
    Log::Instance.LogInfo(TAG, "... Deleting Temporary PackageData");
    
    DirectoryInfo::DeleteDirectory(DirectoryInfo::ResourcesDirectory() + "/PackageContent/PackageData/");

	DeInitSubsystems();

	return 0;
};
