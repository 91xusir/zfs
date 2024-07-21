#### 引言

Core是引擎的最底层，提供了和操作系统相关的基本操作，让上层致力于实际的功能。

*未来将精简删除未使用的一些功能*

- 初始化

- 文件管理

- 线程

- 资源锁/信号量/互斥量 *将使用boost库重构*

- Log输出

- 数学函数 有些实现过于旧 需要修改

- 内存管理 将采用gperftools-2.15代替

- 基本对象/RTTI

- 游戏进程管理

- 智能指针

- 字符串

- 系统信息

- 配置文件读取  *配置文件将来可能统一使用更现代化的方式，如json等*    INI文件CSV文件XML文件文本配置程序

- 时间相关

- 算法 MD5 CRC LZW ZLIB DES ASE

#### 1.初始化

#### 2.文件管理

#### 3.线程

#### 4.资源锁/信号量/互斥量

#### 5.Log输出

#### 6.数学函数

#### 7.内存管理

##### 7.1内存安全指导

###### 内存越界知识

有时候程序莫名其妙地出现问题，但经常调试来调试去，都不知原因所在，大部分都是内存操作越界引起的。

```c++
//字符串数组越界
char str[10];
　 ::strcpy(str, "0123456789");
此时将会造成越界，因为strcpy将在str为起始地址的第11个单元写入’0’，
这样它将覆盖这个单元的值，对于程序来说，有可能带来灾难性的后果。
//以指针指向数组来进行操作
指针相对数组的下标操作来说，更灵活、更高效，但常常一不小心它就跑到不知地方的地方去了，在循环批量处理中，多移一位就产生了越界，如果是读还好，但如果是写，这就跟上面出现的问题一样了。
//将对数据类型的错误操作
short int x;
int y;
x=y
这种操作是不会出错，编译器会解释为 x=(short int)y;丢弃x的高位；但大家看看这个，
void *p
p=&x;
 *(int*)p=y;
x只有两个字节的空间，而int需要4个字节的空间, 
实际上己出问题了，但你放心，现在的编译器编译后不会引起错误，
因为由于内存对齐操作，实际上它分配了4个字节给x，你没有写入到其它数中去，运行会完全正常。
但当你定义的是数组时，那就会有问题了，它会覆盖你后面的数据。
或者你在不同的编程语言中进行数据交换时，而类型与类型宽度不一致就会出问题，
如VB中的Integer只有2个字节，而VC中的int是4个，当接口定义不够好时，那就会引发程序崩溃。
//使用new分配空间，但操作越界
这类越界要容易发现一点，只要你以debug版运行，只要你越界，程序肯定会在delete处停下来。同理malloc，free有此问题。
```

###### RT系列解决方案

基本思路
	1、接管或保护所有的内存分配，定期检查是否越界
	2、替换strn系列函数，并在内存操作越界后assert
	3、替换str系统函数，结合RT内存管理器，在内存操作越界后assert

使用方法
	首先，在程序公用头文件中定义ENABLE_OPERATOR_NEW宏，
	然后调用rt2_ActiveSafestr函数激活Safestr系列函数

内存分配方面
	堆内存
		全部使用RT_NEW分配动态内存，DEL_ONE和DEL_ARRAY释放内存
	全局内存
		使用G_MEMDEF宏进行声明
		所有操作前使用G_MEMPROTECTOR宏向RT内存管理器注册这块内存
	静态内存
		使用S_MEMDEF宏进行声明
		所有操作前使用S_MEMPROTECTOR宏向RT内存管理器注册这块内存
	类成员内存
		使用CM_MEMDEF宏进行声明
		在类构造函数中使用CM_MEMPROTECTOR宏向RT内存管理器注册这块内存
		在析构函数中使用CM_MEMUNPROTECTOR宏向RT内存管理器注销这块内存
	栈内存
		栈内存作为临时变量，由于生存期短，不由RT内存管理器保护，由堆栈保护技术保护

内存操作方面
	主要定义了以下6个操作函数

```c++
RT2_CORE_API char* rt2_strncpy(char *strDest,
		const char *strSource,
		size_t count );//count 为dest长度，不是要拷贝的长度
		
RT2_CORE_API char* rt2_strncat(char *strDest,
					 const char *strSource,
					 size_t count );
					 //count 为strDest总长度，不是本次要追加的长度
RT2_CORE_API int rt2_snprintf(char *buffer,
						  size_t count,
						  const char *format, ...);//count 为buffer总长度
/************************************************************************/
/* 以下函数用于替换老版不安全函数，在新代码中勿用                      */
/************************************************************************/
RT2_CORE_API char* rt2_strcpy(char *strDest,
							   const char *strSource);


RT2_CORE_API char* rt2_strcat(char *strDest,
							   const char *strSource);


RT2_CORE_API int rt2_sprintf(char *buffer,
							  const char *format, ...);
/*
新代码：新代码中请使用rt2_strn系列函数，这组函数可以直接提供内存越界发现并assert的能力，禁止使用系统的相关函数，需要特别注意的，所有的count参数，都是指的目标内存的总大小，和系统函数是不一样的

老代码:由于老代码中有大量如strcpy之类极不安全的函数，所以写了rt2_str系列函数替换，这个工作将在我们所有的内存都受到RT内存管理器保护之后进行。

替换指导:
我们在服务端和客户端应用者套库，并替换原有函数的指导如下：
首先、在工程中定义ENABLE_OPERATOR_NEW宏，并在程序启动时调用rt2_ActiveSafestr函数激活Safestr系列函数
（建议放在注册全局内存的函数里调用）
*/
```


4.1、内存登记

```
对于全局内存
在工程中搜索 char g_关键字，找到后将这些内存重新用G_MEMDEF声明
在内存未被使用前，用G_MEMPROTECTOR宏向RT内存管理器注册这块内存，建议写一个全局内存注册函数，在main函数刚进入的时候就把所有全局内存注册了
对于静态内存
在工程中搜索static char关键字，找到后将这些内存重新用S_MEMDEF声明
在内存未被使用前，也就是紧随声明之后，用S_MEMPROTECTOR宏向RT内存管理器注册这块内存
对于栈上的static char内存块，不需要理会。
对于类成员内存
在工程中搜索char m_关键字，找到后将这些内存重新用CM_MEMDEF声明
在类构造函数中使用CM_MEMPROTECTOR宏向RT内存管理器注册这块内存
对于需要在STL里保存引用的结构、类等，必须实现拷贝构造函数，在拷贝构造函数里使用CM_MEMPROTECTOR宏
在类析构函数中使用CM_MEMUNPROTECTOR宏向RT内存管理器注销这块内存
对于联合结构时：
注册联合里最长的内存块即可，不要把联合里全部的内存块都注册一遍。
以上几类宏都具有2号版本，对应2维数组的应用。
对于堆内存
确保工程中的堆内存分配都用了RT_NEW，也就是确保用RT_NEW替换new和malloc，用DEL_ONE和DEL_ARRAY释放内存，（delete和delete[]仍然可用，但不推荐）
对于栈上的临时内存
无需保护
对于遗漏登记的内存
对于遗漏登记的内存，在被rt2系列函数操作时会被识别出来并中断程序执行
```





4.2、内存操作

```c++
//原有strn系列函数的替换

//strncpy
用rt2_strncpy替换，因为接口兼容，只需查找替换即可，比较麻烦的是，原有strncpy的标准用法需要在buf末尾手动补0，例子如下：
strncpy(pRequisition->szSalesmanUsername, pUser->name.c_str(), 23);
pRequisition->szSalesmanUsername[23] = 0;//手动补0
而rt2_strncpy则会自动补0，故手动补0语句就不需要了，需要我们手动删除之

//strncat 
以rt2_strncat替换，因为接口兼容，只需查找替换即可，需要注意的是rt2_strncat的count参数是目标内存的总长度，strncat的count是本次追加字符串长度，好在strncat在我们的代码里基本没有用，似乎只有一处

//snprintf 
以rt2_snprintf替换snprintf函数，因为接口兼容，只需查找替换即可，另外，我们的代码中有人用了WIN32本地的_snprintf函数，也找出来替换掉

//原有str系列函数的替换
//strcpy
以rt2_strcpy替换strcpy函数，因为接口兼容，只需查找替换即可
//strcat
以rt2_strcat替换strcat函数，因为接口兼容，只需查找替换即可
//sprintf
以rt2_sprintf替换sprintf函数，因为接口兼容，只需查找替换即可
```

5、	服务器内存块新规范：

```
添加新的全局内存块时，需要把该内存块声明代码添加到GloableBuffer.cpp里，并在该cpp里的RegisterMemoryBlock(void)里添加注册内存块调用。
然后再实际使用的代码里extern这个内存块即可。
全局内存块不要使用static前缀。
成员内存块注意在构造函数里向管理器注册，和在析构函数里向管理器注销。
成员static内存块也和普通成员内存块一样在构造里注册，管理器收到第2次注册内存地址时不会出问题。
只要是类或结构成员的char xxx[x];形式，成员变量名必须以m_开头，不得随意命名。
S_MEMPROTECTOR用法：
S_MEMPROTECTOR(内存块首地址，内存块大小，随便一个唯一的新BOOL变量标示符)。
CM_MEMPROTECTOR用法：
CM_MEMPROTECTOR(内存块首地址，内存块大小)。
CM_MEMUNPROTECTOR用法:
CM_MEMUNPROTECTOR(内存块首地址)。
```



#### 8.基本对象/RTTI

#### 9.游戏进程管理

#### 10.智能指针

#### 11.字符串

#### 12.系统信息

#### 13.配置文件读取  

#### 14.时间相关

#### 15.算法