#include "FlamingCore.hpp"

using namespace FlamingTorch;
#include <stdio.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define TAG "Baker"

std::string FLGameName()
{
	return TAG;
};

int main(int argc, char **argv)
{
	Log::Instance.Register();

	InitSubsystems();

	Log::Instance.LogInfo(TAG, "Version %s starting up", CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
	Log::Instance.LogInfo(TAG, "Searching PackageData Directories...");
    
    FileStream ConfigStream;
    GenericConfig Configuration;
    
    /*
    if(!ConfigStream.Open(DirectoryInfo::ResourcesDirectory() + "/Baker.cfg", StreamFlags::Read | StreamFlags::Text) || !Configuration.DeSerialize(&ConfigStream))
    {
        Log::Instance.LogErr(TAG, "Failed to load Baker.cfg");
        
        DeInitSubsystems();
        
        return 1;
    };
    */
    
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
        
        //TODO: Do this the "right way"
        
        std::string CommandString = "../../Binaries/Packer/Release/Packer -dir \"" + TargetDirectory + "\" \"\" -out \"Content/" + DirectoryName + ".package\"";
        
        system(CommandString.c_str());
    };
    
    Log::Instance.LogInfo(TAG, "... Deleting Temporary PackageData");
    
    DirectoryInfo::DeleteDirectory(DirectoryInfo::ResourcesDirectory() + "/PackageContent/PackageData/");

	DeInitSubsystems();

	return 0;
};
