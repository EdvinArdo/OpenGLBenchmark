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

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define TAG "openglbenchmarktag"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

AAssetManager *mgr;

int width;
int height;
bool initialized = false;

unsigned int VBO, VAO;
unsigned int texture;
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
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

                -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

                0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glEnable(GL_DEPTH_TEST);

        // texture
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            LOGE("Failed to load texture");
        }
        stbi_image_free(data);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);

    glm::mat4 model = glm::mat4(1.0f);
    LOGD("%f", res.tv_sec * 1.e9 + res.tv_nsec);
    float time = res.tv_sec + res.tv_nsec / 1.e9;
    model = glm::rotate(model, (float) time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));;

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);


    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // draw
    shader->use();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time

//     egl: swap buffers
    eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));
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