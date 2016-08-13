-- GUI
GUI = {
	state = nil,
	ttdelay = 0,
	anim = 0,
	cursor = {}}

function GUI:Init()
	for i=1,25 do
		self.cursor[i] = system:LoadTexture(string.format("data/%04d.png", i))
	end
end

function GUI:Update(millis)
	-- for cursor animation
	self.anim = (self.anim + millis) % 1000

	self.state:Update(millis)
	if self.ttdelay > 0 then
		self.ttdelay = self.ttdelay - 1
	end
end

function GUI:Draw()
	self.state:Draw(self.focus, self.active)
	if not self.state.nocursor then
		local curframe = math.floor(self.anim / 40) + 1
		self.cursor[curframe]:Draw(state.mouse.x, state.mouse.y)
	end
	if self.tooltip and self.ttdelay == 0 then
		local boxw = textures.tooltip.width
		local boxh = textures.tooltip.height
		textures.tooltip:Draw(self.tx, self.ty)
		gl.Color3(0, 0, 0)
		fonts.small:DrawRegion(self.tooltip, self.tx + 40, self.ty + 30, boxw - 60, boxh - 40)
		gl.Color3(1, 1, 1)
	end
end

function GUI:OnKeyDown(key)
	self.tooltip = nil
	if key == profile.key_arot or key == KEY_RETURN or key == MOUSEBUTTON1 then
		state.mouse.sx, state.mouse.sy = state.mouse.x, state.mouse.y
		self.focus = self.state:GetFocus(state.mouse.x, state.mouse.y)
		self.active = self.focus
		if self.active then self.active:Action();return end
	end
	self.state:OnKeyDown(key)
end

function GUI:OnKeyUp(key)
	self.state:OnKeyUp(key)
	if key == profile.key_arot or key == KEY_RETURN or key == MOUSEBUTTON1 then
		self.active = nil
	end
end

function GUI.FGColor()
	gl.Color3(0.97, 0.97, 0.97)
end

function GUI.BGColor()
	gl.Color3(0, 0, 0)
end

function GUI:OnText(ch)
	self.state:OnText(ch)
end

function GUI:OnMouseMotion(x, y)
	state.mouse.x, state.mouse.y = x, y
	self.focus = self.state:GetFocus(x, y)
	if not self.focus then
		self.tooltip = nil
	elseif self.focus.tooltip ~= self.tooltip then
		self.tooltip = self.focus.tooltip
		self.tx = self.focus.x + self.focus.width
		self.ty = math.floor(self.focus.y + self.focus.height/2 - 15)
		self.ttdelay = 87
	end
end

function GUI:OnError(msg)
	Message:Hook("Error:", msg)
end
