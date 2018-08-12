LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/audio/include)
$(call import-add-path,$(LOCAL_PATH)/../../Classes/rapidjson)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := hellocpp/main.cpp\
				   ../../Classes/AppDelegate.cpp\
				   ../../Classes/GameStartMovie.cpp\
                   ../../Classes/MainScene.cpp\
				   ../../Classes/TipsLabel.cpp\
				   ../../Classes/MissionMG.cpp\
				   ../../Classes/ExtensionsAlgorithm.cpp\
				   ../../Classes/DebugLayer.cpp\
                   ../../Classes/Tools/AnimateTool.cpp\
				   ../../Classes/Tools/MyBodyParser.cpp\
				   ../../Classes/Tools/JsonParser.cpp\
				   ../../Classes/Tools/AudioManagement.cpp\
				   ../../Classes/Tools/MajitooTool.cpp\
				   ../../Classes/Scene/SettingMenu.cpp\
				   ../../Classes/Scene/FailScene.cpp\
				   ../../Classes/Scene/MissionScene.cpp\
				   ../../Classes/Scene/DeveloperMenuScene.cpp\
				   ../../Classes/Scene/LoginScene.cpp\
				   ../../Classes/Scene/CreateCharacterScene.cpp\
				   ../../Classes/Scene/SplashSceneFactory.cpp\
				   ../../Classes/Scene/ItemScene.cpp\
				   ../../Classes/Scene/StoreScene.cpp\
				   ../../Classes/Scene/ScrollMode/ScrollSceneFactory.cpp\
				   ../../Classes/Scene/ScrollMode/ScrollSceneObject.cpp\
				   ../../Classes/Scene/ScrollMode/DropItem.cpp\
				   ../../Classes/Scene/BattleMode/BattleScene.cpp\
				   ../../Classes/Map/ScrollMode/ScrollMap.cpp\
				   ../../Classes/Map/BattleMode/BattleMap.cpp\
				   ../../Classes/MainCharacter/MainCharacter.cpp\
				   ../../Classes/Monster/ScrollMode/ScrollMonsterFactory.cpp\
				   ../../Classes/Monster/ScrollMode/MonsterAI.cpp\
				   ../../Classes/Monster/BattleMode/BattleSoldierFactory.cpp\
				   ../../Classes/Monster/BattleMode/SoldierAI.cpp\
				   ../../Classes/UI/ScrollMode/HUDlayer.cpp\
				   ../../Classes/UI/ScrollMode/Dialog.cpp\
				   ../../Classes/UI/BattleMode/BattleHUDlayer.cpp\
				   ../../Classes/UI/BattleMode/JoyStick.cpp\
				   ../../Classes/UI/MyButton.cpp\
				   ../../Classes/UI/DamageMG.cpp\
				   ../../Classes/NPC/NPC_AI.cpp\
				   ../../Classes/NPC/NPCFactory.cpp\
				   ../../Classes/RPGTools/SkillMG.cpp\
				   ../../Classes/RPGTools/RPGTool.cpp\
				   ../../Classes/RPGTools/ItemSprite.cpp\
				   ../../Classes/HelloWorldScene.cpp\
				   ../../Classes/dragonBones/animation/Animation.cpp \
                   ../../Classes/dragonBones/animation/AnimationState.cpp \
				   ../../Classes/dragonBones/animation/BaseTimelineState.cpp \
                   ../../Classes/dragonBones/animation/TimelineState.cpp \
                   ../../Classes/dragonBones/animation/WorldClock.cpp \
                   ../../Classes/dragonBones/armature/Armature.cpp \
                   ../../Classes/dragonBones/armature/Bone.cpp \
				   ../../Classes/dragonBones/armature/Constraint.cpp \
                   ../../Classes/dragonBones/armature/Slot.cpp \
				   ../../Classes/dragonBones/armature/TransformObject.cpp \
				   ../../Classes/dragonBones/cocos2dx/CCArmatureDisplay.cpp \
                   ../../Classes/dragonBones/cocos2dx/CCFactory.cpp \
                   ../../Classes/dragonBones/cocos2dx/CCSlot.cpp \
				   ../../Classes/dragonBones/cocos2dx/CCTextureAtlasData.cpp \
                   ../../Classes/dragonBones/core/BaseObject.cpp \
				   ../../Classes/dragonBones/core/DragonBones.cpp \
                   ../../Classes/dragonBones/events/EventObject.cpp \
                   ../../Classes/dragonBones/factories/BaseFactory.cpp \
				   ../../Classes/dragonBones/geom/Point.cpp \
				   ../../Classes/dragonBones/geom/Transform.cpp \
				   ../../Classes/dragonBones/model/AnimationConfig.cpp \
                   ../../Classes/dragonBones/model/AnimationData.cpp \
                   ../../Classes/dragonBones/model/ArmatureData.cpp \
				   ../../Classes/dragonBones/model/BoundingBoxData.cpp \
				   ../../Classes/dragonBones/model/ConstraintData.cpp \
				   ../../Classes/dragonBones/model/DisplayData.cpp \
                   ../../Classes/dragonBones/model/DragonBonesData.cpp \
				   ../../Classes/dragonBones/model/TextureAtlasData.cpp \
				   ../../Classes/dragonBones/model/UserData.cpp \
				   ../../Classes/dragonBones/parsers/BinaryDataParser.cpp \
                   ../../Classes/dragonBones/parsers/DataParser.cpp \
                   ../../Classes/dragonBones/parsers/JSONDataParser.cpp
                   
				   
LOCAL_C_INCLUDES :=     $(LOCAL_PATH)/../../Classes/ \
                        $(LOCAL_PATH)/../../Classes/animation \
                        $(LOCAL_PATH)/../../Classes/armature \
                        $(LOCAL_PATH)/../../Classes/core \
                        $(LOCAL_PATH)/../../Classes/events \
                        $(LOCAL_PATH)/../../Classes/factories \
                        $(LOCAL_PATH)/../../Classes/geom \
                        $(LOCAL_PATH)/../../Classes/model \
                        $(LOCAL_PATH)/../../Classes/parsers \
                        $(LOCAL_PATH)/../../Classes/cocos2dx \
                        $(LOCAL_PATH)/../../Classes/textures \

LOCAL_EXPORT_C_INCLUDES :=     $(LOCAL_PATH)/../../Classes/ \
                        $(LOCAL_PATH)/../../Classes/animation \
                        $(LOCAL_PATH)/../../Classes/armature \
                        $(LOCAL_PATH)/../../Classes/core \
                        $(LOCAL_PATH)/../../Classes/events \
                        $(LOCAL_PATH)/../../Classes/factories \
                        $(LOCAL_PATH)/../../Classes/geom \
                        $(LOCAL_PATH)/../../Classes/model \
                        $(LOCAL_PATH)/../../Classes/parsers \
                        $(LOCAL_PATH)/../../Classes/cocos2dx \
                        $(LOCAL_PATH)/../../Classes/textures \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES += cocos_curl_static


# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,./curl/prebuilt/android)
# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
