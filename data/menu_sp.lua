-- SPMenu
SPMenu = GUIState:new()

function SPMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function SPMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function SPMenu:OnHook()
	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("Solo Game"))
	self.options:AddButton("AIDemo", function() SPGame:Hook(system:NewGame("aidemo")) end)
	self.options:AddButton("AI Versus", function() AIMenu:Hook() end, "Play against the AI.")
	self.options:AddButton("Sprint", function() SprintMenu:Hook() end, "Clear 40 (or more) lines as quickly as possible.")
	self.options:AddButton("Easy", function() EasyMenu:Hook() end, "This game mode is suitable for beginners.")
	self.options:AddButton("Radar", function() RadarMenu:Hook() end, "nobody wrote anything useful here yet")
	self.options:AddButton("Classic", function() ClassicMenu:Hook() end, "You need considerable skill to finish this game mode.")
	self.options:AddButton("Master", function() MasterMenu:Hook() end, "Only true experts can attain GM rank in this game mode.")
	self.options:AddButton("Death", function() DeathMenu:Hook() end, "Maximum drop speed from the first level. How long can you survive?")
	self.options:AddButton("M Roll",
		function()
			SPGame:Hook(system:NewGame(string.format("mroll(version=%s,rot=%s)", state.version,profile.rotsys)))
		end, "Survive for one minute with invisible blocks and maximum speed.")
	self.options:AddButton("Back", function() self:DeHook() end)
end
