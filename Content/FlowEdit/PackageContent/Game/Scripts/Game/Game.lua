NodeConnections = {}

DrawBezierLine = function(Renderer, a, b)
	local StartPosition = Vector3(a)

	local EndPosition = Vector3(b)
	
	local StartPositionQuarter = Vector3(StartPosition)
	local StartPositionThreeQuarter = Vector3(EndPosition)
	
	local t = 0.3
	
	StartPositionQuarter.x = StartPosition.x * t + EndPosition.x * (1 - t)
	StartPositionThreeQuarter.x = StartPosition.x * (1 - t) + EndPosition.x * t
	
	Renderer:RenderLines(StartPosition, StartPositionQuarter, StartPositionThreeQuarter,
		EndPosition, 50, Vector4(1, 1, 1, 1))
end

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

	Renderer.Camera:SetOrtho(PlatformInfo.RotateScreen(Rect(0, Renderer.Size.x, 0, Renderer.Size.y)), -1, 1)
	
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
	local Spr = Sprite()
	Spr:Draw(Renderer)
	
	for key, value in pairs(NodeConnections) do
		local StartPosition = key.ParentPosition
		local EndPosition = value.ParentPosition
		
		StartPosition.y = StartPosition.y + key.Size.y / 2
		EndPosition.y = EndPosition.y + value.Size.y / 2
		
		if StartPosition.x + key.Size.x < EndPosition.x then
			StartPosition.x = StartPosition.x + key.Size.x
		elseif EndPosition.x + value.Size.x < StartPosition.x then
			EndPosition.x = EndPosition.x + value.Size.x
		end
		
		DrawBezierLine(Renderer, StartPosition, EndPosition)
	end
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
end

GameDeInitialize = function()
	return true
end

GameShouldQuit = function()
	return not g_RendererManager:RequestFrame()
end
