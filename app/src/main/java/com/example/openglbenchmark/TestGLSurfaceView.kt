package com.example.openglbenchmark

import android.content.Context
import android.opengl.GLSurfaceView

class TestGLSurfaceView(context: Context) : GLSurfaceView(context) {

    private val renderer: TestGLRenderer

    init {

        // Create an OpenGL ES 3.0 context
        setEGLContextClientVersion(3)

        renderer = TestGLRenderer()

        // Set the Renderer for drawing on the GLSurfaceView
        setRenderer(renderer)

        //todo: config
//        renderMode = RENDERMODE_WHEN_DIRTY
//        preserveEGLContextOnPause = true
    }
}