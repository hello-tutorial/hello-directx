### DirectX9入门练习

此代码是下面这本书中的例子代码(source code/demo code)，部分做了些修改并修正了一些bug。

中文版：《DirectX 9.0 3D 游戏开发编程基础》
英文版：《Introduction to 3D Game Programming with DirectX9.0》(ISBN 1-55622-913-5)  

### 实验环境：windows xp sp3(VMware10) + directx9.0c + VS2005 
### 环境配置     [参考](http://www.cnblogs.com/leexuzhi/archive/2011/10/12/2206587.html).
> * 环境设置：TOOLS->Projects and Solutions->VC++ Directories 
>   - 头文件(例如“D:\Microsoft DirectX SDK (November 2008)\Include”). 
>   - 库(例如“D:\Microsoft DirectX SDK (November 2008)\Lib\x86”). 
> * 项目属性配置:工程名字上右键 Project->Properties->Linker->Input Folder
>    - 库 d3d9.lib d3dx9.lib winmm.lib 
>    - 配置属性 字符集->未设置 
