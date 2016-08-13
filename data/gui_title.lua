-- Numeric Entry Field
GUITitle = {
	width = 250,
	font = fonts.medium,
	unselectable = true}
GUITitle.__index = GUITitle

function GUITitle:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUITitle:Init(text)
	self.text = text
end

function GUITitle:MoveTo(x, y)
	self.x, self.y = x, y
	self.height = self.font.height
	self.tx = math.floor(self.x + (self.width - self.font*self.text)/2)
end

function GUITitle:GetFocus(x, y)
	return nil
end

function GUITitle:Action()
end

function GUITitle:OnKeyDown(key)
end

function GUITitle:OnKeyUp(key)
end

function GUITitle:Draw(focus, active)
	self.font:Draw(self.text, self.tx, self.y)
end

function GUITitle:Update()
end
