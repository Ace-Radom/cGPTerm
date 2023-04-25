# cGPTerm
Chat with GPT in Terminal

## 简介

这是一个将 @xiaoxx970 的使用Python实现的 [GPT-Term](https://github.com/xiaoxx970/chatgpt-in-terminal) 项目以C/C++实现的方式重构的计划

目标为用C++实现CLI界面交互 C实现后端 尽可能还原原版界面和各项功能

开发工作在Linux环境下进行 由于这个项目更多是出于好玩和C语言练习的目的而发起的 目前不会太考虑可移植性问题

**Still in Development, NO Function achieved yet**

### 项目依赖

| 依赖库名 | 开发用版本 | 库功能 | 安装命令
| --- | --- | --- | --- |
| `argtable` | 2.13 | 命令行参数解析 | `sudo apt install libargtable2-dev` |
| `curl4-openssl` | 7.74.0 | API对接 | `sudo apt install libcurl4-openssl-dev` |
