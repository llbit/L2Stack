-- Radar Mode Menu
RadarMenu = GUIState:new()

function RadarMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function RadarMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function RadarMenu:OnHook()
	self.options = OptionsList:new()
	local sonic = self.options:AddCheckbox("Sonic lock", nil, 0)
	self.options:AddButton("Play",
		function()
			self:DeHook()
			SPGame:Hook(system:NewGame(
				string.format("radar(version=%s,sonic=%d,rot=%s)",
					state.version, sonic.state, profile.rotsys)))
		end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
