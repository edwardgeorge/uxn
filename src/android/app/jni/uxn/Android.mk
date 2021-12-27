LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main
SDL_PATH := $(LOCAL_PATH)/../SDL
UXN_DIR := $(LOCAL_PATH)/../../../../../
LOCAL_C_INCLUDES := $(SDL_PATH)/include $(UXN_DIR)/src
LOCAL_SRC_FILES :=  \
	$(UXN_DIR)/src/uxn-fast.c \
	$(UXN_DIR)/src/uxnemu.c \
	$(UXN_DIR)/src/devices/apu.c \
	$(UXN_DIR)/src/devices/file.c \
	$(UXN_DIR)/src/devices/mouse.c \
	$(UXN_DIR)/src/devices/ppu.c \
	$(UXN_DIR)/src/devices/ppu_aarch64.c
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
