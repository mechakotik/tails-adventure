LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../SDL2_image/include

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

LOCAL_SRC_FILES := main.cpp \
                   TA_Game.cpp \
                   TA_Texture.cpp \
                   TA_Sprite.cpp \
                   TA_ScreenStateMachine.cpp \
                   TA_IntroScreen.cpp \
                   TA_Tools.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
