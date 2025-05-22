package com.ailink.mnn

object LlmErrCode {
    const val NO_ERROR = 0
    const val OUT_OF_MEMORY = 1
    const val NOT_SUPPORT = 2
    const val COMPUTE_SIZE_ERROR = 3
    const val NO_EXECUTION = 4
    const val INVALID_VALUE = 5

    // User error
    const val INPUT_DATA_ERROR = 10
    const val CALL_BACK_STOP = 11

    // Op Resize Error
    const val TENSOR_NOT_SUPPORT = 20
    const val TENSOR_NEED_DIVIDE = 21

    // File error
    const val FILE_CREATE_FAILED = 30
    const val FILE_REMOVE_FAILED = 31
    const val FILE_OPEN_FAILED = 32
    const val FILE_CLOSE_FAILED = 33
    const val FILE_RESIZE_FAILED = 34
    const val FILE_SEEK_FAILED = 35
    const val FILE_NOT_EXIST = 36
    const val FILE_UNMAP_FAILED = 37
}