package com.ailink.mnn

object LlmForwardType {
    const val MNN_FORWARD_CPU = 0

    /**
     * Firstly find the first available backends not equal to CPU
     * If no other backends, use cpu
     */
    const val MNN_FORWARD_AUTO = 4

    /*Android / Common Device GPU API*/
    const val MNN_FORWARD_OPENCL = 3
    const val MNN_FORWARD_OPENGL = 6
    const val MNN_FORWARD_VULKAN = 7

    /*Android 8.1's NNAPI or CoreML for ios*/
    const val MNN_FORWARD_NN = 5
}