#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_SOUND
#	define TAG "SoundManager"
	uint64 SoundManager::SoundCounter = 0, SoundManager::MusicCounter = 0;
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

	DisposablePointer<SoundManager::Sound> SoundManager::GetSound(StringID ID)
	{
		if(!WasStarted)
		{
			return DisposablePointer<Sound>();
		};

		SoundMap::iterator it = Sounds.find(ID);

		if(it == Sounds.end())
		{
			return DisposablePointer<Sound>();
		};

		if(it->second.Get() == NULL)
		{
			Sounds.erase(it);

			return DisposablePointer<Sound>();
		};

		return it->second;
	};

	DisposablePointer<SoundManager::Music> SoundManager::GetMusic(StringID ID)
	{
		if(!WasStarted)
		{
			return DisposablePointer<Music>();
		};

		MusicMap::iterator it = Musics.find(ID);

		if(it == Musics.end())
		{
			return DisposablePointer<Music>();
		};

		if(it->second.Get() == NULL)
		{
			Musics.erase(it);

			return DisposablePointer<Music>();
		};

		return it->second;
	};

	StringID SoundManager::SoundFromStream(Stream *In)
	{
		if(!WasStarted)
		{
			return 0xFFFFFFFF;
		};

		StringID SoundID = MakeStringID("SOUND__STREAM__" + StringUtils::PointerString(In) + "_" + StringUtils::MakeIntString(SoundCounter));

		SoundMap::iterator sit = Sounds.find(SoundID);

		if(sit != Sounds.end())
		{
			return sit->first;
		};

		SoundBufferMap::iterator it = SoundBuffers.find(SoundID);

		DisposablePointer<sf::SoundBuffer> Buffer;

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

		DisposablePointer<Sound> TheSound(new Sound());
		TheSound->TheSound.setBuffer(*Buffer);

		Sounds[SoundID] = TheSound;

		SoundCounter++;

		return SoundID;
	};

	StringID SoundManager::SoundFromPackage(StringID Directory, StringID FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		DisposablePointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(Directory, FileName);

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

		DisposablePointer<FileStream> Stream(new FileStream());

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
		str << "MUSIC__STREAM__" << std::hex << (uint64)In << GameClockTimeNoPause();

		StringID MusicID = MakeStringID("MUSIC__STREAM__" + StringUtils::PointerString(In) + "_" + StringUtils::MakeIntString(MusicCounter));

		MusicMap::iterator sit = Musics.find(MusicID);

		if(sit != Musics.end())
		{
			return sit->first;
		};

		DisposablePointer<Music> Out(new Music());

		Out->Data.resize((uint32)(In->Length() - In->Position()));

		if(!In->Read2<uint8>(&Out->Data[0], Out->Data.size()))
			return 0xFFFFFFFF;

		if(!Out->TheMusic.openFromMemory(&Out->Data[0], Out->Data.size()))
		{
			return 0xFFFFFFFF;
		};

		Musics[MusicID] = Out;

		MusicCounter++;

		return MusicID;
	};

	StringID SoundManager::MusicFromPackage(StringID Directory, StringID FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		DisposablePointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(Directory, FileName);

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

		DisposablePointer<FileStream> Stream(new FileStream());

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

		Log::Instance.LogInfo(TAG, "SoundManager Subsystem starting..");
	};

	void SoundManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "SoundManager Subsystem terminating..");

		while(Sounds.begin() != Sounds.end())
		{
			if(Sounds.begin()->second.Get())
				Sounds.begin()->second->Stop();

			Sounds.begin()->second.Dispose();
			Sounds.erase(Sounds.begin());
		};

		while(Musics.begin() != Musics.end())
		{
			if(Musics.begin()->second.Get())
				Musics.begin()->second->Stop();

			Musics.begin()->second.Dispose();
			Musics.erase(Musics.begin());
		};

		while(SoundBuffers.begin() != SoundBuffers.end())
		{
			SoundBuffers.begin()->second.Dispose();
			SoundBuffers.erase(SoundBuffers.begin());
		};

		SubSystem::Shutdown(Priority);
	};

	void SoundManager::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Cleanup();
	};

	void SoundManager::Cleanup()
	{
		uint32 TotalObjects = Sounds.size() + Musics.size();

		for(SoundMap::iterator it = Sounds.begin(); it != Sounds.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.use_count() == 1)
			{
				if (it->second.Get())
					it->second->Stop();

				Sounds.erase(it);
				it = Sounds.begin();

				if(it == Sounds.end())
					break;
			};

			if(it == Sounds.end())
				break;
		};

		for(MusicMap::iterator it = Musics.begin(); it != Musics.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.use_count() == 1)
			{
				if (it->second.Get())
					it->second->Stop();

				Musics.erase(it);
				it = Musics.begin();

				if(it == Musics.end())
					break;
			};

			if(it == Musics.end())
				break;
		};

		uint32 CurrentObjects = Sounds.size() + Musics.size();

		if(CurrentObjects != TotalObjects)
		{
			Log::Instance.LogDebug(TAG, "Cleared %d objects (Prev: %d/Now: %d)", TotalObjects - CurrentObjects, TotalObjects, CurrentObjects);
		};
	};
#endif
};
