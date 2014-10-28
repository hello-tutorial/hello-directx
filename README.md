DirectX Demo
========

## 简介
>  DirectX 3D 入门练习,目前主要是DirectX9的,部分代码来源自书籍和网络。


## 例子：
1. DirectX9例子 [环境配置图文参考](http://www.cnblogs.com/leexuzhi/archive/2011/10/12/2206587.html).
  * 解决方案：directx9-demo.sln
  * 实验环境：windows xp sp3(VMware10) + directx9.0c + VS2005 
  * 环境设置：TOOLS->Projects and Solutions->VC++ Directories 
    - 头文件(例如“D:\Microsoft DirectX SDK (November 2008)\Include”). 
    - 库(例如“D:\Microsoft DirectX SDK (November 2008)\Lib\x86”). 
  * 项目属性配置:工程名字上右键 Project->Properties->Linker->Input Folder
    - 库 d3d9.lib d3dx9.lib winmm.lib 

2. DirectX11例子
