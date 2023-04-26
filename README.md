# cGPTerm
Chat with GPT in Terminal

## 简介

这是一个将 @xiaoxx970 的使用Python实现的 [GPT-Term](https://github.com/xiaoxx970/chatgpt-in-terminal) 项目以C/C++实现的方式重构的计划

目标为用C++实现CLI界面交互 C实现后端 尽可能还原原版界面和各项功能 但在遇到还原功能困难的时候会选择用其他的实现方法替代

开发工作在Linux环境下进行 由于这个项目更多是出于好玩和C语言练习的目的而发起的 目前不会太考虑可移植性问题

~~不要问我为什么用C++做前端 问就是尝试用C做前端时候宽字符报错了一晚上~~

自现在起 main分支下是最近的功能实现版本 开发分支位于dev

### 项目进度

目前已经实现的功能：

- 自动创建config文件夹 位置为 `~/.cgpterm`
- 自动创建 `config.ini` 配置文件
- 通过命令行对 `config.ini` 内的值进行更改
- 自建log库 支持 `DEBUG` `INFO` `ERROR` `FATAL` 层级
- 基本的API访问：可以正常与ChatGPT交流
- 基本的交互界面
    - 在等待GPT回复时显示 `ChatGPT is thinking...`
    - 在等待回复时锁死终端 不回显并隐藏光标 在回复后解锁

目前正在开发的功能：

- 实现部分底层的斜杠命令：`/timeout` `/model` `/last` `/help` `/exit`
- tokens 统计 逐步实现 `/tokens` 命令
- credit 统计 逐步实现 `/usage` 命令
- `/copy` 命令 包括整个回答和代码段的拷贝

已经计划实现 但在研究实现方法的功能：

- 多行模式
- 绑定Tabs 实现斜杠命令自动补全
- 在CLI上的斜杠命令匹配提示
- 在输入未定义的斜杠命令时提示一个用户最有可能想输入的命令
- 守护线程和自动标题生成
- 聊天记录保存和加载

目前计划中 但不清楚能力是否允许实现的功能：

- 自建/封装富文本库 以一种类似于python的rich库的方式输出富文本
  
    目的是为了让界面更好看 ~~不然还能因为什么啊喂~~
- 自建段落式Markdown解析库 用于适配流式输出的文本解析【若找到更好的替代方法会取消】
- 流式输出

### 目前已知的问题

- 在Debian 11.6, zsh下测试中发现：中文无法正确退格（以确定为utf8编码）
  
    目前不清楚是程序问题还是zsh的问题 但我编写的极其简单的 `cin >> string` 遇到了一模一样的问题 若这个问题普遍存在将是个很头疼的事 但以我的能力可能是没什么办法 `setlocale` 什么的都试过了

### 项目依赖

| 依赖库名 | 开发用版本 | 库功能 | 安装命令
| --- | --- | --- | --- |
| `argtable` | 2.13 | 命令行参数解析 | `sudo apt install libargtable2-dev` |
| `curl4-openssl` | 7.74.0 | API对接 | `sudo apt install libcurl4-openssl-dev` |
| `jansson` | 2.13.1 | json解析 | `sudo apt install libjansson-dev` |