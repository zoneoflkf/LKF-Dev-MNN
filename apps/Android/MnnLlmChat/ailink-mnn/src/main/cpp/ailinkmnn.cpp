#include <jni.h>
#include <string>
#include <MNN/Interpreter.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include "jni.h"
#include "android/log.h"

/*typedef struct LlmResult {
    float *buff = nullptr;
    int count = 0;
} llm_result;*/

/*extern "C" JNIEXPORT jstring JNICALL
Java_com_ailink_mnn_AiLinkMnnLib_stringFromJNI(
        JNIEnv *env,
        jobject *//* this *//*) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}*/

static MNN::Interpreter *s_Interpreter = nullptr;
static MNN::Session *s_Session = nullptr;

inline const char *vec_to_cstr_ref(const std::vector<int> &v, std::string &out_str) {
    std::ostringstream oss;
    oss << '[';
    for (size_t i = 0; i < v.size(); ++i) {
        oss << v[i];
        if (i + 1 < v.size())
            oss << ',';
    }
    oss << ']';
    out_str = oss.str();
    return out_str.c_str();
}

/*void initSession(char *modelPath) {
    // 加载模型文件（路径为Android设备上的绝对路径）
    //std::string modelPath = "/data/data/com.alibaba.mnnllm.android/files/best-fp16_v32.mnn";
    s_Interpreter = MNN::Interpreter::createFromFile(modelPath);

    // 配置会话
    MNN::ScheduleConfig config;
    config.type = MNN_FORWARD_VULKAN;  // 选择合适的后端
    config.numThread = 4;              // 线程数
    s_Session = s_Interpreter->createSession(config);
}*/

/*int runSession(float *inputBuff, size_t count, llm_result &llmResult) {
    auto input = s_Interpreter->getSessionInput(s_Session, nullptr);
    MNN::Tensor inputTensor(input, input->getDimensionType());

    // 形状是: [1,320,320,3] 因此直接跳过第一个
    auto inputPtr = inputTensor.host<float>();
    inputPtr[0] = 1.0F;
    memcpy(inputPtr + 1, inputBuff, count * sizeof(float));
    input->copyFromHostTensor(&inputTensor); // 复制到输入张量

    // TODO 测试 打印输入尺寸
    auto inputSize = inputTensor.size();
    auto inputShape = inputTensor.shape();
    std::string inputShapeStr;
    vec_to_cstr_ref(inputShape, inputShapeStr);
    __android_log_print(ANDROID_LOG_DEBUG, "AiLink", "runSession: inShape=%s, inSize=%d",
                        inputShapeStr.c_str(), inputSize);

    // 执行推理
    MNN::ErrorCode rlt = s_Interpreter->runSession(s_Session);
    __android_log_print(ANDROID_LOG_DEBUG, "AiLink", "runSession: rlt=%d", rlt);
    if (rlt != MNN::ErrorCode::NO_ERROR) {
        return (int) rlt;
    }

    // 获取输出结果
    auto output = s_Interpreter->getSessionOutput(s_Session, nullptr);
    MNN::Tensor outputTensor(output, output->getDimensionType());
    output->copyToHostTensor(&outputTensor);

    // 拷贝输出结果
    auto outputSize = outputTensor.size();
    auto outputPtr = outputTensor.host<float>();
    memcpy(llmResult.buff, outputPtr, outputSize);
    llmResult.count = outputSize / (int) sizeof(float);

    // TODO 打印输出结果
    auto outputShape = outputTensor.shape();
    std::string shapeStr;
    vec_to_cstr_ref(outputShape, shapeStr);
    __android_log_print(ANDROID_LOG_DEBUG, "AiLink", "runSession: outShape=%s, outSize=%d",
                        shapeStr.c_str(), outputSize);

    return MNN::ErrorCode::NO_ERROR;
}*/

void releaseSession() {
    s_Interpreter->releaseSession(s_Session);
    s_Session = nullptr;

    MNN::Interpreter::destroy(s_Interpreter);
    s_Interpreter = nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_ailink_mnn_AiLinkMnnLib_initSession(JNIEnv *env, jclass clazz, jstring modelPath) {
    const char *modelPathCStr = env->GetStringUTFChars(modelPath, JNI_FALSE);

    // 加载模型文件（路径为Android设备上的绝对路径）
    //const char *modelPathCStr = "/data/data/com.alibaba.mnnllm.android/files/best-fp16_v32.mnn";
    s_Interpreter = MNN::Interpreter::createFromFile(modelPathCStr);

    // 配置会话
    MNN::ScheduleConfig config;
    config.type = MNN_FORWARD_VULKAN;  // 选择合适的后端
    config.numThread = 4;              // 线程数
    s_Session = s_Interpreter->createSession(config);
    return s_Session != nullptr ? JNI_TRUE : JNI_FALSE;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_ailink_mnn_AiLinkMnnLib_runSession(JNIEnv *env, jclass clazz, jfloatArray input_buff, jfloatArray output_buff) {
    auto input = s_Interpreter->getSessionInput(s_Session, nullptr);
    MNN::Tensor inputTensor(input, input->getDimensionType());

    auto inputPtr = inputTensor.host<float>();
    // 拷贝图像输入
    auto inputBuff = env->GetFloatArrayElements(input_buff, JNI_FALSE);
    auto inputBuffSize = env->GetArrayLength(input_buff) * sizeof(jfloat);
    memcpy(inputPtr, inputBuff, inputBuffSize);
    // 复制到输入张量
    input->copyFromHostTensor(&inputTensor);

    // TODO 测试 打印输入尺寸
    auto inputSize = inputTensor.size();
    auto inputShape = inputTensor.shape();
    std::string inputShapeStr;
    vec_to_cstr_ref(inputShape, inputShapeStr);
    __android_log_print(ANDROID_LOG_DEBUG, "AiLink", "runSession: inShape=%s, inSize=%d",
                        inputShapeStr.c_str(), inputSize);

    // 执行推理
    MNN::ErrorCode rlt = s_Interpreter->runSession(s_Session);
    __android_log_print(ANDROID_LOG_DEBUG, "AiLink", "runSession: rlt=%d", rlt);
    if (rlt != MNN::ErrorCode::NO_ERROR) {
        return (int) rlt;
    }

    // 获取输出结果
    auto output = s_Interpreter->getSessionOutput(s_Session, nullptr);
    MNN::Tensor outputTensor(output, output->getDimensionType());
    output->copyToHostTensor(&outputTensor);

    // 拷贝输出结果
    auto outputSize = outputTensor.size();
    auto outputPtr = outputTensor.host<float>();
    env->SetFloatArrayRegion(output_buff, 0, env->GetArrayLength(output_buff), outputPtr);

    // TODO 打印输出结果
    auto outputShape = outputTensor.shape();
    std::string shapeStr;
    vec_to_cstr_ref(outputShape, shapeStr);
    __android_log_print(ANDROID_LOG_DEBUG, "AiLink", "runSession: outShape=%s, outSize=%d",
                        shapeStr.c_str(), outputSize);

    return MNN::ErrorCode::NO_ERROR;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ailink_mnn_AiLinkMnnLib_releaseSession(JNIEnv *env, jclass clazz) {
    if (s_Session) {
        s_Interpreter->releaseSession(s_Session);
        s_Session = nullptr;
    }

    if (s_Interpreter) {
        MNN::Interpreter::destroy(s_Interpreter);
        s_Interpreter = nullptr;
    }
}