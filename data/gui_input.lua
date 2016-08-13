-- Numeric Entry Field
GUIInput = {font = fonts.medium}
GUIInput.__index = GUIInput

function GUIInput:new(default, pattern, x, y)
	local new = {
		default = default,
		text = default,
		pattern = pattern,
		width = textures.input.width,
		height = textures.input.height,
		x = x,
		y = y,
		tx = 5}
	setmetatable(new, self)
	new.ty = math.floor((textures.input.height - new.font.height) / 2)
	return new
end

function GUIInput:MoveTo(x, y)
	self.x, self.y = x, y
end

function GUIInput:GetFocus(x, y)
	if x >= self.x and x <= self.x+self.width and
		y >= self.y and y <= self.y+self.height then
		return self
	end
	return nil
end

function GUIInput:OnKeyDown(key)
	if key == KEY_RETURN then
		self:Apply()
	elseif key == KEY_BACKSPACE then
		self.text = ""
	end
end

function GUIInput:Apply()
	if not re_test(self.pattern, self.text) then
		Message:Hook("Error:", "Bad input: \""..self.text.."\"", GUI.state)
		self.text = ""
	else
		self:Ok()
	end
end

function GUIInput:Action()
	self.text = ""
end

function GUIInput:OnText(ch)
	self.text = self.text..ch
end

function GUIInput:Draw(focus, active)
	local texture = textures.input
	if focus == self then
		texture = textures.input_f
	end
	texture:Draw(self.x, self.y)
	gl.Color3(0,0,0)
	self.font:Draw(self.text, self.x + self.tx, self.y + self.ty)
	gl.Color3(1,1,1)
end
