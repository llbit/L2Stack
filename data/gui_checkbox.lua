-- Checkbox
GUICheckbox = {
	text = "",
	state = 0,
	width = 250,
	font = fonts.medium}
GUICheckbox.__index = GUICheckbox

function GUICheckbox:new(...)
	new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUICheckbox:Init(text, apply)
	self.text = text
	self.button = GUIButton:new("", function() end,
		0, 0, textures.checkbox, textures.checkbox_f, textures.checkbox_a)
	self.Apply = function() apply(self.state) end
	self.height = textures.checkbox.height
end

function GUICheckbox:MoveTo(x, y)
	self.x, self.y = x, y
	local yoffset = (textures.checkbox.height - self.font.height) / 2
	self.tx = self.x + textures.checkbox.width + 10
	self.ty = math.floor(self.y + yoffset)
	self.button:MoveTo(self.x, self.y)
end

function GUICheckbox:GetFocus(x, y)
	self.focus = self.button:GetFocus(x, y)
	if x >= self.x and x <= self.x+self.width and
		y >= self.y and y <= self.y+self.height then
		return self
	end
	return nil
end

function GUICheckbox:Action()
	self.state = (self.state + 1) % 2
	sounds.button:Play()
end

function GUICheckbox:Draw(focus, active)
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
	if active == self then a = self.button end
	if self.state > 0 then
		self.button.texture, self.button.texture_f, self.button.texture_a =
			textures.checkbox, textures.checkbox_f, textures.checkbox_a
	else
		self.button.texture, self.button.texture_f, self.button.texture_a =
			textures.box, textures.box_f, textures.box_a
	end
	self.button:Draw(f, a)
	if self == focus then gl.Color3(0, 0, 0) else gl.Color3(215/255, 224/255, 55/255) end
	self.font:Draw(self.text, self.tx, self.ty)
	gl.Color3(1, 1, 1)
end

function GUICheckbox:Update()
end

function GUICheckbox:OnKeyDown(key)
end

function GUICheckbox:OnKeyUp(key)
end

