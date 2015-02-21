#include "FlamingCore.hpp"
#if USE_SOUND && USE_SFML_SOUNDMANAGER
namespace FlamingTorch
{
#	define TAG "SFMLSoundManager"

#	define REPORT_INVALID() g_Log.LogWarn(TAG, "%s call to invalid handle", __FUNCTION__);

	uint64 SFMLSoundManagerImplementation::SoundCounter = 0, SFMLSoundManagerImplementation::MusicCounter = 0;

	SFMLSoundManagerImplementation::~SFMLSoundManagerImplementation()
	{
		for (uint64 i = 1; i <= SoundCounter; i++)
			DestroySound(i);

		for (uint64 i = 1; i <= MusicCounter; i++)
			DestroyMusic(i);
	}

	MusicHandle SFMLSoundManagerImplementation::CreateMusic(const void *Source, uint32 Length)
	{
		DisposablePointer<MusicInfo> Info(new MusicInfo());

		Info->Data.resize(Length);

		memcpy(&Info->Data[0], Source, Length);

		Info->Music.Reset(new sf::Music());

		if (!Info->Music->openFromMemory(&Info->Data[0], Length))
			return INVALID_FTGHANDLE;

		MusicHandle Out = ++MusicCounter;
		
		Musics[Out] = Info;

		return Out;
	}

	SoundHandle SFMLSoundManagerImplementation::CreateSound(const void *Source, uint32 Length)
	{
		DisposablePointer<SoundInfo> Info(new SoundInfo());

		DisposablePointer<sf::SoundBuffer> Buffer(new sf::SoundBuffer());

		if (!Buffer->loadFromMemory(Source, Length))
			return INVALID_FTGHANDLE;

		Info->Buffer = Buffer;
		Info->Sound.Reset(new sf::Sound());
		Info->Sound->setBuffer(*Buffer);

		SoundHandle Out = ++SoundCounter;

		Sounds[Out] = Info;

		return Out;
	}

	void SFMLSoundManagerImplementation::DestroyMusic(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
			return;

		it->second->Music.Dispose();
		it->second.Dispose();

		Musics.erase(it);
	}

	void SFMLSoundManagerImplementation::DestroySound(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
			return;

		it->second->Sound->stop();
		it->second->Sound.Dispose();
		it->second->Buffer.Dispose();
		it->second.Dispose();

		Sounds.erase(it);
	}

	void SFMLSoundManagerImplementation::SetMusicVolume(MusicHandle Handle, f32 Volume)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Music->setVolume(Volume * 100);
	}

	void SFMLSoundManagerImplementation::SetSoundVolume(SoundHandle Handle, f32 Volume)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Sound->setVolume(Volume * 100);
	}

	f32 SFMLSoundManagerImplementation::GetMusicVolume(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return 0;
		}

		return it->second->Music->getVolume() / 100;
	}

	f32 SFMLSoundManagerImplementation::GetSoundVolume(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return 0;
		}

		return it->second->Sound->getVolume() / 100;
	}

	uint8 SFMLSoundManagerImplementation::GetMusicStatus(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return SoundStatus::Stopped;
		}

		switch (it->second->Music->getStatus())
		{
		case sf::SoundSource::Playing:
			return SoundStatus::Playing;

			break;

		case sf::SoundSource::Paused:
			return SoundStatus::Paused;

			break;
		}

		return SoundStatus::Stopped;
	}

	uint8 SFMLSoundManagerImplementation::GetSoundStatus(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return SoundStatus::Stopped;
		}

		switch (it->second->Sound->getStatus())
		{
		case sf::SoundSource::Playing:
			return SoundStatus::Playing;

			break;

		case sf::SoundSource::Paused:
			return SoundStatus::Paused;

			break;
		}

		return SoundStatus::Stopped;
	}

	void SFMLSoundManagerImplementation::SetMusicLooping(MusicHandle Handle, bool Loop)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Music->setLoop(Loop);
	}

	void SFMLSoundManagerImplementation::SetSoundLooping(SoundHandle Handle, bool Loop)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Sound->setLoop(Loop);
	}

	bool SFMLSoundManagerImplementation::MusicLooping(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return false;
		}

		return it->second->Music->getLoop();
	}

	bool SFMLSoundManagerImplementation::SoundLooping(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return false;
		}

		return it->second->Sound->getLoop();
	}

	void SFMLSoundManagerImplementation::SetMusicPitch(MusicHandle Handle, f32 Pitch)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Music->setPitch(Pitch);
	}

	void SFMLSoundManagerImplementation::SetSoundPitch(SoundHandle Handle, f32 Pitch)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Sound->setPitch(Pitch);
	}

	f32 SFMLSoundManagerImplementation::MusicPitch(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return 1;
		}

		return it->second->Music->getPitch();
	}

	f32 SFMLSoundManagerImplementation::SoundPitch(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return 1;
		}

		return it->second->Sound->getPitch();
	}

	void SFMLSoundManagerImplementation::PlayMusic(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Music->play();
	}

	void SFMLSoundManagerImplementation::PlaySound(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Sound->play();
	}

	void SFMLSoundManagerImplementation::PauseMusic(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Music->pause();
	}

	void SFMLSoundManagerImplementation::PauseSound(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Sound->pause();
	}

	void SFMLSoundManagerImplementation::StopMusic(MusicHandle Handle)
	{
		MusicMap::iterator it = Musics.find(Handle);

		if (it == Musics.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Music->stop();
	}

	void SFMLSoundManagerImplementation::StopSound(SoundHandle Handle)
	{
		SoundMap::iterator it = Sounds.find(Handle);

		if (it == Sounds.end())
		{
			REPORT_INVALID();

			return;
		}

		it->second->Sound->stop();
	}
}
#endif
