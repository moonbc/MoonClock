/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <time.h>

#include <android/log.h>
#include <android/bitmap.h>


#define LOG_TAG "Qrcode"

#define NELEM(x) ((int)(sizeof(x)/sizeof((x)[0])))

#ifdef ANDROID

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)

#endif


jobject nativeApplyOptionEffectObject(JNIEnv* env, jobject thiz,
		jbyteArray rawData, jint color_mode, jobject abs_info_buf,
		jobject option_buf, jobject imageOpt_buf, jint nWidth, jint nHeight,
		jint nColorCh);



int jniRegisterNativMethod(JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods ) {

	jclass clazz;

	clazz = (*env)->FindClass(env, className);

	if(clazz == NULL){
		return -1;
	}
	if((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0){
		return -1;
	}
//	LOGI("jniRegisterNativMethod ------" );
	return 0;
}

static JNINativeMethod sMethod[] = {
    /* name, signature, funcPtr */
    {"nativeApplyOptionEffectObject", "([BILjava/nio/Buffer;Ljava/nio/Buffer;Ljava/nio/Buffer;III)Landroid/graphics/Bitmap;", (jbyteArray*)nativeApplyOptionEffectObject},

};

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    if((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK){
        return result;
    }

    jniRegisterNativMethod(env, "com/example/qrcodetest", sMethod, NELEM(sMethod));

    LOGI("Qrcode JNI_OnLoad ------" );


    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
	LOGI("JNI_OnUnload ------" );
}



jobject nativeApplyOptionEffectObject(JNIEnv* env, jobject thiz,
		jbyteArray rawData, jint color_mode, jobject abs_info_buf,
		jobject option_buf, jobject imageOpt_buf, jint nWidth, jint nHeight,
		jint nColorCh)
{
		LOGI("nativeApplyOptionEffectObject" );

		return NULL;
}


