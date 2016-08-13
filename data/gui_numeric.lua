-- Numeric Entry Field
GUINumeric = {
	width = 250,
	font = fonts.medium}
GUINumeric.__index = GUINumeric

function GUINumeric:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUINumeric:Init(text, start, stop, step, x, y)
	self.text = text
	self.start = start
	self.stop = stop
	self.step = step
	self.state = start
	self.height = self.font.height*2+2
	self.x, self.y = x, y
	self.btnsub = GUIButton:new("",
		function()
			self:PrevItem()
			self.delay = 23
		end, x, y, textures.btnsub, textures.btnsub_f, textures.btnsub_a)
	self.btnadd = GUIButton:new("",
		function()
			self:NextItem()
			self.delay = 23
		end, x, y, textures.btnadd, textures.btnadd_f, textures.btnadd_a)
end

function GUINumeric:MoveTo(x, y)
	self.x, self.y = x, y
	local buttonwidth = textures.arrowleft.width
	local yoffset = (self.height - textures.arrowleft.height) / 2
	self.rb_offset = GUISelector.width - buttonwidth
	self.tx = math.floor(self.x + buttonwidth + 5)
	self.btnsub:MoveTo(self.x, self.y + yoffset)
	self.btnadd:MoveTo(self.x + self.rb_offset, self.y + yoffset)
end

function GUINumeric:GetFocus(x, y)
	self.focus = self.btnsub:GetFocus(x, y)
		or self.btnadd:GetFocus(x, y)
	if x >= self.x and x <= self.x+self.width and
		y >= self.y and y <= self.y+self.height then
		return self
	end
	return nil
end

function GUINumeric:Action()
	if self.focus then self.focus:Action() end
end

function GUINumeric:OnKeyDown(key)
	if key == profile.key_right or key == KEY_RIGHT then
		self:NextItem()
		self.delay = 23
		GUI.active = self
		self.focus = self.btnadd
		state.mouse.x, state.mouse.y = self.btnadd.x+self.btnadd.width-5, self.btnadd.y+self.btnadd.height-5
	elseif key == profile.key_left or key == KEY_LEFT then
		self:PrevItem()
		self.delay = 23
		GUI.active = self
		self.focus = self.btnsub
		state.mouse.x, state.mouse.y = self.btnsub.x+self.btnsub.width-5, self.btnsub.y+self.btnsub.height-5
	end
end

function GUINumeric:OnKeyUp(key)
	GUI.active = nil
end

function GUINumeric:NextItem()
	self.state = self.state + self.step
	if self.state > self.stop then
		self.state = self.start
	end
end

function GUINumeric:PrevItem()
	self.state = self.state - self.step
	if self.state < self.start then
		self.state = self.stop
	end
end

function GUINumeric:Draw(focus, active)
	local x, y = self.x, self.y
	local width, height = self.width, self.height
	
	if self == focus then
		gl.Color4(1, 1, 1, 0.5)
		gl.BeginQuads()
		gl.Vertex3(x, y, 0)
		gl.Vertex3(x+width, y, 0)
		gl.Vertex3(x+width, y+height, 0)
		gl.Vertex3(x, y+height, 0)
		gl.End()
		gl.Color4(1, 1, 1, 1)
	end
	
	local f, a = nil, nil
	if focus == self then f = self.focus end
	if active == self then a = self.focus end
	self.btnsub:Draw(f, a)
	self.btnadd:Draw(f, a)
	if self == focus then gl.Color3(0, 0, 0) else gl.Color3(215/255, 224/255, 55/255) end
	self.font:Draw(self.text, self.tx, y)
	if self == focus then gl.Color3(1, 1, 1) else gl.Color3(0.7, 0.7, 0.7) end
	self.font:Draw(string.format("%d", self.state), self.tx + 7, y + self.font.height + 2)
	gl.Color3(1, 1, 1)
end

function GUINumeric:Update()
	if GUI.active == self and self.focus == self.btnadd then
		if self.delay > 0 then
			self.delay = self.delay - 1
		else
			self:NextItem()
		end
	elseif GUI.active == self and self.focus == self.btnsub then
		if self.delay > 0 then
			self.delay = self.delay - 1
		else
			self:PrevItem()
		end
	end
end
