Renderable = {
	OnStart = function(Self, Owner)
		local Properties = Self.Properties
		
		Self.Properties["Sprite"] = Sprite()
	end,
	
	OnUpdate = function(Self, Owner)
		if Owner:HasComponent("Transform") then
			Self.Properties["Sprite"].Options:Position(Owner:GetComponent("Transform").Properties["Position"])
		end
		
		Self.Properties["Sprite"]:Draw(g_RendererManager.ActiveRenderer)
	end
}