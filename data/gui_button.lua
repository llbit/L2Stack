--	GUIButton
GUIButton = {}
GUIButton.__index = GUIButton

function GUIButton:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUIButton:Init(text, action, x, y, face, face_f, face_a)
	self.font = fonts.medium
	self.text = text
	self.Action = function() if action then action();sounds.button:Play() end end
	self.texture = face or textures.button
	self.texture_f = face_f or textures.button_f
	self.texture_a = face_a or textures.button_a
	self.width = self.texture.width
	self.height = self.texture.height
	self:MoveTo(x, y)
end

function GUIButton:MoveTo(x, y)
	self.x, self.y = x, y
	self.tx = math.floor(x + (self.width - self.font*self.text) / 2)
	self.ty = math.floor(y + (self.texture.height - self.font.height) / 2)
end

function GUIButton:GetFocus(x, y)
	if x >= self.x and x <= self.x+self.width and
		y >= self.y and y <= self.y+self.height then
		return self
	else
		return nil
	end
end

function GUIButton:Draw(focus, active)
	local tx, ty = self.tx, self.ty
	if self == active then
		self.texture_a:Draw(self.x, self.y)
		tx, ty = tx+2, ty+2
	elseif self == focus then
		self.texture_f:Draw(self.x, self.y)
	else
		self.texture:Draw(self.x, self.y)
	end
	if self == focus or self == active then
		GUI.FGColor()
	else
		GUI.BGColor()
	end
	if self.text then
		self.font:Draw(self.text, tx, ty)
	end
	gl.Color3(1, 1, 1)
end

function GUIButton:Update()
end

function GUIButton:OnKeyDown(key)
end

function GUIButton:OnKeyUp(key)
end

