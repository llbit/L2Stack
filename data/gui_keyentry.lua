--	Key Entry
KeyEntry = GUIState:new()

function KeyEntry:Draw()
	self.owner:Draw()
	textures.keyentry:Draw(self.x, self.y) 
end

function KeyEntry:OnKeyDown(key)
	self.func(key)
	self:DeHook()
end

function KeyEntry:OnHook(func)
	self.width = textures.keyentry.width
	self.height = textures.keyentry.height
	self.x, self.y = (state.width - self.width) / 2, (state.height - self.height) / 2
	self.func = func
end
