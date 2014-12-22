#pragma once
#if USE_SOUND && USE_SFML_SOUNDMANAGER
}

#	include <SFML/Audio.hpp>

namespace FlamingTorch
{
	class SFMLSoundManagerImplementation : public ISoundManagerImplementation
	{
	private:
		static uint64 SoundCounter, MusicCounter;

		struct SoundInfo
		{
			DisposablePointer<sf::Sound> Sound;

			DisposablePointer<sf::SoundBuffer> Buffer;
		};

		struct MusicInfo
		{
			std::vector<uint8> Data;
			DisposablePointer<sf::Music> Music;
		};

		typedef std::map<SoundHandle, DisposablePointer<SoundInfo> > SoundMap;
		typedef std::map<MusicHandle, DisposablePointer<MusicInfo> > MusicMap;

		SoundMap Sounds;
		MusicMap Musics;
	public:
		~SFMLSoundManagerImplementation();

		MusicHandle CreateMusic(const void *Source, uint32 Length) override;
		SoundHandle CreateSound(const void *Source, uint32 Length) override;
		void DestroyMusic(MusicHandle Handle) override;
		void DestroySound(SoundHandle Handle) override;
		void SetMusicVolume(MusicHandle Handle, f32 Volume) override;
		void SetSoundVolume(SoundHandle Handle, f32 Volume) override;
		f32 GetMusicVolume(MusicHandle Handle) override;
		f32 GetSoundVolume(SoundHandle Handle) override;
		uint8 GetMusicStatus(MusicHandle Handle) override;
		uint8 GetSoundStatus(SoundHandle Handle) override;
		void SetMusicLooping(MusicHandle Handle, bool Loop) override;
		void SetSoundLooping(SoundHandle Handle, bool Loop) override;
		bool MusicLooping(MusicHandle Handle) override;
		bool SoundLooping(SoundHandle Handle) override;
		void SetMusicPitch(MusicHandle Handle, f32 Pitch) override;
		void SetSoundPitch(SoundHandle Handle, f32 Pitch) override;
		f32 MusicPitch(MusicHandle Handle) override;
		f32 SoundPitch(SoundHandle Handle) override;
		void PlayMusic(MusicHandle Handle) override;
		void PlaySound(SoundHandle Handle) override;
		void PauseMusic(MusicHandle Handle) override;
		void PauseSound(SoundHandle Handle) override;
		void StopMusic(MusicHandle Handle) override;
		void StopSound(SoundHandle Handle) override;
	};

#endif
