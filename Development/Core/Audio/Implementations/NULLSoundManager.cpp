#include "FlamingCore.hpp"
#if USE_SOUND
namespace FlamingTorch
{
#	include "Audio/Implementations/NULLSoundManager.hpp"

	MusicHandle NULLSoundManagerImplementation::CreateMusic(const void *Source, uint32 Length)
	{
		return 1;
	};

	SoundHandle NULLSoundManagerImplementation::CreateSound(const void *Source, uint32 Length)
	{
		return 1;
	};

	void NULLSoundManagerImplementation::DestroyMusic(MusicHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::DestroySound(SoundHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::SetMusicVolume(MusicHandle Handle, f32 Volume)
	{
	};

	void NULLSoundManagerImplementation::SetSoundVolume(SoundHandle Handle, f32 Volume)
	{
	};

	f32 NULLSoundManagerImplementation::GetMusicVolume(MusicHandle Handle)
	{
		return 1;
	};

	f32 NULLSoundManagerImplementation::GetSoundVolume(SoundHandle Handle)
	{
		return 1;
	};

	uint8 NULLSoundManagerImplementation::GetMusicStatus(MusicHandle Handle)
	{
		return SoundStatus::Stopped;
	};

	uint8 NULLSoundManagerImplementation::GetSoundStatus(SoundHandle Handle)
	{
		return SoundStatus::Stopped;
	};

	void NULLSoundManagerImplementation::SetMusicLooping(MusicHandle Handle, bool Loop)
	{
	};

	void NULLSoundManagerImplementation::SetSoundLooping(SoundHandle Handle, bool Loop)
	{
	};

	bool NULLSoundManagerImplementation::MusicLooping(MusicHandle Handle)
	{
		return false;
	};

	bool NULLSoundManagerImplementation::SoundLooping(SoundHandle Handle)
	{
		return false;
	};

	void NULLSoundManagerImplementation::SetMusicPitch(MusicHandle Handle, f32 Pitch)
	{
	};

	void NULLSoundManagerImplementation::SetSoundPitch(SoundHandle Handle, f32 Pitch)
	{
	};

	f32 NULLSoundManagerImplementation::MusicPitch(MusicHandle Handle)
	{
		return 1;
	};

	f32 NULLSoundManagerImplementation::SoundPitch(SoundHandle Handle)
	{
		return 1;
	};

	void NULLSoundManagerImplementation::PlayMusic(MusicHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::PlaySound(SoundHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::PauseMusic(MusicHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::PauseSound(SoundHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::StopMusic(MusicHandle Handle)
	{
	};

	void NULLSoundManagerImplementation::StopSound(SoundHandle Handle)
	{
	};
};
#endif
