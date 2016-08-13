-- Default Settings

textures = {}
setmetatable(textures, textures)
textures.__index = function(table, key)
	return system:LoadTexture("data/"..key..".png")
end

sounds = {}
setmetatable(sounds, sounds)
sounds.__index = function(table, key)
	return system:LoadSound("data/"..key..".wav")
end

fonts = {}
setmetatable(fonts, fonts)
fonts.__index = function(table, key)
	return system:LoadFont("data/font_"..key..".png")
end

state = {mouse = {x=-50, y=-50, buttons={}}}

settings = system:LoadSettings("config.ini")
if not settings then
	settings = system:LoadSettings("data/config.ini")
end
system:WriteToLog("profile: "..settings.profile)
profile = system:LoadSettings("profiles/"..settings.profile..".ini")
if not profile then
	profile = system:LoadSettings("data/default.ini")
end
system:WriteToLog("profile loaded")
system:WriteToLog("window width: "..profile.width)
system:WriteToLog("window height: "..profile.height)
system:WriteToLog("fullscreen mode: "..profile.fullscreen)

---- test writing back settings
--system:SaveSettings(settings.profile, profile);

state.width = profile.width
state.height = profile.height
