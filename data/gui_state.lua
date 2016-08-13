-- GUIState
GUIState = {owner = nil}

function GUIState:new()
	local new = {}
	setmetatable(new, self)
	self.__index = self
	return new
end

function GUIState:Update(...)
	system:UpdateMetacube()
	if self.options then
		self.options:Update(...)
	end
end

function GUIState:Draw(focus, active)
	system:DrawMetacube()
end

function GUIState:OnKeyDown(key)
	if key == profile.key_menu or key == KEY_ESCAPE then
		self:DeHook()
	elseif self.options and key == MOUSEWHEELUP then
		local sb = self.options.scrollbar
		sb:MoveTo(sb.pos-1)
	elseif self.options and key == MOUSEWHEELDOWN then
		local sb = self.options.scrollbar
		sb:MoveTo(sb.pos+1)
	elseif self.options then
		self.options:OnKeyDown(key)
	elseif GUI.focus and GUI.focus.OnKeyDown then
		GUI.focus:OnKeyDown(key)
	end
end

function GUIState:OnKeyUp(key)
	if GUI.focus and GUI.focus.OnKeyUp then
		GUI.focus:OnKeyUp(key)
	end
end

function GUIState:OnText(ch)
	if GUI.focus and GUI.focus.OnText then
		GUI.focus:OnText(ch)
	end
end

function GUIState:GetFocus(x, y)
	return nil
end

function GUIState:Hook(...)
	system:MetaMorph()
	self.owner = GUI.state
	GUI.state = self
	GUI.description = nil
	
	self:OnHook(...)
	GUI.focus = self:GetFocus(state.mouse.x, state.mouse.y)
end

function GUIState:DeHook()
	system:MetaMorph()
	GUI.state = self.owner
	if GUI.state then GUI.focus = self.owner:GetFocus(state.mouse.x, state.mouse.y) end
end
