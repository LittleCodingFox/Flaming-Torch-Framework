#include "FlamingCore.hpp"
#include "DefaultProject.hpp"
namespace FlamingTorch
{
	DefaultProject::DefaultProject() : GameNameValue("DefaultProject")
	{
		FrameRateValue = 0;
		UpdateRateValue = 30;
		QuitFlag = false;
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

				g_Console.GetVariable("r_drawuirects")->UIntValue = 1;
				g_Console.GetVariable("r_drawuifocuszones")->UIntValue = 1;
			}
			else if (Argument == "-dev")
			{
				DevelopmentBuild = true;
			}
		}


		RenderCreateOptions Options;
		Options.Title = GameName();
		Options.Width = 800;
		Options.Height = 600;
		Options.FrameRate = FrameRateValue;

		if (!CreateRenderer(Options))
			return false;

		OnResize((uint32)g_Renderer.Size().x, (uint32)g_Renderer.Size().y);

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

#if USE_GRAPHICS
	void DefaultProject::OnFrameBegin()
	{
		g_Renderer.Clear(RendererBuffer::Color);
	}

	void DefaultProject::OnFrameDraw()
	{
	}

	void DefaultProject::OnFrameEnd()
	{
		GameInterface::OnFrameEnd();
	}

	void DefaultProject::OnResize(uint32 Width, uint32 Height)
	{
		Rect Viewport;
		Matrix4x4 Projection, World;

		g_Renderer.ScreenResizedTransforms(Viewport, Projection, World);

		g_Renderer.SetViewport(Viewport.Left, Viewport.Top, Viewport.Size().x, Viewport.Size().y);

		g_Renderer.SetProjectionMatrix(Projection);
		g_Renderer.SetWorldMatrix(World);
	}

	void DefaultProject::OnResourcesReloaded()
	{
		OnResize((uint32)g_Renderer.Size().x, (uint32)g_Renderer.Size().y);
	}
#endif

	void DefaultProject::OnFixedUpdate()
	{
	}

	void DefaultProject::OnFrameUpdate()
	{
	}

	bool DefaultProject::ShouldQuit()
	{
#if USE_GRAPHICS
		return QuitFlag;
#else
		return true;
#endif
	}
}
