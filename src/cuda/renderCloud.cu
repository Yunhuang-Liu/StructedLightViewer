#include "cudaTypeDef.cuh"

__global__ void makeCloud(const cv::cuda::PtrStep<float> depthImg,const cv::cuda::PtrStep<uchar3> colorImg,const Eigen::Matrix3f intrinsic_inv,
                          const int rows, const int cols, float* ptrCloud){
    const int x = blockDim.x * blockIdx.x + threadIdx.x;
    const int y = blockDim.y * blockIdx.y + threadIdx.y;
    if(x < cols && y < rows){
        const int indexStart = (y * cols + x) * 7;
        const float depth = depthImg.ptr(y)[x];
        if(depth <= 0){
            ptrCloud[indexStart] = 0.f;
            ptrCloud[indexStart + 1] = 0.f;
            ptrCloud[indexStart + 2] = 0.f;
            ptrCloud[indexStart + 3] = 0.f;
            ptrCloud[indexStart + 4] = 0.f;
            ptrCloud[indexStart + 5] = 0.f;
            ptrCloud[indexStart + 6] = 0.f;
            return;
        }
        Eigen::Vector3f vertex(x * depth, y * depth, depth);
        Eigen::Vector3f result = intrinsic_inv * vertex;
        ptrCloud[indexStart] = result(0,0);
        ptrCloud[indexStart + 1] = result(1,0);
        ptrCloud[indexStart + 2] = result(2,0);
        ptrCloud[indexStart + 3] = colorImg.ptr(y)[x].z;
        ptrCloud[indexStart + 4] = colorImg.ptr(y)[x].y;
        ptrCloud[indexStart + 5] = colorImg.ptr(y)[x].x;
        ptrCloud[indexStart + 6] = 1.0f;
    }
}

extern "C" void renderCloud_CUDA(const cv::cuda::GpuMat & depthImg, const cv::cuda::GpuMat & colorImg, const Eigen::Matrix3f & intrinsic_inv,
                            float* ptrCloud){
    dim3 block(32, 8);
    dim3 grid((depthImg.cols + block.x - 1) / block.x, (depthImg.rows + block.y - 1) / block.y);
    makeCloud<<<grid,block>>>(depthImg, colorImg, intrinsic_inv, depthImg.rows, depthImg.cols, ptrCloud);
    cudaThreadSynchronize();
}
