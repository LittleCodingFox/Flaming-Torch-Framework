#include "FlamingCore.hpp"
#include <Tmx.h>
#include <vector>
#include <map>
#include <sstream>
#include <stdio.h>
#include <SFML/System.hpp>

using namespace FlamingTorch;

#define VERSION_MAJOR 0
#define VERSION_MINOR 5
#define TAG "TexturePacker"

std::string FLGameName()
{
	return "TexturePacker";
};

int32 MaxWidth = 4096, MaxHeight = 4096;

/*!
*	Expects Config File in this form:
*
*	[Animations]
*	Walk=Frame1.png|Frame2.png|Frame3.png|Frame4.png
*	Run=Frame2.png|Frame1.png|Frame5.png
*
*	Outputs PNG File and CFG File in this form:
*
*	[Animations]
*	Walk=x,y,width,height,index|x,y,width,height,index
*/
int main(int argc, char **argv)
{
#if FLPLATFORM_WINDOWS
	EnableMinidumps(FLGameName().c_str(), CoreUtils::MakeVersionString(VERSION_MAJOR, VERSION_MINOR).c_str());
#endif

	Log::Instance.Register();
	ResourceManager::Instance.Register();

	Log::Instance.FolderName = FLGameName();

	InitSubsystems();

	if(argc == 1)
	{
		Log::Instance.LogInfo(TAG, "Usage: %s [-dir outdirectory] [-resdir resourcedirectory] [-maxwidth width] [-maxheight height] [-out outfilename] filename", argv[0]);
		Log::Instance.LogInfo(TAG, "Default Max Width and Height: 4096x4096");

		DeInitSubsystems();

		return 1;
	};

	std::string OutDirectory(".");
	std::vector<std::string> ResourceDirectories;
	std::string FileName, OutFileName;

	for(int32 i = 1; i < argc; i++)
	{
		if(std::string(argv[i]) == "-dir")
		{
			if(i + 1 < argc)
			{
				OutDirectory = argv[i + 1];

				Log::Instance.LogInfo(TAG, "Set Out directory '%s'", argv[i + 1]);

				i++;
			};
		}
		else if(std::string(argv[i]) == "-resdir")
		{
			if(i + 1 < argc)
			{
				ResourceDirectories.push_back(argv[i + 1]);

				Log::Instance.LogInfo(TAG, "Added resource directory '%s'", argv[i + 1]);

				i++;
			};
		}
		else if(std::string(argv[i]) == "-maxwidth")
		{
			int32 Temp = 0;

			if(i + 1 < argc && 1 == sscanf(argv[i], "%d", &Temp))
			{
				MaxWidth = Temp;

				i++;
			};
		}
		else if(std::string(argv[i]) == "-maxheight")
		{
			int32 Temp = 0;

			if(i + 1 < argc && 1 == sscanf(argv[i], "%d", &Temp))
			{
				MaxHeight = Temp;

				i++;
			};
		}
		else if(std::string(argv[i]) == "-out")
		{
			if(i + 1 < argc)
			{
				OutFileName = argv[i + 1];

				Log::Instance.LogInfo(TAG, "Set Out file '%s'", argv[i + 1]);

				i++;
			};
		}
		else
		{
			FileName = Path(argv[i]).FullPath();

			if(OutFileName.length() == 0)
			{
				OutFileName = Path(FileName).BaseName;
			};
		};
	};

	Log::Instance.LogInfo(TAG, "Starting version %d.%d", VERSION_MAJOR, VERSION_MINOR);
	Log::Instance.LogInfo(TAG, "Will store compiled data to directory '%s' as '%s'", OutDirectory.c_str(), OutFileName.c_str());
	Log::Instance.LogInfo(TAG, "Maximum size: %dx%d", MaxWidth, MaxHeight);
	Log::Instance.LogInfo(TAG, "Processing '%s'", FileName.c_str());

	SuperSmartPointer<FileStream> Stream(new FileStream());
	GenericConfig InConfig;

	if(!Stream->Open(FileName, StreamFlags::Read | StreamFlags::Text) || !InConfig.DeSerialize(Stream))
	{
		Log::Instance.LogInfo(TAG, "Unable to open '%s' for reading!", FileName.c_str());

		DeInitSubsystems();

		return 1;
	};

	Stream.Dispose();

	GenericConfig::Section &AnimationsSection = InConfig.Sections["Animations"];

	std::vector<SuperSmartPointer<Texture> > Frames;
	std::map<std::string, uint32> AnimationIndices;

	Log::Instance.LogInfo(TAG, "Parsing Animations...");

	//Read the animation frames
	for(GenericConfig::Section::ValueMap::iterator it = AnimationsSection.Values.begin(); it != AnimationsSection.Values.end(); it++)
	{
		std::string AnimationName = it->first;

		std::vector<std::string> AnimationFrames = StringUtils::Split(it->second.Content, '|');

		Log::Instance.LogInfo(TAG, "... %s (%d frames)", AnimationName.c_str(), AnimationFrames.size());

		for(uint32 i = 0; i < AnimationFrames.size(); i++)
		{
			SuperSmartPointer<Texture> Frame = ResourceManager::Instance.GetTexture(AnimationFrames[i]);
			
			if(!Frame)
			{
				for(uint32 j = 0; j < ResourceDirectories.size(); j++)
				{
					Frame = ResourceManager::Instance.GetTexture(Path(ResourceDirectories[j] + "/" + AnimationFrames[i]).FullPath());

					if(Frame)
						break;
				};
			};

			if(!Frame.Get())
			{
				Log::Instance.LogWarn(TAG, "Unable to load frame '%s' for animation '%s'", AnimationFrames[i].c_str(), AnimationName.c_str());

				continue;
			};

			if(AnimationIndices.find(AnimationName) == AnimationIndices.end())
			{
				AnimationIndices[AnimationName] = Frames.size();
			};
			
			Log::Instance.LogWarn(TAG, "Loaded frame '%s'", AnimationFrames[i].c_str());

			Frames.push_back(Frame);
		};
	};

	SuperSmartPointer<TexturePacker> Packed = TexturePacker::FromTextures(Frames, MaxWidth, MaxHeight);

	if(Packed.Get() == NULL || Packed->Indices.size() != Frames.size())
	{
		Log::Instance.LogErr(TAG, "Unable to pack all textures together; Make sure you have a large enough size and don't have too many textures set up. "
			"Current Max Size: %dx%d", MaxWidth, MaxHeight);

		DeInitSubsystems();

		return 1;
	};

	GenericConfig OutConfig;

	//Write the animation data
	for(std::map<std::string, uint32>::iterator it = AnimationIndices.begin(); it != AnimationIndices.end(); it++)
	{
		static std::stringstream str;

		str.str("");

		uint32 Start = it->second;
		uint32 End = Frames.size();

		//Get the end index
		for(std::map<std::string, uint32>::iterator tit = AnimationIndices.begin(); tit != AnimationIndices.end(); tit++)
		{
			if(tit->second > Start && tit->second < End)
				End = tit->second;
		};

		for(uint32 i = Start; i != End; i++)
		{
			TexturePacker::SortedTexture &Index = Packed->Indices[i];

			str << (i > Start ? "|" : "") << Index.x << "," << Index.y << "," << Index.Width << "," << Index.Height << "," << Index.Index;
		};

		OutConfig.SetValue("Animations", it->first.c_str(), str.str().c_str());
	};

	SuperSmartPointer<FileStream> OutStream(new FileStream());

	if(!OutFileName.length())
	{
		std::string OutFileName = Path(FileName).BaseName;

		OutFileName = OutFileName.substr(0, OutFileName.rfind('.')) + "_out";
	};

	std::string OutFileNamePNG = Path(OutDirectory + "/" + OutFileName.c_str()).ChangeExtension("png").FullPath(), OutFileNameCFG = Path(OutDirectory + "/" + OutFileName.c_str()).ChangeExtension("cfg").FullPath();

	TextureEncoderInfo TEInfo;
	TEInfo.Encoder = TextureEncoderType::PNG;

	if(!OutStream->Open(OutFileNamePNG, StreamFlags::Write) || !Packed->MainTexture->GetData()->Save(OutStream, TEInfo))
	{
		Log::Instance.LogErr(TAG, "Unable to open '%s' for writing", OutFileNamePNG.c_str());

		DeInitSubsystems();

		return 1;
	};

	OutStream->Close();

	if(!OutStream->Open(OutFileNameCFG, StreamFlags::Write | StreamFlags::Text) || !OutConfig.Serialize(OutStream))
	{
		Log::Instance.LogErr(TAG, "Unable to open '%s' for writing", OutFileNameCFG.c_str());

		DeInitSubsystems();

		return 1;
	};

	OutStream->Close();

	Log::Instance.LogInfo(TAG, "Wrote '%d' Frames and '%d' Animations", Frames.size(), AnimationIndices.size());

	Log::Instance.LogInfo(TAG, "Testing for correct deserialization");

	SuperSmartPointer<Texture> MainTexture = Packed->MainTexture;

	if(!OutStream->Open(OutFileNameCFG, StreamFlags::Read | StreamFlags::Text) || !OutConfig.DeSerialize(OutStream))
	{
		Log::Instance.LogErr(TAG, "Unable to open '%s' for reading", OutFileNameCFG.c_str());

		DeInitSubsystems();

		return 1;
	};

	Packed = TexturePacker::FromConfig(MainTexture, OutConfig);

	Log::Instance.LogInfo(TAG, "Status of reloading: %s", Packed.Get() && Packed->Indices.size() == Frames.size() ? "OK" : "FAIL");

	Packed.Dispose();

	OutStream.Dispose();

	DeInitSubsystems();

	return 0;
};
