-- GraphicsSettings
GraphicsSettings = GUIState:new()

function GraphicsSettings:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function GraphicsSettings:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function GraphicsSettings:OnHook()
	self.update_display = false
	local ghostmodes = {"Transparent", "Hollow", "Disabled"}
	local lcamodes = {"Particle", "3D", "Disabled"}
	local resolutions = system:ListDisplayModes()
	local current_resolution = profile.width.."x"..profile.height
	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("Graphics"))
	self.options:AddSelector("Resolution", resolutions,
		function(state)
			match = re_match("(\\d+)x(\\d+)", resolutions[state])
			if match[1] and match[2] then
				profile.width, profile.height = match[1], match[2]
			end
			if (profile.width.."x"..profile.height) ~= current_resolution then
				self.update_display = true
			end
		end, current_resolution, "Window resolution / screen size in pixels.")
	self.options:AddCheckbox("Fullscreen",
		function(state)
			if state ~= profile.fullscreen then
				self.update_display = true
				profile.fullscreen = state
			end
		end, profile.fullscreen)
	self.options:AddSelector("Line Clear Animations", lcamodes,
		function(state)
			profile.lcanim = lcamodes[state]
		end, profile.lcanim)
	self.options:AddCheckbox("3D Blocks", function(state) profile.blocks_3d = state end, profile.blocks_3d)
	self.options:AddCheckbox("Show Grid", function(state) profile.grid = state end, profile.grid)
	self.options:AddCheckbox("Smooth Moves", function(state) profile.smooth_moves = state end, profile.smooth_moves)
	self.options:AddSelector("Ghost Type", ghostmodes,
		function(state)
			profile.ghost_type = ghostmodes[state]
		end, profile.ghost_type)
	self.options:AddCheckbox("Drop Effects", function(state) profile.dropeffects = state end, profile.dropeffects)
	self.options:AddButton("Back", function () self:ApplyConfig() end, "Return to config menu and save changes.")
end

function GraphicsSettings:ApplyConfig()
	for k,v in pairs(self.options.opts) do
		if v.Apply then
			v.Apply()
		end
	end
	system:SaveSettings(settings.profile, profile)
	if self.update_display == true then
		Message:Hook("Restart", "Due to some of the settings you have selected the game will now exit. Restart the game when it has exited for your changes to take effect.", self, function() system:Exit() end);
	else
		self:DeHook()
	end
end
