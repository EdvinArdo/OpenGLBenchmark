#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>
#include <EGL/egl.h>
#include <unistd.h>
#include <time.h>
#include "glad/include/glad/glad.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"
#include <sys/stat.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define TAG "openglbenchmarktag"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

float getTime();

float startTime;

AAssetManager *mgr;

int width;
int height;
bool initialized = false;

Shader *shader;
Model *ourModel;
Camera *camera;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_init(JNIEnv *env, jobject thiz) {
    if (!initialized) {
        initialized = true;

        // Initialize glad
        if (!gladLoadGLES2Loader((GLADloadproc) eglGetProcAddress)) {
            LOGE("Failed to initialize GLAD");
        }

        // init shader
        shader = new Shader("shader.vert", "shader.frag");

        glEnable(GL_DEPTH_TEST);

        ourModel = new Model("sponza/sponza.obj");
        camera = new Camera(glm::vec3(-1400.0f, 100.0f, -250.0f));

        startTime = getTime();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_drawFrame(JNIEnv *env, jobject thiz) {
// render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = getTime();
    float timeDiff = time - startTime;

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float) width / (float) height, 1.0f, 10000.0f);

    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", camera->GetViewMatrix());
    shader->setVec3("viewPos", camera->Position);
    shader->setMat4("projection", projection);
    float angle = glm::radians(360.0f / 10.0f * timeDiff);
    shader->setVec3("lightPos", 1400.0f * cos(angle), 100, 0);
    shader->setVec3("lightColor", 1.0f, 1.0, 1.0);

    ourModel->Draw(*shader);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_setDimensions(JNIEnv *env, jobject thiz, jint screenWidth, jint screenHeight) {
    width = screenWidth;
    height = screenHeight;
    glViewport(0, 0, width, height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_MainActivity_passAssetManager(JNIEnv *env, jobject thiz, jobject assetManager) {
    mgr = AAssetManager_fromJava(env, assetManager);

    jclass activityClass = env->GetObjectClass(thiz);

    // Get path to cache dir (/data/data/org.wikibooks.OpenGL/cache)
    jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject file = env->CallObjectMethod(thiz, getCacheDir);
    jclass fileClass = env->FindClass("java/io/File");
    jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring jpath = (jstring) env->CallObjectMethod(file, getAbsolutePath);
    const char *app_dir = env->GetStringUTFChars(jpath, NULL);


    // chdir in the application cache directory
    chdir(app_dir);

    AAssetDir *assetDir = AAssetManager_openDir(mgr, "");
    const char *filename = (const char *) NULL;
    while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
        AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
        char buf[BUFSIZ];
        int nb_read = 0;
        FILE *out = fopen(filename, "w");
        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0) {
            fwrite(buf, nb_read, 1, out);
        }
        fclose(out);
        AAsset_close(asset);
    }
    AAssetDir_close(assetDir);


    const int num_sub_dirs = 2;
    std::string sub_dirs[num_sub_dirs] = {"sponza", "sponza/textures"};
    for (int i = 0; i < num_sub_dirs; i++) {
        string sub_dir = string(app_dir).append("/").append(sub_dirs[i]);

        AAssetDir *assetDir = AAssetManager_openDir(mgr, sub_dirs[i].c_str());
        filename = (const char *) NULL;
        while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
            string file_path = string(sub_dirs[i]).append("/").append(filename);
            AAsset *asset = AAssetManager_open(mgr, file_path.c_str(), AASSET_MODE_STREAMING);
            char buf[BUFSIZ];
            int nb_read = 0;
            mkdir(sub_dirs[i].c_str(), 0777);
            FILE *out = fopen(file_path.c_str(), "w");
            while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0) {
                fwrite(buf, nb_read, 1, out);
            }
            fclose(out);
            AAsset_close(asset);
        }
        AAssetDir_close(assetDir);
    }
    env->ReleaseStringUTFChars(jpath, app_dir);
}

float getTime() {
    timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return res.tv_sec + res.tv_nsec / 1.e9;
}