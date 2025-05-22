package com.ailink.mnn

object AiLinkMnnLib {
    init {
        System.loadLibrary("ailink-mnn")
    }

//    external fun stringFromJNI(): String

    /**
     * 初始化 LLM 会话
     * @param modelPath 模型路径
     * @param backends 推理后端, 参看 [LlmBackends]
     * @param numThread CPU推理并发数
     */
    @JvmStatic
    external fun initSession(modelPath: String, backends: Int, numThread: Int = 4): Boolean

    @JvmStatic
    external fun runSession(inputBuff: FloatArray, outputBuff: FloatArray): Int

    @JvmStatic
    external fun releaseSession()
}