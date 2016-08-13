-- MainMenu
MainMenu = GUIState:new()

function MainMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
	fonts.small:Draw(state.version, self.vx, self.vy)
end

function MainMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function MainMenu:OnHook()
	self.vx = state.width - fonts.small*state.version
	self.vy = state.height - fonts.small.height

	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("L2Stack"))
	self.options:AddButton("Solo", function() SPMenu:Hook() end, "Start a new single player game.")
--	self.options:AddButton("Versus", function() MPMenu:Hook() end, "Join or host a multiplayer game.")
	self.options:AddButton("Settings", function() SettingsMenu:Hook() end)
	self.options:AddButton("View Replay", function() FileMenu:Hook(
		function(fn)
			local replay = system:LoadReplay(fn)
			if replay then
				SPGame:Hook(replay)
				local version = re_match("^\\w+\\((.*,)?version=([\\d\\.]+).*\\)$", replay.mode)[2]
				if version ~= state.version then
					Message:Hook("Warning", "This replay was recorded with another version of L2Stack. The simulation may not be 100% accurate.", SPGame)
				end
			end
		end) end)
	if system.AviOpen then
		self.options:AddButton("Record AVI",
			function() FileMenu:Hook(
				function(fn)
					local game = system:LoadReplay(fn)
					if game then
						local avifile = system:AviOpen(fn..".avi")
						if avifile then
							SPGame:OnHook(game)
							AviRecorder:Hook(SPGame, avifile)
						end
					end
				end) end, "Record a replay to AVI.")
	end
	self.options:AddButton("Exit", function() self:DeHook() end)
end

function MainMenu:DeHook()
	system:Exit()
end
