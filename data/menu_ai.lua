-- Sprint Menu
AIMenu = GUIState:new()

function AIMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function AIMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function AIMenu:OnHook()
	self.options = OptionsList:new()
	local delays = self.options:AddNumeric("AI Delays", 0, 20, 1, 5)
	self.options:AddButton("Play", function()
		self:DeHook()
		MPGame:Hook(
				system:NewGame(string.format("mpgame(version=%s,sonic=%d,das=%d,dasg=%d,rot=%s,rand=TAP)",
					state.version, 1, profile.das, profile.dasg, profile.rotsys)),
				system:NewGame(string.format("aidemo(delays=%d)", delays.state))) end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
