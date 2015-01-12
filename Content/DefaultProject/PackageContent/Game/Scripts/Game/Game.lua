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
	
	g_ObjectModel:RegisterObjectFeature(Transform)
	g_ObjectModel:RegisterObjectFeature(LogoSpriteFeature)
	
	local LogoEntityDef = ObjectDef()

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
	local Transform = LogoEntity:GetFeature("Transform")
	local LogoSpriteFeature = LogoEntity:GetFeature("Sprite")
	local Position = Transform.Position
	
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
		
		if Position.x > g_RendererManager.ActiveRenderer.Size.x - LogoSpriteFeature.Sprite.Texture.Size.x then
			Position.x = g_RendererManager.ActiveRenderer.Size.x - LogoSpriteFeature.Sprite.Texture.Size.x
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
		
		if Position.y > g_RendererManager.ActiveRenderer.Size.y - LogoSpriteFeature.Sprite.Texture.Size.y then
			Position.y = g_RendererManager.ActiveRenderer.Size.y - LogoSpriteFeature.Sprite.Texture.Size.y
		end
	end
	
	Transform.Position = Position
end

GameDeInitialize = function()
	return true
end

GameShouldQuit = function()
	return not g_RendererManager:RequestFrame()
end
