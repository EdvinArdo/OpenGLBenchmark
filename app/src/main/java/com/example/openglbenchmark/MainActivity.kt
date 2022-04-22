package com.example.openglbenchmark

import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.openglbenchmark.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private lateinit var gLView: GLSurfaceView
    private var mgr: AssetManager? = null

    external fun passAssetManager(assetManager: AssetManager)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        mgr = resources.assets;
        mgr?.let { passAssetManager(it) }

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        gLView = TestGLSurfaceView(this)
        setContentView(gLView)
    }

    companion object {
        // Used to load the 'openglbenchmark' library on application startup.
        init {
            System.loadLibrary("openglbenchmark")
        }
    }
}