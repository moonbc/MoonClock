# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog -ljnigraphics
#LOCAL_LDLIBS += jni/libpreview.a
#LOCAL_LDLIBS += jni/x86/libpreview.a
LOCAL_MODULE    := libjni_qrcode


LOCAL_SRC_FILES := \
				jni_qrcode.c \
				qrInterface.c \
				bitstream.c \
				qrspec.c \
				mask.c \
				qrenc.c \
				qrinput.c \
				rscode.c \
				split.c \
				qrencoder.c \
				ZLib/expand_flate.c \
				ZLib/adler32.c \
				ZLib/compress.c \
				ZLib/crc32.c \
				ZLib/deflate.c \
				ZLib/infback.c \
				ZLib/inffast.c \
				ZLib/inflate.c \
				ZLib/inftrees.c \
				ZLib/trees.c \
				ZLib/uncompr.c \
				ZLib/zutil.c \
				png/pngvcrd.c \
				png/pngerror.c \
				png/pngpread.c \
				png/pngread.c \
				png/pngrtran.c \
				png/pngrutil.c \
				png/pngtest.c \
				png/pngtrans.c \
				png/pngwio.c \
				png/pngwrite.c \
				png/pngwtran.c \
				png/pngwutil.c \
				png/png.c \
				png/pngget.c \
				png/pngrio.c \
				png/pngset.c \
				png/pngmem.c \
								
			

				
LOCAL_STATIC_LIBRARIES := libqrcode
include $(BUILD_SHARED_LIBRARY)
