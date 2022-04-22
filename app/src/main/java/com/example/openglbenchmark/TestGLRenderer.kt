package com.example.openglbenchmark

import android.opengl.GLES20
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

import android.opengl.GLSurfaceView

class TestGLRenderer : GLSurfaceView.Renderer {
    external fun init()
    external fun drawFrame()
    external fun setDimensions(width: Int, height: Int)

    override fun onSurfaceCreated(unused: GL10, config: EGLConfig) {
        GLES20.glClearColor(0.4f, 0.3f, 1.0f, 1.0f)
//        init()
    }

    override fun onDrawFrame(unused: GL10) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
//        drawFrame()
    }

    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {
//        setDimensions(width, height)
        GLES20.glViewport(0, 0, width, height)
    }
}