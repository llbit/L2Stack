--	Message
Message = GUIState:new()

function Message:Draw()
	if self.parent then
		self.parent:Draw()
	end
	textures.message:Draw(self.x, self.y)
	gl.Color3(0, 0, 0)
	fonts.medium:Draw(self.title, self.x+50, self.y+30)
	fonts.small:DrawRegion(self.text, self.x+50, self.y+60, self.w, self.h)
	gl.Color3(1, 1, 1)
end

function Message:OnKeyDown()
	self:DeHook()
	if self.func then self:func() end
end

function Message:OnHook(title, text, parent, func)
	self.func = func
	self.title = title
	self.nocursor = true
	self.parent = parent
	self.text = text
	local w, h = textures.message.width, textures.message.height
	self.x, self.y = (state.width - w) / 2, (state.height - h) / 2
	self.w, self.h = w - 100, h - 140
end
