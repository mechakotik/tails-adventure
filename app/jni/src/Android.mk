LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/../SDL2_image/include \
                    $(LOCAL_PATH)/../SDL2_mixer/include \
                    $(LOCAL_PATH)/../tinyxml2

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

LOCAL_SRC_FILES := main.cpp \
                   game.cpp \
                   sprite.cpp \
                   screen_state_machine.cpp \
                   tools.cpp \
                   sound.cpp \
                   touchscreen.cpp \
                   geometry.cpp \
                   tilemap.cpp \
                   camera.cpp \
                   pawn.cpp \
                   object_set.cpp \
                   gamepad.cpp \
                   resource_manager.cpp \
                   hitbox_container.cpp \
                   font.cpp \
                   keyboard.cpp \
                   controller.cpp \
                   character.cpp \
                   hud.cpp \
                   intro_screen.cpp \
                   title_screen.cpp \
                   game_screen.cpp \
                   devmenu_screen.cpp \
                   character_movement.cpp \
                   character_collision.cpp \
                   objects/explosion.cpp \
                   objects/bomb.cpp \
                   objects/breakable_block.cpp \
                   objects/particle.cpp \
                   objects/ring.cpp \
                   objects/walker.cpp \
                   objects/hover_pod.cpp \
                   objects/dead_kukku.cpp \
                   objects/pushable_object.cpp \
                   objects/transition.cpp \
                   objects/bridge.cpp \
                   objects/bird_walker.cpp \
                   objects/bullet.cpp \
                   $(LOCAL_PATH)/../tinyxml2/tinyxml2.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
