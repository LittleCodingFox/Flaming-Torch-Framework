#ifndef __DEFAULTPROJECT_HPP__
#define __DEFAULTPROJECT_HPP__

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

namespace FlamingTorch
{
#	define TAG "DefaultProject"

	class DefaultProject : public NativeGameInterface
	{
	public:
		std::string GameNameValue;

		DefaultProject();
		const std::string &GameName() override;
		int32 FixedUpdateRate() override;
		void OnFixedUpdate() override;
		void OnFrameUpdate() override;
		bool ShouldQuit() override;
		bool Initialize(int32 argc, char **argv) override;
		bool DeInitialize() override;

#if USE_GRAPHICS
		void OnFrameBegin() override;
		void OnFrameDraw() override;
		void OnFrameEnd() override;
		void OnResize(uint32 Width, uint32 Height) override;
		void OnResourcesReloaded() override;
#endif
	};
};

#endif
