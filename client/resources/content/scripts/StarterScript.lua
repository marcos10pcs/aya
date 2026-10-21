local GameSettings = UserSettings().GameSettings
local CurrentVirtualVersion = GameSettings.VirtualVersion
local ScriptContext = game:GetService("ScriptContext")
local RobloxGui = game:GetService("CoreGui"):WaitForChild("RobloxGui")
local ContextActionService = game:GetService("ContextActionService")
local GuiService = game:GetService("GuiService")

local function clearDialogs()
	for _, obj in pairs(game:GetService("CoreGui"):GetChildren()) do
		if obj:IsA("BillboardGui") then
			print("Clearing old dialogs")
			obj:Destroy()
		end
	end
end

function fireNotification()
	Spawn(function()
		local function easeInOutQuad(t, b, c, d)
			t = t / (d / 2)
			if t < 1 then
				return c / 2 * t * t + b
			end
			t = t - 1
			return -c / 2 * (t * (t - 2) - 1) + b
		end
	
		local function fadeOut(blurEffect, duration)
			local startTime = tick()
			local connection
			connection = game:GetService("RunService").RenderStepped:Connect(function()
				local elapsedTime = tick() - startTime
				if elapsedTime < duration then
					blurEffect.Size = easeInOutQuad(elapsedTime, 9, -9, duration)
				else
					blurEffect.Size = 0
					connection:disconnect()
					blurEffect:Destroy()
				end
			end)
		end
	
		local blurEffect = workspace.CurrentCamera:FindFirstChild("VirtualVersionBlurEffect")
		if blurEffect then
			fadeOut(blurEffect, 0.3)
		end
	end)
end

local function createCameraControls(parent)
	-- @mdolli 
	-- old 2012 camera control buttons
	
	local cameraTiltDown = Instance.new("ImageButton")
	cameraTiltDown.Name = "CameraTiltDown"
	cameraTiltDown.Size = UDim2.new(0, 25, 0, 25)
	cameraTiltDown.Position = UDim2.new(1, -56, 1, -29)
	cameraTiltDown.Image = "rbxasset://textures/CameraTiltDown.png"
	cameraTiltDown.Parent = parent
	cameraTiltDown.BackgroundTransparency = 1
	cameraTiltDown:SetVerb("CameraTiltDown")
	cameraTiltDown.RobloxLocked = true

	local cameraTiltUp = Instance.new("ImageButton")
	cameraTiltUp.Name = "CameraTiltUp"
	cameraTiltUp.Size = UDim2.new(0, 25, 0, 25)
	cameraTiltUp.Position = UDim2.new(1, -56, 1, -55)
	cameraTiltUp.Image = "rbxasset://textures/CameraTiltUp.png"
	cameraTiltUp.Parent = parent
	cameraTiltUp.BackgroundTransparency = 1
	cameraTiltUp:SetVerb("CameraTiltUp")
	cameraTiltUp.RobloxLocked = true
	
	local cameraZoomIn = Instance.new("ImageButton")
	cameraZoomIn.Name = "CameraZoomIn"
	cameraZoomIn.Size = UDim2.new(0, 25, 0, 25)
	cameraZoomIn.Position = UDim2.new(1, -29, 1, -55)
	cameraZoomIn.Image = "rbxasset://textures/CameraZoomIn.png"
	cameraZoomIn.Parent = parent
	cameraZoomIn.BackgroundTransparency = 1
	cameraZoomIn:SetVerb("CameraZoomIn")
	cameraZoomIn.RobloxLocked = true
	
	local cameraZoomOut = Instance.new("ImageButton")
	cameraZoomOut.Name = "CameraZoomOut"
	cameraZoomOut.Size = UDim2.new(0, 25, 0, 25)
	cameraZoomOut.Position = UDim2.new(1, -29, 1, -29)
	cameraZoomOut.Image = "rbxasset://textures/CameraZoomOut.png"
	cameraZoomOut.Parent = parent
	cameraZoomOut.BackgroundTransparency = 1
	cameraZoomOut:SetVerb("CameraZoomOut")
	cameraZoomOut.RobloxLocked = true
	
	cameraTiltDown.MouseEnter:connect(function()
		cameraTiltDown.Image = "rbxasset://textures/CameraTiltDown_ovr.png"
	end)
	
	cameraTiltDown.MouseLeave:connect(function()
		cameraTiltDown.Image = "rbxasset://textures/CameraTiltDown.png"
	end)
	
	cameraTiltUp.MouseEnter:connect(function()
		cameraTiltUp.Image = "rbxasset://textures/CameraTiltUp_ovr.png"
	end)
	
	cameraTiltUp.MouseLeave:connect(function()
		cameraTiltUp.Image = "rbxasset://textures/CameraTiltUp.png"
	end)
	
	cameraZoomIn.MouseEnter:connect(function()
		cameraZoomIn.Image = "rbxasset://textures/CameraZoomIn_ovr.png"
	end)
	
	cameraZoomIn.MouseLeave:connect(function()
		cameraZoomIn.Image = "rbxasset://textures/CameraZoomIn.png"
	end)
	
	cameraZoomOut.MouseEnter:connect(function()
		cameraZoomOut.Image = "rbxasset://textures/CameraZoomOut_ovr.png"
	end)
	
	cameraZoomOut.MouseLeave:connect(function()
		cameraZoomOut.Image = "rbxasset://textures/CameraZoomOut.png"
	end)
	
end

local function clearCorescripts()
	for _, obj in pairs(RobloxGui:GetChildren()) do
		if obj.Name ~= "Modules" then
			obj:Destroy()
		end
	end
end

local function runStarterScript(newVersion)
	if newVersion == Enum.VirtualVersion["2016"] then
		--[[
		    shared_ptr<Frame> controls = Creatable<Instance>::create<Frame>();
			controls->setName("ControlFrame");
			controls->setBackgroundTransparency(1);
			controls->setSize(UDim2(1, 0, 1, 0));
			controls->setRobloxLocked(true);

			shared_ptr<Frame> bottomLeftControl = Creatable<Instance>::create<Frame>();
			bottomLeftControl->setSize(UDim2(0, 130, 0, 46));
			bottomLeftControl->setPosition(UDim2(0, 0, 1, -46));
			bottomLeftControl->setBackgroundTransparency(1);
			bottomLeftControl->setName("BottomLeftControl");
			bottomLeftControl->setRobloxLocked(true);

			shared_ptr<Frame> bottomRightControl = Creatable<Instance>::create<Frame>();
			bottomRightControl->setSize(UDim2(0, 180, 0, 41));
			bottomRightControl->setPosition(UDim2(1, -180, 1, -41));
			bottomRightControl->setBackgroundTransparency(1);
			bottomRightControl->setName("BottomRightControl");
			bottomRightControl->setRobloxLocked(true);

			shared_ptr<Frame> topLeftControl = Creatable<Instance>::create<Frame>();
			topLeftControl->setSize(UDim2(0.05f, 0, 0.05f, 0));
			topLeftControl->setBackgroundTransparency(1);
			topLeftControl->setName("TopLeftControl");
			Instance::propRobloxLocked.set(topLeftControl.get(), true);
		--]]
	
		GuiService:SetGlobalGuiInset(0, 36, 0, 0)

		if not RobloxGui:FindFirstChild("ControlFrame") then
			clearCorescripts()
			local controlFrame = Instance.new("Frame")
			controlFrame.Name = "ControlFrame"
			controlFrame.Size = UDim2.new(1, 0, 1, 0)
			controlFrame.BackgroundTransparency = 1
			controlFrame.RobloxLocked = true
			controlFrame.Parent = RobloxGui

			local bottomLeftControl = Instance.new("Frame")
			bottomLeftControl.Name = "BottomLeftControl"
			bottomLeftControl.Size = UDim2.new(0, 130, 0, 46)
			bottomLeftControl.Position = UDim2.new(0, 0, 1, -46)
			bottomLeftControl.BackgroundTransparency = 1
			bottomLeftControl.RobloxLocked = true
			bottomLeftControl.Parent = controlFrame

			local bottomRightControl = Instance.new("Frame")
			bottomRightControl.Name = "BottomRightControl"
			bottomRightControl.Size = UDim2.new(0, 180, 0, 41)
			bottomRightControl.Position = UDim2.new(1, -180, 1, -41)
			bottomRightControl.BackgroundTransparency = 1
			bottomRightControl.RobloxLocked = true
			bottomRightControl.Parent = controlFrame

			local topLeftControl = Instance.new("Frame")
			topLeftControl.Name = "TopLeftControl"
			topLeftControl.Size = UDim2.new(0.05, 0, 0.05, 0)
			topLeftControl.BackgroundTransparency = 1
			topLeftControl.RobloxLocked = true
			topLeftControl.Parent = controlFrame
		end

		local soundFolder = Instance.new("Folder")
		soundFolder.Name = "Sounds"
		soundFolder.Parent = RobloxGui
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/Topbar", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/MainBotChatScript2", RobloxGui)

		if not RobloxGui:FindFirstChild("CoreScripts/TwentySixteen/DeveloperConsole") then
			ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/DeveloperConsole", RobloxGui)
		end

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/NotificationScript2", RobloxGui)

		require(RobloxGui.Modules.TwentySixteen.Chat)
		require(RobloxGui.Modules.TwentySixteen.PlayerlistModule)

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/BubbleChat", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/PurchasePromptScript2", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/HealthScript", RobloxGui)

		require(RobloxGui.Modules.TwentySixteen.BackpackScript)

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/VehicleHud", RobloxGui)

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentySixteen/GamepadMenu", RobloxGui)
	elseif GameSettings.VirtualVersion == Enum.VirtualVersion["2015"] then
		local controlFrame = Instance.new("Frame")
		controlFrame.Name = "ControlFrame"
		controlFrame.Size = UDim2.new(1, 0, 1, 0)
		controlFrame.BackgroundTransparency = 1
		controlFrame.Parent = RobloxGui

		local topLeftControl = Instance.new("Frame")
		topLeftControl.Name = "TopLeftControl"
		topLeftControl.Size = UDim2.new(0, 100, 0, 50)
		topLeftControl.Position = UDim2.new(0, 0, 0, 0)
		topLeftControl.BackgroundColor3 = Color3.new(1, 0, 0)
		topLeftControl.BackgroundTransparency = 1 
		topLeftControl.Parent = controlFrame
		
		local bottomLeftControl = Instance.new("Frame")
		bottomLeftControl.Name = "BottomLeftControl"
		bottomLeftControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomLeftControl.Position = UDim2.new(0, 0, 1, -50) 
		bottomLeftControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomLeftControl.BackgroundTransparency = 1 
		bottomLeftControl.Parent = controlFrame
		
		local bottomRightControl = Instance.new("Frame")
		bottomRightControl.Name = "BottomRightControl"
		bottomRightControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomRightControl.Position = UDim2.new(0, 0, 1, -50) 
		bottomRightControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomRightControl.BackgroundTransparency = 1 
		bottomRightControl.Parent = controlFrame
		
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/Settings", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/ToolTip", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/BubbleChat", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/MainBotChatScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/NotificationScript2", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/ChatScript2", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/PurchasePromptScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/HealthScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/PlayerListScript2", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/BackpackScript", RobloxGui)
	elseif GameSettings.VirtualVersion == Enum.VirtualVersion["2014"] then
		local controlFrame = Instance.new("Frame")
		controlFrame.Name = "ControlFrame"
		controlFrame.Size = UDim2.new(1, 0, 1, 0)
		controlFrame.BackgroundTransparency = 1
		controlFrame.Parent = RobloxGui

		local topLeftControl = Instance.new("Frame")
		topLeftControl.Name = "TopLeftControl"
		topLeftControl.Size = UDim2.new(0, 100, 0, 50)
		topLeftControl.Position = UDim2.new(0, 0, 0, 0)
		topLeftControl.BackgroundColor3 = Color3.new(1, 0, 0)
		topLeftControl.BackgroundTransparency = 1 
		topLeftControl.Parent = controlFrame
		
		local bottomLeftControl = Instance.new("Frame")
		bottomLeftControl.Name = "BottomLeftControl"
		bottomLeftControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomLeftControl.Position = UDim2.new(0, 0, 1, -50) 
		bottomLeftControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomLeftControl.BackgroundTransparency = 1 
		bottomLeftControl.Parent = controlFrame
	
		local bottomRightControl = Instance.new("Frame")
		bottomRightControl.Name = "BottomRightControl"
		bottomRightControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomRightControl.Position = UDim2.new(0, 0, 1, -50) 
		bottomRightControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomRightControl.BackgroundTransparency = 1 
		bottomRightControl.Parent = controlFrame

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/BackpackBuilder", RobloxGui)

		RobloxGui:WaitForChild("CurrentLoadout")
		RobloxGui:WaitForChild("Backpack")
		local Backpack = RobloxGui.Backpack

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/BackpackManager", Backpack)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/BackpackGear", Backpack)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/LoadoutScript", RobloxGui.CurrentLoadout)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/BubbleChat", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFifteen/Settings", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/ToolTip", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/MainBotChatScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/DeveloperConsole", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/PopupScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/NotificationScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/ChatScript", RobloxGui)	
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/PurchasePromptScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/HealthScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/PlayerlistScript", RobloxGui)
	elseif GameSettings.VirtualVersion == Enum.VirtualVersion["2013"] then
		local controlFrame = Instance.new("Frame")
		controlFrame.Name = "ControlFrame"
		controlFrame.Size = UDim2.new(1, 0, 1, 0)
		controlFrame.BackgroundTransparency = 1
		controlFrame.Parent = RobloxGui

		local topLeftControl = Instance.new("Frame")
		topLeftControl.Name = "TopLeftControl"
		topLeftControl.Size = UDim2.new(0, 100, 0, 50)
		topLeftControl.Position = UDim2.new(0, 0, 0, 0)
		topLeftControl.BackgroundColor3 = Color3.new(1, 0, 0)
		topLeftControl.BackgroundTransparency = 1 
		topLeftControl.Parent = controlFrame
		
		local bottomLeftControl = Instance.new("Frame")
		bottomLeftControl.Name = "BottomLeftControl"
		bottomLeftControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomLeftControl.Position = UDim2.new(0, 0, 1, -50) 
		bottomLeftControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomLeftControl.BackgroundTransparency = 1 
		bottomLeftControl.Parent = controlFrame
	
		local bottomRightControl = Instance.new("Frame")
		bottomRightControl.Name = "BottomRightControl"
		bottomRightControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomRightControl.Position = UDim2.new(1, -100, 1, -50) 
		bottomRightControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomRightControl.BackgroundTransparency = 1 
		bottomRightControl.Parent = controlFrame

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/HealthScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/ToolTip", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/MainBotChatScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/Settings", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/PopupScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/NotificationScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/ChatScript", RobloxGui)	
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/PurchasePromptScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/PlayerListScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/BubbleChat", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/BackpackBuilder", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/BackpackManager", RobloxGui:WaitForChild("Backpack"))
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/LoadoutScript", RobloxGui:WaitForChild("CurrentLoadout"))
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/BackpackResizer", RobloxGui:WaitForChild("Backpack"))
	elseif GameSettings.VirtualVersion == Enum.VirtualVersion["2012"] then
		local controlFrame = Instance.new("Frame")
		controlFrame.Name = "ControlFrame"
		controlFrame.Size = UDim2.new(1, 0, 1, 0)
		controlFrame.BackgroundTransparency = 1
		controlFrame.Parent = RobloxGui

		local topLeftControl = Instance.new("Frame")
		topLeftControl.Name = "TopLeftControl"
		topLeftControl.Size = UDim2.new(0, 100, 0, 50)
		topLeftControl.Position = UDim2.new(0, 0, 0, 0)
		topLeftControl.BackgroundColor3 = Color3.new(1, 0, 0)
		topLeftControl.BackgroundTransparency = 1 
		topLeftControl.Parent = controlFrame
		
		local bottomLeftControl = Instance.new("Frame")
		bottomLeftControl.Name = "BottomLeftControl"
		bottomLeftControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomLeftControl.Position = UDim2.new(0, 0, 1, -50) 
		bottomLeftControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomLeftControl.BackgroundTransparency = 1 
		bottomLeftControl.Parent = controlFrame
	
		local bottomRightControl = Instance.new("Frame")
		bottomRightControl.Name = "BottomRightControl"
		bottomRightControl.Size = UDim2.new(0, 100, 0, 50) 
		bottomRightControl.Position = UDim2.new(1, -100, 1, -50) 
		bottomRightControl.BackgroundColor3 = Color3.new(0, 1, 0)
		bottomRightControl.BackgroundTransparency = 1 
		bottomRightControl.Parent = controlFrame

		local cameraTiltDown = Instance.new("ImageButton")
		cameraTiltDown.Name = "CameraTiltDown"
		cameraTiltDown.Size = UDim2.new(0, 25, 0, 25)
		cameraTiltDown.Position = UDim2.new(1, -56, 1, -29)
		cameraTiltDown.Image = "rbxasset://textures/CameraTiltDown.png"
		cameraTiltDown.Parent = bottomRightControl
		cameraTiltDown.BackgroundTransparency = 1
		cameraTiltDown:SetVerb("CameraTiltDown")
		cameraTiltDown.RobloxLocked = true
	
		local cameraTiltUp = Instance.new("ImageButton")
		cameraTiltUp.Name = "CameraTiltUp"
		cameraTiltUp.Size = UDim2.new(0, 25, 0, 25)
		cameraTiltUp.Position = UDim2.new(1, -56, 1, -55)
		cameraTiltUp.Image = "rbxasset://textures/CameraTiltUp.png"
		cameraTiltUp.Parent = bottomRightControl
		cameraTiltUp.BackgroundTransparency = 1
		cameraTiltUp:SetVerb("CameraTiltUp")
		cameraTiltUp.RobloxLocked = true
		
		local cameraZoomIn = Instance.new("ImageButton")
		cameraZoomIn.Name = "CameraZoomIn"
		cameraZoomIn.Size = UDim2.new(0, 25, 0, 25)
		cameraZoomIn.Position = UDim2.new(1, -29, 1, -55)
		cameraZoomIn.Image = "rbxasset://textures/CameraZoomIn.png"
		cameraZoomIn.Parent = bottomRightControl
		cameraZoomIn.BackgroundTransparency = 1
		cameraZoomIn:SetVerb("CameraZoomIn")
		cameraZoomIn.RobloxLocked = true
		
		local cameraZoomOut = Instance.new("ImageButton")
		cameraZoomOut.Name = "CameraZoomOut"
		cameraZoomOut.Size = UDim2.new(0, 25, 0, 25)
		cameraZoomOut.Position = UDim2.new(1, -29, 1, -29)
		cameraZoomOut.Image = "rbxasset://textures/CameraZoomOut.png"
		cameraZoomOut.Parent = bottomRightControl
		cameraZoomOut.BackgroundTransparency = 1
		cameraZoomOut:SetVerb("CameraZoomOut")
		cameraZoomOut.RobloxLocked = true
		
		cameraTiltDown.MouseEnter:connect(function()
			cameraTiltDown.Image = "rbxasset://textures/CameraTiltDown_ovr.png"
		end)
		
		cameraTiltDown.MouseLeave:connect(function()
			cameraTiltDown.Image = "rbxasset://textures/CameraTiltDown.png"
		end)
		
		cameraTiltUp.MouseEnter:connect(function()
			cameraTiltUp.Image = "rbxasset://textures/CameraTiltUp_ovr.png"
		end)
		
		cameraTiltUp.MouseLeave:connect(function()
			cameraTiltUp.Image = "rbxasset://textures/CameraTiltUp.png"
		end)
		
		cameraZoomIn.MouseEnter:connect(function()
			cameraZoomIn.Image = "rbxasset://textures/CameraZoomIn_ovr.png"
		end)
		
		cameraZoomIn.MouseLeave:connect(function()
			cameraZoomIn.Image = "rbxasset://textures/CameraZoomIn.png"
		end)
		
		cameraZoomOut.MouseEnter:connect(function()
			cameraZoomOut.Image = "rbxasset://textures/CameraZoomOut_ovr.png"
		end)
		
		cameraZoomOut.MouseLeave:connect(function()
			cameraZoomOut.Image = "rbxasset://textures/CameraZoomOut.png"
		end)

		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/HealthScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/ToolTip", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyFourteen/MainBotChatScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyThirteen/Settings", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/PopupScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/NotificationScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/ChatScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/PurchasePromptScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/PlayerListScript", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/BubbleChat", RobloxGui)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/BackpackBuilder", RobloxGui)

		local Backpack = RobloxGui:WaitForChild("Backpack")
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/BackpackManager", Backpack)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/BackpackGear", Backpack)
		ScriptContext:AddCoreScriptLocal("CoreScripts/TwentyTwelve/LoadoutScript", RobloxGui:WaitForChild("CurrentLoadout"))

		GuiService:SetGlobalGuiInset(0, 0, 0, 0)
	end

	CurrentVirtualVersion = newVersion

	-- ScriptContext:AddCoreScriptLocal("VirtualVersionHelper", RobloxGui)
end

runStarterScript(CurrentVirtualVersion)

GameSettings.Changed:connect(function(property)
	if property ~= "VirtualVersion" then return end
	if GameSettings.VirtualVersion == CurrentVirtualVersion then return end

	local currentVersion = tostring(CurrentVirtualVersion):gsub("Enum.VirtualVersion.", "")
	local newVersion = tostring(GameSettings.VirtualVersion):gsub("Enum.VirtualVersion.", "")

	print("-- Switching VirtualVersion from " .. currentVersion .. " to " .. newVersion .. " --")

	if CurrentVirtualVersion == Enum.VirtualVersion["2016"] and GameSettings.VirtualVersion ~= Enum.VirtualVersion["2016"] then
		if RobloxGui:FindFirstChild("Modules") then
			for _, module in pairs(RobloxGui.Modules:GetChildren()) do
				module:Destroy()
			end
		end
	end

	clearCorescripts()
	clearDialogs()

	if CurrentVirtualVersion == Enum.VirtualVersion["2016"] then
		ContextActionService:UnbindCoreAction("RbxSettingsHubSwitchTab")
		ContextActionService:UnbindCoreAction("RbxSettingsHubStopCharacter")
		ContextActionService:UnbindCoreAction("RbxSettingsScrollHotkey")
		ContextActionService:UnbindCoreAction("RBXEscapeMainMenu")
	end

	GuiService:SetMenuIsOpen(false)
	GuiService.SelectedCoreObject = nil

	runStarterScript(GameSettings.VirtualVersion)
	fireNotification(GameSettings.VirtualVersion)
end)