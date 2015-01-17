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
		DisposablePointer<LuaScript> ScriptInstance;
		DisposablePointer<ObjectDef> LogoEntity, LogoEntityDef;
		std::string GameNameValue;

		DefaultProject();
		bool Register(lua_State *State);
		const std::string &GameName() override;
		int32 FixedUpdateRate() override;
		DisposablePointer<LuaScript> GetScriptInstance() override;
		void OnFixedUpdate() override;
		void OnFrameUpdate() override;
		bool ShouldQuit() override;
		bool Initialize(int32 argc, char **argv) override;
		bool DeInitialize() override;

#if USE_GRAPHICS
		void OnFrameBegin(Renderer *TheRenderer, const std::string &ScenePass) override;
		void OnFrameDraw(Renderer *TheRenderer, const std::string &ScenePass) override;
		void OnFrameEnd(Renderer *TheRenderer, const std::string &ScenePass) override;
		void OnResize(Renderer *TheRenderer, uint32 Width, uint32 Height) override;
		void OnResourcesReloaded(Renderer *TheRenderer) override;
#endif
	};
};

#endif
