# cGPTerm

[![English badge](https://img.shields.io/badge/%E8%8B%B1%E6%96%87-English-blue)](https://github.com/Ace-Radom/cGPTerm/blob/main/README.md)
[![简体中文 badge](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-Simplified%20Chinese-blue)](https://github.com/Ace-Radom/cGPTerm/blob/main/README.zh-CN.md)
[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg)](https://github.com/RichardLitt/standard-readme)

<img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/cGPTerm/cmake.yml?branch=main" /> <img src="https://img.shields.io/badge/Platform-Linux-green" /> <img src="https://img.shields.io/github/license/Ace-Radom/cGPTerm">

This is a refactoring project of the original one [xiaoxx970/chatgpt-in-terminal](https://github.com/xiaoxx970/chatgpt-in-terminal) in C/C++.

cGPTerm enables chatting with ChatGPT in the terminal.

It supports for rendering markdown contents from ChatGPT's API responses.

Supports history retrieval with the up arrow key, stream output, and tokens counting.

Slash (/) commands are available in the chat box to toggle raw response mode, undo the last question and answer, modify the system prompt and more, see the available commands below for details.

It also supports saving chat messages to a JSON file and loading them from the file.

Uses the gpt-3.5-turbo model as default model, which is the same model used by ChatGPT (Free Edition).

You can also change API Host to configure proxy (See [Available Arguments](#available-arguments)).

## Preparation

An OpenAI API key. You need to register an OpenAI account and obtain an API key.

OpenAI's API key can be generated on the page opened by clicking "View API keys" in the upper right corner of the homepage, direct link: https://platform.openai.com/account/api-keys

![image-API_Key_Preparation](https://github.com/Ace-Radom/cGPTerm/blob/main/README.assets/image-API_Key_Preparation.png)

## Installation

You can download all EXE file and DLLs within a zip file in [Release](https://github.com/Ace-Radom/cGPTerm/releases), but you can also compile this project by yourself (Please see [Section Compilation](#compilation)).

Before or after downloading the release package, you should install all dependencies which are:

| Library | Library name with APT | Installation Command with APT |
| --- | --- | --- |
| curl | libcurl4 | `sudo apt install libcurl4` |
| argtable | libargtable2-0 | `sudo apt install libargtable2-0` |
| jansson | libjansson4 | `sudo apt install libjansson4` |
| readline | readline-common | `sudo apt install readline-common` |
| x11 | x11-common | `sudo apt install x11-common` |
| pcre2 | libpcre2-8-0 | `sudo apt install libpcre2-8-0` |

You need to check your Package manager to see how to install these libraries.

Then, unzip the package, run `./cgpterm` in the outcome folder. If all thing goes well, you should see cGPTerm start to work.

You can also add this folder to environment path, in order to run cGPTerm anywhere.

## Compilation

You need GCC and CMake to compile. Make sure that your GCC supports **C++20**.

1.  clone this repo with:

    ```
    git clone --recursive https://github.com/Ace-Radom/cGPTerm.git
    ```

2.  install all dependencies' dev packages

    | Library | Installation Command with APT |
    | --- | --- |
    | curl | `sudo apt install libcurl4-openssl-dev` |
    | argtable | `sudo apt install libargtable2-dev` |
    | jansson | `sudo apt install libjansson-dev` |
    | readline | `sudo apt install libreadline-dev` |
    | x11 | `sudo apt install libx11-dev` |
    | pcre2 | `sudo apt install libpcre2-dev` |

    Same, you need to check your own Package manager to see how to install these libraries, when you are not using apt.

3.  make build directory with:

    ```sh
    mkdir build
    cd build
    ```

4.  configure CMake with:

    ```
    cmake -DCMAKE_BUILD_TYPE=Release -DCLIP_EXAMPLES=OFF -DCLIP_TESTS=OFF ..
    ```

5.  build

    ```
    make
    ```

After that, you should see all EXE files, DLLs and static libraries under `$/bin`. You can run `./cgpterm` there and it should work.

## First launch

cGPTerm will automatic create log and config files under `~/.cgpterm` by the first launch.

You should configure your API Key with:

```
./cgpterm --set-apikey <YOUR_API_KEY>
```

Same as setting API Key, you don't need to open the config file (which is `~/.cgpterm/config.ini`) and edit it by yourself. cGPTerm provides a couple of commands to set all configs in config file. See [Available Arguments](#available-arguments) to learn more.

## How To Use

Run with the following command:

```
cgpterm
```

Use `Enter` to submit questions.

Here are some common shortcut keys (also shortcut keys for the shell):

- `Ctrl+L`: Clear screen, equivalent to `clear` command in shell
- `Ctrl+C`: Stop the current request
- `Tab`: Autocomplete commands or parameters
- `Ctrl+U`: Delete all characters to the left of the cursor
- `Ctrl+K`: Delete all characters to the right of the cursor
- `Ctrl+W`: Delete the word to the left of the cursor

> Original chat logs will be saved to `~/.cgpterm/chat.log`

## Available Arguments

cGPTerm provides a couple of arguments to control the software or set configs.

| Arguments | Description | Example |
| --- | --- | --- |
| -h, --help | Show Help Messages and exit | `cgpterm --help` |
| -r, --raw | Enable raw mode | `cgpterm --raw` |
| --load FILE | Load chat history from file | `cgpterm --load chat_history_code_check.json` |
| --set-host HOST | Set API Host to use | `cgpterm --set-host https://api.openai.com` |
| --set-apikey KEY | Set API Key for OpenAI | `cgpterm --set-apikey sk-....` |
| --set-timeout TIMEOUT | Set maximum waiting time for API requests | `cgpterm --set-timeout 40` |
| --set-gentitle | Set whether to automatically generate a title for chat | `cgpterm --set-gentitle` |
| --set-saveperfix PERFIX | Set chat history file's save perfix | `cgpterm --set-saveperfix chat_history_` |
| --set-loglevel LEVEL | Set log level: DEBUG, INFO, ERROR, FATAL | `cgpterm --set-loglevel DEBUG` |

> Multi-line mode and raw mode can be used simultaneously

## Configuration File

The configuration file is located at `~/.gpt-term/config.ini` and is autogenerated. It can be modified using the program's `--set` option or edited manually.

The default configuration is as follows:

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

## Available Slash Commands

- `/raw`: Switch response output style between raw and rendered Markdown format
    > After switching, use the `/last` command to reprint the last reply
- `/stream`: Toggle stream output mode
    > In stream mode, the answer will start outputting as soon as the first response arrives, which can reducing waiting time. Stream mode is on by default.
- `/title`: Toggle whether to enable automatic title generation
    > When automatic title generation is on, cGPTerm will generate a new title after the first conversation of the current chat.
- `/tokens`: Display the total tokens spent and the tokens for the current conversation
    > GPT-3.5 has a token limit of 4096; use this command to check if you're approaching the limit
- `/usage`: Show account credits summary
- `/timeout [new_timeout]`: Modify the API timeout
    > The default timeout is 30 seconds, it can also be configured by setting `OPENAI_API_TIMEOUT=` in the config file, or using `--set-timeout` argument.
- `/model [model_name]`: Change AI model
    > `gpt-3.5-turbo`, `gpt-4` and `gpt-4-32k` are supported.
- `/system [new_prompt]`: Modify the system prompt
- `/rand [randomness]`: Set Model sampling randomness (temperature in OpenAI official docs)
    > Default 1.
    > 
    > Given randomness should be a real between 0 and 2.
- `/save [filename_or_path]`: Save the chat history to the specified JSON file
    > If no filename or path is provided, the filename `<SAVE PERFIX>YEAR-MONTH-DAY_HOUR,MINUTE,SECOND.json` will be suggested on input.
- `/undo`: Delete the previous question and answer
- `/delete` or `/delete first`: Delete the first conversation in current chat
    > When the token is about to reach the upper limit, the user will be warned.
- `/delete all`: Clear current chat, delete all questions and responses
- `/last`: Display last ChatGPT's reply
- `/copy` or `/copy all`: Copy the full ChatGPT's last reply (raw) to Clipboard
- `/copy code`: Copy the code in ChatGPT's last reply to Clipboard
- `/version`: Show cGPTerm local and remote version
- `/list`: List all settings in use
- `/help`: Show Slash Commands' help page
- `/exit`: Exit the application

## Exit Words

You can submit Exit Words to exit cGPTerm. Exit words will be sent as a question to ChatGPT, and cGPTerm will exit after GPT replies.

Exit words include:

```
['再见', 'bye', 'goodbye', '结束', 'end', '退出', 'exit', 'quit']
```

Upon exit, the token count for the chat session will be showed.

## Third Party Components

- [dacap/clip](https://github.com/dacap/clip): For accessing the clipboard.
- [gh-markt/cpp-tiktoken](https://github.com/gh-markt/cpp-tiktoken): For tokens count when using stream mode.
- [Ace-Radom/cpprich](https://github.com/Ace-Radom/cpprich): For rich text output and Markdown render. (Rich Format library subproject under cGPTerm) 
- [ndevilla/iniparser](https://github.com/ndevilla/iniparser): For parsering ini config file.
- [jonhoo/pthread_pool](https://github.com/jonhoo/pthread_pool): For thread-pool support.

You can find their licenses in [THIRD_PARTY_LICENSE](https://github.com/Ace-Radom/cGPTerm/blob/main/THIRD_PARTY_LICENSE).

Sincere thanks to the developers of these projects.

## Contributing

If you want, you can always dive in. Feel free to open an issue, submit PRs or report bugs!

## License

This project is licensed under the [GPLv3](https://github.com/Ace-Radom/cGPTerm/blob/main/LICENSE) License after v1.0.0 release.

From v0.1.0 to v0.3.1 is licensed under the Apache-2.0 License.