-- Sprint Menu
SprintMenu = GUIState:new()

function SprintMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function SprintMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function SprintMenu:OnHook()
	self.options = OptionsList:new()
	local sonic = self.options:AddCheckbox("Swap Hard/Soft Drop", nil, 0)
	local goal = self.options:AddNumeric("Goal", 10, 1000, 10, 40)
	local das = self.options:AddNumeric("Auto Shift Delay", 0, 60, 1, profile.das)
	local dasg = self.options:AddNumeric("Auto Shift Speed", 0, 2560, 8, profile.dasg)
	self.options:AddButton("Play", function()
		self:DeHook()
		profile.das = das.state
		profile.dasg = dasg.state
		system:SaveSettings(settings.profile, profile)
		SPGame:Hook(system:NewGame(
			string.format("sprint(version=%s,goal=%d,sonic=%d,das=%d,dasg=%d,rot=%s,rand=TAP)",
				state.version, goal.state, 1-sonic.state, das.state, dasg.state, profile.rotsys))) end)
	self.options:AddButton("Cancel", function() self:DeHook() end)
end
