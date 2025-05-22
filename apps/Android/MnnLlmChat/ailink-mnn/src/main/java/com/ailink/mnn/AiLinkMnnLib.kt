package com.ailink.mnn

object AiLinkMnnLib {
    init {
        System.loadLibrary("ailink-mnn")
    }

//    external fun stringFromJNI(): String

    /**
     * 初始化 LLM 会话
     * @param modelPath 模型路径
     * @param forwardType 首选推理加速类型, 参看 [LlmForwardType]
     * @param numThread CPU推理并发数
     */
    @JvmStatic
    external fun initSession(modelPath: String, forwardType: Int, numThread: Int = 4): Boolean

    /**
     * 运行推理
     * @return 参看 [LlmErrCode]
     */
    @JvmStatic
    external fun runSession(inputBuff: FloatArray, outputBuff: FloatArray): Int

    @JvmStatic
    external fun releaseSession()
}