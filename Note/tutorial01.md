# Tutorial01 学习笔记

## 1 | 搭建编译环境

### 代码文件

1. leptjson.h：leptjson 的头文件（header file），含有对外的类型和 API 函数声明。
2. leptjson.c：leptjson 的实现文件（implementation file），含有内部的类型声明和函数实现。此文件会编译成库。
3. test.c：我们使用测试驱动开发（test driven development, TDD）。此文件包含测试程序，需要链接 leptjson 库。

## Cmake

我这里**使用Clion**

A: CLion与CMake构建系统完全集成，可以无缝的创建，打开构建及运行，调试CMake项目。 CMake本身捆绑在CLion中，因此除非决定使用自定义版本，否则无需单独安装它。

## Json语法子集

下面是此单元的 JSON 语法子集，使用 [RFC7159](https://link.zhihu.com/?target=http%3A//rfc7159.net/rfc7159) 中的 [ABNF](https://link.zhihu.com/?target=https%3A//tools.ietf.org/html/rfc5234) 表示：

```text
JSON-text = ws value ws
ws = *(%x20 / %x09 / %x0A / %x0D)
value = null / false / true 
null  = "null"
false = "false"
true  = "true"
```

当中 %xhh 表示以 16 进制表示的字符，/ 是多选一，* 是零或多个，( ) 用于分组。

那么第一行的意思是，JSON 文本由 3 部分组成，首先是空白（whitespace），接着是一个值，最后是空白。

第二行告诉我们，所谓空白，是由零或多个空格符（space U+0020）、制表符（tab U+0009）、换行符（LF U+000A）、回车符（CR U+000D）所组成。

第三行是说，我们现时的值只可以是 null、false 或 true，它们分别有对应的字面值（literal）。

我们的解析器应能判断输入是否一个合法的 JSON。如果输入的 JSON 不合符这个语法，我们要产生对应的错误码，方便使用者追查问题。

在这个 JSON 语法子集下，我们定义 **3 种错误码**：

- 若一个 JSON 只含有空白，传回 LEPT_PARSE_EXPECT_VALUE。
- 若一个值之后，在空白之后还有其他字符，传回 LEPT_PARSE_ROOT_NOT_SINGULAR。
- 若值不是那三种字面值，传回 LEPT_PARSE_INVALID_VALUE。

## 单元测试

`test.c`中包含了一个极简的单元测试框架

