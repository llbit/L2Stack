-- NewProfile
NewProfile = GUIState:new()

function NewProfile:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function NewProfile:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function NewProfile:OnHook()
	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("New Profile"))
	self.options:AddButton("<todo>", function() self:DeHook() end)
end
