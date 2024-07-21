# 引擎文档

## 一、修改记录

```c++
game_engine\graph\d3dapp.cpp

			//TODO 这里之前注释了，不知道为啥，先还原看看
            // case HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ):
        case HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ):
            _tcscpy( strMsg, _T("Could not load required media." ) );
            break;
```

## 二、模块及文档

Engine共分为8大模块：

1.[Core](./engine/core.md) 是最基本的模块,为其他模块提供与操作系统相关的操作,以及一些最基本的公用函数；

2.Audio 是音频模块；

3.FilePack 是文件打包模块；

4.[Graph](./engine/graph.md) 是图形模块,封装了DX的一些操作；

5.Net 是网络模块,封装着基本的Socket等操作；

6.UI 模块提供大量与用户界面相关的控件；

7.Scene 模块处理地形,场景物件,事件触发,环境因素等内容；

8.Character 是角色模块,封装与角色相关的动画,材质,特效,骨骼,皮肤等等内容；
