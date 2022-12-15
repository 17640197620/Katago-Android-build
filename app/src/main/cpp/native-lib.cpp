//
// Created by k on 2021/12/17.
//

#include <jni.h>
#include <string>
#include <iostream>
#include <unistd.h>


JNIEXPORT jint  extern "C" void Java_com_example_katago_Native_jni1(
        JNIEnv *env,
        jobject /* this */) {
    return 1;
}

JNIEXPORT jint  extern "C" void Java_com_example_katago_Native_jni2(
        JNIEnv *env,
        jobject /* this */) {
    return 1;
}