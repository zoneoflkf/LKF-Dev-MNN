#include "opencl_source_map.hpp" 
namespace MNN { 
#ifndef MNN_OPENCL_BUFFER_CLOSED
#ifdef MNN_SUPPORT_INTEL_SUBGROUP
const char* unary_subgroup_buf = 
"#ifdef MNN_SUPPORT_FP16\n"
"#pragma OPENCL EXTENSION cl_khr_fp16 : enable\n"
"#endif\n"
"#define GLOBAL_SIZE_3_DIMS "" __private const int global_size_dim0,__private const int global_size_dim1,__private const int global_size_dim2,\n"
"#define DEAL_NON_UNIFORM_DIM3(input1, input2, input3) "" if (input1 >= global_size_dim0 || input2 >= global_size_dim1 || input3 >= global_size_dim2) { "" return; "" }\n"
"inline float4 gelu(float4 in){\n"
" float4 value=0.79788458f*(0.044715f*in*in*in+in);\n"
" float4 x2=value*value;\n"
" float4 dst=value>(float4)5.0f ? (float4)1.0f : (value <= -(float4)5.0f ? -(float4)1.0f :\n"
" (value*(135135.0f+x2*(17325.0f+x2*(378.0f+x2))))/(135135.0f+x2*(62370.0f+x2*(3150.0f+x2*28.0f))));\n"
" return (1.0f+dst)*in*0.5f;\n"
"}\n"
"__kernel void unary_buf_c4_c4(GLOBAL_SIZE_3_DIMS\n"
" __global const INPUT_TYPE *input,\n"
" __global OUTPUT_TYPE *output,\n"
" __private const int width,\n"
" __private const int height,\n"
" __private const int channel,\n"
" __private const int batch,\n"
" __private const int input_pad_left,__private const int input_pad_right,\n"
" __private const int output_pad_left,__private const int output_pad_right) {\n"
" const int channel_block_idx=get_global_id(0);\n"
" const int w=get_global_id(1);\n"
" const int hb=get_global_id(2);\n"
" DEAL_NON_UNIFORM_DIM3(channel_block_idx,w,hb);\n"
" const int batch_idx=hb/height;\n"
" const int height_idx=hb % height;\n"
" const int offset=(((batch_idx+channel_block_idx*batch)*height+height_idx)*width+w)*4;\n"
" float4 in=convert_float4(vload4(0,input+offset));\n"
" float4 out=OPERATOR;\n"
" vstore4(CONVERT_OUTPUT4(out),0,output+offset);\n"
"}\n"
"__kernel void unary_buf_c4_c16(GLOBAL_SIZE_3_DIMS\n"
" __global const INPUT_TYPE *input,\n"
" __global OUTPUT_TYPE *output,\n"
" __private const int width,\n"
" __private const int height,\n"
" __private const int channel,\n"
" __private const int batch,\n"
" __private const int input_pad_left,__private const int input_pad_right,\n"
" __private const int output_pad_left,__private const int output_pad_right) {\n"
" const int channel_block_idx=get_global_id(0);\n"
" const int w=get_global_id(1);\n"
" const int hb=get_global_id(2);\n"
" DEAL_NON_UNIFORM_DIM3(channel_block_idx,w,hb);\n"
" const int batch_idx=hb/height;\n"
" const int height_idx=hb % height;\n"
" const int dst_width=output_pad_left+width+output_pad_right;\n"
" const int channel16=(channel+15)/16;\n"
" const int channe_out_idx=channel_block_idx >> 2;\n"
" const int offset=(((batch_idx+channel_block_idx*batch)*height+height_idx)*width+w)*4;\n"
" const int dst_offset=(((batch_idx*channel16+channe_out_idx)*height+height_idx)*dst_width+w+output_pad_left)*16+(channel_block_idx % 4)*4;\n"
" float4 in=convert_float4(vload4(0,input+offset));\n"
" float4 out=OPERATOR;\n"
" vstore4(CONVERT_OUTPUT4(out),0,output+dst_offset);\n"
" if(w == 0){\n"
" int pad_offset=(((batch_idx*channel16+channe_out_idx)*height+height_idx)*dst_width)*16+(channel_block_idx % 4)*4;\n"
" for(int i=0; i<output_pad_left; ++i){\n"
" vstore4((OUTPUT_TYPE4)0,0,output+pad_offset+i*16);\n"
" }\n"
" pad_offset += (width+output_pad_left)*16;\n"
" for(int i=0; i<output_pad_right; ++i){\n"
" vstore4((OUTPUT_TYPE4)0,0,output+pad_offset+i*16);\n"
" }\n"
" }\n"
"}\n"
"__attribute__((intel_reqd_sub_group_size(16)))\n"
"__kernel void unary_buf_c16_c16(GLOBAL_SIZE_3_DIMS\n"
" __global const INPUT_TYPE *input,\n"
" __global OUTPUT_TYPE *output,\n"
" __private const int width,\n"
" __private const int height,\n"
" __private const int channel,\n"
" __private const int batch,\n"
" __private const int input_pad_left,__private const int input_pad_right,\n"
" __private const int output_pad_left,__private const int output_pad_right) {\n"
" const int channel_idx=get_group_id(0);\n"
" const int w=get_global_id(1) << 2;\n"
" const int hb=get_global_id(2);\n"
" const int sglid=get_sub_group_local_id();\n"
" const int batch_idx=hb/height;\n"
" const int height_idx=hb % height;\n"
" const int src_width=width+input_pad_left+input_pad_right;\n"
" const int dst_width=width+output_pad_left+output_pad_right;\n"
" const int channel16=(channel+15)/16;\n"
" const int src_offset=(((batch_idx*channel16+channel_idx)*height+height_idx)*src_width+w+input_pad_left)*16;\n"
" const int dst_offset=(((batch_idx*channel16+channel_idx)*height+height_idx)*dst_width+w+output_pad_left)*16;\n"
" \n"
" float4 in=convert_float4(AS_INPUT_DATA4(INTEL_SUB_GROUP_READ4((__global INTEL_DATA*)(input+src_offset))));\n"
" float4 out=OPERATOR;\n"
" if (w+4>width) {\n"
" for (int i=0; i<width % 4; i++) {\n"
" output[dst_offset+i*16+sglid]=(OUTPUT_TYPE)out[i];\n"
" }\n"
" } else{\n"
" INTEL_SUB_GROUP_WRITE4((__global INTEL_DATA*)(output+dst_offset),AS_OUTPUT_DATA4(CONVERT_OUTPUT4(out)));\n"
" }\n"
" if(w == 0){\n"
" int pad_offset=(((batch_idx*channel+channel_idx)*height+height_idx)*dst_width)*16+sglid;\n"
" for(int i=0; i<output_pad_left; ++i){\n"
" output[pad_offset+i*16]=(OUTPUT_TYPE)0;\n"
" }\n"
" pad_offset += (width+output_pad_left)*16;\n"
" for(int i=0; i<output_pad_right; ++i){\n"
" output[pad_offset+i*16]=(OUTPUT_TYPE)0;\n"
" }\n"
" }\n"
"}\n"
"__attribute__((intel_reqd_sub_group_size(16)))\n"
"__kernel void unary_buf_c16_c4(GLOBAL_SIZE_3_DIMS\n"
" __global const INPUT_TYPE *input,\n"
" __global OUTPUT_TYPE *output,\n"
" __private const int width,\n"
" __private const int height,\n"
" __private const int channel,\n"
" __private const int batch,\n"
" __private const int input_pad_left,__private const int input_pad_right,\n"
" __private const int output_pad_left,__private const int output_pad_right) {\n"
" const int channel_idx=get_group_id(0);\n"
" const int w=get_global_id(1) << 2;\n"
" const int hb=get_global_id(2);\n"
" const int sglid=get_sub_group_local_id();\n"
" const int batch_idx=hb/height;\n"
" const int height_idx=hb % height;\n"
" const int src_width=width+input_pad_left+input_pad_right;\n"
" const int channel16=(channel+15)/16;\n"
" const int src_offset=(((batch_idx*channel16+channel_idx)*height+height_idx)*src_width+w+input_pad_left)*16;\n"
" const int dst_offset=(((batch_idx+(channel_idx<<2)*batch)*height+height_idx)*width+w)*4;\n"
" const int height_width=height*width*4;\n"
" \n"
" float4 in=convert_float4(AS_INPUT_DATA4(INTEL_SUB_GROUP_READ4((__global INTEL_DATA*)(input+src_offset))));\n"
" float4 out=OPERATOR;\n"
" const int lid_x=sglid % 4;\n"
" const int lid_y=sglid/4;\n"
" int block_size=w+4>width ? (width % 4) : 4;\n"
" for (int i=0; i<block_size; i++) {\n"
" output[dst_offset+i*4+lid_y*height_width+lid_x]=(OUTPUT_TYPE)out[i];\n"
" }\n"
"}\n"
;
#endif
#endif
}
