LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := ${LOCAL_PATH}/include \
                    ${LOCAL_PATH}/include/objects \
                    ${LOCAL_PATH}/../SDL/include \
                    ${LOCAL_PATH}/../SDL_image/include \
                    ${LOCAL_PATH}/../SDL_mixer/include \
                    ${LOCAL_PATH}/../tinyxml2

LOCAL_SRC_FILES := $(shell cat ${LOCAL_PATH}/src/sources.list) \
                    ${LOCAL_PATH}/../tinyxml2/tinyxml2.cpp

LOCAL_SHARED_LIBRARIES := SDL3 SDL3_image SDL3_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)

