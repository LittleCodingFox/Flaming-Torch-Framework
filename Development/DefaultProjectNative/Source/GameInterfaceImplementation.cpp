#include "FlamingCore.hpp"
#include "DefaultProject.hpp"
namespace FlamingTorch
{
	DisposablePointer<LuaScript> ScriptInstance;

	DefaultProject::DefaultProject() : GameNameValue("DefaultProject Native")
	{
		FrameRateValue = 0;
		UpdateRateValue = 30;
		QuitFlag = false;
	}

	bool DefaultProject::Register(lua_State *State)
	{
		return true;
	}

	bool DefaultProject::Initialize(int32 argc, char **argv)
	{
		if (!LoadPackage(FileSystemUtils::ResourcesDirectory() + "/Content/Game.package"))
			return false;

		for (int32 i = 1; i < argc; i++)
		{
			std::string Argument = argv[i];

			if (Argument == "-guisandbox")
			{
				IsGUISandbox = true;

				Console::Instance.GetVariable("r_drawuirects")->UIntValue = 1;
				Console::Instance.GetVariable("r_drawuifocuszones")->UIntValue = 1;
			}
		}

		DevelopmentBuild = true;

		RenderCreateOptions Options;
		Options.Title = GameName();
		Options.Width = 960;
		Options.Height = 720;
		Options.FrameRate = FrameRateValue;

		Renderer *TheRenderer = CreateRenderer(Options);

		DisposablePointer<Texture> LogoTexture = ResourceManager::Instance.GetTexture("/torch_small.png");

		if (LogoTexture.Get() == NULL)
			return false;

		DisposablePointer<TransformFeature> Transform(new TransformFeature());
		DisposablePointer<SpriteFeature> LogoSpriteFeature(new SpriteFeature());

		LogoSpriteFeature->TheSprite.SpriteTexture = LogoTexture;

		LogoEntityDef.Reset(new ObjectDef());
		LogoEntityDef->Name = "LogoEntity";
		LogoEntityDef->Features.push_back(Transform);
		LogoEntityDef->Features.push_back(LogoSpriteFeature);

		ObjectModelManager::Instance.RegisterObjectDef(LogoEntityDef);

		LogoEntity = LogoEntityDef->Clone();

		ObjectModelManager::Instance.RegisterObject(LogoEntity);

		OnResize(TheRenderer, (uint32)TheRenderer->Size().x, (uint32)TheRenderer->Size().y);

		return true;
	}

	bool DefaultProject::DeInitialize()
	{
		return true;
	}

	const std::string &DefaultProject::GameName()
	{
		return GameNameValue;
	}

	int32 DefaultProject::FixedUpdateRate()
	{
		return UpdateRateValue;
	}

	DisposablePointer<LuaScript> DefaultProject::GetScriptInstance()
	{
		return ScriptInstance;
	}


#if USE_GRAPHICS
	void DefaultProject::OnFrameBegin(Renderer *TheRenderer, const std::string &ScenePass)
	{
		TheRenderer->Clear(RendererBuffer::Color);
	}

	void DefaultProject::OnFrameDraw(Renderer *TheRenderer, const std::string &ScenePass)
	{
	}

	void DefaultProject::OnFrameEnd(Renderer *TheRenderer, const std::string &ScenePass)
	{
		GameInterface::OnFrameEnd(TheRenderer, ScenePass);
	}

	void DefaultProject::OnResize(Renderer *TheRenderer, uint32 Width, uint32 Height)
	{
		Rect Viewport;
		Matrix4x4 Projection, World;

		TheRenderer->ScreenResizedTransforms(Viewport, Projection, World);

		TheRenderer->SetViewport(Viewport.Left, Viewport.Top, Viewport.Size().x, Viewport.Size().y);

		TheRenderer->SetProjectionMatrix(Projection);
		TheRenderer->SetWorldMatrix(World);
	}

	void DefaultProject::OnResourcesReloaded(Renderer *TheRenderer)
	{
		OnResize(TheRenderer, (uint32)TheRenderer->Size().x, (uint32)TheRenderer->Size().y);
	}

#endif

	void DefaultProject::OnFixedUpdate()
	{
	}

	void DefaultProject::OnFrameUpdate()
	{
		Vector2 Position;
		bool MakeNew = false;

		if (RendererManager::Instance.Input.Touches[0].JustPressed)
		{
			Position = RendererManager::Instance.Input.Touches[0].Position;
			MakeNew = true;
		}

		if (RendererManager::Instance.Input.MouseButtons[InputMouseButton::Left].FirstPress)
		{
			Position = RendererManager::Instance.Input.MousePosition;
			MakeNew = true;
		}

		if (MakeNew)
		{
			DisposablePointer<ObjectDef> Entity = LogoEntityDef->Clone();

			DisposablePointer<TransformFeature> Transform = Entity->GetFeature("Transform");

			Transform->Position = Position;

			ObjectModelManager::Instance.RegisterObject(Transform);
		}
	}

	bool DefaultProject::ShouldQuit()
	{
#if USE_GRAPHICS
		return !RendererManager::Instance.RequestFrame() && !QuitFlag;
#else
		return true;
#endif
	}
}
