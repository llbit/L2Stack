-- MPMenu
MPMenu = GUIState:new{options = nil}

function MPMenu:Draw()
	self.options:Draw()
end

function MPMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function MPMenu:OnHook()	
	self.options = OptionsList:new()
	self.options:AddButton("Find Server", function () end)
	self.options:AddButton("Join Local", function () end)
	self.options:AddButton("Host a Game", function () end)
	self.options:AddButton("Back", function () self:DeHook() end)
end
