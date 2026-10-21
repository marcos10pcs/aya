


#include "DataModel/factoryregistration.hpp"

#include "FastLog.hpp"

#include "DataModel/Adornment.hpp"
#include "DataModel/BillboardGui.hpp"
#include "DataModel/SurfaceGui.hpp"
#include "DataModel/Accoutrement.hpp"
#include "DataModel/Backpack.hpp"
#include "DataModel/BadgeService.hpp"
#include "DataModel/BasicPartInstance.hpp"
#include "DataModel/MeshPartInstance.hpp"
#include "DataModel/BevelMesh.hpp"
#include "DataModel/BlockMesh.hpp"
#include "DataModel/CacheableContentProvider.hpp"
#include "DataModel/Camera.hpp"
#include "DataModel/ChangeHistory.hpp"
#include "DataModel/CharacterAppearance.hpp"
#include "DataModel/CharacterMesh.hpp"
#include "DataModel/ChatService.hpp"
#include "DataModel/ClickDetector.hpp"
#include "DataModel/ContentProvider.hpp"
#include "DataModel/Configuration.hpp"
#include "DataModel/CollectionService.hpp"
#include "DataModel/CSGDictionaryService.hpp"
#include "DataModel/CustomEvent.hpp"
#include "DataModel/CustomEventReceiver.hpp"
#include "DataModel/CylinderMesh.hpp"
#include "DataModel/VirtualUser.hpp"
#include "DataModel/LogService.hpp"
#include "DataModel/DataModelMesh.hpp"
#include "DataModel/DebrisService.hpp"
#include "DataModel/Decal.hpp"
#include "DataModel/DialogRoot.hpp"
#include "DataModel/DialogChoice.hpp"
#include "DataModel/DebugSettings.hpp"
#include "DataModel/PhysicsSettings.hpp"
#include "DataModel/ExtrudedPartInstance.hpp"
#include "DataModel/FriendService.hpp"
#include "DataModel/Folder.hpp"
#include "DataModel/RenderHooksService.hpp"
#include "DataModel/Test.hpp"
#include "DataModel/CookiesEngineService.hpp"
#include "DataModel/TeleportService.hpp"
#include "DataModel/PersonalServerService.hpp"
#include "DataModel/UserInputService.hpp"
#include "DataModel/AssetService.hpp"
#include "DataModel/HttpService.hpp"
#include "DataModel/HttpRbxApiService.hpp"
#include "DataModel/DataStoreService.hpp"
#include "DataModel/TerrainRegion.hpp"
#include "DataModel/PathfindingService.hpp"
#include "DataModel/StarterPlayerService.hpp"
#include "DataModel/HandleAdornment.hpp"

#include "DataModel/PrismInstance.hpp"
#include "DataModel/PyramidInstance.hpp"
#include "DataModel/ParallelRampInstance.hpp"
#include "DataModel/RightAngleRampInstance.hpp"
#include "DataModel/CornerWedgeInstance.hpp"

#include "DataModel/Explosion.hpp"
#include "DataModel/FaceInstance.hpp"
#include "DataModel/Feature.hpp"
#include "DataModel/FileMesh.hpp"
#include "DataModel/Fire.hpp"
#include "DataModel/Flag.hpp"
#include "DataModel/FlagStand.hpp"
#include "DataModel/FlyweightService.hpp"
#include "DataModel/ForceField.hpp"
#include "DataModel/GameSettings.hpp"
#include "DataModel/GameBasicSettings.hpp"
#include "Script/LuaSettings.hpp"
#include "Script/DebuggerManager.hpp"
#include "Script/ModuleScript.hpp"
#include "Script/LuaSourceContainer.hpp"
#include "DataModel/GeometryService.hpp"
#include "DataModel/GlobalSettings.hpp"
#include "DataModel/Gyro.hpp"
#include "DataModel/Handles.hpp"
#include "DataModel/HandlesBase.hpp"
#include "DataModel/ArcHandles.hpp"
#include "DataModel/Hopper.hpp"
#include "DataModel/JointInstance.hpp"
#include "DataModel/JointsService.hpp"
#include "DataModel/Lighting.hpp"
#include "DataModel/MeshContentProvider.hpp"
#include "DataModel/Message.hpp"
#include "DataModel/Hint.hpp"
#include "DataModel/Mouse.hpp"
#include "DataModel/NonReplicatedCSGDictionaryService.hpp"
#include "DataModel/ParametricPartInstance.hpp"
#include "DataModel/PartInstance.hpp"
#include "DataModel/PhysicsService.hpp"
#include "DataModel/Platform.hpp"
#include "DataModel/PlayerGui.hpp"
#include "DataModel/PlayerScripts.hpp"
#include "DataModel/PlayerMouse.hpp"
#include "DataModel/PluginManager.hpp"
#include "DataModel/PluginMouse.hpp"
#include "DataModel/Seat.hpp"
#include "DataModel/SelectionBox.hpp"
#include "DataModel/SelectionSphere.hpp"
#include "DataModel/Sky.hpp"
#include "DataModel/SkateboardPlatform.hpp"
#include "DataModel/SkateboardController.hpp"
#include "DataModel/Smoke.hpp"
#include "DataModel/CustomParticleEmitter.hpp"
#include "DataModel/SolidModelContentProvider.hpp"
#include "DataModel/Sparkles.hpp"
#include "DataModel/SpawnLocation.hpp"
#include "DataModel/SpecialMesh.hpp"
#include "DataModel/Stats.hpp"
#include "DataModel/SurfaceSelection.hpp"
#include "DataModel/Teams.hpp"
#include "DataModel/TextService.hpp"
#include "DataModel/TextureContentProvider.hpp"
#include "DataModel/TimerService.hpp"
#include "DataModel/Tool.hpp"
#include "DataModel/StudioTool.hpp"
#include "DataModel/TouchTransmitter.hpp"
#include "DataModel/UserController.hpp"
#include "DataModel/Value.hpp"
#include "DataModel/VehicleSeat.hpp"
#include "DataModel/Visit.hpp"
#include "DataModel/Workspace.hpp"
#include "DataModel/LocalWorkspace.hpp"
#include "Humanoid/Humanoid.hpp"
#include "Humanoid/StatusInstance.hpp"
#include "Humanoid/HumanoidState.hpp"
#include "Script/ScriptContext.hpp"
#include "Script/script.hpp"
#include "Script/CoreScript.hpp"
#include "DataModel/MarketplaceService.hpp"
#include "DataModel/GuiService.hpp"
#include "DataModel/GuiBase.hpp"
#include "DataModel/GuiBase3d.hpp"
#include "DataModel/TweenService.hpp"
#include "DataModel/GuiObject.hpp"
#include "DataModel/ScreenGui.hpp"
#include "DataModel/Frame.hpp"
#include "DataModel/Scale9Frame.hpp"
#include "DataModel/ImageButton.hpp"
#include "DataModel/ImageLabel.hpp"
#include "DataModel/TextButton.hpp"
#include "DataModel/TextLabel.hpp"
#include "DataModel/TextBox.hpp"
#include "DataModel/ChromiumFrame.hpp"
#include "DataModel/VideoFrame.hpp"
#include "DataModel/SelectionLasso.hpp"
#include "DataModel/TextureTrail.hpp"
#include "DataModel/FloorWire.hpp"
#include "DataModel/Animation.hpp"
#include "DataModel/AnimationController.hpp"
#include "DataModel/AnimationTrack.hpp"
#include "DataModel/AnimationTrackState.hpp"
#include "DataModel/Animator.hpp"
#include "DataModel/KeyframeSequenceProvider.hpp"
#include "DataModel/KeyframeSequence.hpp"
#include "DataModel/Keyframe.hpp"
#include "DataModel/Pose.hpp"
#include "DataModel/MegaCluster.hpp"
#include "DataModel/Bindable.hpp"
#include "DataModel/Light.hpp"
#include "DataModel/Remote.hpp"
#include "DataModel/PartOperation.hpp"
#include "DataModel/PartOperationAsset.hpp"
#include "DataModel/Attachment.hpp"
#include "DataModel/TouchInputService.hpp"
#include "DataModel/HapticService.hpp"
#include "DataModel/PostEffect.hpp"
#include "DataModel/BlurEffect.hpp"
#include "DataModel/ColorCorrectionEffect.hpp"
#include "DataModel/BloomEffect.hpp"
#include "Network/NetworkPacketCache.hpp"
#include "Network/NetworkClusterPacketCache.hpp"
#include "Network/ChatFilter.hpp"

#include "Utility/Sound.hpp"
#include "Utility/SoundService.hpp"
#include "Utility/UDim.hpp"
#include "Utility/Faces.hpp"
#include "Utility/Axes.hpp"
#include "Utility/ScriptInformationProvider.hpp"
#include "Utility/Action.hpp"
#include "Utility/Region3.hpp"
#include "Utility/KeywordFilter.hpp"
#include "Utility/SystemAddress.hpp"
#include "Utility/LuaWebService.hpp"
#include "Utility/rbxrandom.hpp"
#include "Utility/RunStateOwner.hpp"
#include "Utility/PhysicalProperties.hpp"
#include "DataModel/InsertService.hpp"
#include "DataModel/SocialService.hpp"
#include "DataModel/GamePassService.hpp"
#include "DataModel/ContextActionService.hpp"
#include "DataModel/LoginService.hpp"
#include "Utility/ContentFilter.hpp"
#include "Tool/LuaDragger.hpp"
#include "Tool/AdvLuaDragger.hpp"
#include "Graphics3D/aya/RbxTime.hpp"
#include "DataModel/InputObject.hpp"
#include "DataModel/ReplicatedStorage.hpp"
#include "DataModel/RobloxReplicatedStorage.hpp"
#include "DataModel/ReplicatedFirst.hpp"
#include "DataModel/ServerScriptService.hpp"
#include "DataModel/ServerStorage.hpp"
#include "Utility/standardout.hpp"
#include "Utility/KeyCode.hpp"
#include "DataModel/PointsService.hpp"
#include "DataModel/ScrollingFrame.hpp"
#include "DataModel/AdService.hpp"
#include "DataModel/NotificationService.hpp"
#include "DataModel/GroupService.hpp"
#include "DataModel/GamepadService.hpp"

#include "DataModel/NumberSequence.hpp"
#include "DataModel/NumberRange.hpp"
#include "DataModel/ColorSequence.hpp"

#include "Utility/TweenInfo.hpp"
#include "Utility/VoiceChatOutput.hpp"

#include "Utility/AyaService.hpp"

using namespace Aya;

RBX_REGISTER_TYPE(void);
RBX_REGISTER_TYPE(bool);
RBX_REGISTER_TYPE(float);
RBX_REGISTER_TYPE(int);
RBX_REGISTER_TYPE(long);
RBX_REGISTER_TYPE(double);
RBX_REGISTER_TYPE(std::string);
RBX_REGISTER_TYPE(Aya::ProtectedString);
RBX_REGISTER_TYPE(Aya::TweenInfo);
RBX_REGISTER_TYPE(const Reflection::PropertyDescriptor*);
RBX_REGISTER_TYPE(Aya::BrickColor);
RBX_REGISTER_TYPE(Aya::SystemAddress);
RBX_REGISTER_TYPE(Aya::MeshId);
RBX_REGISTER_TYPE(Aya::AnimationId);
RBX_REGISTER_TYPE(boost::shared_ptr<const Reflection::Tuple>);
RBX_REGISTER_TYPE(G3D::Vector3);
RBX_REGISTER_TYPE(G3D::Vector3int16);
RBX_REGISTER_TYPE(Aya::Region3);
RBX_REGISTER_TYPE(Aya::RbxRay);
RBX_REGISTER_TYPE(G3D::Rect2D);
RBX_REGISTER_TYPE(Aya::PhysicalProperties);
RBX_REGISTER_TYPE(Aya::Vector2);
RBX_REGISTER_TYPE(G3D::Vector2int16);
RBX_REGISTER_TYPE(G3D::Color3);
RBX_REGISTER_TYPE(G3D::CoordinateFrame);
RBX_REGISTER_TYPE(Aya::ContentId);
RBX_REGISTER_TYPE(Aya::TextureId);
RBX_REGISTER_TYPE(Aya::UDim);
RBX_REGISTER_TYPE(Aya::UDim2);
RBX_REGISTER_TYPE(Aya::Faces);
RBX_REGISTER_TYPE(Aya::Axes);
RBX_REGISTER_TYPE(boost::shared_ptr<const Instances>);
RBX_REGISTER_TYPE(boost::shared_ptr<class Reflection::DescribedBase>);
RBX_REGISTER_TYPE(boost::shared_ptr<class Aya::Instance>);
RBX_REGISTER_TYPE(Lua::WeakFunctionRef);
RBX_REGISTER_TYPE(shared_ptr<Lua::GenericFunction>);
RBX_REGISTER_TYPE(shared_ptr<Lua::GenericAsyncFunction>);
RBX_REGISTER_TYPE(boost::shared_ptr<const Reflection::ValueArray>);
RBX_REGISTER_TYPE(boost::shared_ptr<const Reflection::ValueMap>);
RBX_REGISTER_TYPE(boost::shared_ptr<const Reflection::ValueTable>);
RBX_REGISTER_TYPE(Soundscape::SoundId);
RBX_REGISTER_TYPE(Aya::NumberSequenceKeypoint);
RBX_REGISTER_TYPE(Aya::ColorSequenceKeypoint);
RBX_REGISTER_TYPE(Aya::NumberSequence);
RBX_REGISTER_TYPE(Aya::ColorSequence);
RBX_REGISTER_TYPE(Aya::NumberRange);
RBX_REGISTER_TYPE(Aya::Guid::Data);

RBX_REGISTER_CLASS(InputObject);
RBX_REGISTER_CLASS(TestService);
RBX_REGISTER_CLASS(FunctionalTest);
RBX_REGISTER_CLASS(Lighting);
RBX_REGISTER_CLASS(DebugSettings);
RBX_REGISTER_CLASS(PhysicsSettings);
RBX_REGISTER_CLASS(TaskSchedulerSettings);
RBX_REGISTER_CLASS(DataModel);
RBX_REGISTER_CLASS(PhysicsService);
RBX_REGISTER_CLASS(BadgeService);
RBX_REGISTER_CLASS(DialogRoot);
RBX_REGISTER_CLASS(DialogChoice);
RBX_REGISTER_CLASS(Tool);
RBX_REGISTER_CLASS(StudioTool);
RBX_REGISTER_CLASS(LuaDragger);
RBX_REGISTER_CLASS(AdvLuaDragger);
RBX_REGISTER_CLASS(Accoutrement);
RBX_REGISTER_CLASS(Backpack);
RBX_REGISTER_CLASS(BodyColors);
RBX_REGISTER_CLASS(ClickDetector);
RBX_REGISTER_CLASS(ControllerService);
RBX_REGISTER_CLASS(ChatService);
RBX_REGISTER_CLASS(TextService);
RBX_REGISTER_CLASS(VirtualUser);
RBX_REGISTER_CLASS(Explosion);
RBX_REGISTER_CLASS(Team);
RBX_REGISTER_CLASS(Instance);
RBX_REGISTER_CLASS(Flag);
RBX_REGISTER_CLASS(FlagStand);
RBX_REGISTER_CLASS(FlagStandService);
RBX_REGISTER_CLASS(ForceField);
RBX_REGISTER_CLASS(Fire);
RBX_REGISTER_CLASS(GameSettings);
RBX_REGISTER_CLASS(GameBasicSettings);
RBX_REGISTER_CLASS(GeometryService);
RBX_REGISTER_CLASS(Settings);
RBX_REGISTER_CLASS(GlobalAdvancedSettings);
RBX_REGISTER_CLASS(GlobalBasicSettings);
RBX_REGISTER_CLASS(Hat);
RBX_REGISTER_CLASS(Accessory);
RBX_REGISTER_CLASS(Hint);
RBX_REGISTER_CLASS(Message);
RBX_REGISTER_CLASS(Humanoid);
RBX_REGISTER_CLASS(StatusInstance);
RBX_REGISTER_CLASS(RunService);
RBX_REGISTER_CLASS(LegacyHopperService);
RBX_REGISTER_CLASS(LocalScript);
RBX_REGISTER_CLASS(LocalWorkspace);
RBX_REGISTER_CLASS(LuaSettings);
RBX_REGISTER_CLASS(CoreScript);
RBX_REGISTER_CLASS(Selection);
RBX_REGISTER_CLASS(ShirtGraphic);
RBX_REGISTER_CLASS(Shirt);
RBX_REGISTER_CLASS(Pants);
RBX_REGISTER_CLASS(Smoke);
RBX_REGISTER_CLASS(CustomParticleEmitter);
RBX_REGISTER_CLASS(Sparkles);
RBX_REGISTER_CLASS(StarterPackService);
RBX_REGISTER_CLASS(StarterPlayerService);
RBX_REGISTER_CLASS(StarterGuiService);
RBX_REGISTER_CLASS(UserInputService);
RBX_REGISTER_CLASS(CoreGuiService);
RBX_REGISTER_CLASS(StarterGear);
RBX_REGISTER_CLASS(Visit);
RBX_REGISTER_CLASS(ObjectValue);
RBX_REGISTER_CLASS(IntValue);
RBX_REGISTER_CLASS(DoubleValue);
RBX_REGISTER_CLASS(BoolValue);
RBX_REGISTER_CLASS(StringValue);
RBX_REGISTER_CLASS(BinaryStringValue);
RBX_REGISTER_CLASS(Vector3Value);
RBX_REGISTER_CLASS(RayValue);
RBX_REGISTER_CLASS(CFrameValue);
RBX_REGISTER_CLASS(Color3Value);
RBX_REGISTER_CLASS(BrickColorValue);
RBX_REGISTER_CLASS(IntConstrainedValue);
RBX_REGISTER_CLASS(DoubleConstrainedValue);
RBX_REGISTER_CLASS(Aya::Platform);
RBX_REGISTER_CLASS(SkateboardPlatform);
RBX_REGISTER_CLASS(Seat);
RBX_REGISTER_CLASS(VehicleSeat);
RBX_REGISTER_CLASS(DebrisService);
RBX_REGISTER_CLASS(TimerService);
RBX_REGISTER_CLASS(SpawnerService);
RBX_REGISTER_CLASS(ContentFilter);
RBX_REGISTER_CLASS(InsertService);
RBX_REGISTER_CLASS(LuaWebService);
RBX_REGISTER_CLASS(FriendService);
RBX_REGISTER_CLASS(RenderHooksService);
RBX_REGISTER_CLASS(CookiesService);
RBX_REGISTER_CLASS(SocialService);
RBX_REGISTER_CLASS(GamePassService);
RBX_REGISTER_CLASS(MarketplaceService);
RBX_REGISTER_CLASS(GroupService);
RBX_REGISTER_CLASS(ContextActionService);
RBX_REGISTER_CLASS(PersonalServerService);
RBX_REGISTER_CLASS(AssetService);
RBX_REGISTER_CLASS(ContentProvider);
RBX_REGISTER_CLASS(MeshContentProvider);
RBX_REGISTER_CLASS(TextureContentProvider);
RBX_REGISTER_CLASS(SolidModelContentProvider);
RBX_REGISTER_CLASS(CacheableContentProvider);
RBX_REGISTER_CLASS(ChangeHistoryService);
RBX_REGISTER_CLASS(HttpService);
RBX_REGISTER_CLASS(HttpRbxApiService);
RBX_REGISTER_CLASS(DataStoreService);
RBX_REGISTER_CLASS(PathfindingService);
RBX_REGISTER_CLASS(Path);
RBX_REGISTER_CLASS(Clothing);
RBX_REGISTER_CLASS(Skin);
RBX_REGISTER_CLASS(CharacterMesh);
RBX_REGISTER_CLASS(DataModelMesh);
RBX_REGISTER_CLASS(FileMesh);
RBX_REGISTER_CLASS(SpecialShape);
RBX_REGISTER_CLASS(BevelMesh);
RBX_REGISTER_CLASS(BlockMesh);
RBX_REGISTER_CLASS(CylinderMesh);
// RBX_REGISTER_CLASS(EggMesh);
RBX_REGISTER_CLASS(ServiceProvider);
RBX_REGISTER_CLASS(RootInstance);
RBX_REGISTER_CLASS(ModelInstance);
RBX_REGISTER_CLASS(BaseScript);
RBX_REGISTER_CLASS(Script);
RBX_REGISTER_CLASS(ScriptContext);
RBX_REGISTER_CLASS(RuntimeScriptService);
RBX_REGISTER_CLASS(ScriptInformationProvider);
RBX_REGISTER_CLASS(Workspace);
RBX_REGISTER_CLASS(Controller);
RBX_REGISTER_CLASS(HumanoidController);
RBX_REGISTER_CLASS(VehicleController);
RBX_REGISTER_CLASS(SkateboardController);
RBX_REGISTER_CLASS(Pose);
RBX_REGISTER_CLASS(Keyframe);
RBX_REGISTER_CLASS(KeyframeSequence);
RBX_REGISTER_CLASS(KeyframeSequenceProvider);
RBX_REGISTER_CLASS(Animation);
RBX_REGISTER_CLASS(AnimationController);
RBX_REGISTER_CLASS(AnimationTrack);
RBX_REGISTER_CLASS(AnimationTrackState);
RBX_REGISTER_CLASS(Animator);
RBX_REGISTER_CLASS(TeleportService);
RBX_REGISTER_CLASS(CharacterAppearance);
RBX_REGISTER_CLASS(LogService);
RBX_REGISTER_CLASS(ScrollingFrame);
RBX_REGISTER_CLASS(FlyweightService);
RBX_REGISTER_CLASS(CSGDictionaryService);
RBX_REGISTER_CLASS(NonReplicatedCSGDictionaryService);
RBX_REGISTER_CLASS(TouchInputService);


// network
RBX_REGISTER_CLASS(Network::PhysicsPacketCache);
RBX_REGISTER_CLASS(Network::InstancePacketCache);
RBX_REGISTER_CLASS(Network::ClusterPacketCache);
RBX_REGISTER_CLASS(Network::OneQuarterClusterPacketCache);
RBX_REGISTER_CLASS(Network::ChatFilter);

// Joints - in alpha order
RBX_REGISTER_CLASS(JointsService);
RBX_REGISTER_CLASS(Glue);
RBX_REGISTER_CLASS(Motor);
RBX_REGISTER_CLASS(Motor6D);
RBX_REGISTER_CLASS(Rotate);
RBX_REGISTER_CLASS(RotateP);
RBX_REGISTER_CLASS(RotateV);
RBX_REGISTER_CLASS(Snap);
RBX_REGISTER_CLASS(Weld);
RBX_REGISTER_CLASS(ManualSurfaceJointInstance);
RBX_REGISTER_CLASS(ManualWeld);
RBX_REGISTER_CLASS(ManualGlue);
RBX_REGISTER_CLASS(BodyMover);
RBX_REGISTER_CLASS(TouchTransmitter);
RBX_REGISTER_CLASS(FaceInstance);
RBX_REGISTER_CLASS(Sky);
RBX_REGISTER_CLASS(PVInstance);
RBX_REGISTER_CLASS(VelocityMotor);
RBX_REGISTER_CLASS(Feature);
RBX_REGISTER_CLASS(DynamicRotate);
RBX_REGISTER_CLASS(JointInstance);
RBX_REGISTER_CLASS(Attachment);
RBX_REGISTER_CLASS(SpawnLocation);
RBX_REGISTER_CLASS(Mouse);
RBX_REGISTER_CLASS(PlayerMouse);
RBX_REGISTER_CLASS(Teams);
RBX_REGISTER_CLASS(BackpackItem);
RBX_REGISTER_CLASS(HopperBin);
RBX_REGISTER_CLASS(Camera);
RBX_REGISTER_CLASS(BasePlayerGui);
RBX_REGISTER_CLASS(PlayerGui);
RBX_REGISTER_CLASS(PlayerScripts);
RBX_REGISTER_CLASS(StarterPlayerScripts);
RBX_REGISTER_CLASS(StarterCharacterScripts);
RBX_REGISTER_CLASS(PartInstance);
RBX_REGISTER_CLASS(MeshPartInstance);
RBX_REGISTER_CLASS(FormFactorPart);
RBX_REGISTER_CLASS(BasicPartInstance);
RBX_REGISTER_CLASS(ExtrudedPartInstance);
RBX_REGISTER_CLASS(PART::Wedge);
RBX_REGISTER_CLASS(Decal);
RBX_REGISTER_CLASS(DecalTexture);
RBX_REGISTER_CLASS(TweenService);
RBX_REGISTER_CLASS(TweenBase);
RBX_REGISTER_CLASS(GuiItem);
RBX_REGISTER_CLASS(GuiBase);
RBX_REGISTER_CLASS(GuiBase2d);
RBX_REGISTER_CLASS(GuiBase3d);
RBX_REGISTER_CLASS(GuiRoot);
RBX_REGISTER_CLASS(GuiObject);
RBX_REGISTER_CLASS(GuiButton);
RBX_REGISTER_CLASS(GuiLabel);
RBX_REGISTER_CLASS(GuiMain);
RBX_REGISTER_CLASS(GuiLayerCollector);
RBX_REGISTER_CLASS(BillboardGui);
RBX_REGISTER_CLASS(ScreenGui);
RBX_REGISTER_CLASS(SurfaceGui);
RBX_REGISTER_CLASS(SelectionLasso);
RBX_REGISTER_CLASS(SelectionPartLasso);
RBX_REGISTER_CLASS(SelectionPointLasso);
RBX_REGISTER_CLASS(TextureTrail);
RBX_REGISTER_CLASS(FloorWire);
RBX_REGISTER_CLASS(GuiService);
RBX_REGISTER_CLASS(Frame);
RBX_REGISTER_CLASS(Scale9Frame);
RBX_REGISTER_CLASS(GuiImageButton);
RBX_REGISTER_CLASS(ImageLabel);

RBX_REGISTER_CLASS(PostEffect);
RBX_REGISTER_CLASS(BlurEffect);
RBX_REGISTER_CLASS(ColorCorrectionEffect);
RBX_REGISTER_CLASS(BloomEffect);

#ifdef ENABLE_CHROMIUM_FRAMES
RBX_REGISTER_CLASS(ChromiumFrame);
#endif
#ifdef ENABLE_VOICE_CHAT
RBX_REGISTER_CLASS(VoiceChatOutput);
#endif
RBX_REGISTER_CLASS(AyaService);
RBX_REGISTER_CLASS(VideoFrame);

RBX_REGISTER_CLASS(GuiTextButton);
RBX_REGISTER_CLASS(TextBox);
RBX_REGISTER_CLASS(TextLabel);
RBX_REGISTER_CLASS(PartAdornment);
RBX_REGISTER_CLASS(PVAdornment);
RBX_REGISTER_CLASS(Handles);
RBX_REGISTER_CLASS(HandlesBase);
RBX_REGISTER_CLASS(ArcHandles);
RBX_REGISTER_CLASS(SelectionBox);
RBX_REGISTER_CLASS(SelectionSphere);
RBX_REGISTER_CLASS(HandleAdornment);
RBX_REGISTER_CLASS(BoxHandleAdornment);
RBX_REGISTER_CLASS(ConeHandleAdornment);
RBX_REGISTER_CLASS(CylinderHandleAdornment);
RBX_REGISTER_CLASS(SphereHandleAdornment);
RBX_REGISTER_CLASS(LineHandleAdornment);
RBX_REGISTER_CLASS(ImageHandleAdornment);
RBX_REGISTER_CLASS(SurfaceSelection);
RBX_REGISTER_CLASS(CollectionService);
RBX_REGISTER_CLASS(Configuration);
RBX_REGISTER_CLASS(Folder);
RBX_REGISTER_CLASS(MotorFeature);
RBX_REGISTER_CLASS(Hole);
RBX_REGISTER_CLASS(MegaClusterInstance);
RBX_REGISTER_CLASS(PluginMouse);
RBX_REGISTER_CLASS(PluginManager);
RBX_REGISTER_CLASS(Plugin);
RBX_REGISTER_CLASS(Toolbar);
RBX_REGISTER_CLASS(Aya::Button);
RBX_REGISTER_CLASS(PrismInstance);
RBX_REGISTER_CLASS(PyramidInstance);
RBX_REGISTER_CLASS(ParallelRampInstance);
RBX_REGISTER_CLASS(RightAngleRampInstance);
RBX_REGISTER_CLASS(CornerWedgeInstance);
RBX_REGISTER_CLASS(CustomEvent);
RBX_REGISTER_CLASS(CustomEventReceiver);
// RBX_REGISTER_CLASS(PropertyInstance);
RBX_REGISTER_CLASS(BindableFunction);
RBX_REGISTER_CLASS(BindableEvent);
RBX_REGISTER_CLASS(Aya::Scripting::DebuggerManager);
RBX_REGISTER_CLASS(Aya::Scripting::ScriptDebugger);
RBX_REGISTER_CLASS(Aya::Scripting::DebuggerBreakpoint);
RBX_REGISTER_CLASS(Aya::Scripting::DebuggerWatch);
RBX_REGISTER_CLASS(Light);
RBX_REGISTER_CLASS(PointLight);
RBX_REGISTER_CLASS(SpotLight);
RBX_REGISTER_CLASS(SurfaceLight);
RBX_REGISTER_CLASS(LoginService);
RBX_REGISTER_CLASS(ReplicatedStorage);
RBX_REGISTER_CLASS(RobloxReplicatedStorage);
RBX_REGISTER_CLASS(ServerScriptService);
RBX_REGISTER_CLASS(ServerStorage);
RBX_REGISTER_CLASS(RemoteFunction);
RBX_REGISTER_CLASS(RemoteEvent);
RBX_REGISTER_CLASS(TerrainRegion);
RBX_REGISTER_CLASS(ModuleScript);
RBX_REGISTER_CLASS(PointsService);
RBX_REGISTER_CLASS(AdService);
RBX_REGISTER_CLASS(NotificationService);
RBX_REGISTER_CLASS(ReplicatedFirst);
RBX_REGISTER_CLASS(PartOperation);
RBX_REGISTER_CLASS(PartOperationAsset);
RBX_REGISTER_CLASS(UnionOperation);
RBX_REGISTER_CLASS(NegateOperation);
RBX_REGISTER_CLASS(Soundscape::SoundService);
RBX_REGISTER_CLASS(Soundscape::SoundChannel);
RBX_REGISTER_CLASS(GamepadService);
RBX_REGISTER_CLASS(LuaSourceContainer);
RBX_REGISTER_CLASS(HapticService);

// Xbox
#if defined(RBX_PLATFORM_DURANGO)
#include "DataModel/PlatformService.hpp"
RBX_REGISTER_CLASS(PlatformService);
#endif

static void onSlotException(std::exception& ex)
{
    FASTLOG(FLog::Error, "Slot Exception");
    Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "exception while signalling: %s", ex.what());
}

FactoryRegistrator::FactoryRegistrator()
{
    G3D::System::time(); // Initialize the Program Start Time.
    registerSound();
    Aya::registerScriptDescriptors();
    registerBodyMovers();

    registerValueClasses();
    Aya::registerStatsClasses();
    Aya::Surface::registerSurfaceDescriptors();

    Aya::signals::slot_exception_handler = onSlotException;

    srand(Aya::randomSeed());

    ModelInstance::hackPhysicalCharacter();
}

// Enum types
RBX_REGISTER_ENUM(ChangeHistoryService::RuntimeUndoBehavior);
RBX_REGISTER_ENUM(FunctionalTest::Result);
RBX_REGISTER_ENUM(TaskScheduler::PriorityMethod);
RBX_REGISTER_ENUM(TaskScheduler::Job::SleepAdjustMethod);
RBX_REGISTER_ENUM(TaskScheduler::ThreadPoolConfig);
RBX_REGISTER_ENUM(Action::ActionType);
RBX_REGISTER_ENUM(Controller::Button);
RBX_REGISTER_ENUM(HopperBin::BinType);
RBX_REGISTER_ENUM(GuiObject::SizeConstraint);
RBX_REGISTER_ENUM(PlayBackState);
RBX_REGISTER_ENUM(TweenInfo::TweenEasingStyle);
RBX_REGISTER_ENUM(TweenInfo::TweenStatus);
RBX_REGISTER_ENUM(TweenInfo::TweenEasingDirection);
RBX_REGISTER_ENUM(TextService::XAlignment);
RBX_REGISTER_ENUM(TextService::YAlignment);
RBX_REGISTER_ENUM(TextService::FontSize);
RBX_REGISTER_ENUM(TextService::Font);
RBX_REGISTER_ENUM(Camera::CameraType);
RBX_REGISTER_ENUM(Camera::CameraMode);
RBX_REGISTER_ENUM(Camera::CameraPanMode);
RBX_REGISTER_ENUM(LegacyController::InputType);
RBX_REGISTER_ENUM(DataModelArbiter::ConcurrencyModel);
RBX_REGISTER_ENUM(DataModelMesh::LODType);
RBX_REGISTER_ENUM(DebugSettings::ErrorReporting);
RBX_REGISTER_ENUM(EThrottle::EThrottleType);
RBX_REGISTER_ENUM(Feature::InOut);
RBX_REGISTER_ENUM(Feature::LeftRight);
RBX_REGISTER_ENUM(Feature::TopBottom);
RBX_REGISTER_ENUM(Joint::JointType);
RBX_REGISTER_ENUM(KeywordFilterType);
RBX_REGISTER_ENUM(Legacy::SurfaceConstraint);
RBX_REGISTER_ENUM(NormalId);
RBX_REGISTER_ENUM(Vector3::Axis);
RBX_REGISTER_ENUM(Humanoid::Status);
RBX_REGISTER_ENUM(Humanoid::HumanoidRigType);
RBX_REGISTER_ENUM(Humanoid::NameOcclusion);
RBX_REGISTER_ENUM(Humanoid::HumanoidDisplayDistanceType);
RBX_REGISTER_ENUM(Aya::HUMAN::StateType);
RBX_REGISTER_ENUM(DataModel::CreatorType);
RBX_REGISTER_ENUM(DataModel::Genre);
RBX_REGISTER_ENUM(DataModel::GearGenreSetting);
RBX_REGISTER_ENUM(DataModel::GearType);
RBX_REGISTER_ENUM(Instance::SaveFilter);
RBX_REGISTER_ENUM(BasicPartInstance::LegacyPartType);
RBX_REGISTER_ENUM(KeyframeSequence::Priority);
RBX_REGISTER_ENUM(SocialService::StuffType);
RBX_REGISTER_ENUM(PersonalServerService::PrivilegeType);
RBX_REGISTER_ENUM(ExtrudedPartInstance::VisualTrussStyle);
RBX_REGISTER_ENUM(PrismInstance::NumSidesEnum);
RBX_REGISTER_ENUM(PyramidInstance::NumSidesEnum);
RBX_REGISTER_ENUM(FriendService::FriendStatus);
RBX_REGISTER_ENUM(FriendService::FriendEventType);
RBX_REGISTER_ENUM(Handles::VisualStyle);
RBX_REGISTER_ENUM(SkateboardPlatform::MoveState);
RBX_REGISTER_ENUM(SoundType);
RBX_REGISTER_ENUM(SpecialShape::MeshType);
RBX_REGISTER_ENUM(SurfaceType);
RBX_REGISTER_ENUM(PartInstance::FormFactor);
RBX_REGISTER_ENUM(CollisionFidelity);
RBX_REGISTER_ENUM(UserInputService::SwipeDirection);
RBX_REGISTER_ENUM(UserInputService::Platform);
RBX_REGISTER_ENUM(UserInputService::MouseType);
RBX_REGISTER_ENUM(PartMaterial);
RBX_REGISTER_ENUM(PhysicalPropertiesMode);
RBX_REGISTER_ENUM(NetworkOwnership);
RBX_REGISTER_ENUM(Time::SampleMethod);
RBX_REGISTER_ENUM(GuiService::SpecialKey);
RBX_REGISTER_ENUM(GuiService::CenterDialogType);
RBX_REGISTER_ENUM(GuiService::UiMessageType);
RBX_REGISTER_ENUM(ChatService::ChatColor);
RBX_REGISTER_ENUM(MarketplaceService::CurrencyType);
RBX_REGISTER_ENUM(MarketplaceService::InfoType);
RBX_REGISTER_ENUM(CharacterMesh::BodyPart);
RBX_REGISTER_ENUM(GameSettings::VideoQuality);
RBX_REGISTER_ENUM(GameSettings::UploadSetting);
RBX_REGISTER_ENUM(GameBasicSettings::ControlMode);
RBX_REGISTER_ENUM(GameBasicSettings::MaxFramerate);
RBX_REGISTER_ENUM(GameBasicSettings::RenderQualitySetting);
RBX_REGISTER_ENUM(GameBasicSettings::CameraMode);
RBX_REGISTER_ENUM(GameBasicSettings::TouchCameraMovementMode);
RBX_REGISTER_ENUM(GameBasicSettings::ComputerCameraMovementMode);
RBX_REGISTER_ENUM(GameBasicSettings::TouchMovementMode);
RBX_REGISTER_ENUM(GameBasicSettings::ComputerMovementMode);
RBX_REGISTER_ENUM(GameBasicSettings::RotationType);
RBX_REGISTER_ENUM(GameBasicSettings::VirtualVersion);
RBX_REGISTER_ENUM(Frame::Style);
RBX_REGISTER_ENUM(GuiButton::Style);
RBX_REGISTER_ENUM(DialogRoot::DialogPurpose);
RBX_REGISTER_ENUM(DialogRoot::DialogTone);
RBX_REGISTER_ENUM(Voxel::CellMaterial);
RBX_REGISTER_ENUM(Voxel::CellBlock);
RBX_REGISTER_ENUM(Voxel::CellOrientation);
RBX_REGISTER_ENUM(Voxel::WaterCellForce);
RBX_REGISTER_ENUM(Voxel::WaterCellDirection);
RBX_REGISTER_ENUM(Explosion::ExplosionType);
RBX_REGISTER_ENUM(InputObject::UserInputType);
RBX_REGISTER_ENUM(InputObject::UserInputState);
RBX_REGISTER_ENUM(AssetService::AccessType);
RBX_REGISTER_ENUM(HttpService::HttpContentType);
RBX_REGISTER_ENUM(StarterGuiService::CoreGuiType);
RBX_REGISTER_ENUM(StarterPlayerService::DeveloperTouchCameraMovementMode);
RBX_REGISTER_ENUM(StarterPlayerService::DeveloperComputerCameraMovementMode);
RBX_REGISTER_ENUM(StarterPlayerService::DeveloperCameraOcclusionMode);
RBX_REGISTER_ENUM(StarterPlayerService::DeveloperTouchMovementMode);
RBX_REGISTER_ENUM(StarterPlayerService::DeveloperComputerMovementMode);
RBX_REGISTER_ENUM(TeleportService::TeleportState);
RBX_REGISTER_ENUM(TeleportService::TeleportType);
RBX_REGISTER_ENUM(KeyCode);
RBX_REGISTER_ENUM(MessageType);
RBX_REGISTER_ENUM(MarketplaceService::ProductPurchaseDecision);
RBX_REGISTER_ENUM(ThrottlingPriority);
RBX_REGISTER_ENUM(Soundscape::ReverbType);
RBX_REGISTER_ENUM(Soundscape::ListenerType);
RBX_REGISTER_ENUM(Soundscape::RollOffMode);
RBX_REGISTER_ENUM(PlayerActionType);
RBX_REGISTER_ENUM(RunService::RenderPriority);
RBX_REGISTER_ENUM(AdvArrowToolBase::JointCreationMode);
RBX_REGISTER_ENUM(GuiObject::ImageScale);
RBX_REGISTER_ENUM(UserInputService::OverrideMouseIconBehavior);
RBX_REGISTER_ENUM(Pose::PoseEasingStyle);
RBX_REGISTER_ENUM(Pose::PoseEasingDirection);
RBX_REGISTER_ENUM(HapticService::VibrationMotor);
RBX_REGISTER_ENUM(UserInputService::UserCFrame);

#if defined(RBX_PLATFORM_DURANGO)
RBX_REGISTER_ENUM(XboxKeyBoardType)
RBX_REGISTER_ENUM(VoiceChatState)
#endif
