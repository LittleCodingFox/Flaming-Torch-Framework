LogoTexture = nil
Entity = nil

GamePreInitialize = function()
	g_Log:Register()
	g_Clock:Register()
	g_FPSCounter.Instance:Register()
	g_ResourceManager.Instance:Register()
	g_RendererManager.Instance:Register()
	
	g_Game.DevelopmentBuild = true

	return true
end

GameInitialize = function()
	local Options = RenderCreateOptions()
	Options.Title = g_Game.GameName
	Options.Width = 960
	Options.Height = 600
	
	local Renderer = g_Game:CreateRenderer(Options)
	
	LogoTexture = g_ResourceManager.Instance:GetTextureFromPackage("/", "torch_small.png")
	
	if LogoTexture == nil then
		return false
	end

	g_RendererManager.Instance.ActiveRenderer.Camera:SetOrtho(Rect(0, 960, 600, 0), -1, 1)
	
	g_World:LoadComponent("/Scripts/Base/Components/Transform.lua")
	g_World:LoadComponent("/Scripts/Base/Components/Renderable.lua")
	
	Entity = g_World:NewEntity("Test")
	Entity:AddComponent("Transform")
	Entity:AddComponent("Renderable")

	Entity:GetComponent("Renderable").Properties["Sprite"].Texture = LogoTexture

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
		
		if Position.x > g_RendererManager.Instance.ActiveRenderer.Size.x - LogoTexture.Size.x then
			Position.x = g_RendererManager.Instance.ActiveRenderer.Size.x - LogoTexture.Size.x
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
		
		if Position.y > g_RendererManager.Instance.ActiveRenderer.Size.y - LogoTexture.Size.y then
			Position.y = g_RendererManager.Instance.ActiveRenderer.Size.y - LogoTexture.Size.y
		end
	end
	
	Entity:GetComponent("Transform").Properties["Position"] = Position
end

GameDeInitialize = function()
	return true
end

GameShouldQuit = function()
	return not g_RendererManager.Instance:RequestFrame()
end
