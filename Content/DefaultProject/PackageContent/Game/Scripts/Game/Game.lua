LogoEntity = nil

GamePreInitialize = function()
	g_Log:Register()
	g_Clock:Register()
	g_FPSCounter:Register()
	g_ResourceManager:Register()
	g_RendererManager:Register()
	g_Console:Register()
	g_ObjectModel:Register()
	
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
	
	local LogoTexture = g_ResourceManager:GetTextureFromPackage("/", "torch_small.png")
	
	if LogoTexture == nil then
		return false
	end
	
	local Transform = TransformFeature()
	local LogoSpriteFeature = SpriteFeature()
	
	LogoSpriteFeature.Sprite.Texture = LogoTexture
	
	LogoEntityDef = ObjectDef()

	LogoEntityDef.Name = "LogoEntity"
	
	LogoEntityDef:AddFeature(Transform)
	LogoEntityDef:AddFeature(LogoSpriteFeature)
	
	g_ObjectModel:RegisterObjectDef(LogoEntityDef)
	
	LogoEntity = LogoEntityDef:Clone()
	
	g_ObjectModel:RegisterObject(LogoEntity)
	
	local SkinStream = g_PackageManager:GetFile(MakeStringID("/UIThemes/PolyCode/"), MakeStringID("skin.cfg"))
	local SkinConfig = GenericConfig()
	
	if SkinStream == nil or not SkinConfig:DeSerialize(SkinStream) then
		return false
	end
	
	Renderer.UI.Skin = SkinConfig
	
	GameResize(Renderer, Renderer.Size.x, Renderer.Size.y)

	return true
end

GameFrameBegin = function(Renderer)
	Renderer:Clear(RendererManager.Clear_Color)
end

GameFrameDraw = function(Renderer)
end

GameFrameEnd = function(Renderer)
end

GameResize = function(Renderer, Width, Height)
	local Viewport = Rect()
	local Projection = Matrix4x4()
	local World = Matrix4x4()

	Renderer:ScreenResizedTransforms(Viewport, Projection, World)
	
	Renderer:SetViewport(Viewport.Left, Viewport.Top, Viewport.Size.x, Viewport.Size.y)
	
	Renderer.ProjectionMatrix = Projection
	Renderer.WorldMatrix = World
end

GameResourcesReloaded = function(Renderer)
	GameResize(Renderer, Renderer.Size.x, Renderer.Size.y)
end

GameFrameUpdate = function()
	local Position = Vector2()
	local MakeNew = false
	
	if g_Input:GetTouch(InputCenter.Touch_0).JustPressed then
		Position = g_Input:GetTouch(InputCenter.Touch_0).Position
		MakeNew = true
	end
	
	if g_Input:GetMouseButton(InputCenter.Mouse_Left).FirstPress then
		Position = g_Input.MousePosition
		MakeNew = true
	end
	
	if MakeNew then
		local Entity = LogoEntityDef:Clone()

		local Transform = Entity:GetFeature("Transform")
		
		Transform.Position = Vector3(Position)
		
		g_ObjectModel:RegisterObject(Entity)
	end
	
end

GameDeInitialize = function()
	return true
end

GameShouldQuit = function()
	return not g_RendererManager:RequestFrame()
end
