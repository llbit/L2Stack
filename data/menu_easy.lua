-- Easy Mode Menu
EasyMenu = GUIState:new()

function EasyMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function EasyMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function EasyMenu:OnHook()
	self.options = OptionsList:new()
	local sonic = self.options:AddCheckbox("Sonic lock", nil, 0)
	self.options:AddButton("Play",
		function()
			self:DeHook()
			SPGame:Hook(system:NewGame(
				string.format("easy(version=%s,sonic=%d,rot=%s)",
					state.version, sonic.state, profile.rotsys)))
		end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
