
# LHPE-Lightweight-2D-Human-Pose-Estimation

This repository is based on [Real-time 2D Multi-Person Pose Estimation on CPU: Lightweight OpenPose](https://arxiv.org/pdf/1811.12004.pdf)] paper. And also thanks to [ytai-chn](https://github.com/ytai-chn) to 2D Human Pose Implementation by using C++ code. 

![PE_cover_img](https://github.com/user-attachments/assets/fb57041a-4a84-41b8-9059-587a0b404229)

**What is different than [ytai-chn](https://github.com/ytai-chn) to 2D Human Pose Implementation code ?** 

 - OS Windows Implementation (windows 11 x64-based PC)
 - Quantized FLOAT16 ONNX model (7.8 MB) 
 - Reading IMAGES, VIDEOS and CAMERA to inference

**How to use LHPE-Lightweight-2D-Human-Pose-Estimation?**

 - Clone the repository
 
 - **Open** VS2022 (recommended) -> **Create a new project** -> **Empty Project** -> (Please check : Project -> Properties -> Configuration Properties -> **General -> C++ Language Standard** -> ISO C++17 Standard(/std:c++17))
 
 -  Make **main.cpp** under Source Files folder and copy cloned github **main.cpp** code.
 
 - Make **pose.hpp** and **poseEstimation.hpp** under Header Files folder and copy cloned github **pose.hpp** and **poseEstimation.hpp** codes.

 - **Install OpenCV** to VS2022 -> from toolbar **Project** -> **Manage NuGet Packages ...** -> **Search** OpenCV and **Install** (Recommended : *opencv4.2 by Fugro Roadware*)
 
 - **Build** 
 
 - Open your **cmd** and  **type** : 
 

      **{your_exe file PATH} {model path} {file name} {file type}**

    - **your_exe file PATH :** your exe file  after you build your project in VS

    - **model path** : PATH to your PE (Pose Estimation) ONNX model *(recommened : poseEstimationModel.onnx)*

   - **file name** : the PATH to you file which can be IMAGE, VIDEO or CAMERA INDEX
   
   -  **file type** : could be **image** , **video** or **camera** based on your file type


**Note :** If you want to use ONNX quantized model, please build ONNXRUNTIME and make inference.

Thank you OpenPose and  [ytai-chn](https://github.com/ytai-chn) to such a hard  work. If you have questions, please contact with me uacoding01@gmail.com
