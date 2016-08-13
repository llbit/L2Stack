-- GameSettings
GameSettings = GUIState:new()

function GameSettings:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function GameSettings:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function GameSettings:OnHook()
	self.update_display = false
	local rotmodes = {"ARS", "SRS"}
	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("Game"))
	self.options:AddSelector("Rotation system", rotmodes,
		function(state)
			profile.rotsys = rotmodes[state]
		end, profile.rotsys)
	self.das = self.options:AddNumeric("Auto Shift Delay", 0, 60, 1, profile.das)
	self.dasg = self.options:AddNumeric("Auto Shift Speed", 0, 2560, 8, profile.dasg)
	self.options:AddButton("Back", function () self:ApplyConfig() end, "Return to config menu and save changes.")
end

function GameSettings:ApplyConfig()
	for k,v in pairs(self.options.opts) do
		if v.Apply then
			v.Apply()
		end
	end
	profile.das = self.das.state
	profile.dasg = self.dasg.state
	system:SaveSettings(settings.profile, profile)
	if self.update_display == true then
		Message:Hook("Restart", "Due to some of the settings you have selected the game will now exit. Restart the game when it has exited for your changes to take effect.", self, function() system:Exit() end);
	else
		self:DeHook()
	end
end
