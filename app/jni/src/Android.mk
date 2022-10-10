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
                   intro_screen.cpp \
                   tools.cpp \
                   sound.cpp \
                   title_screen.cpp \
                   touchscreen.cpp \
                   geometry.cpp \
                   tilemap.cpp \
                   game_screen.cpp \
                   $(LOCAL_PATH)/../tinyxml2/tinyxml2.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
