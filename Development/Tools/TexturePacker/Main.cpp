#include "FlamingCore.hpp"
#include <Tmx.h>
#include <vector>
#include <map>
#include <sstream>
#include <stdio.h>

using namespace FlamingTorch;

#define VERSION_MAJOR 0
#define VERSION_MINOR 5
#define TAG "TexturePacker"

std::string FLGameName()
{
	return "TexturePacker";
};

int32 MaxWidth = 4096, MaxHeight = 4096, Padding = 1, WidthOverride = -1, HeightOverride = -1;

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

	g_Log.Register();
	g_ResourceManager.Register();

	g_Log.FolderName = FLGameName();

	InitSubsystems();

	if(argc == 1)
	{
		g_Log.LogInfo(TAG, "Usage: %s [-dir outdirectory] [-resdir resourcedirectory] [-maxwidth width] [-maxheight height] [-padding pixels] [-widthoverride width] [-heightoverride height] [-out outfilename] filename", argv[0]);
		g_Log.LogInfo(TAG, "Default Max Width and Height: 4096x4096");

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

				g_Log.LogInfo(TAG, "Set Out directory '%s'", argv[i + 1]);

				i++;
			};
		}
		else if(std::string(argv[i]) == "-resdir")
		{
			if(i + 1 < argc)
			{
				ResourceDirectories.push_back(argv[i + 1]);

				g_Log.LogInfo(TAG, "Added resource directory '%s'", argv[i + 1]);

				i++;
			};
		}
		else if(std::string(argv[i]) == "-maxwidth")
		{
			int32 Temp = 0;

			if(i + 1 < argc && 1 == sscanf(argv[i + 1], "%d", &Temp))
			{
				MaxWidth = Temp;

				i++;
			};
		}
		else if(std::string(argv[i]) == "-maxheight")
		{
			int32 Temp = 0;

			if(i + 1 < argc && 1 == sscanf(argv[i + 1], "%d", &Temp))
			{
				MaxHeight = Temp;

				i++;
			};
		}
		else if (std::string(argv[i]) == "-widthoverride")
		{
			int32 Temp = 0;

			if (i + 1 < argc && 1 == sscanf(argv[i + 1], "%d", &Temp))
			{
				WidthOverride = Temp;

				i++;
			};
		}
		else if (std::string(argv[i]) == "-heightoverride")
		{
			int32 Temp = 0;

			if (i + 1 < argc && 1 == sscanf(argv[i + 1], "%d", &Temp))
			{
				HeightOverride = Temp;

				i++;
			};
		}
		else if (std::string(argv[i]) == "-padding")
		{
			int32 Temp = 0;

			if (i + 1 < argc && 1 == sscanf(argv[i + 1], "%d", &Temp))
			{
				Padding = Temp;

				i++;
			};
		}
		else if(std::string(argv[i]) == "-out")
		{
			if(i + 1 < argc)
			{
				OutFileName = argv[i + 1];

				g_Log.LogInfo(TAG, "Set Out file '%s'", argv[i + 1]);

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

	if (WidthOverride <= 0 || HeightOverride <= 0)
	{
		WidthOverride = HeightOverride = -1;
	};

	g_Log.LogInfo(TAG, "Starting version %d.%d", VERSION_MAJOR, VERSION_MINOR);
	g_Log.LogInfo(TAG, "Will store compiled data to directory '%s' as '%s'", OutDirectory.c_str(), OutFileName.c_str());
	g_Log.LogInfo(TAG, "Maximum size: %dx%d", MaxWidth, MaxHeight);

	if (WidthOverride > 0)
	{
		g_Log.LogInfo(TAG, "Size override: %dx%d", WidthOverride, HeightOverride);
	}

	g_Log.LogInfo(TAG, "Padding: %d", Padding);
	g_Log.LogInfo(TAG, "Processing '%s'", FileName.c_str());

	DisposablePointer<FileStream> Stream(new FileStream());
	GenericConfig InConfig;

	if(!Stream->Open(FileName, StreamFlags::Read | StreamFlags::Text) || !InConfig.DeSerialize(Stream))
	{
		g_Log.LogInfo(TAG, "Unable to open '%s' for reading!", FileName.c_str());

		DeInitSubsystems();

		return 1;
	};

	Stream.Dispose();

	GenericConfig::Section &AnimationsSection = InConfig.Sections["Animations"];

	std::vector<DisposablePointer<Texture> > Frames;
	std::map<std::string, uint32> AnimationIndices;

	g_Log.LogInfo(TAG, "Parsing Animations...");

	//Read the animation frames
	for(GenericConfig::Section::ValueMap::iterator it = AnimationsSection.Values.begin(); it != AnimationsSection.Values.end(); it++)
	{
		std::string AnimationName = it->first;

		std::vector<std::string> AnimationFrames = StringUtils::Split(it->second.Content, '|');

		g_Log.LogInfo(TAG, "... %s (%d frames)", AnimationName.c_str(), AnimationFrames.size());

		for(uint32 i = 0; i < AnimationFrames.size(); i++)
		{
			DisposablePointer<Texture> Frame = g_ResourceManager.GetTexture(AnimationFrames[i]);
			
			if(!Frame)
			{
				for(uint32 j = 0; j < ResourceDirectories.size(); j++)
				{
					Frame = g_ResourceManager.GetTexture(Path(ResourceDirectories[j] + "/" + AnimationFrames[i]).FullPath());

					if(Frame)
						break;
				};
			};

			if(!Frame.Get())
			{
				g_Log.LogWarn(TAG, "Unable to load frame '%s' for animation '%s'", AnimationFrames[i].c_str(), AnimationName.c_str());

				continue;
			};

			if(AnimationIndices.find(AnimationName) == AnimationIndices.end())
			{
				AnimationIndices[AnimationName] = Frames.size();
			};

			if (WidthOverride > 0 && (Frame->Width() > (uint32)WidthOverride || Frame->Height() > (uint32)HeightOverride))
			{
				g_Log.LogWarn(TAG, "Unable to load frame '%s' for animation '%s' because it is bigger than the size override '%dx%d'", AnimationFrames[i].c_str(), AnimationName.c_str(), WidthOverride, HeightOverride);

				continue;
			};
			
			g_Log.LogWarn(TAG, "Loaded frame '%s'", AnimationFrames[i].c_str());

			if (WidthOverride > 0)
			{
				DisposablePointer<TextureBuffer> ResizeBuffer(TextureBuffer::CreateFromColor(WidthOverride, HeightOverride));

				ResizeBuffer->Blend(0, 0, Frame->GetData());

				Frame = Texture::CreateFromBuffer(ResizeBuffer);
			};

			Frames.push_back(Frame);
		};
	};

	DisposablePointer<TexturePacker> Packed = TexturePacker::FromTextures(Frames, MaxWidth, MaxHeight, Padding);

	if(Packed.Get() == NULL || Packed->Indices.size() != Frames.size())
	{
		g_Log.LogErr(TAG, "Unable to pack all textures together; Make sure you have a large enough size and don't have too many textures set up. "
			"Current Max Size: %dx%d", MaxWidth, MaxHeight);

		DeInitSubsystems();

		return 1;
	};

	Packed->Bind();

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

	OutConfig.SetValue("Config", "Padding", StringUtils::MakeIntString(Padding).c_str());

	DisposablePointer<FileStream> OutStream(new FileStream());

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
		g_Log.LogErr(TAG, "Unable to open '%s' for writing", OutFileNamePNG.c_str());

		DeInitSubsystems();

		return 1;
	};

	OutStream->Close();

	if(!OutStream->Open(OutFileNameCFG, StreamFlags::Write | StreamFlags::Text) || !OutConfig.Serialize(OutStream))
	{
		g_Log.LogErr(TAG, "Unable to open '%s' for writing", OutFileNameCFG.c_str());

		DeInitSubsystems();

		return 1;
	};

	OutStream->Close();

	g_Log.LogInfo(TAG, "Wrote '%d' Frames and '%d' Animations", Frames.size(), AnimationIndices.size());

	g_Log.LogInfo(TAG, "Testing for correct deserialization");

	DisposablePointer<Texture> MainTexture = Packed->MainTexture;

	if(!OutStream->Open(OutFileNameCFG, StreamFlags::Read | StreamFlags::Text) || !OutConfig.DeSerialize(OutStream))
	{
		g_Log.LogErr(TAG, "Unable to open '%s' for reading", OutFileNameCFG.c_str());

		DeInitSubsystems();

		return 1;
	};

	Packed = TexturePacker::FromConfig(MainTexture, OutConfig);

	g_Log.LogInfo(TAG, "Status of reloading: %s", Packed.Get() && Packed->Indices.size() == Frames.size() ? "OK" : "FAIL");

	Packed.Dispose();

	OutStream.Dispose();

	DeInitSubsystems();

	return 0;
};
