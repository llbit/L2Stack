--	OptionsList
OptionsList = {
	xoffs = 20,
	yoffs = 20,
	ypadd = 12,
	opts = nil}
OptionsList.__index = OptionsList

function OptionsList:new(...)
	local new = {}
	setmetatable(new, self)
	new:Init(...)
	return new
end

function OptionsList:Init(bg)
	self.bg = bg or textures.menu
	self.width = self.bg.width
	self.height = self.bg.height
	self.nopts = 0
	self.x, self.y = (state.width - self.width) / 2, (state.height - self.height) / 2
	local x, y = self.x, self.y
	self.bottom = y + self.yoffs
	local sbx = x + self.width - textures.arrowup.width - self.xoffs
	self.ym = 0
	self.drawbar = false
	self.scrollbar = GUIScrollbar:new(
		function(pos)
			self.ym = ((pos-1)/self.nopts) * (-math.max(self.bottom - self.y - (self.height - self.yoffs*2), 0))
		end, sbx, y+self.yoffs, self.height-self.yoffs*2, 1, 1, 1)
end

function OptionsList:GetNextOption()
	local opt = GUI.focus
	while true do
		opt = self:GetNext(opt)
		if not opt or (not opt.unselectable) then return opt end
	end
end

function OptionsList:GetNext(obj)
	for i,v in ipairs(self.opts) do
		if v == obj then
			if i == self.nopts then
				break
			else
				self.scrollbar:MoveTo(i+1)
				return self.opts[i+1]
			end
		end
	end
	self.scrollbar:MoveTo(1)
	return self.opts[1]
end

function OptionsList:GetPrevOption()
	local opt = GUI.focus
	while true do
		opt = self:GetPrev(opt)
		if not opt or (not opt.unselectable) then return opt end
	end
end

function OptionsList:GetPrev(obj)
	for i,v in ipairs(self.opts) do
		if v == obj then
			if i == 1 then
				break
			else
				self.scrollbar:MoveTo(i-1)
				return self.opts[i-1]
			end
		end
	end
	self.scrollbar:MoveTo(self.nopts)
	return self.opts[self.nopts]
end

function OptionsList:AddButton(text, func, tooltip)
	local button = GUIButton:new(text, func, 0, 0)
	button.tooltip = tooltip
	self:AddOption(button, x, y)
	return button
end

function OptionsList:AddCheckbox(text, func, state, tooltip)
	local checkbox = GUICheckbox:new(text, func)
	checkbox.tooltip = tooltip
	checkbox.state = state
	self:AddOption(checkbox)
	return checkbox
end

function OptionsList:AddSelector(text, opts, apply, default, tooltip)
	local selector = GUISelector:new(text, opts, apply, 0, 0)
	selector.tooltip = tooltip
	for i,v in ipairs(opts) do
		if v == default then
			selector.state = i
			break
		end
	end
	self:AddOption(selector)
	return selector
end

function OptionsList:AddNumeric(text, start, stop, step, default, tooltip)
	local numeric = GUINumeric:new(text, start, stop, step, 0, 0)
	numeric.state = default or start
	numeric.tooltip = tooltip
	self:AddOption(numeric)
	return numeric
end

function OptionsList:AddInput(default, pattern, tooltip)
	local input = GUIInput:new(default, pattern, 0, 0)
	input.tooltip = tooltip
	self:AddOption(input)
	return input
end

function OptionsList:AddOption(option)
	self.opts = self.opts or {}
	self.nopts = self.nopts + 1
	self.opts[self.nopts] = option
	option:MoveTo(self.x + (self.width - option.width) / 2, self.bottom)
	self.bottom = self.bottom + option.height + self.ypadd
	if self.bottom > (self.y+self.height-self.yoffs) then
		self.drawbar = true
	end
	self.scrollbar:SetRange(1, self.nopts, 1)
end

function OptionsList:Draw(...)
	if self.bg then
		self.bg:Draw(self.x, self.y)
	end
	
	if self.drawbar then
		self.scrollbar:Draw(...)
		
		gl.ClipPlane(0, 0, 1, 0, -(self.y+self.yoffs))
		gl.ClipPlane(1, 0, -1, 0, self.y+self.height-self.yoffs)
		gl.EnableClipPlane(0)
		gl.EnableClipPlane(1)
		gl.PushMatrix()
		gl.Translate(0, self.ym, 0)
	end
	
	for k,v in pairs(self.opts) do
		v:Draw(...)
	end
	
	if self.drawbar then
		gl.PopMatrix()
		gl.DisableClipPlane(0)
		gl.DisableClipPlane(1)
	end
end

function OptionsList:Update(...)
	self.scrollbar:Update(...)
	for k,v in pairs(self.opts) do
		v:Update(...)
	end
end

function OptionsList:OnKeyDown(key)
	if key == profile.key_soft or key == KEY_DOWN then
		local focus = self:GetNextOption()
		state.mouse.x, state.mouse.y = focus.x+focus.width-5, focus.y+focus.height-5+self.ym
		GUI.focus = focus
		GUI.active = nil
	elseif key == profile.key_hard or key == KEY_UP then
		local focus = self:GetPrevOption()
		state.mouse.x, state.mouse.y = focus.x+focus.width-5, focus.y+focus.height-5+self.ym
		GUI.focus = focus
		GUI.active = nil
	elseif GUI.focus then
		GUI.focus:OnKeyDown(key, self.ym)
	end
end

function OptionsList:GetFocus(x, y)
	local f = self.scrollbar:GetFocus(x, y)
	if f then return f end
	for k,v in pairs(self.opts) do
		f = v:GetFocus(x, y - self.ym)
		if f then return f end
	end
	return nil
end
