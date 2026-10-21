--[[
    [VirtualVersionHelper]
    [by @mdolli]
    
    Some fidelity stuff... handles hotswapping Player animations, sounds, whatever
]]

local GameSettings = UserSettings().GameSettings
local CurrentVirtualVersion = GameSettings.VirtualVersion

function CleanUp(Character)
    -- Remove current animation and sound scripts
    local oldAnimate = Character:FindFirstChild("Animate")
    if oldAnimate then oldAnimate.Disabled = true oldAnimate:Remove() end

    local oldSound = Character:FindFirstChild("Sound")
    if oldSound then oldSound.Disabled = true oldSound:Remove() end
end

function ApplyLegacyScripts(Character)
    -- InsertService can't insert local ContentIds.
    -- This is the way to go, as insecure as it could potentially be
    -- @mdolli
    
    game:Load("rbxasset://fonts/humanoidAnimateLocal.rbxm")

    local newAnimate = game:WaitForChild("Animate")
    -- die
    if game:FindFirstChild("RobloxTeam") then game:FindFirstChild("RobloxTeam"):Remove() end

    newAnimate.Parent = Character

    -- reactivate, pretty please
    newAnimate.Disabled = true newAnimate.Disabled = false

    return
end

function ApplyModernScripts(Character)
    -- InsertService can't insert local ContentIds.
    -- This is the way to go, as insecure as it could potentially be
    -- @mdolli

    game:Load("rbxasset://fonts/humanoidAnimateLocalKeyFrame2.rbxm")
    local newAnimate = game:WaitForChild("Animate")

    newAnimate.Parent = game.Players.LocalPlayer.Character

    newAnimate.Disabled = true newAnimate.Disabled = false
    
    return
end

function Listener(property)
    if not property == "VirtualVersion" then return end

    local currentVersion = tostring(CurrentVirtualVersion):gsub("Enum.VirtualVersion.", "")
	local newVersion = tostring(GameSettings.VirtualVersion):gsub("Enum.VirtualVersion.", "")


    -- Fetch the player
    local Player = game.Players.LocalPlayer
    if not Player then return end

    -- Fetch the character
    local Character = Player.Character or Player.CharacterAdded:wait()
    if not Character then return end

    --CleanUp(Character)

    if newVersion == "2012" or newVersion == "2013" then
        ApplyLegacyScripts(Character)
    else
        ApplyModernScripts(Character)
    end
end

GameSettings.Changed:connect(Listener)