#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>
#include <EGL/egl.h>

AAssetManager *mgr;

struct PosColorVertex {
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] = {
        {-1.0f, 1.0f,  1.0f,  0xff000000},
        {1.0f,  1.0f,  1.0f,  0xff0000ff},
        {-1.0f, -1.0f, 1.0f,  0xff00ff00},
        {1.0f,  -1.0f, 1.0f,  0xff00ffff},
        {-1.0f, 1.0f,  -1.0f, 0xffff0000},
        {1.0f,  1.0f,  -1.0f, 0xffff00ff},
        {-1.0f, -1.0f, -1.0f, 0xffffff00},
        {1.0f,  -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cubeTriList[] = {
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        5, 6, 7,
        0, 2, 4,
        4, 2, 6,
        1, 5, 3,
        5, 7, 3,
        0, 4, 1,
        4, 5, 1,
        2, 3, 6,
        6, 3, 7,
};

int width;
int height;
bool initialized = false;


extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_init(JNIEnv *env, jobject thiz) {
    if (!initialized) {
        initialized = true;

    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_drawFrame(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_setDimensions(JNIEnv *env, jobject thiz,
                                                               jint screenWidth,
                                                               jint screenHeight) {
    width = screenWidth;
    height = screenHeight;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_MainActivity_passAssetManager(JNIEnv *env, jobject thiz,
                                                                jobject assetManager) {
    mgr = AAssetManager_fromJava(env, assetManager);
}