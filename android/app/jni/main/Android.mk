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

LOCAL_STATIC_LIBRARIES := SDL3_static SDL3_image_static SDL3_mixer_static

include $(BUILD_SHARED_LIBRARY)
