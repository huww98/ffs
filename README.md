# FFS 基于文件的模拟文件系统

FFS 是 File based File System 的缩写，这是我大二升大三暑假的操作系统大作业。

## 编译

本项目使用CMake编译，在 Ubuntu 18.04 系统上使用 gcc 8.0 编译测试。

1. 安装 gcc 8.0
   ```sh
   sudo apt install g++-8
   ```

2. 编译
   ```sh
   mkdir build
   cd build
   CXX=g++-8 cmake ..
   make
   sudo make install # 可选
   ```

## 使用方法

运行 `ffs help` 查看。
