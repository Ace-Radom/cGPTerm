# cGPTerm
Chat with GPT in Terminal

<img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/cGPTerm/cmake.yml?branch=main" /> <img src="https://img.shields.io/github/languages/top/Ace-Radom/cGPTerm"> <img src="https://img.shields.io/github/license/Ace-Radom/cGPTerm"> <img src="https://img.shields.io/badge/Platform-Linux-green" />

### README已经冻结准备1.0发布 目前进度请参考TODO.md

## 简介

这是一个将 @xiaoxx970 的使用Python实现的 [GPT-Term](https://github.com/xiaoxx970/chatgpt-in-terminal) 项目以近乎纯C实现的方式重构的计划

目标为用近乎纯C实现CLI界面交互和后端 尽可能还原原版界面和各项功能 但在遇到还原功能困难的时候会选择用其他的实现方法替代【比如tokens计算使用了Rust现成的库 也因此编译需要Rust环境】

开发工作在Linux环境下进行 由于这个项目更多是出于好玩和C语言练习的目的而发起的 目前不会太考虑可移植性问题

自现在起 main分支下是最近的功能实现版本 开发分支位于dev

### 项目进度

目前已经实现的功能：

- 自动创建config文件夹 位置为 `~/.cgpterm`
- 自动创建 `config.ini` 配置文件
- 通过命令行对 `config.ini` 内的值进行更改
- 自建log库 支持 `DEBUG` `INFO` `ERROR` `FATAL` 层级
- 基本的API访问：可以正常与ChatGPT交流和解析错误
- 基本的交互界面
    - 在等待GPT回复时显示 `ChatGPT is thinking...`
    - 历史记录功能 记录在本次运行中的每一次输入
    - 已经实现的斜杠命令可以按Tab补全【此处的补全逻辑和原版不同 在有多项可能时不会补全而是显示所有可能命令】
- 基于 [tiktoken-rs](https://github.com/zurawiki/tiktoken-rs) 的tokens计算
- 部分斜杠命令 (`/tokens` `/timeout` `/help` `/exit`)

目前正在开发的功能：

- 实现部分底层的斜杠命令：`/timeout` `/model` `/last` `/help` `/exit`
- credit 统计 逐步实现 `/usage` 命令
- `/copy` 命令 包括整个回答和代码段的拷贝
- 优化斜杠命令自动补全的界面
- 在输入未定义的斜杠命令时提示一个用户最有可能想输入的命令
- 自建/封装富文本库 以一种类似于python的rich库的方式输出富文本

已经计划实现 但在研究实现方法的功能：

- 多行模式
- 守护线程和自动标题生成
- 聊天记录保存和加载

目前计划中 但不清楚能力是否允许实现的功能：

- 自建段落式Markdown解析库 用于适配流式输出的文本解析【若找到更好的替代方法会取消】
- 流式输出
- 使用C/C++实现一套tiktoken分词库 替代目前使用的Rust分词库库

### 编译 项目依赖

编译需要GCC和Rust环境

首先 clone本仓库

```
git clone https://github.com/Ace-Radom/cGPTerm
```

然后 安装所有C的依赖项

| 依赖库名 | 开发用版本 | 库功能 | 安装命令 (Debian, Ubuntu) |
| --- | --- | --- | --- |
| `argtable` | 2.13 | 命令行参数解析 | `sudo apt install libargtable2-dev` |
| `curl4-openssl` | 7.74.0 | API对接 | `sudo apt install libcurl4-openssl-dev` |
| `jansson` | 2.13.1 | json解析 | `sudo apt install libjansson-dev` |
| `GNU readline` | 8.1 | 命令行输入 历史记录 Tab补全 | `sudo apt install libreadline-dev` |

随后 依次执行以下命令

```shell
mkdir build && cd build
cmake ..
make
```

如果一切正常 所有生成的文件都会被存放在项目根文件夹下的bin文件夹内

不过此处编译的是Debug版本 若需编译Release版本可以使用

```
cmake -DCMAKE_BUILD_TYPE=Release .. 
make
```
