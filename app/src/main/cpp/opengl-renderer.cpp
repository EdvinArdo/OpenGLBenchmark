#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>
#include <EGL/egl.h>
#include <unistd.h>
#include "glad/include/glad/glad.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"

#define TAG "openglbenchmarktag"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

AAssetManager *mgr;

int width;
int height;
bool initialized = false;

unsigned int VBO, VAO, EBO;

Shader *shader;

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

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
                0.5f, 0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f, 0.5f, 0.0f   // top left
        };
        unsigned int indices[] = {  // note that we start from 0!
                0, 1, 3,  // first Triangle
                1, 2, 3   // second Triangle
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_drawFrame(JNIEnv *env, jobject thiz) {
// render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw
    shader->use();
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time

//     egl: swap buffers
    eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));
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

    jclass activityClass = env->GetObjectClass(thiz);

    // Get path to cache dir (/data/data/org.wikibooks.OpenGL/cache)
    jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject file = env->CallObjectMethod(thiz, getCacheDir);
    jclass fileClass = env->FindClass("java/io/File");
    jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath",
                                                 "()Ljava/lang/String;");
    jstring jpath = (jstring) env->CallObjectMethod(file, getAbsolutePath);
    const char *app_dir = env->GetStringUTFChars(jpath, NULL);

    // chdir in the application cache directory
    chdir(app_dir);
    env->ReleaseStringUTFChars(jpath, app_dir);

    AAssetDir *assetDir = AAssetManager_openDir(mgr, "");
    const char *filename = (const char *) NULL;
    while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
        AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
        char buf[BUFSIZ];
        int nb_read = 0;
        FILE *out = fopen(filename, "w");
        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
            fwrite(buf, nb_read, 1, out);
        fclose(out);
        AAsset_close(asset);
    }
    AAssetDir_close(assetDir);
}