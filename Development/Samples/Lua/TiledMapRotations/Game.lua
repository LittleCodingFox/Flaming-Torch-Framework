LogoTexture = nil
Entity = nil

GamePreInitialize = function()
	g_Log:Register()
	g_Clock:Register()
	g_FPSCounter:Register()
	g_ResourceManager:Register()
	g_RendererManager:Register()
	g_Console:Register()
	
	--g_FileSystemWatcher:Register()
	
	Game.DevelopmentBuild = true

	return true
end

MakeMap = function(Order)
	local Map = TiledMap()
	Map.TileOrder = Order
	
	local Stream = g_PackageManager:GetFile(MakeStringID("/CompiledMaps/"), MakeStringID("Map.fttm"))
	
	if Stream == nil or Map:DeSerialize(Stream) == false or Map:InitResources(TiledMap.InitOptions():FromPackage(true):AdditionalPackageDirectory("/CompiledMaps/")) == false then
		return nil
	end
	
	return Map
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

	Renderer.Camera:SetOrtho(Rect(0, Renderer.Size.x, 0, Renderer.Size.y), -1, 1)
	
	g_World:LoadComponent("/Scripts/Base/Components/Transform.lua")
	g_World:LoadComponent("/Scripts/Base/Components/Renderable.lua")
	
	Entity = g_World:NewEntity("Test")
	Entity:AddComponent("Transform")
	Entity:AddComponent("Renderable")

	Entity:GetComponent("Renderable").Properties["Sprite"].Texture = LogoTexture
	
	local SkinStream = g_PackageManager:GetFile(MakeStringID("/UIThemes/PolyCode/"), MakeStringID("skin.cfg"))
	local SkinConfig = GenericConfig()

	if SkinStream == nil or not SkinConfig:DeSerialize(SkinStream) then
		return false
	end
	
	Renderer.UI.Skin = SkinConfig
	
	NorthMap = MakeMap(0)
	EastMap = MakeMap(1)
	SouthMap = MakeMap(2)
	WestMap = MakeMap(3)
	
	if NorthMap == nil or EastMap == nil or SouthMap == nil or WestMap == nil then
		return false
	end
	
	SouthMap.Translation = Vector2(NorthMap.MapPixelSize.x, 0)
	
	WestMap.Translation = Vector2(0, NorthMap.MapPixelSize.y)
	
	EastMap.Translation = NorthMap.MapPixelSize

	return true
end

GameFrameBegin = function(Renderer)
	Renderer:Clear(RendererManager.Clear_Color)
	Renderer.Camera:BeginTransforms(Renderer)
end

GameFrameDraw = function(Renderer)
	--Entity:GetComponent("Renderable"):Update()
	
	NorthMap:Draw(0, Renderer)
	EastMap:Draw(0, Renderer)
	SouthMap:Draw(0, Renderer)
	WestMap:Draw(0, Renderer)
	
	RenderTextUtils.RenderText(Renderer, "NORTH", RenderTextUtils.TextParams():BorderSize(1):BorderColor(Vector4(0,0,0,1))
		:Position(Vector2((NorthMap.MapPixelSize.x - RenderTextUtils.MeasureTextSimple(Renderer, "NORTH", RenderTextUtils.TextParams()).Size.x) / 2, 10)))
	
	RenderTextUtils.RenderText(Renderer, "SOUTH", RenderTextUtils.TextParams():BorderSize(1):BorderColor(Vector4(0,0,0,1))
		:Position(Vector2(NorthMap.MapPixelSize.x + (NorthMap.MapPixelSize.x - RenderTextUtils.MeasureTextSimple(Renderer, "SOUTH", RenderTextUtils.TextParams()).Size.x) / 2, 10)))
	
	RenderTextUtils.RenderText(Renderer, "EAST", RenderTextUtils.TextParams():BorderSize(1):BorderColor(Vector4(0,0,0,1))
		:Position(Vector2(NorthMap.MapPixelSize.x + (NorthMap.MapPixelSize.x - RenderTextUtils.MeasureTextSimple(Renderer, "EASY", RenderTextUtils.TextParams()).Size.x) / 2, NorthMap.MapPixelSize.y + 10)))
	
	RenderTextUtils.RenderText(Renderer, "WEST", RenderTextUtils.TextParams():BorderSize(1):BorderColor(Vector4(0,0,0,1))
		:Position(Vector2((NorthMap.MapPixelSize.x - RenderTextUtils.MeasureTextSimple(Renderer, "WEST", RenderTextUtils.TextParams()).Size.x) / 2, NorthMap.MapPixelSize.y + 10)))
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
	local Position = Entity:GetComponent("Transform").Properties["Position"]
	
	if g_Input:GetKey(InputCenter.Key_Left).Pressed then
		Position.x = Position.x - g_Clock:Delta() * 500
		
		if Position.x < 0 then
			Position.x = 0
		end
	end

	if g_Input:GetKey(InputCenter.Key_Right).Pressed then
		Position.x = Position.x + g_Clock:Delta() * 500
		
		if Position.x > g_RendererManager.ActiveRenderer.Size.x - LogoTexture.Size.x then
			Position.x = g_RendererManager.ActiveRenderer.Size.x - LogoTexture.Size.x
		end
	end
	
	if g_Input:GetKey(InputCenter.Key_Up).Pressed then
		Position.y = Position.y - g_Clock:Delta() * 500
		
		if Position.y < 0 then
			Position.y = 0
		end
	end

	if g_Input:GetKey(InputCenter.Key_Down).Pressed then
		Position.y = Position.y + g_Clock:Delta() * 500
		
		if Position.y > g_RendererManager.ActiveRenderer.Size.y - LogoTexture.Size.y then
			Position.y = g_RendererManager.ActiveRenderer.Size.y - LogoTexture.Size.y
		end
	end
	
	Entity:GetComponent("Transform").Properties["Position"] = Position
end

GameDeInitialize = function()
	return true
end

GameShouldQuit = function()
	return not g_RendererManager:RequestFrame()
end
