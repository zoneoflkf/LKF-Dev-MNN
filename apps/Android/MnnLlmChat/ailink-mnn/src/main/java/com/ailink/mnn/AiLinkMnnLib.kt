package com.ailink.mnn

object AiLinkMnnLib {
    init {
        System.loadLibrary("ailink-mnn")
    }

//    external fun stringFromJNI(): String

    @JvmStatic
    external fun initSession(modelPath: String): Boolean

    @JvmStatic
    external fun runSession(inputBuff: FloatArray, outputBuff: FloatArray): Int

    @JvmStatic
    external fun releaseSession()
}