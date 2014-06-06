#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_SOUND
	SoundManager SoundManager::Instance;

	void SoundManager::Music::Play()
	{
		TheMusic.play();
	};

	void SoundManager::Music::Stop()
	{
		TheMusic.stop();
	};

	void SoundManager::Music::Pause()
	{
		TheMusic.pause();
	};

	void SoundManager::Music::SetVolume(f32 Volume)
	{
		TheMusic.setVolume(Volume * 100);
	};

	f32 SoundManager::Music::GetVolume()
	{
		return TheMusic.getVolume() / 100;
	};

	sf::Music::Status SoundManager::Music::GetStatus()
	{
		return TheMusic.getStatus();
	};

	bool SoundManager::Music::IsPlaying()
	{
		return TheMusic.getStatus() == sf::Music::Playing;
	};

	void SoundManager::Music::SetLoop(bool Value)
	{
		TheMusic.setLoop(Value);
	};

	bool SoundManager::Music::IsLooping()
	{
		return TheMusic.getLoop();
	};

	void SoundManager::Music::SetPitch(f32 Pitch)
	{
		TheMusic.setPitch(Pitch);
	};

	f32 SoundManager::Music::GetPitch()
	{
		return TheMusic.getPitch();
	};

	void SoundManager::Sound::Play()
	{
		TheSound.play();
	};

	void SoundManager::Sound::Stop()
	{
		TheSound.stop();
	};

	void SoundManager::Sound::Pause()
	{
		TheSound.pause();
	};

	void SoundManager::Sound::SetVolume(f32 Volume)
	{
		TheSound.setVolume(Volume * 100);
	};

	f32 SoundManager::Sound::GetVolume()
	{
		return TheSound.getVolume() / 100;
	};
	
	sf::Sound::Status SoundManager::Sound::GetStatus()
	{
		return TheSound.getStatus();
	};

	bool SoundManager::Sound::IsPlaying()
	{
		return GetStatus() == sf::Sound::Playing;
	};

	void SoundManager::Sound::SetLoop(bool value)
	{
		TheSound.setLoop(value);
	};

	bool SoundManager::Sound::IsLooping()
	{
		return TheSound.getLoop();
	};

	SuperSmartPointer<SoundManager::Sound> SoundManager::GetSound(StringID ID)
	{
		if(!WasStarted)
		{
			return SuperSmartPointer<Sound>();
		};

		SoundMap::iterator it = Sounds.find(ID);

		if(it == Sounds.end())
		{
			return SuperSmartPointer<Sound>();
		};

		if(it->second.Get() == NULL)
		{
			Sounds.erase(it);

			return SuperSmartPointer<Sound>();
		};

		return it->second;
	};

	SuperSmartPointer<SoundManager::Music> SoundManager::GetMusic(StringID ID)
	{
		if(!WasStarted)
		{
			return SuperSmartPointer<Music>();
		};

		MusicMap::iterator it = Musics.find(ID);

		if(it == Musics.end())
		{
			return SuperSmartPointer<Music>();
		};

		if(it->second.Get() == NULL)
		{
			Musics.erase(it);

			return SuperSmartPointer<Music>();
		};

		return it->second;
	};

	StringID SoundManager::SoundFromStream(Stream *In)
	{
		if(!WasStarted)
		{
			return 0xFFFFFFFF;
		};

		std::stringstream str;
		str << "SOUND__STREAM__" << std::hex << (uint32)In << GameClockTimeNoPause();

		StringID SoundID = MakeStringID(str.str());

		SoundMap::iterator sit = Sounds.find(SoundID);

		FLASSERT(sit == Sounds.end(), "Sound already loaded!");

		if(sit != Sounds.end())
		{
			return 0xFFFFFFFF;
		};

		SoundBufferMap::iterator it = SoundBuffers.find(SoundID);

		SuperSmartPointer<sf::SoundBuffer> Buffer;

		if(it != SoundBuffers.end() && it->second.Get() != NULL)
		{
			Buffer = it->second;
		}
		else
		{
			std::vector<uint8> Data((uint32)(In->Length() - In->Position()));

			if(!In->Read2<uint8>(&Data[0], Data.size()))
				return 0xFFFFFFFF;

			Buffer.Reset(new sf::SoundBuffer());

			if(!Buffer->loadFromMemory(&Data[0], Data.size()))
			{
				return 0xFFFFFFFF;
			};
		};

		FLASSERT(Buffer.Get(), "Invalid Sound Buffer!");

		if(!Buffer.Get())
		{
			return 0xFFFFFFFF;
		};

		SoundBuffers[SoundID] = Buffer;

		SuperSmartPointer<Sound> TheSound(new Sound());
		TheSound->TheSound.setBuffer(*Buffer);

		Sounds[SoundID] = TheSound;

		return SoundID;
	};

	StringID SoundManager::SoundFromPackage(StringID Directory, StringID FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		SuperSmartPointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(Directory, FileName);

		if(PackageStream.Get() == NULL)
		{
			return 0xFFFFFFFF;
		};

		return SoundFromStream(PackageStream.Get());
	};

	StringID SoundManager::SoundFromFile(const char *FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		SuperSmartPointer<FileStream> Stream(new FileStream());

		if(!Stream->Open(FileName, StreamFlags::Read))
		{
			return 0xFFFFFFFF;
		};

		return SoundFromStream(Stream.Get());
	};

	StringID SoundManager::MusicFromStream(Stream *In)
	{
		if(!WasStarted)
		{
			return 0xFFFFFFFF;
		};

		std::stringstream str;
		str << "MUSIC__STREAM__" << std::hex << (uint32)In << GameClockTimeNoPause();

		StringID MusicID = MakeStringID(str.str());

		MusicMap::iterator sit = Musics.find(MusicID);

		FLASSERT(sit == Musics.end(), "Music already loaded!");

		if(sit != Musics.end())
		{
			return 0xFFFFFFFF;
		};

		SuperSmartPointer<Music> Out(new Music());

		Out->Data.resize((uint32)(In->Length() - In->Position()));

		if(!In->Read2<uint8>(&Out->Data[0], Out->Data.size()))
			return 0xFFFFFFFF;

		if(!Out->TheMusic.openFromMemory(&Out->Data[0], Out->Data.size()))
		{
			return 0xFFFFFFFF;
		};

		Musics[MusicID] = Out;

		return MusicID;
	};

	StringID SoundManager::MusicFromPackage(StringID Directory, StringID FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		SuperSmartPointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(Directory, FileName);

		if(PackageStream.Get() == NULL)
		{
			return 0xFFFFFFFF;
		};

		return MusicFromStream(PackageStream.Get());
	};

	StringID SoundManager::MusicFromFile(const char *FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		SuperSmartPointer<FileStream> Stream(new FileStream());

		if(!Stream->Open(FileName, StreamFlags::Read))
		{
			return 0xFFFFFFFF;
		};

		return MusicFromStream(Stream.Get());
	};


	void SoundManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("SoundManager", "SoundManager Subsystem starting..");
	};

	void SoundManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("SoundManager", "SoundManager Subsystem terminating..");

		while(Sounds.begin() != Sounds.end())
		{
			Sounds.begin()->second.Dispose();
			Sounds.erase(Sounds.begin());
		};

		while(SoundBuffers.begin() != SoundBuffers.end())
		{
			SoundBuffers.begin()->second.Dispose();
			SoundBuffers.erase(SoundBuffers.begin());
		};

		while(Musics.begin() != Musics.end())
		{
			Musics.begin()->second.Dispose();
			Musics.erase(Musics.begin());
		};

		SubSystem::Shutdown(Priority);
	};

	void SoundManager::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();
	};

#endif
};
