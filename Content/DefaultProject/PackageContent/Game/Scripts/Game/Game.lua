LogoTexture = nil
Entity = nil

GamePreInitialize = function()
	g_Log:Register()
	g_Clock:Register()
	g_FPSCounter:Register()
	g_ResourceManager:Register()
	g_RendererManager:Register()
	g_Console:Register()
	g_FileSystemWatcher:Register()
	
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
	Options.Height = 600
	Options.FrameRate = Game.FrameRate
	
	local Renderer = Game:CreateRenderer(Options)
	
	LogoTexture = g_ResourceManager:GetTextureFromPackage("/", "torch_small.png")
	
	if LogoTexture == nil then
		return false
	end

	g_RendererManager.ActiveRenderer.Camera:SetOrtho(Rect(0, 960, 600, 0), -1, 1)
	
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

	return true
end

GameFrameBegin = function(Renderer)
	Renderer:Clear(RendererManager.Clear_Color)
	Renderer.Camera:BeginTransforms()
end

GameFrameDraw = function(Renderer)
	Entity:GetComponent("Renderable"):Update()
end

GameFrameEnd = function(Renderer)
	Renderer.Camera:EndTransforms()
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
