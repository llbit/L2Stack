--	SPGame
SPGame = GUIState:new()

function SPGame:Restart()
	self.game:Restart()
	self:SetUp()
end

function SPGame:End()
	self:DeHook()
	self.game:End()
	state.score = nil
	state.level = nil
	state.goal = nil
end

function SPGame:Update(millis)
	
	self.flash = (self.flash + 1) % 3
	
	if self.ready > 0 then
		self.ready = self.ready - 1
		if self.ready == 0 then
			sounds.go:Play()
		end
	elseif self.go > 0 then
		self.go = self.go - 1
	else
		if state.roll and not self.roll then
			sounds.roll:Play()
			self.roll = state.roll
		end
		self.credits = state.credits
		if state.rank and state.rank ~= self.rank then
			sounds.grade:Play()
			self.rank = state.rank
			self.rankup = 20
		else
			self.rankup = math.max(self.rankup - 1, 0)
		end
		if state.goal and state.goal ~= self.goal then
			sounds.section:Play()
			self.goal = state.goal
		end
		if state.game_over then
			self.gotime = self.gotime + 1
			if self.gotime == 240 then PauseMenu:Hook() end
		end
		self.game:Update()
		if not (state.game_over or state.roll) then
			state.time = state.time + millis
		end

		self.sl = self.sl + millis
	end
	
end

function SPGame:DrawScanlines()
	gl.ClipPlane(0, 0, 1, 0, 0)
	gl.ClipPlane(1, 0, -1, 0, 640)
	gl.EnableClipPlane(0)
	gl.EnableClipPlane(1)
	local ys = math.floor(self.sl / 3) % (640+256)
	state.scanline = ys
	textures.scanlines:Draw(0, ys-256, 320, ys)
	gl.DisableClipPlane(0)
	gl.DisableClipPlane(1)
end

function SPGame:Draw()
	local scale = self.scale
	local xoffset = (state.width - 320*scale) / 2
	local yoffset = (state.height - 640*scale) / 2
	
	-- background image
	self:DrawBg()
	
	gl.PushMatrix()
	gl.Translate(xoffset, yoffset, 0)
	gl.Scale(scale, scale, scale)
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
	if self.mode == "radar" then
		self:DrawScanlines()
	end
	self.preview:Draw(-16, -128)
	
	if self.credits ~= nil and self.credits > 0 then
		local y = 640 * (self.credits / 1845 - 1)
		local v0, v1 = 0, 1
		if y < 0 then
			v0 = -y / 640
			y = 0
		else
			v1 = 1 - y / 640
		end
		textures.credits:DrawRegion(0, y, 0, v0, 1, v1)
	end
	
	self.game:Draw()-- Draw playfield
	
	gl.Color3(self.bc.r, self.bc.g, self.bc.b)
	textures.border_tl:Draw(-16, -16)
	textures.border_t:DrawTiled(0, -16, 320, 16)
	textures.border_tr:Draw(320, -16)
	textures.border_r:DrawTiled(320, 0, 16, 640)
	textures.border_br:Draw(320, 640)
	textures.border_b:DrawTiled(0, 640, 320, 16)
	textures.border_bl:Draw(-16, 640)
	textures.border_l:DrawTiled(-16, 0, 16, 640)
	gl.Color3(1, 1, 1)
	
	if state.game_over and state.result then
		local msg = state.result
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
	
	self:DrawScore()
	
	self.game:PostRender()
	
	if self.ready > 0 then
		textures.ready:Draw(self.readyx, self.readyy - 0.1 * self.ready^2)
	elseif self.go > 0 then 
		textures.go:Draw(self.gox, self.goy - 0.1 * self.go^2)
	end
	gl.PopMatrix()
end

function SPGame:DrawTgmBg()
	self.bg[math.floor(state.level / 100) + 1]:Draw(0, self.bgy, state.width, self.bgh)
end

function SPGame:DrawSprintBg()
	self.bg:Draw(0, self.bgy, state.width, self.bgh)
end

function SPGame:DrawScore()
	local height = fonts.large.height*1.5
	
	local t = state.time
	text = string.format('%02d:%02d:%02d',
		math.floor(t / 60000) % 60,
		math.floor(t / 1000) % 60,
		math.floor(t / 10) % 100)
	fonts.large:Draw(text, (320 - self.timew)/2, 32*21)
	
	if state.score then
		fonts.large:Draw(string.format('%d', state.score), 32*11, 0)
		textures.score:Draw(32*11, -14)
	end
	if state.level then
		fonts.large:Draw(string.format('%d', state.level), 32*11, 32*21-height*2)
		textures.level:Draw(32*11, 32*21-height*2-14)
	end
	if state.goal then
		gl.Color3(1,0,0)
		fonts.large:Draw(string.format('%d', state.goal), 32*11, 32*21-height)
		textures.goal:Draw(32*11, 32*21-height-14)
		gl.Color3(1,1,1)
	end
	
	local y = fonts.large.height * 1.2
	local x = 32*11
	for i = 0, 9 do
		local t = state["st"..i]
		if t then
			fonts.medium:Draw(t, x, y)
			y = y + fonts.medium * 1.2
		end
	end
	
	if state.rank then
		local scale = 1 + self.rankup / 5
		local texture = textures["rank"..state.rank]
		local width = texture.width/2
		local height = texture.height/2
		gl.PushMatrix()
		gl.Translate(-32-width, height, 0)
		gl.Scale(scale, scale, 1)
		textures["rank"..state.rank]:Draw(-width, -height)
		gl.PopMatrix()
	end
end

function SPGame:OnKeyDown(key)
	if key == profile.key_menu or key == KEY_ESCAPE then
		PauseMenu:Hook()
	elseif self.game.OnKeyDown then
		local gk = self.gamekeys[key]
		if gk then self.game:OnKeyDown(gk) end
	end
end

function SPGame:OnKeyUp(key)
	if self.game.OnKeyUp then
		local gk = self.gamekeys[key]
		if gk then self.game:OnKeyUp(gk) end
	end
end

function SPGame:Serialize()
	if self.mode == "mroll" then
		return string.format('%s_%02d_%02d_%02d',
		self.mode,
		math.floor(state.time / 60000) % 60,
		math.floor(state.time / 1000) % 60,
		math.floor(state.time / 10) % 100)
	else
		return string.format('%s_lvl%03d_%02d_%02d_%02d',
			self.mode,
			state.level,
			math.floor(state.time / 60000) % 60,
			math.floor(state.time / 1000) % 60,
			math.floor(state.time / 10) % 100)
	end
end

function SPGame:OnHook(game)
	self.nocursor = true
	self.game = game
	self.bc = {}
	self.scale = 0.8 * (state.height / 768)
	local mode = re_match("^\\w+", game.mode)[0]
	self.mode = mode
	self.bc.r, self.bc.g, self.bc.b = 1, 1, 1
	if mode == "easy" then
		self.bc.r, self.bc.g, self.bc.b = 0, 1, 1
	elseif mode == "radar" then
		self.bc.r, self.bc.g, self.bc.b = 0, 1, 0.11
	elseif mode == "death" then
		self.bc.r, self.bc.g, self.bc.b = 0.8, 0, 0
	elseif mode == "sprint" then
		self.bc.r, self.bc.g, self.bc.b = 0.3, 0.3, 1
	end
	if self.game.SaveReplay then
		self.SaveReplay =
			function(self)
				FileMenu:Hook(
					function(fn)
						self.game:SaveReplay(fn)
						Message:Hook("Replay saved!", "Replay saved as \""..fn.."\"", self)
					end, self:Serialize())
			end
	else self.SaveReplay = nil end
	self.timew = fonts.large*"00:00:00"
	self:Preload()
	self:SetUp()
end

function SPGame:Preload()
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
	if self.mode == "sprint" then
		self.DrawBg = self.DrawSprintBg
		self.bg = textures.bgspeed
		self.bgh = (self.bg.height / self.bg.width) * state.width
	elseif self.mode == "mroll" then
		self.DrawBg = self.DrawSprintBg
		self.bg = textures.bgai
		self.bgh = (self.bg.height / self.bg.width) * state.width
	else
		self.DrawBg = self.DrawTgmBg
		if self.mode ~= "easy" then self.preview = textures.preview end
		self.bg = {
			textures.bg0,
			textures.bg1,
			textures.bg2,
			textures.bg3,
			textures.bg4,
			textures.bg5,
			textures.bg6,
			textures.bg7,
			textures.bg8,
			textures.bg9}
		self.bgh = (self.bg[1].height / self.bg[1].width) * state.width
	end
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
end

function SPGame:SetUp()
	self.rank = state.rank
	self.goal = state.goal
	state.result = nil
	self.rankup = 0
	state.st0 = nil
	state.st1 = nil
	state.st2 = nil
	state.st3 = nil
	state.st4 = nil
	state.st5 = nil
	state.st6 = nil
	state.st7 = nil
	state.st8 = nil
	state.st9 = nil
	state.time = 0
	state.scanline = -1
	self.sl = 0
	self.roll = nil
	self.credits = nil
	self.gotime = 0
	self.flash = 0
	self.ready, self.go = 40, 40
	sounds.ready:Play()
end
