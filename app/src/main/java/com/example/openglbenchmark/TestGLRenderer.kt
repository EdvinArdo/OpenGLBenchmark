package com.example.openglbenchmark

import android.opengl.GLES32
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

import android.opengl.GLSurfaceView

class TestGLRenderer : GLSurfaceView.Renderer {
    external fun init()
    external fun drawFrame()
    external fun setDimensions(width: Int, height: Int)

    override fun onSurfaceCreated(unused: GL10, config: EGLConfig) {
        init()
    }

    override fun onDrawFrame(unused: GL10) {
        drawFrame()
    }

    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {
        setDimensions(width, height)
    }
}