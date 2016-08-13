-- Checkbox
GUIKeySelector = {
	text = "",
	state = 0,
	width = 250,
	font = fonts.medium}
GUIKeySelector.__index = GUIKeySelector

function GUIKeySelector:new(...)
	new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUIKeySelector:Init(text, apply, key1, key2)
	self.key1 = system:GetKeyName(key1)
	if key2 then
		self.key2 = system:GetKeyName(key2)
	end
	self.text = text
	self.Apply = function() apply(self.state) end
	self.height = self.font.height
end

function GUIKeySelector:MoveTo(x, y)
	self.x, self.y = x, y
	local yoffset = (textures.checkbox.height - self.font.height) / 2
	self.tx = self.x + textures.checkbox.width + 10
	self.ty = math.floor(self.y + yoffset)
end

function GUIKeySelector:GetFocus(x, y)
	if x >= self.x and x <= self.x+self.width and
		y >= self.y and y <= self.y+self.height then
		return self
	end
	return nil
end

function GUIKeySelector:Action()
end

function GUIKeySelector:Draw(focus, active)
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
	
	if self == focus then gl.Color3(0, 0, 0) else gl.Color3(215/255, 224/255, 55/255) end
	self.font:Draw(self.text..self.key1, self.tx, self.ty)
	gl.Color3(1, 1, 1)
end

function GUIKeySelector:Update()
end
