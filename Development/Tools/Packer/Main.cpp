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

	g_Log.Register();

	g_Log.FolderName = FLGameName();

	if(argc == 1)
	{
		g_Log.LogInfo(TAG, "Usage: %s -dir [folder] [folder in package] -file [file][folder in package] "
			"-out [filename.package] [-key] [package key]\nPackage Key must be a 2-digit hex number", argv[0]);

		g_Log.LogInfo(TAG, "Alternative Usage: %s -extract [package] -out [outdir] [-key] [package key]\nPackage Key must be a 2-digit hex number", argv[0]);

		return 1;
	};

	g_PackageManager.Register();

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

	g_Log.LogInfo(TAG, "Version %s starting up", CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
	g_Log.LogInfo(TAG, "Output %s: %s", Extract ? "Directory": "File", OutPackageName.c_str());

	if(Extract)
	{
		std::string ActualOutPackageDirectory = FileSystemUtils::ActiveDirectory();

		g_Log.LogInfo(TAG, "Preparing to extract to directory '%s'", Path(ActualOutPackageDirectory).FullPath().c_str());

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

		DisposablePointer<FileStream> In(new FileStream());

		if(!In->Open((FileSystemUtils::ActiveDirectory() + "/" + OutPackageName).c_str(), StreamFlags::Read))
		{
			g_Log.LogErr(TAG, "Failed to read input file '%s'", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};

		DisposablePointer<StreamProcessor> Processor(new XORStreamProcessor());

		if(PackageKey > 0)
			Processor.AsDerived<XORStreamProcessor>()->Key = (uint8)PackageKey;

		In->SetProcessor(Processor);

		PackageFileSystemManager::Package ThePackage;

		if(!ThePackage.DeSerialize(In))
		{
			g_Log.LogErr(TAG, "Failed to deserialize input file '%s'", OutPackageName.c_str());

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

				DisposablePointer<PackageFileSystemManager::PackageStream> PIn(new PackageFileSystemManager::PackageStream());

				PIn->Source = ThePackage.PackageStream;
				PIn->StartOffset = fit->second->Offset;
				PIn->LengthValue = fit->second->Length;
				PIn->PositionValue = 0;

				if(!Out.Open(Path(TempDirectory + "/" + FileName).FullPath(), StreamFlags::Write) || !PIn->CopyTo(&Out))
				{
					g_Log.LogWarn(TAG, "Failed to restore file '%s%s'", Directory.c_str(), FileName.c_str());

					continue;
				};
			};
		};
	}
	else
	{
		g_Log.LogInfo(TAG, "Searching Directories...");

		for(uint32 i = 0; i < RequestedDirectories.size(); i++)
		{
			std::vector<std::string> ReceivedFiles = FileSystemUtils::ScanDirectory(RequestedDirectories[i], "*", true);

			for(uint32 j = 0; j < ReceivedFiles.size(); j++)
			{
				std::pair<std::string, std::string> Pair(ReceivedFiles[j], TargetDirectories[i]);
				FinalFiles.push_back(Pair);
				FinalDirectories.push_back(RequestedDirectories[i]);
			};

			g_Log.LogInfo(TAG, "... %s (as %s, %d files)", RequestedDirectories[i].c_str(), TargetDirectories[i].c_str(),
				ReceivedFiles.size());
		};

		g_Log.LogInfo(TAG, "Adding extra files...");

		for(uint32 i = 0; i < RequestedFiles.size(); i++)
		{
			std::pair<std::string, std::string> Pair("/" + RequestedFiles[i], TargetDirectories[i + RequestedDirectories.size()]);

			FinalFiles.push_back(Pair);
			FinalDirectories.push_back(std::string());

			g_Log.LogInfo(TAG, "... %s (as %s)", RequestedDirectories[i].c_str(), TargetDirectories[i + RequestedDirectories.size()].c_str());
		};

		DisposablePointer<PackageFileSystemManager::Package> Package = g_PackageManager.NewPackage();

		g_Log.LogInfo(TAG, "Adding Files to Package Info...");

		for(uint32 i = 0; i < FinalFiles.size(); i++)
		{
			DisposablePointer<FileStream> Stream(new FileStream);

			if(!Stream->Open(FinalFiles[i].first, StreamFlags::Read))
			{
				g_Log.LogInfo(TAG, "... Unable to open '%s', skipping...", FinalFiles[i].first.c_str());

				continue;
			};

			std::string FinalDirectory = std::string("/") + (FinalFiles[i].second.length() ?
				FinalFiles[i].second + std::string("/") : "") + FinalFiles[i].first.substr(FinalDirectories[i].length() + 1,
				FinalFiles[i].first.rfind('/') - FinalDirectories[i].length());

			std::string FinalName = Path(FinalFiles[i].first).BaseName;

			if(!Package->AddFile(FinalDirectory, FinalName, Stream))
			{
				g_Log.LogInfo(TAG, "... Unable to add '%s', skipping...", FinalFiles[i].first.c_str());

				continue;
			};

			g_Log.LogInfo(TAG, "... Added %s%s (%08x/%08x, %lld bytes)", FinalDirectory.c_str(), FinalName.c_str(), 
				MakeStringID(FinalDirectory), MakeStringID(FinalName), Stream->Length());
		};

		g_Log.LogInfo(TAG, "Final Packing!");

		DisposablePointer<Stream> Stream(new FileStream());
		FileStream *ActualStream = (FileStream *)Stream.Get();

		if(!ActualStream->Open(OutPackageName, StreamFlags::Write))
		{
			g_Log.LogInfo(TAG, "Unable to open '%s' for writing!", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};

		DisposablePointer<XORStreamProcessor> Processor(new XORStreamProcessor());

		if(PackageKey > 0)
			Processor->Key = (uint8)PackageKey;

		Stream->SetProcessor(Processor);

		if(!Package->Serialize(Stream))
		{
			g_Log.LogInfo(TAG, "Failed to save package data into '%s'!", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};

		g_Log.LogInfo(TAG, "Success! Now verifying package data!");

		ActualStream->Close();

		Package.Dispose();

		if(!ActualStream->Open(OutPackageName, StreamFlags::Read))
		{
			g_Log.LogInfo(TAG, "Unable to open '%s' for reading!", OutPackageName.c_str());
			
			DeInitSubsystems();

			return 1;
		};

		Package.Reset(new PackageFileSystemManager::Package());

		if(!Package->FromStream(Stream))
		{
			g_Log.LogInfo(TAG, "Unable to deserialize package data from '%s'!", OutPackageName.c_str());

			DeInitSubsystems();

			return 1;
		};
	};

	g_Log.LogInfo(TAG, "All OK!");

	DeInitSubsystems();

	return 0;
};
