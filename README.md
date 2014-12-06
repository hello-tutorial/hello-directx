# DirectX 练习

## DirectX9 部分

#### 简介：
学习dx9过程中写的测试例子，代码中有一部分是下面这本龙书中的例子代码(source code)
中文版：《DirectX 9.0 3D 游戏开发编程基础》
英文版：《Introduction to 3D Game Programming with DirectX9.0》(ISBN 1-55622-913-5)


龙书源代码修改说明：
1. 改正原书例子代码中的部分错误。
2. 原书例子环境是 windowsXP+dx9+vs2005，为了适应win7+dx11+vs2013 修正了环境差异导致的错误。


#### 实验环境：win7 + directX11 + vs2013
 * [DirectX SDK 下载](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
 * 如果安装dx11出现S1023错误，卸载"Microsoft Visual C++ 2010 x86/x64 Redistributable"后再装一遍。
 * 安装好DX_SDK后会自动在vs里面加入$(DXSDK_DIR)宏，这个宏指向SDK安装的根目录。
 * directx11中包含9/10/11的头和库，本例子用的dx9的头和库。


#### 项目使用说明:
 * 双击“directx9-demo.sln”即打开dx9的例子解决方案，所有的项目文件都在"directx9\"目录下面。
 * 不出意外的话，搭建好环境应该能直接运行例子了不需要再修改项目配置等。


#### 项目配置说明
> vs2013 vc++ directories 头文件和库不再能够统一设置了，而必须在每个项目上设置了。

* 项目(project)右键 Properties -> Configuration Properties
  - General
    * Character Set : Not Set
  - VC++ Directories
    * Include Directories: $(DXSDK_DIR)Include”。
    * Library Directories: $(DXSDK_DIR)Lib\x86”。
  - Linker
    * Input Folder -> Addtional Dependencies: d3d9.lib d3dx9.lib winmm.lib

## DirectX11部分

Comming Soon