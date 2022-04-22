#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>
#include <EGL/egl.h>
#include "glad/include/glad/glad.h"

AAssetManager *mgr;

int width;
int height;
bool initialized = false;


extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_init(JNIEnv *env, jobject thiz) {
    if (!initialized) {
        initialized = true;


        // Initialize glad
        if (!gladLoadGLES2Loader((GLADloadproc) eglGetProcAddress)) {
            //todo: handle exception
        }

        // Todo: Build shaders

    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_drawFrame(JNIEnv *env, jobject thiz) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_setDimensions(JNIEnv *env, jobject thiz,
                                                              jint screenWidth,
                                                              jint screenHeight) {
    width = screenWidth;
    height = screenHeight;
    glViewport(0, 0, width, height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_MainActivity_passAssetManager(JNIEnv *env, jobject thiz,
                                                               jobject assetManager) {
    mgr = AAssetManager_fromJava(env, assetManager);
}