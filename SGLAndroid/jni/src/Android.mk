LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2-2.0.4
SDL_IMAGE_PATH := ../SDL2_image-2.0.1
SGL_PATH := ../SGL

LOCAL_C_INCLUDES := $(SDL_PATH)/include \
$(SDL_IMAGE_PATH) \
$(SGL_PATH) 


# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	demo.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SGL 

LOCAL_LDLIBS := -ldl -lGLESv3 -llog

include $(BUILD_SHARED_LIBRARY)
