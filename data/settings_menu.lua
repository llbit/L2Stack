-- SettingsMenu
SettingsMenu = GUIState:new()

function SettingsMenu:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function SettingsMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function SettingsMenu:OnHook()
	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("Settings"))
	self.options:AddButton("Select Profile", function() ProfileSelect:Hook() end)
	self.options:AddButton("Graphics", function() GraphicsSettings:Hook() end)
	self.options:AddButton("Game", function() GameSettings:Hook() end)
	self.options:AddButton("Key Bindings", function() KeySettings:Hook() end)
	self.options:AddButton("Back", function() self:DeHook() end)
end
