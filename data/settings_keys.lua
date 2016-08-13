-- Keys Menu
KeySettings = GUIState:new()

function KeySettings:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function KeySettings:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function KeySettings:OnHook()	
	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("Key Setup"))
	self.key1 = GUIKeySelector:new("Hard Drop", function(key) profile.key_hard = key end, profile.key_hard)
	self.options:AddOption(self.key1)
	self.options:AddButton("Hard Drop", function() KeyEntry:Hook(function(key) profile.key_hard = key end) end, "Hard Drop / Sonic Drop")
	self.options:AddButton("Soft Drop", function() KeyEntry:Hook(function(key) profile.key_soft = key end) end)
	self.options:AddButton("Move Left", function() KeyEntry:Hook(function(key) profile.key_left = key end) end)
	self.options:AddButton("Move Right", function() KeyEntry:Hook(function(key) profile.key_right = key end) end)
	self.options:AddButton("ARot (CCW)", function() KeyEntry:Hook(function(key) profile.key_arot = key end) end)
	self.options:AddButton("BRot (CW)", function() KeyEntry:Hook(function(key) profile.key_brot = key end) end)
	self.options:AddButton("CRot (CCW)", function() KeyEntry:Hook(function(key) profile.key_crot = key end) end)
	self.options:AddButton("Hold Piece", function() KeyEntry:Hook(function(key) profile.key_hold = key end) end)
	self.options:AddButton("Menu Button", function() KeyEntry:Hook(function(key) profile.key_menu = key end) end, "Menu / Pause / Exit")
	self.options:AddButton("Done",
	function()
		system:SaveSettings(settings.profile, profile)
		self:DeHook()
	end,
	"Save keys and return to Main Menu.")
end
