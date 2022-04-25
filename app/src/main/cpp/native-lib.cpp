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

unsigned int shaderProgram;
unsigned int VBO, VAO, EBO;

const char *loadShader(const char *filename) {
    AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
    long fileSize = AAsset_getLength(asset);
    char *buffer = new char[fileSize + 1]();
    AAsset_read(asset, buffer, fileSize);
    AAsset_close(asset);
    buffer[fileSize] = '\0';
    __android_log_print(ANDROID_LOG_DEBUG, "openglbenchmarktag",
                        "%s", buffer);
    return buffer;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_openglbenchmark_TestGLRenderer_init(JNIEnv *env, jobject thiz) {
    if (!initialized) {
        initialized = true;

        // Initialize glad
        if (!gladLoadGLES2Loader((GLADloadproc) eglGetProcAddress)) {
            __android_log_print(ANDROID_LOG_ERROR, "openglbenchmarktag",
                                "Failed to initialize GLAD");
        }

        // build and compile our shader program
        // ------------------------------------
        // vertex shader
        const char *vertexShaderSource = loadShader("shader.vert");
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        delete vertexShaderSource;
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            __android_log_print(ANDROID_LOG_ERROR, "openglbenchmarktag",
                                "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
        }
        // fragment shader
        const char *fragmentShaderSource = loadShader("shader.frag");
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        delete fragmentShaderSource;
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            __android_log_print(ANDROID_LOG_ERROR, "openglbenchmarktag",
                                "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            __android_log_print(ANDROID_LOG_ERROR, "openglbenchmarktag",
                                "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);

        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

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
    glUseProgram(shaderProgram);
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
}