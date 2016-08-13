--	PauseMenu
PauseMenu = GUIState:new()

function PauseMenu:Update()
end

function PauseMenu:Draw(...)
	self.owner:Draw()
	self.options:Draw(...)
end

function PauseMenu:GetFocus(x, y)
	return self.options:GetFocus(x, y)
end

function PauseMenu:OnHook()
	self.options = OptionsList:new(textures.pause)
	self.options:AddButton("Resume", function() self:DeHook() end)
	if self.owner.Restart then
		self.options:AddButton("Restart", function() state.mouse.visible = false; self.owner:Restart()self:DeHook() end)
	end
	if self.owner.SaveReplay then
		self.options:AddButton("Save Replay", function() self.owner:SaveReplay() end)
	end
	self.options:AddButton("Quit", function() self.owner:End() end)
end
