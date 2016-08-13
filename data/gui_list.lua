--	GUI List
GUIList = {
	width = 418,
	height = 162}
GUIList.__index = GUIList

function GUIList:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function GUIList:Init(items, x, y)
	local font = fonts.medium
	local nitems = #items
	
	self.font = font
	self.lineh = font.height + 3
	self.items = items
	self.x, self.y = x, y
	self.selected = 1
	self.istart = 1
	self.lineheight = font.height + 4
	self.nmax = math.floor(self.height / self.lineheight) - 1
	self.istop = math.min(self.nmax+1, nitems)
	--system:WriteToLog("istop: "..self.istop)
	self.nitems = nitems
	local sbx = x + self.width - textures.arrowup.width
	self.scrollbar = GUIScrollbar:new(
		function(pos)
			self.selected = math.floor(pos)
			self:OnUpdate(self.items[self.selected])
		end, sbx, y, self.height, 1, nitems, 1)
end

function GUIList:Draw(...)
	local x, y = self.x, self.y
	local width, height = self.width, self.height
	
	gl.Color3(0.7, 0.7, 0.7)
	gl.BeginQuads()
	gl.Vertex3(x, y, 0)
	gl.Vertex3(x + width, y, 0)
	gl.Vertex3(x + width, y + height, 0)
	gl.Vertex3(x, y + height, 0)
	gl.End()
	
	if self.nitems > 0 then
		self:DrawItems()
	end
	
	self.scrollbar:Draw(...)
end

function GUIList:DrawItems()
	local x, y = self.x, self.y
	local width, height = self.width, self.height
	local font = self.font
	local lineheight = self.lineheight
	
	gl.Color3(0, 0, 0)
	local selected = self.selected
	if selected > self.istop then
		self.istop = selected
		self.istart = self.istop - self.nmax
	elseif selected < self.istart then
		self.istart = selected
		self.istop = self.istart + self.nmax
	end
	for i = self.istart, self.istop do
		if selected == i then
			gl.Color3(0.9, 0.9, 0.9)
			gl.BeginQuads()
			gl.Vertex3(x, y, 0)
			gl.Vertex3(x + width, y, 0)
			gl.Vertex3(x + width, y + lineheight, 0)
			gl.Vertex3(x, y + lineheight, 0)
			gl.End()
			gl.Color3(0, 0, 0)
		else
		end
		y = y + 2
		self.font:Draw(self.items[i], x+10, y)
		y = y + lineheight-2
	end
	gl.Color3(1, 1, 1)
end

function GUIList:Update(...)
	self.scrollbar:Update(...)
end

function GUIList:OnUpdate(item)
end

function GUIList:GetFocus(...)
	return self.scrollbar:GetFocus(...)
end

function GUIList:OnKeyDown(key)
	self.scrollbar:OnKeyDown(key)
end

function GUIList:OnKeyUp(key)
	self.scrollbar:OnKeyUp(key)
end

function GUIList:Action(key)
end

function GUIList:NextItem()
	self.selected = self.selected + 1
	if self.selected > self.nitems then
		self.selected = 1
	end
	self.scrollbar:MoveTo(self.selected)
	self:OnUpdate(self.items[self.selected])
end

function GUIList:PrevItem()
	self.selected = self.selected - 1
	if self.selected < 1 then
		self.selected = self.nitems
	end
	self.scrollbar:MoveTo(self.selected)
	self:OnUpdate(self.items[self.selected])
end
