-- Selector
GUISelector = {
	width = 250,
	state = 1,
	font = fonts.medium}
GUISelector.__index = GUISelector

function GUISelector:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUISelector:Init(text, opts, apply, x, y)
	self.text = text
	self.opts = opts
	self.focus = nil
	self.x, self.y = x, y
	self.height = self.font.height*2+2
	self.Apply = function() apply(self.state) end
	self.btnleft = GUIButton:new("",
		function ()
			self:PrevItem()
			self.delay = 23
		end, x, y, textures.arrowleft, textures.arrowleft_f, textures.arrowleft_a)
	self.btnright = GUIButton:new("",
		function ()
			self:NextItem()
			self.delay = 23
		end, x, y, textures.arrowright, textures.arrowright_f, textures.arrowright_a)
end

function GUISelector:MoveTo(x, y)
	self.x, self.y = x, y
	local buttonwidth = textures.arrowleft.width
	local yoffset = (self.height - textures.arrowleft.height) / 2
	self.rb_offset = GUISelector.width - buttonwidth
	self.tx = math.floor(self.x + buttonwidth + 5)
	self.btnleft:MoveTo(self.x, self.y + yoffset)
	self.btnright:MoveTo(self.x + self.rb_offset, self.y + yoffset)
end

function GUISelector:GetFocus(x, y)
	self.focus = self.btnleft:GetFocus(x, y)
		or self.btnright:GetFocus(x, y)
	if x >= self.x and x <= self.x+self.width and
		y >= self.y and y <= self.y+self.height then
		return self
	end
	return nil
end

function GUISelector:Action()
	if self.focus then self.focus:Action() end
end

function GUISelector:OnKeyDown(key, yoffs)
	if key == profile.key_right or key == KEY_RIGHT then
		self:NextItem()
		self.delay = 23
		GUI.active = self
		self.focus = self.btnright
		state.mouse.x = self.btnright.x+self.btnright.width-5
		state.mouse.y = self.btnright.y+self.btnright.height-5 + yoffs
	elseif key == profile.key_left or key == KEY_LEFT then
		self:PrevItem()
		self.delay = 23
		GUI.active = self
		self.focus = self.btnleft
		state.mouse.x = self.btnleft.x+self.btnleft.width-5
		state.mouse.y = self.btnleft.y+self.btnleft.height-5 + yoffs
	end
end

function GUISelector:OnKeyUp(key)
	GUI.active = nil
end

function GUISelector:NextItem()
	self.state = self.state + 1
	if self.state > #self.opts then
		self.state = 1
	end
end

function GUISelector:PrevItem()
	self.state = self.state - 1
	if self.state < 1 then
		self.state = #self.opts
	end
end

function GUISelector:Draw(focus, active)
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
	self.btnleft:Draw(f, a)
	self.btnright:Draw(f, a)
	if self == focus then gl.Color3(0, 0, 0) else gl.Color3(215/255, 224/255, 55/255) end
	self.font:Draw(self.text, self.tx, y)
	if self == focus then gl.Color3(1, 1, 1) else gl.Color3(0.7, 0.7, 0.7) end
	self.font:Draw(self.opts[self.state], self.tx + 7, y + self.font.height + 2)
	gl.Color3(1, 1, 1)
end

function GUISelector:Update()
	if GUI.active == self and self.focus == self.btnright then
		if self.delay > 0 then
			self.delay = self.delay - 1
		else
			self:NextItem()
		end
	elseif GUI.active == self and self.focus == self.btnleft then
		if self.delay > 0 then
			self.delay = self.delay - 1
		else
			self:PrevItem()
		end
	end
end
