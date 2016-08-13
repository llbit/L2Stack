--	GUI List
GUIScrollbar = {}
GUIScrollbar.__index = GUIScrollbar

function GUIScrollbar:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUIScrollbar:Init(OnUpdate, x, y, height, start, stop, step)
	local buttonh = textures.arrowup.height
	local by = y + buttonh
	
	self.x, self.y = x, y
	self.width = textures.arrowup.width
	self.height = height
	self:SetRange(start, stop, step)
	self.cy = textures.box.height / 2
	self.OnUpdate = function() OnUpdate(self.pos) end
	self.btnup = GUIButton:new("",
		function()
			self:MoveTo(self.pos-step)
			self.delay = 23
		end, x, y, textures.arrowup, textures.arrowup_f, textures.arrowup_a)
	self.btndown = GUIButton:new("",
		function()
			self:MoveTo(self.pos+step)
			self.delay = 23
		end, x, y + height - buttonh, textures.arrowdown, textures.arrowdown_f, textures.arrowdown_a)
	self.ystart = y + buttonh
	self.scrollmarker = GUIButton:new("", nil, x, self.ystart, textures.box, textures.box_f, textures.box_a)
end

function GUIScrollbar:SetRange(start, stop, step)
	self.start, self.stop = start, stop
	self.step = step
	self.yrange = self.height - textures.arrowup.height*3
	self.range = stop - start
	self.pos = start
end

function GUIScrollbar:Draw(...)
	local x, y = self.x, self.y
	local pos = self.pos
	local width, height = self.width, self.height
	
	gl.Color3(0.86, 0.86, 0.86)
	gl.BeginQuads()
	gl.Vertex3(x, y, 0)
	gl.Vertex3(x + width, y, 0)
	gl.Vertex3(x + width, y + height, 0)
	gl.Vertex3(x, y + height, 0)
	gl.End()
	gl.Color3(1, 1, 1)
	
	self.btnup:Draw(...)
	self.btndown:Draw(...)
	self.scrollmarker:Draw(...)
end

function GUIScrollbar:MoveTo(pos)
	pos = math.max(pos, self.start)
	pos = math.min(pos, self.stop)
	self.pos = pos
	if self.range > 0 then
		self.scrollmarker.y = self.ystart + self.yrange * (pos - self.start) / self.range
		self:OnUpdate()
	end
end

function GUIScrollbar:GetFocus(...)
	if GUI.active == self.scrollmarker then
		local pos = self.start + self.range * (state.mouse.y - self.ystart - self.cy) / self.yrange
		self:MoveTo(pos)
	end
	
	return self.scrollmarker:GetFocus(...) or
		self.btnup:GetFocus(...) or
		self.btndown:GetFocus(...)
end

function GUIScrollbar:Update()
	if GUI.active == self.btnup then
		if self.delay > 0 then
			self.delay = self.delay - 1
		else
			self:MoveTo(self.pos-self.step)
		end
	elseif GUI.active == self.btndown then
		if self.delay > 0 then
			self.delay = self.delay - 1
		else
			self:MoveTo(self.pos+self.step)
		end
	end
end

function GUIScrollbar:OnKeyDown(key)
	if key == profile.key_hard or key == KEY_UP or key == MOUSEWHEELUP then
		system:WriteToLog("paging up")
		self:MoveTo(self.pos-self.step)
		self.delay = 23
		GUI.active = self.btnup
	elseif key == profile.key_soft or key == KEY_DOWN or key == MOUSEWHEELDOWN then
		self:MoveTo(self.pos+self.step)
		self.delay = 23
		GUI.active = self.btndown
	end
end

function GUIScrollbar:OnKeyUp(key)
	GUI.active = nil
end
