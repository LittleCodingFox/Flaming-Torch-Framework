LogoTexture = nil
LogoSprite = Sprite()

GamePreInitialize = function()
	g_Log:Register()
	g_Clock:Register()
	g_FPSCounter:Register()
	g_ResourceManager:Register()
	g_RendererManager:Register()
	g_Console:Register()
	
	if PlatformInfo.PlatformType() == PlatformInfo.PlatformType_PC then
		g_FileSystemWatcher:Register()
	end
	
	Game.DevelopmentBuild = true

	return true
end

GameInitialize = function(Arguments)
	for i = 1, table.getn(Arguments) do
		if Arguments[i] == "-guisandbox" then
			Game.IsGUISandbox = true
			
			g_Console:GetVariable("r_drawuirects").UintValue = 1
			g_Console:GetVariable("r_drawuifocuszones").UintValue = 1
		end
	end
	
	local Options = RenderCreateOptions()
	Options.Title = Game.GameName
	Options.Width = 960
	Options.Height = 720
	Options.FrameRate = Game.FrameRate
	
	local Renderer = Game:CreateRenderer(Options)
	
	LogoTexture = g_ResourceManager:GetTextureFromPackage("/", "torch_small.png")
	
	if LogoTexture == nil then
		return false
	end

	Renderer.Camera:SetOrtho(PlatformInfo.RotateScreen(Rect(0, Renderer.Size.x, 0, Renderer.Size.y)), -1, 1)
	
	LogoSprite.Texture = LogoTexture
	
	local SkinStream = g_PackageManager:GetFile(MakeStringID("/UIThemes/PolyCode/"), MakeStringID("skin.cfg"))
	local SkinConfig = GenericConfig()

	if SkinStream == nil or not SkinConfig:DeSerialize(SkinStream) then
		return false
	end
	
	Renderer.UI.Skin = SkinConfig

	return true
end

GameFrameBegin = function(Renderer)
	Renderer:Clear(RendererManager.Clear_Color)
	Renderer.Camera:BeginTransforms(Renderer)
end

GameFrameDraw = function(Renderer)
	LogoSprite:Draw(Renderer)
end

GameFrameEnd = function(Renderer)
	Renderer.Camera:EndTransforms(Renderer)
end

GameResize = function(Renderer, Width, Height)
	Renderer:SetViewport(0, 0, Width, Height)
	Renderer.Camera:SetOrtho(Rect(0, Width, 0, Height), -1, 1)
end

GameResourcesReloaded = function(Renderer)
	GameResize(Renderer, Renderer.Size.x, Renderer.Size.y)
end

GameFrameUpdate = function()
	local Position = LogoSprite.Options.PositionValue
	
	if g_Input:GetTouch(InputCenter.Touch_0).JustPressed then
		Position = g_Input:GetTouch(InputCenter.Touch_0).Position
	end

	if g_Input:GetKey(InputCenter.Key_Left).Pressed then
		Position.x = Position.x - g_Clock.Delta * 500
		
		if Position.x < 0 then
			Position.x = 0
		end
	end

	if g_Input:GetKey(InputCenter.Key_Right).Pressed then
		Position.x = Position.x + g_Clock.Delta * 500
		
		if Position.x > g_RendererManager.ActiveRenderer.Size.x - LogoTexture.Size.x then
			Position.x = g_RendererManager.ActiveRenderer.Size.x - LogoTexture.Size.x
		end
	end
	
	if g_Input:GetKey(InputCenter.Key_Up).Pressed then
		Position.y = Position.y - g_Clock.Delta * 500
		
		if Position.y < 0 then
			Position.y = 0
		end
	end

	if g_Input:GetKey(InputCenter.Key_Down).Pressed then
		Position.y = Position.y + g_Clock.Delta * 500
		
		if Position.y > g_RendererManager.ActiveRenderer.Size.y - LogoTexture.Size.y then
			Position.y = g_RendererManager.ActiveRenderer.Size.y - LogoTexture.Size.y
		end
	end
	
	LogoSprite.Options:Position(Position)
end

GameDeInitialize = function()
	return true
end

GameShouldQuit = function()
	return not g_RendererManager:RequestFrame()
end
