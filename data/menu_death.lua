-- Death Mode Menu
DeathMenu = GUIState:new()

function DeathMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function DeathMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function DeathMenu:OnHook()
	self.options = OptionsList:new()
	local level = self.options:AddNumeric("Level", 0, 900, 100)
	self.options:AddButton("Play",
		function()
			self:DeHook()
			SPGame:Hook(system:NewGame(
				string.format("death(version=%s,level=%d,rot=%s)",
					state.version, level.state, profile.rotsys)))
		end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
