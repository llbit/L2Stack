--	MPGame
MPGame = GUIState:new()

function MPGame:Restart()
	self.game1:Restart()
	self.game2:Restart()
	self:SetUp()
end

function MPGame:End()
	self:DeHook()
	self.game1:End()
	self.game2:End()
end

function MPGame:Update(millis)
	self.flash = (self.flash + 1) % 3
	
	if self.ready > 0 then
		self.ready = self.ready - 1
		if self.ready == 0 then
			sounds.go:Play()
		end
	elseif self.go > 0 then
		self.go = self.go - 1
	else
		if state.game_over then
			self.gotime = self.gotime + 1
			if self.gotime == 240 then PauseMenu:Hook() end
		end
		self.game1:Update()
		if not self.winner and state.game_over then
			self.winner = self.game2
		end
		self.game2:Update()
		if not self.winner and state.game_over then
			self.winner = self.game1
		end
		if not state.game_over then
			state.time = state.time + millis
		end
	end
	
end

function MPGame:Draw()
	local scale = self.scale
	local xoffset = (state.width/2 - 320*scale) / 2
	local yoffset = (state.height - 640*scale) / 2
	
	self:DrawBg()
	
	gl.PushMatrix()
	gl.Translate(xoffset, yoffset, 0)
	gl.Scale(scale, scale, scale)
	self:DrawGame(self.game1, 0.13, 0.96, 0.21)
	gl.PopMatrix()
	
	gl.PushMatrix()
	gl.Translate(xoffset+state.width/2, yoffset, 0)
	gl.Scale(scale, scale, scale)
	self:DrawGame(self.game2, 0.12, 0.23, 0.96)
	gl.PopMatrix()
	
	gl.PopMatrix()
end

function MPGame:DrawGame(game, bc0, bc1, bc2)
	if profile.grid > 0 then
		textures.grid:DrawTiled(0, 0, 320, 640)
	else
		gl.Color4(0, 0, 0, 0.85)
		gl.BeginQuads()
		gl.Vertex3(0, 0, 0)
		gl.Vertex3(320, 0, 0)
		gl.Vertex3(320, 640, 0)
		gl.Vertex3(0, 640, 0)
		gl.End()
		gl.Color4(1, 1, 1, 1)
	end
	self.preview:Draw(-16, -128)
	
	if self.credits ~= nil and self.credits > 0 then
		local y = 640 * (self.credits / 1800 - 1)
		local v0, v1 = 0, 1
		if y < 0 then
			v0 = -y / 640
			y = 0
		else
			v1 = 1 - y / 640
		end
		textures.credits:DrawRegion(0, y, 0, v0, 1, v1)
	end
	
	game:Draw()-- Draw playfield
	
	gl.Color3(bc0, bc1, bc2)
	textures.border_tl:Draw(-16, -16)
	textures.border_t:DrawTiled(0, -16, 320, 16)
	textures.border_tr:Draw(320, -16)
	textures.border_r:DrawTiled(320, 0, 16, 640)
	textures.border_br:Draw(320, 640)
	textures.border_b:DrawTiled(0, 640, 320, 16)
	textures.border_bl:Draw(-16, 640)
	textures.border_l:DrawTiled(-16, 0, 16, 640)
	gl.Color3(1, 1, 1)
	
	local height = fonts.large.height*1.5
	
	local t = state.time
	text = string.format('%02d:%02d:%02d',
		math.floor(t / 60000) % 60,
		math.floor(t / 1000) % 60,
		math.floor(t / 10) % 100)
	fonts.large:Draw(text, (320 - self.timew)/2, 32*21)
	
	if state.time > 0 then
		fonts.large:Draw(string.format('%d', game:GetNumTets()/(state.time/60000)), 32*11, 0)
	end
	
	game:PostRender()
	
	if self.ready > 0 then
		textures.ready:Draw(self.readyx, self.readyy - 0.1 * self.ready^2)
	elseif self.go > 0 then 
		textures.go:Draw(self.gox, self.goy - 0.1 * self.go^2)
	elseif state.game_over and self.winner == game then
		local msg = "Winner"
		local x, y = (320 - fonts.large * msg) / 2, (640 - fonts.large.height) / 2
		if self.flash == 0 then
			gl.Color3(1, 0.9, 0)
		elseif self.flash == 1 then
			gl.Color3(1, 0.8, 0)
		elseif self.flash == 2 then
			gl.Color3(1, 1, 1)
		end
		fonts.large:Draw(msg, x, y)
		gl.Color3(1, 1, 1)
	end
end

function MPGame:DrawBg()
	self.bg:Draw(0, self.bgy, state.width, self.bgh)
end

function MPGame:OnKeyDown(key)
	if key == profile.key_menu or key == KEY_ESCAPE then
		PauseMenu:Hook()
	elseif self.game1.OnKeyDown then
		local gk = self.gamekeys[key]
		if gk then self.game1:OnKeyDown(gk) end
	end
end

function MPGame:OnKeyUp(key)
	if self.game1.OnKeyUp then
		local gk = self.gamekeys[key]
		if gk then self.game1:OnKeyUp(gk) end
	end
end

function MPGame:OnHook(game1, game2)
	self.nocursor = true
	self.game1 = game1
	self.game2 = game2
	self.bc = {}
	self.scale = 0.8 * (state.height / 768)
	self:Preload()
	self:SetUp()
end

function MPGame:Preload()
	system:LoadSound("data/roll.wav")
	system:LoadSound("data/section.wav")
	system:LoadSound("data/grade.wav")
	system:LoadSound("data/sectionwarn.wav")
	system:LoadSound("data/drop.wav")
	system:LoadSound("data/lock.wav")
	system:LoadSound("data/impact.wav")
	system:LoadSound("data/clear.wav")
	system:LoadSound("data/irs.wav")
	system:LoadSound("data/hard.wav")
	system:LoadSound("data/ready.wav")
	system:LoadSound("data/go.wav")
	system:LoadSound("data/i.wav")
	system:LoadSound("data/j.wav")
	system:LoadSound("data/l.wav")
	system:LoadSound("data/o.wav")
	system:LoadSound("data/s.wav")
	system:LoadSound("data/t.wav")
	system:LoadSound("data/z.wav")
	system:LoadSound("data/shift.wav")
	system:LoadSound("data/rotate.wav")
	system:LoadSound("data/tetris.wav")
	system:LoadSound("data/hold.wav")
	system:LoadTexture("data/grid.png")
	system:LoadTexture("data/ready.png")
	system:LoadTexture("data/go.png")
	system:LoadTexture("data/score.png")
	system:LoadTexture("data/level.png")
	system:LoadTexture("data/goal.png")
	system:LoadTexture("data/preview.png")
	system:LoadTexture("data/previewhold.png")
	system:LoadTexture("data/border_tl.png")
	system:LoadTexture("data/border_t.png")
	system:LoadTexture("data/border_tr.png")
	system:LoadTexture("data/border_r.png")
	system:LoadTexture("data/border_br.png")
	system:LoadTexture("data/border_b.png")
	system:LoadTexture("data/border_bl.png")
	system:LoadTexture("data/border_l.png")
	system:LoadTexture("data/credits.png")
	self.preview = textures.previewhold
	self.bg = textures.bgai
	self.bgh = (self.bg.height / self.bg.width) * state.width
	self.bgy = math.floor((state.height - self.bgh) / 2)
	self.readyx = (320 - textures.ready.width) / 2
	self.readyy = (640 - textures.ready.height) / 2
	self.gox = (320 - textures.go.width) / 2
	self.goy = (640 - textures.go.height) / 2
	local gamekeys = {}
	gamekeys[profile.key_hard] = GAMEKEY_HARD
	gamekeys[profile.key_soft] = GAMEKEY_SOFT
	gamekeys[profile.key_left] = GAMEKEY_LEFT
	gamekeys[profile.key_right] = GAMEKEY_RIGHT
	gamekeys[profile.key_arot] = GAMEKEY_AROT
	gamekeys[profile.key_brot] = GAMEKEY_BROT
	gamekeys[profile.key_crot] = GAMEKEY_CROT
	gamekeys[profile.key_hold] = GAMEKEY_HOLD
	self.gamekeys = gamekeys
	self.timew = fonts.large*"00:00:00"
end

function MPGame:SetUp()
	self.game1:AddOpponent(self.game2)
	self.game2:AddOpponent(self.game1)
	self.rank = state.rank
	self.goal = state.goal
	state.result = nil
	self.rankup = 0
	state.time = 0
	state.game_over = nil
	self.roll = nil
	self.credits = nil
	self.gotime = 0
	self.flash = 0
	self.winner = nil
	self.ready, self.go = 40, 40
	sounds.ready:Play()
end
