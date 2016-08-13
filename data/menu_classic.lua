-- Classic Mode Menu
ClassicMenu = GUIState:new()

function ClassicMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function ClassicMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function ClassicMenu:OnHook()
	self.options = OptionsList:new()
	local level = self.options:AddNumeric("Level", 0, 900, 100)
	self.options:AddButton("Play",
		function()
			self:DeHook()
			SPGame:Hook(system:NewGame(
				string.format("classic(version=%s,level=%d,rot=%s)",
					state.version, level.state, profile.rotsys)))
		end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
