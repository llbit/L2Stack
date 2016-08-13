--	AviRecorder
AviRecorder = GUIState:new()

function AviRecorder:End()
	self.avifile:Close()
	self.game:End()
	self:DeHook()
	GUI.state = self.owner
end

function AviRecorder:Update(millis)
	self.game:Update(millis)
end

function AviRecorder:Draw()
	self.game:Draw()
	if GUI.state == self then
		self.avifile:WriteFrame()
	end
end

function AviRecorder:OnKeyDown(key)
	if key == profile.key_menu or key == KEY_ESCAPE then
		PauseMenu:Hook()
	end
end

function AviRecorder:OnKeyUp(key)
end

function AviRecorder:OnHook(game, avifile)
	self.nocursor = true
	self.game = game
	self.avifile = avifile
end
