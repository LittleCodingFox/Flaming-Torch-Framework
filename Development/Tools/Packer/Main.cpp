#include "FlamingConfig.hpp"
#include "CoreVersion.hpp"
#include <vector>
#include <map>
#include <SFML/System.hpp>

namespace FlamingTorch
{
#	include "SubSystem.hpp"
#	include "Log.hpp"
#	include "GameClock.hpp"
#	include "StringID.hpp"
#	include "SuperSmartPointer.hpp"
#	include "FileSystem.hpp"
};

using namespace FlamingTorch;
#include <stdio.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define TAG "Packer"

std::string FLGameName()
{
	return "Packer";
};

int main(int argc, char **argv)
{
	Log::Instance.Register();

	if(argc == 1)
	{
		Log::Instance.LogInfo(TAG, "Usage: %s -dir [folder] [folder in package] -file [file][folder in package] "
			"-out [filename.package] [-key] [package key]\nPackage Key must be a 2-digit hex number", argv[0]);

		return 1;
	};

	PackageFileSystemManager::Instance.Register();

	InitSubsystems();

	std::vector<std::string> RequestedDirectories, RequestedFiles, TargetDirectories, FinalDirectories;
	std::vector<std::pair<std::string, std::string> > FinalFiles;
	std::string OutPackageName = "out.package";
	int32 PackageKey = -1;

	for(int32 i = 1; i < argc; i++)
	{
		if(std::string(argv[i]) == "-dir" && i < argc - 2)
		{
			RequestedDirectories.push_back(argv[i + 1]);
			TargetDirectories.push_back(argv[i + 2]);
			i+=2;
		};

		if(std::string(argv[i]) == "-file" && i < argc - 2)
		{
			RequestedFiles.push_back(argv[i + 2]);
			TargetDirectories.push_back(argv[i + 2]);
			i+=2;
		};

		if(std::string(argv[i]) == "-out" && i < argc - 1)
		{
			OutPackageName = argv[i + 1];
			i++;
		};

		if(std::string(argv[i]) == "-key" && i < argc - 1)
		{
			std::string Key = argv[i + 1];
			sscanf(Key.c_str(), "%x", &PackageKey);

			i++;
		};
	};

	Log::Instance.LogInfo(TAG, "Version %s starting up", CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
	Log::Instance.LogInfo(TAG, "Searching Directories...");

	for(uint32 i = 0; i < RequestedDirectories.size(); i++)
	{
		std::vector<std::string> ReceivedFiles = DirectoryInfo::ScanDirectory(RequestedDirectories[i], "*", true);

		for(uint32 j = 0; j < ReceivedFiles.size(); j++)
		{
			std::pair<std::string, std::string> Pair(ReceivedFiles[j], TargetDirectories[i]);
			FinalFiles.push_back(Pair);
			FinalDirectories.push_back(RequestedDirectories[i]);
		};

		Log::Instance.LogInfo(TAG, "... %s (as %s, %d files)", RequestedDirectories[i].c_str(), TargetDirectories[i].c_str(),
			ReceivedFiles.size());
	};

	Log::Instance.LogInfo(TAG, "Adding extra files...");

	for(uint32 i = 0; i < RequestedFiles.size(); i++)
	{
		std::pair<std::string, std::string> Pair("/" + RequestedFiles[i], TargetDirectories[i + RequestedDirectories.size()]);

		FinalFiles.push_back(Pair);
		FinalDirectories.push_back(std::string());

		Log::Instance.LogInfo(TAG, "... %s (as %s)", RequestedDirectories[i].c_str(), TargetDirectories[i + RequestedDirectories.size()].c_str());
	};

	SuperSmartPointer<PackageFileSystemManager::Package> Package = PackageFileSystemManager::Instance.NewPackage();

	Log::Instance.LogInfo(TAG, "Adding Files to Package Info...");

	for(uint32 i = 0; i < FinalFiles.size(); i++)
	{
		SuperSmartPointer<FileStream> Stream(new FileStream);

		if(!Stream->Open(FinalFiles[i].first, StreamFlags::Read))
		{
			Log::Instance.LogInfo(TAG, "... Unable to open '%s', skipping...", FinalFiles[i].first.c_str());

			continue;
		};

		std::string FinalDirectory = std::string("/") + (FinalFiles[i].second.length() ?
			FinalFiles[i].second + std::string("/") : "") + FinalFiles[i].first.substr(FinalDirectories[i].length() + 1,
			FinalFiles[i].first.rfind('/') - FinalDirectories[i].length());

		std::string FinalName = FinalFiles[i].first.substr(FinalFiles[i].first.rfind('/') + 1);

		if(!Package->AddFile(FinalDirectory, FinalName, Stream))
		{
			Log::Instance.LogInfo(TAG, "... Unable to add '%s', skipping...", FinalFiles[i].first.c_str());

			continue;
		};

		Log::Instance.LogInfo(TAG, "... Added %s%s (%08x/%08x, %lld bytes)", FinalDirectory.c_str(), FinalName.c_str(), 
			MakeStringID(FinalDirectory), MakeStringID(FinalName), Stream->Length());
	};

	Log::Instance.LogInfo(TAG, "Final Packing!");

	SuperSmartPointer<Stream> Stream(new FileStream());
	FileStream *ActualStream = (FileStream *)Stream.Get();

	if(!ActualStream->Open(OutPackageName, StreamFlags::Write))
	{
		Log::Instance.LogInfo(TAG, "Unable to open '%s' for writing!", OutPackageName.c_str());

		return 1;
	};

	SuperSmartPointer<XORStreamProcessor> Processor(new XORStreamProcessor());

	if(PackageKey > 0)
		Processor->Key = (uint8)PackageKey;

	Stream->SetProcessor(Processor);

	if(!Package->Serialize(Stream))
	{
		Log::Instance.LogInfo(TAG, "Failed to save package data into '%s'!", OutPackageName.c_str());

		return 1;
	};

	Log::Instance.LogInfo(TAG, "Success! Now verifying package data!");

	ActualStream->Close();

	Package.Dispose();

	if(!ActualStream->Open(OutPackageName, StreamFlags::Read))
	{
		Log::Instance.LogInfo(TAG, "Unable to open '%s' for reading!", OutPackageName.c_str());

		return 1;
	};

	Package.Reset(new PackageFileSystemManager::Package());

	if(!Package->FromStream(Stream))
	{
		Log::Instance.LogInfo(TAG, "Unable to deserialize package data from '%s'!", OutPackageName.c_str());

		return 1;
	};

	Log::Instance.LogInfo(TAG, "All OK!");

	DeInitSubsystems();

	return 0;
};
