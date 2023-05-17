# cGPTerm

[![English badge](https://img.shields.io/badge/%E8%8B%B1%E6%96%87-English-blue)](https://github.com/Ace-Radom/cGPTerm/blob/main/README.md)
[![简体中文 badge](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-Simplified%20Chinese-blue)](https://github.com/Ace-Radom/cGPTerm/blob/main/README.zh-CN.md)
[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg)](https://github.com/RichardLitt/standard-readme)

<img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/cGPTerm/cmake.yml?branch=main" /> <img src="https://img.shields.io/badge/Platform-Linux-green" /> <img src="https://img.shields.io/github/license/Ace-Radom/cGPTerm">

cGPTerm是一个将 [xiaoxx970/chatgpt-in-terminal](https://github.com/xiaoxx970/chatgpt-in-terminal) 项目使用C/C++重构的计划。

cGPTerm 实现了在终端中与 ChatGPT 聊天。

支持渲染 ChatGPT API 响应中的 Markdown 内容。

支持通过上方向键检索历史记录、流式输出和tokens计数。

聊天框中可用斜杠命令(/)切换raw模式、撤消上一次问题和回答、修改系统 prompt 等，详情请参阅下面的[可用斜杠命令](#可用的斜杠命令)板块。

支持将聊天消息保存到 JSON 文件并从文件中加载。

默认AI模型为 `gpt-3.5-turbo`，这是 ChatGPT (免费版) 使用的模型。

还可以更改 API Host 来配置代理 (请参阅[可用参数](#available-arguments)板块)。

## 准备工作

cGPTerm需要一个 OpenAI API 密钥。您需要注册 OpenAI 帐户并获得 API 密钥。

在首页右上角单击 "View API keys" 打开的页面上可以生成 OpenAI 的 API 密钥，直链：https://platform.openai.com/account/api-keys

![image-API_Key_Preparation](https://github.com/Ace-Radom/cGPTerm/blob/main/README.assets/image-API_Key_Preparation.png)

## 安装

您可以在 [Release](https://github.com/Ace-Radom/cGPTerm/releases) 中下载带有所有可执行文件和动态链接库的压缩包，也可以自己编译此项目 (请参见[编译](#编译)板块)。

在下载压缩包后，您需要安装所有依赖项，其中包括：

| 库 | APT 库名 | 通过 APT 安装的命令 |
| --- | --- | --- |
| curl | libcurl4 | `sudo apt install libcurl4` |
| argtable | libargtable2-0 | `sudo apt install libargtable2-0` |
| jansson | libjansson4 | `sudo apt install libjansson4` |
| readline | readline-common | `sudo apt install readline-common` |
| x11 | x11-common | `sudo apt install x11-common` |
| pcre2 | libpcre2-8-0 | `sudo apt install libpcre2-8-0` |

如果您使用的包管理器不是 apt，则您需要自行检查包管理器内的库名以查看如何安装这些依赖项。

然后，解压压缩包，在解压出的文件夹内运行 `./cgpterm` 。如果一切顺利，您应该会看到 cGPTerm 开始工作。

您还可以将此文件夹添加到环境变量中，以便在任何地方运行 cGPTerm。

## 编译

-------

**注意：** 所有开发工作是在 Debian 下使用 gcc 10.2.1 完成的。目前我已经发现了在使用别的 gcc 版本时可能出现在 ChatGPT 回复后出现段错误的情况。同时，在使用 cmake 的 release 预设时也会出现无法正常接受处理 SSE Event 的情况但我对这些问题毫无头绪。因此我建议在这个问题被解决前直接使用已经测试可用的编译好的包。

我在编译时的设置为:

```cmake
set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "-Wall -O2")
```

您可以自行编辑 CMakeLists 内的内容。

-------

您需要 GCC 和 CMake 来编译。请确保您的 GCC 支持 **C++20**。

1. 使用以下命令克隆此 repo：

    ```
    git clone --recursive https://github.com/Ace-Radom/cGPTerm.git
    ```

2. 安装所有依赖项的 dev 包

    | 库 | 通过 APT 安装的命令 |
    | --- | --- |
    | curl | `sudo apt install libcurl4-openssl-dev` |
    | argtable | `sudo apt install libargtable2-dev` |
    | jansson | `sudo apt install libjansson-dev` |
    | readline | `sudo apt install libreadline-dev` |
    | x11 | `sudo apt install libx11-dev` |
    | pcre2 | `sudo apt install libpcre2-dev` |

    同样，若您不使用 apt，您需要检查自己的包管理器以查看如何安装这些库。

3. 创建 build 目录

    ```sh
    mkdir build
    cd build
    ```

4. 配置 CMake

    ```
    cmake -DCLIP_EXAMPLES=OFF -DCLIP_TESTS=OFF ..
    ```

5. 编译
    
    ```
    make
    ```

在编译完成后，所有输出的文件（包括可执行文件，动态链接库和静态链接库）都会被输出到项目根目录下的 `bin` 文件夹。您可以从那里启动 cgpterm。

## 首次启动

在首次启动时，cGPTerm 会自动在 `~/.cgpterm` 下创建日志文件和配置文件。

您应该这样配置您的 API Key：

```
./cgpterm --set-apikey <YOUR_API_KEY>
```

和配置 API Key 一样，您无需进入配置文件夹并打开配置文件进行手动修改。cGPTerm 提供了一组用于修改配置文件的参数，参见[可用参数](#可用参数)板块

## 如何使用

您可以使用如下命令启动：

```
cgpterm
```

使用回车键来提交问题。

以下是一些常见的快捷键（同时也是shell的快捷键）：

- `Ctrl+L`: 清屏
- `Ctrl+C`: 停止当前的聊天请求
- `Ctrl+D`: 退出 cGPTerm，或是取消当前的设置
- `Tab`: 自动补全斜杠命令
- `Ctrl+U`: 删除光标左侧的所有字符
- `Ctrl+K`: 删除光标右侧的所有字符
- `Ctrl+W`: 删除光标左侧的单词

> 原始格式的对话记录会存至 `~/.cgpterm/chat.log`

### cGPTerm 更新器

本项目也提供了一个单独的更新器用于从 GitHub 拉取最新版本的发布包并自动安装。自 v1.1.0 版本起，您可以使用更新器更新本地 cGPTerm 版本（之前的版本因为发布包的原因无法支持）。

您可以使用如下命令更新：

```
cgpterm-update
```

更新器会先检查本地版本是否过时。若确实如此，它便会尝试从 GitHub 下载最新的发布包并安装。

您应该将更新器和 cGPTerm 的别的安装文件放在同一文件夹下。否则更新器将无法找到 cGPTerm 的其他文件并会询问是否想要安装 cGPTerm。

更新器需要 `tar` 来解压发布包。您可以使用 `which tar` 命令来检查它是否已经被安装了。

> 若您使用的是 v1.1.0 之前的版本，您可以先删除所有本地 cGPTerm 的安装文件（不包括配置文件），并使用更新器安装新版本。

## 可用参数

cGPTerm提供了一些参数来控制软件或进行配置。

| 参数 | 功能 | 示例 |
| --- | --- | --- |
| -h, --help | 显示此帮助信息并退出 | `cgpterm --help` |
| -r, --raw | 启用raw模式 | `cgpterm --raw` |
| --load FILE | 从文件中加载聊天记录 | `cgpterm --load chat_history_code_check.json` |
| --set-host HOST | 设置 API Host | `cgpterm --set-host https://api.openai.com` |
| --set-apikey KEY | 设置 OpenAI 的 API 密钥 | `cgpterm --set-apikey sk-....` |
| --set-timeout TIMEOUT | 设置 API 请求的超时时间 | `cgpterm --set-timeout 40` |
| --set-gentitle | 设置是否为聊天自动生成标题 | `cgpterm --set-gentitle` |
| --set-saveperfix PERFIX | 设置聊天历史文件的保存前缀 | `cgpterm --set-saveperfix chat_history_` |
| --set-loglevel LEVEL | 设置日志级别: DEBUG, INFO, ERROR, FATAL | `cgpterm --set-loglevel DEBUG` |

## 配置文件

配置文件位于 `~/cgpterm/config.ini` 并应该在首次启动时自动生成。您可以使用 `--set` 参数来设置或直接更改。

默认的日志文件应为：

```ini config.ini
[DEFAULT]
# API Host, use for configuring proxy
# It must in format like "https://xxx.xxxxxx.xxx", no slash at the end is needed or allowed
OPENAI_HOST=https://api.openai.com

# API key for OpenAI
OPENAI_API_KEY=

# The maximum waiting time for API requests, the default is 30s
OPENAI_API_TIMEOUT=30

# Whether to automatically generate titles for conversations, enabled by default (generating titles will consume a small amount of tokens)
AUTO_GENERATE_TITLE=True

# Define the default file prefix when the /save command saves the chat history. The default value is "./chat_history_", which means that the chat history will be saved in the file starting with "chat_history_" in the current directory
# At the same time, the prefix can also be specified as a directory + / to allow the program to save the chat history in a folder (note that the corresponding folder needs to be created in advance), for example: CHAT_SAVE_PERFIX=chat_history/
CHAT_SAVE_PERFIX=./chat_history_

# Log level, default is INFO, available value: DEBUG, INFO, ERROR, FATAL
LOG_LEVEL=INFO
```

## 可用的斜杠命令

- `/raw`：切换输出 raw 文本或是经过 markdown 渲染的文本。

    > 在切换后，您可以使用 `/last` 命令重新打印上一条回复。

- `/stream`：切换是否启用流式输出。

    > 在流式传输模式下，回复将在客户端收到第一部分回应后开始逐字输出，减少等待时间。流式传输默认为开启。

- `/title`：切换是否启用自动标题生成。

    > 当启用时，cGPTerm 会在当前聊天的第一次对话后生成一个新标题。

- `/tokens`：显示已消耗的 token 数统计和本次对话的 token 长度。

    > GPT-3.5 的对话 tokens 限制为4096，可通过此命令实时查看是否接近限制。

- `/usage`：显示当前 API 账号的余额。

- `/timeout [new_timeout]`：修改 API 超时时间。

    > 默认超时时间为30秒。除了使用 `/timeout` 命令修改，您也可以使用 `--set-timeout` 参数或是修改配置文件内 `OPENAI_API_TIMEOUT` 项的值来更改超时时间。

- `/model [model_name]`：切换使用的 AI 模型。

    > 支持 `gpt-3.5-turbo`，`gpt-4` 和 `gpt-4-32k`。

- `/system [new_prompt]`：修改系统 prompt。

- `/rand [randomness]`：设置模型的采样率（在 OpenAI 的官方文档内为 `temperature`）。

    > 采样率默认为1，并应是一个介于0和2之间的浮点数。

- `/save [filename_or_path]`：保存当前聊天记录到一个 JSON 文件内。

    > 若没有提供文件名或路径，cGPTerm 将会以 `<SAVE PERFIX>YEAR-MONTH-DAY_HOUR,MINUTE,SECOND.json` 的格式生成一个标题并显示在屏幕上。当然您在这之后还是可以手动编辑保存名的。

- `/undo`：删除上一条提问和回答。

- `/delete` 或 `/delete first`：删除当前聊天的第一次对话。

- `/delete all`：清除当前聊天记录，删除所有问答。

- `/last`：显示当前最后一条回复。

- `/copy` 或 `/copy all`：将 ChatGPT 的上一条回复的 raw 全文复制到剪切板中。

- `/version`：显示本地版本和远程版本。

- `/list`：显示当前所有设置配置项的值。

- `/help`：显示斜杠命令的帮助页。

- `/exit`：退出 cGPTerm。

> 当你进入设置后你可以使用 `Ctrl_D` 来退出设置。

## 退出词

您可以使用退出词来关闭 cGPTerm。退出词会被作为一个请求发送给 ChatGPT，在 GPT 回复后便会关闭程序。

退出词包括：

```
['再见', 'bye', 'goodbye', '结束', 'end', '退出', 'exit', 'quit']
```

您也可以使用 `/exit` 命令或是 `Ctrl_D` 直接退出。

在退出时会显示本次运行间的总 tokens 消耗统计。

## 第三方组件

- [dacap/clip](https://github.com/dacap/clip): 剪切板支持。
- [gh-markt/cpp-tiktoken](https://github.com/gh-markt/cpp-tiktoken): 在流式输出启用时的 tokens 统计。
- [Ace-Radom/cpprich](https://github.com/Ace-Radom/cpprich): 富文本输出和 Markdown 解析渲染器。（这是 cGPTerm 的富文本库子项目） 
- [ndevilla/iniparser](https://github.com/ndevilla/iniparser): 配置文件解析。
- [jonhoo/pthread_pool](https://github.com/jonhoo/pthread_pool): 线程池支持。

你可以在 [THIRD_PARTY_LICENSE](https://github.com/Ace-Radom/cGPTerm/blob/main/THIRD_PARTY_LICENSE) 中找到这些第三方组件的许可证。

在此对这些项目的开发者致以最诚挚的谢意。

## 许可证

自 v1.0.0 起，本项目将遵循 [GPLv3 许可证](https://github.com/Ace-Radom/cGPTerm/blob/main/LICENSE)开源。

在 v0.1.0 和 v0.3.1 之间则遵循 Apache-2.0 许可证开源。