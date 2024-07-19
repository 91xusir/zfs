

 <h1 align="center">真封神复活计划</h1>

三界大乱单机网

<br />

<p align="center">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">仙魔改真封神计划！</h3>
  <p align="center">
    一个很疯狂也持续了很久的计划！！
    <br />
</p>
------


[TOC]

------

### 1.上手指南

开发环境：visual studio 2022

#### 1.1 Clone

```
git clone https://github.com/reniao69/zfs.git
```

#### 1.2 编译

```
打开各个vs解决方案，编译game_engine—>编译其他
//TODO 后续将将写编译脚本一键编译
```

- [ ] 编写cmake编译脚本

#### 1.3 项目说明

##### 1.3.1 目录结构

```
├─Backup					vs旧版解决方案备份文件
├─bin    					编译生成的二进制文件 可以将客户端内容放在这里，方便调试gc
├─temp						编译生成的临时文件
├─docs						旧docs文档，将采用md重构
├─runtime					游戏运行时需要的dll等
├─SQL						游戏数据库文件
-------------------------
├─engine 					引擎头文件.h和编译生成的lib静态库，供客户端和服务端使用
├─sdk						服务端和客户端依赖的外部库
├─shared					服务端和客户端公用代码
├─game_engine				游戏引擎
├─game_client 				客户端gc
├─gameworld_server 			游戏世界服务器W
├─login_server 				游戏登录服务器L  
├─region_server 			游戏逻辑服务器R  
├─tool_character			模型编辑
├─tool_crashreport			崩溃日志工具
├─tool_files_packet			PAK打包解包工具
├─tool_max9_utility			3dmax9插件
├─tool_max_utility			3dmax插件
├─tool_scene				地图工具
└─tool_ui					ui工具
-------------------------
客户端 服务端 工具  
│      │      │  
└─── 引擎 ────┘
```

##### 1.3.2 命名方式

lib库的命名方式，根据不同的运行库命名，

![image-20240719174731386](README/image-20240719174731386.png) 

![image-20240719172836181](README/image-20240719172836181.png) 

exe命名方式：

xxx_Release.exe 

xxx_Debug.exe 

##### 1.3.3 引擎工程

```
Engine共分为8大模块：

Core是最基本的模块,为其他模块提供与操作系统相关的操作,以及一些最基本的公用函数；

Audio是音频模块；

FilePack是文件打包模块；

Graph是图形模块,封装了DX的一些操作；

Net是网络模块,封装着基本的Socket等操作；

UI模块提供大量与用户界面相关的控件；

Scene模块处理地形,场景物件,事件触发,环境因素等内容；

Character是角色模块,封装与角色相关的动画,材质,特效,骨骼,皮肤等等内容；
-------------------------------------------------------------------

Engine项目需要生成四种版本的库文件
Multi-Threaded 				*_mt.lib
Multi-Threaded Debug		*_mtd.lib
Multi-Threaded DLL 			*_md.lib
Multi-Threaded DLL Debug	*_mdd.lib
工具使用md版本
客户端和服务器端使用mt版本
```

##### 1.3.4 客户端工程

```
游戏客户端工程由于历史原因,存在多种配置,但目前只需要Debug和Release版本配置,其它配置无效。
生成采用Multi-Threaded的运行库方式,故Debug和Release版本均依赖于相应MT的运行库方式的engine库。
```

##### 1.3.5 服务端工程

```
生成采用Multi-Threaded的运行库方式,
故Debug和Release版本均依赖于相应Multi-Threaded的运行库方式的net库（是engine库的一个组件）。
```

##### 1.3.6 工具工程

```
tool_character			目录下存放着角色编辑器的源文件
tool_max_utility		目录下存放着max导出插件的源文件
tool_scene				目录下存放着场景编辑器的源文件
tool_ui					目录下存放着UI编辑器的源文件

由于历史原因,存在多种配置,但目前只需要Debug和Release版本配置,其它配置无效。
生成采用Multi-Threaded DLL的运行库方式
故Debug和Release版本均依赖于相应Multi-Threaded DLL的运行库方式的engine库。
```

------

### 2.详细开发文档

[1.引擎](./doc/engine.md)

2.客户端

3.服务端

...

------

### 3.开发计划与进度

- [x] 项目迁移至vs2022环境，更新相关依赖

- [ ] 阅读引擎源代码，重写md文档
- [ ] 阅读客户端代码，重写md文档
- [ ] ...
