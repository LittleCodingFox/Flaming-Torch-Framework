#include "FlamingCore.hpp"

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
#if FLPLATFORM_WINDOWS
	EnableMinidumps(FLGameName().c_str(), CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
#endif

	Log::Instance.Register();

	Log::Instance.FolderName = FLGameName();

	if(argc == 1)
	{
		Log::Instance.LogInfo(TAG, "Usage: %s -dir [folder] [folder in package] -file [file][folder in package] "
			"-out [filename.package] [-key] [package key]\nPackage Key must be a 2-digit hex number", argv[0]);

		Log::Instance.LogInfo(TAG, "Alternative Usage: %s -extract [package] -out [outdir] [-key] [package key]\nPackage Key must be a 2-digit hex number", argv[0]);

		return 1;
	};

	PackageFileSystemManager::Instance.Register();

	InitSubsystems();

	std::vector<std::string> RequestedDirectories, RequestedFiles, TargetDirectories, FinalDirectories;
	std::vector<std::pair<std::string, std::string> > FinalFiles;
	std::string OutPackageName = "out.package", OutPackageDirectory;
	int32 PackageKey = -1;
	bool Extract = false;

	for(int32 i = 1; i < argc; i++)
	{
		if(std::string(argv[i]) == "-dir" && i < argc - 2)
		{
			RequestedDirectories.push_back(Path(argv[i + 1]).FullPath());
			TargetDirectories.push_back(Path(argv[i + 2]).FullPath());
			i+=2;
		}
		else if(std::string(argv[i]) == "-file" && i < argc - 2)
		{
			RequestedFiles.push_back(Path(argv[i + 2]).FullPath());
			TargetDirectories.push_back(Path(argv[i + 2]).FullPath());
			i+=2;
		}
		else if(std::string(argv[i]) == "-out" && i < argc - 1)
		{
			if(Extract)
			{
				OutPackageDirectory = Path(argv[i + 1]).FullPath();
			}
			else
			{
				OutPackageName = Path(argv[i + 1]).FullPath();
			};

			i++;
		}
		else if(std::string(argv[i]) == "-key" && i < argc - 1)
		{
			std::string Key = argv[i + 1];
			sscanf(Key.c_str(), "%x", &PackageKey);

			i++;
		}
		else if(std::string(argv[i]) == "-extract" && i < argc - 1)
		{
			Extract = true;
			OutPackageDirectory = OutPackageName;
			OutPackageName = argv[i + 1];
		};
	};

	Log::Instance.LogInfo(TAG, "Version %s starting up", CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
	Log::Instance.LogInfo(TAG, "Output %s: %s", Extract ? "Directory": "File", OutPackageName.c_str());

	if(Extract)
	{
		std::string ActualOutPackageDirectory = FileSystemUtils::ActiveDirectory();

		Log::Instance.LogInfo(TAG, "Preparing to extract to directory '%s'", Path(ActualOutPackageDirectory).FullPath().c_str());

		{
			std::vector<std::string> Fragments = StringUtils::Split(Path(OutPackageDirectory).FullPath(), Path::PathSeparator[0]);

			for(uint32 i = 0; i < Fragments.size(); i++)
			{
				ActualOutPackageDirectory += "/" + Fragments[i];

				if(i == 0)
				{
					FileSystemUtils::DeleteDirectory(ActualOutPackageDirectory);
				};

				FileSystemUtils::CreateDirectory(ActualOutPackageDirectory);
			};
		};

		SuperSmartPointer<FileStream> In(new FileStream());

		if(!In->Open((FileSystemUtils::ActiveDirectory() + "/" + OutPackageName).c_str(), StreamFlags::Read))
		{
			Log::Instance.LogErr(TAG, "Failed to read input file '%s'", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};

		SuperSmartPointer<StreamProcessor> Processor(new XORStreamProcessor());

		if(PackageKey > 0)
			Processor.AsDerived<XORStreamProcessor>()->Key = (uint8)PackageKey;

		In->SetProcessor(Processor);

		PackageFileSystemManager::Package ThePackage;

		if(!ThePackage.DeSerialize(In))
		{
			Log::Instance.LogErr(TAG, "Failed to deserialize input file '%s'", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};

		ThePackage.PackageStream = In;

		for(PackageFileSystemManager::Package::EntryMap::iterator it = ThePackage.Entries.begin(); it != ThePackage.Entries.end(); it++)
		{
			for(PackageFileSystemManager::Package::FileMap::iterator fit = it->second.begin(); fit != it->second.end(); fit++)
			{
				std::string Directory = fit->second->DirectoryName;
				std::string FileName = fit->second->Name;

				std::string TempDirectory = ActualOutPackageDirectory;

				std::vector<std::string> Components = StringUtils::Split(Directory, '/');

				for(uint32 i = 0; i < Components.size(); i++)
				{
					TempDirectory += "/" + Components[i];

					FileSystemUtils::CreateDirectory(Path(TempDirectory).FullPath());
				};

				FileStream Out;

				SuperSmartPointer<PackageFileSystemManager::PackageStream> PIn(new PackageFileSystemManager::PackageStream());

				PIn->Source = ThePackage.PackageStream;
				PIn->StartOffset = fit->second->Offset;
				PIn->LengthValue = fit->second->Length;
				PIn->PositionValue = 0;

				if(!Out.Open(Path(TempDirectory, FileName).FullPath(), StreamFlags::Write) || !PIn->CopyTo(&Out))
				{
					Log::Instance.LogWarn(TAG, "Failed to restore file '%s%s'", Directory.c_str(), FileName.c_str());

					continue;
				};
			};
		};
	}
	else
	{
		Log::Instance.LogInfo(TAG, "Searching Directories...");

		for(uint32 i = 0; i < RequestedDirectories.size(); i++)
		{
			std::vector<std::string> ReceivedFiles = FileSystemUtils::ScanDirectory(RequestedDirectories[i], "*", true);

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

			std::string FinalName = Path(FinalFiles[i].first).BaseName;

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

			DeInitSubsystems();

			return 1;
		};

		SuperSmartPointer<XORStreamProcessor> Processor(new XORStreamProcessor());

		if(PackageKey > 0)
			Processor->Key = (uint8)PackageKey;

		Stream->SetProcessor(Processor);

		if(!Package->Serialize(Stream))
		{
			Log::Instance.LogInfo(TAG, "Failed to save package data into '%s'!", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};

		Log::Instance.LogInfo(TAG, "Success! Now verifying package data!");

		ActualStream->Close();

		Package.Dispose();

		if(!ActualStream->Open(OutPackageName, StreamFlags::Read))
		{
			Log::Instance.LogInfo(TAG, "Unable to open '%s' for reading!", OutPackageName.c_str());
			
			DeInitSubsystems();

			return 1;
		};

		Package.Reset(new PackageFileSystemManager::Package());

		if(!Package->FromStream(Stream))
		{
			Log::Instance.LogInfo(TAG, "Unable to deserialize package data from '%s'!", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};
	};

	Log::Instance.LogInfo(TAG, "All OK!");

	DeInitSubsystems();

	return 0;
};
