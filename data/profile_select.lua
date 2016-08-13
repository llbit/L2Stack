-- ProfileSelect
ProfileSelect = GUIState:new()

function ProfileSelect:Draw(...)
	GUIState:Draw(...)
	self.options:Draw(...)
end

function ProfileSelect:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function ProfileSelect:OnHook()
	local files = system.GetFileList("profiles", "^(.*)\\.ini$")

	self.options = OptionsList:new()
	self.options:AddOption(GUITitle:new("Select Profile"))
	for i,v in ipairs(files) do
		self.options:AddButton(v,
		function()
			settings.profile=v
			self:DeHook()
		end)
	end
	self.options:AddButton("New Profile", function() NewProfile:Hook() end)
	self.options:AddButton("Back", function() self:DeHook() end)
end
