Renderable = {
	OnStart = function(Self, Owner)
		Self.Properties["Sprite"] = Sprite()
	end,
	
	OnUpdate = function(Self, Owner)
		if Owner:HasComponent("Transform") then
			Self.Properties["Sprite"].Options:Position(Owner:GetComponent("Transform").Properties["Position"])
		end
		
		Self.Properties["Sprite"]:Draw(RendererManager.Instance.ActiveRenderer)
	end
}