-- Master Mode Menu
MasterMenu = GUIState:new()

function MasterMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function MasterMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function MasterMenu:OnHook()
	self.options = OptionsList:new()
	local level = self.options:AddNumeric("Level", 0, 900, 100)
	local sonic = self.options:AddCheckbox("Sonic lock", nil, 0)
	self.options:AddButton("Play",
		function()
			self:DeHook()
			SPGame:Hook(system:NewGame(
				string.format("master(version=%s,level=%d,sonic=%d,rot=%s)",
					state.version, level.state, sonic.state, profile.rotsys)))
		end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
