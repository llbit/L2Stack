-- File Menu
FileMenu = GUIState:new()

function FileMenu:Update(...)
	self.list:Update(...)
end

function FileMenu:Draw(...)
	self.owner:Draw()
	self.bg:Draw(self.bx, self.by)
	self.font:Draw(self.text, self.tx, self.ty)
	self.font:Draw("filename: ", self.fx, self.fy)
	gl.Color3(0.7, 0.7, 0.7)
	gl.BeginQuads()
	gl.Vertex3(self.lx, self.ly, 0)
	gl.Vertex3(self.lx + self.listw, self.ly, 0)
	gl.Vertex3(self.lx + self.listw, self.ly + self.listh, 0)
	gl.Vertex3(self.lx, self.ly + self.listh, 0)
	gl.End()
	self.list:Draw(...)
	self.input:Draw(...)
	self.ok:Draw(...)
	self.cancel:Draw(...)
end

function FileMenu:GetFocus(x, y)
	return self.ok:GetFocus(x, y)
		or self.cancel:GetFocus(x, y)
		or self.list:GetFocus(x, y)
		or self.input:GetFocus(x, y)
end

function FileMenu:OnKeyDown(key)
	if self.focus == self.input then
		self.focus:OnKeyDown(key)
	elseif key == profile.key_hard or
		key == KEY_UP or
		key == MOUSEWHEELUP or
		key == profile.key_soft or
		key == KEY_DOWN or
		key == MOUSEWHEELDOWN then
		self.list:OnKeyDown(key)
	elseif key == profile.key_arot or key == KEY_RETURN then
		self.input:Apply()
	elseif key == profile.key_menu or key == KEY_ESCAPE then
		self:DeHook()
	else
		GUIState.OnKeyDown(self, key)
	end
end

function FileMenu:OnKeyUp(key)
	if key == profile.key_hard or
		key == KEY_UP or
		key == MOUSEWHEELUP or
		key == profile.key_soft or
		key == KEY_DOWN or
		key == MOUSEWHEELDOWN then
		self.list:OnKeyUp(key)
	end
end

function FileMenu:Action()
	if self.focus then
		self.focus:Action()
	end
end

function FileMenu:Ok()
	self:DeHook()
	self.Load("replays/"..self.input.text..".rec")
end

function FileMenu:OnHook(action, default)
	local font = fonts.medium
	local files = system.GetFileList("replays", "^(.*)\\.rec$")
	
	self.Load = action
	self.bg = textures.filedialog
	self.listw, self.listh = 418, 162
	self.font = font
	self.lineh = font.height
	self.bx = (state.width - self.bg.width) / 2
	self.by = (state.height - self.bg.height) / 2
	self.text = "Select file:"
	self.tx = math.floor((state.width - font*self.text) / 2)
	self.ty = math.floor(self.by + 20)
	self.lx = (state.width - self.listw) / 2
	self.ly = self.ty + font.height + 5
	self.list = GUIList:new(files, self.lx, self.ly)
	self.list.OnUpdate = function(list, item) self.input.text = item end
	self.selected = 1
	self.ix = self.lx + self.listw - textures.input.width
	self.iy = self.ly + self.listh + 10
	local d = ""
	if default then
		d = default
	elseif files[1] then
		d = files[1]
	end
	self.input = GUIInput:new(d, "^[\\w\\d]*$", self.ix, self.iy)
	self.input.Ok = function() self:Ok() end
	self.fx = self.ix - font*"filename: "
	self.fy = self.iy
	self.okx = (state.width - textures.button.width * 2 - 30) / 2
	self.oky = self.iy + textures.input.height + 20
	self.cx = self.okx + textures.button.width + 30
	self.ok = GUIButton:new("Ok", function() self:Ok() end, self.okx, self.oky)
	self.cancel = GUIButton:new("Cancel", function() self:DeHook() end, self.cx, self.oky)
end
