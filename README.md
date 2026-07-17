# Orange NFS MusicEd UI By: Shynian

Need for Speed Carbon/MostWanted Music Editor
Author: Shynian (信源)

<img width="256" height="256" alt="atgki-szyi5" src="https://github.com/user-attachments/assets/6abb763f-9d60-4539-9dd7-bf9213a87f4d" />

##### A special thank you to **[YAMADA RYO](https://github.com/orgs/CARBON-Plus-Team/people/LEE-YAMADARYO)**. He’s someone I deeply cherish, and I’m so grateful for his help with ONME. 
##### 我想特别感谢一下 **[YAMADA RYO](https://github.com/orgs/CARBON-Plus-Team/people/LEE-YAMADARYO)**。他是我所珍视的人，也非常感谢他在 ONME​ 项目上的鼎力相助。

---

# English Version

## Workflow Guide📕
Follow these steps to extract, edit, and repack music files:

##### Step 1: Extract ASF from MUS 　　Click Select MUS File to choose your .mus file. The software will automatically find the matching .mpf file. Then click Extract ASF to unpack all audio files from the MUS archive.

##### Step 2: Convert ASF to WAV 　　In the Extracted Files panel, click Convert All to WAV to convert all ASF files to WAV format. You can then play, edit, or replace WAV files with your own music.

##### Step 3: Replace Music (optional) 　　Select a WAV file in the list, click Replace Music, and choose your own WAV file (44100 Hz, 16-bit recommended). The selected file will be replaced.

##### Step 4: Convert WAV back to ASF 　　After editing, click Convert to ASF to convert all WAV files back to ASF format. Make sure all files are converted before packing.

##### Step 5: Pack back to MUS 　　Click Convert to MUS to repack all ASF files back into MUS/MPF format. The original MUS/MPF files will be automatically overwritten. Before overwriting, the original files are backed up with timestamps to the MusBackUp folder located in the same directory as your MUS/MPF files.

##### Note: Original MUS/MPF files are automatically backed up to the MusBackUp folder (located in the same folder as your MUS/MPF files) before being overwritten. Backups are timestamped (yyyyMMdd_hhmmss format) so multiple versions are preserved. If backup fails due to permission issues, the overwrite will still proceed.

## Programming Languages

- **C++** (C++17 standard) — Primary application language

## UI Framework & Libraries

| Library | Version | License | Purpose |
|---------|---------|---------|---------|
| Qt | 6.10.1 | LGPLv3 / GPLv2 / GPLv3 | Cross-platform UI framework |
| Qt Core | 6.10.1 | LGPLv3 | Core non-GUI functionality |
| Qt GUI | 6.10.1 | LGPLv3 | GUI infrastructure |
| Qt Widgets | 6.10.1 | LGPLv3 | Widget-based UI components |
| Qt Multimedia | 6.10.1 | LGPLv3 | Audio playback engine |
| Qt Network | 6.10.1 | LGPLv3 | Network support |
| Qt SVG | 6.10.1 | LGPLv3 | SVG icon rendering |
| FFmpeg | 6.1 | LGPL 2.1+ / GPL | Audio format conversion (fallback) |
| Microsoft Visual C++ Runtime | 14.x (VS 2022) | Microsoft EULA | C++ standard library runtime |

## Qt Modules Used

- Qt6Core
- Qt6Gui
- Qt6Widgets
- Qt6Multimedia
- Qt6MultimediaWidgets
- Qt6Network
- Qt6Svg

## External Command-Line Tools

| Tool | Source | License | Purpose |
|------|--------|---------|---------|
| **MPFmaster.exe** | [PFDataCompiler](https://github.com/bluesky-dev12/PFDataCompiler) by BlueSkyWestSide, originally by [xan1242](https://github.com/xan1242/MPFmaster) | MIT License | MPF/MUS music pack decompilation/compilation |
| **MPFmaster_GenerateEvent.exe** | [PFDataCompiler](https://github.com/bluesky-dev12/PFDataCompiler) by BlueSkyWestSide | MIT License | Event ID generation for music tracks |
| **sx.exe** | Legacy audio tool (NHL 07-era sound exchange) | Freeware | ASF <-> WAV audio format conversion (EA XA codec) |
| **ffmpeg.exe** | [FFmpeg](https://ffmpeg.org/) | LGPL 2.1+ | Fallback audio conversion and WAV format adjustment |

## Build Toolchain

- **Build System**: qmake (Qt 6.10.1)
- **Compiler**: Microsoft Visual C++ (MSVC) 2022 (v14.44)
- **Windows SDK**: 10.0.26100.0
- **Installer**: [Inno Setup](https://jrsoftware.org/isinfo.php) 6.7.3
- **Deployment**: windeployqt (Qt deployment tool)
- **Platform**: Windows x64 (64-bit only)

## Windows API Integration

- Win32 API (`user32.lib`) — Single-instance application mutex
- UAC manifest — Administrator privilege elevation
- Win32 subsystem linker flag (`/SUBSYSTEM:WINDOWS`) — Hidden console window

## File Formats Supported

| Format | Description | Direction |
|--------|-------------|-----------|
| MUS | NFS Carbon music package container | Read/Write |
| MPF | NFS Carbon music patch file | Read/Write |
| ASF | EA SCHL/XA compressed audio stream | Read/Write |
| WAV | Uncompressed PCM audio (44100Hz, 16-bit) | Read/Write |

---

## Acknowledgments

This software stands on the shoulders of the following projects and individuals who made NFSC/MW music modding possible:

### PFDataCompiler Contributors

- **[xan1242](https://github.com/xan1242)** — Original author of MPFmaster, the foundational tool for NFS music pack manipulation. Without xan1242's reverse engineering work, none of this would be possible.
- **[BlueSkyWestSide (bluesky-dev12)](https://github.com/bluesky-dev12/PFDataCompiler)** — Maintainer of PFDataCompiler, compilation fixes, event ID generation, and making the tool compile and work for Carbon.
- **Hickle** — Assistance with tool development.
- **Nicknine** — mpf2txt.py / mpftotext.py utilities for MPF inspection.
- **Pasta** — MPF Tutorial for the Reformed modding community.
- **Shad** — Help simplifying the modding tutorial.

### Qt Framework

- The Qt Company for the [Qt framework](https://www.qt.io/) — an exceptional cross-platform application framework that made building this UI possible.

### FFmpeg

- The [FFmpeg](https://ffmpeg.org/) project for the industry-standard multimedia processing toolkit used as a fallback audio converter.

### Third-Party Tools

- The unknown authors of **sx.exe** (Sound eXchange / EA XA converter).

### Community

- The **BlackBox Need For Speed community** for keeping Need for Speed alive through decades of modding passion.

---

## License

### Fan-Tool Disclaimer

This software is a fan-made tool for creating modifications ("mods") for the game Need for Speed. It is NOT affiliated with, endorsed by, or approved by Electronic Arts Inc. (EA), EA Canada, EA Black Box, or any of their subsidiaries or partners. All trademarks referenced — including Need for Speed, NFS Carbon, EA, Black Box, and any associated logos — are the property of their respective owners.

Users are responsible for ensuring they legally own a copy of Need for Speed: Carbon before using this tool to modify game files.

### Risk Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED. THE USER ASSUMES THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE, INCLUDING BUT NOT LIMITED TO SAVE FILE CORRUPTION, DATA LOSS, AND SYSTEM MALFUNCTION. IN NO EVENT SHALL THE DEVELOPER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, CONSEQUENTIAL, SPECIAL, EXEMPLARY, OR PUNITIVE DAMAGES (INCLUDING, BUT NOT LIMITED TO, LOSS OF PROFITS OR DATA RECOVERY COSTS) ARISING FROM THE USE OF OR INABILITY TO USE THIS SOFTWARE. CONTINUED USE CONSTITUTES AN EXPRESS WAIVER OF ANY CLAIM FOR PUNITIVE DAMAGES.

### Third-Party Licenses

- **MPFmaster.exe** and **MPFmaster_GenerateEvent.exe** are distributed under the MIT License (Copyright (c) 2022 Lovro Plese / xan1242, with modifications by BlueSkyWestSide). The full MIT license text is included below.
- **FFmpeg** is distributed under the GNU Lesser General Public License (LGPL) version 2.1 or later. FFmpeg binaries included in this distribution are unmodified. Source code for FFmpeg is available at https://ffmpeg.org/download.html.

#### MIT License (PFDataCompiler / MPFmaster)

```
MIT License

Copyright (c) 2022 Lovro Plese

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---
---

# 中文版本


## 操作流程指南📕

##### 按照以下步骤提取、编辑和重新打包音乐文件：

##### 第一步：从 MUS 中提取 ASF 　　点击"选择 MUS 文件"来选择您的 .mus 文件。软件会自动找到匹配的 .mpf 文件。然后点击"提取 ASF"来从 MUS 存档中解包所有音频文件。

##### 第二步：将 ASF 转换为 WAV 　　在"提取的文件"面板中，点击"全部转换为 WAV"将所有 ASF 文件转换为 WAV 格式。然后您可以播放、编辑或用您自己的音乐替换 WAV 文件。

##### 第三步：替换音乐（可选） 　　在列表中选择一个 WAV 文件，点击"替换音乐"，然后选择您自己的 WAV 文件（推荐 44100 Hz、16 位）。选中的文件将被替换。

##### 第四步：将 WAV 转换回 ASF 　　编辑完成后，点击"转换为 ASF"将所有 WAV 文件转换回 ASF 格式。确保在打包前所有文件都已转换。

##### 第五步：打包回 MUS 　　点击"转换为 MUS"将所有 ASF 文件重新打包为 MUS/MPF 格式。原始 MUS/MPF 文件将被自动覆盖。覆盖前，原始文件会以时间戳命名备份到 MUS/MPF 文件所在目录下的 MusBackUp 文件夹中。

##### 注意： 原始 MUS/MPF 文件在被覆盖前会自动备份到 MusBackUp 文件夹（位于 MUS/MPF 文件所在的同一目录下）。备份文件带有时间戳（格式为 yyyyMMdd_hhmmss），可以保留多个版本。如果因权限问题备份失败，覆盖操作仍会继续执行。


## 编程语言

- **C++**（C++17 标准）— 主应用程序语言

## UI 框架与支持库

| 库名称 | 版本 | 许可证 | 用途 |
|--------|------|--------|------|
| Qt | 6.10.1 | LGPLv3 / GPLv2 / GPLv3 | 跨平台 UI 框架 |
| Qt Core | 6.10.1 | LGPLv3 | 核心非 GUI 功能 |
| Qt GUI | 6.10.1 | LGPLv3 | GUI 基础架构 |
| Qt Widgets | 6.10.1 | LGPLv3 | 基于控件的 UI 组件 |
| Qt Multimedia | 6.10.1 | LGPLv3 | 音频播放引擎 |
| Qt Network | 6.10.1 | LGPLv3 | 网络支持 |
| Qt SVG | 6.10.1 | LGPLv3 | SVG 图标渲染 |
| FFmpeg | 6.1 | LGPL 2.1+ / GPL | 音频格式转换（备用） |
| Microsoft Visual C++ 运行时 | 14.x (VS 2022) | Microsoft EULA | C++ 标准库运行时 |

## 使用的 Qt 模块

- Qt6Core
- Qt6Gui
- Qt6Widgets
- Qt6Multimedia
- Qt6MultimediaWidgets
- Qt6Network
- Qt6Svg

## 外部命令行工具

| 工具 | 来源 | 许可证 | 用途 |
|------|------|--------|------|
| **MPFmaster.exe** | [PFDataCompiler](https://github.com/bluesky-dev12/PFDataCompiler) 作者 BlueSkyWestSide，原作者 [xan1242](https://github.com/xan1242/MPFmaster) | MIT 许可证 | MPF/MUS 音乐包解包与打包 |
| **MPFmaster_GenerateEvent.exe** | [PFDataCompiler](https://github.com/bluesky-dev12/PFDataCompiler) 作者 BlueSkyWestSide | MIT 许可证 | 音乐轨道 Event ID 生成 |
| **sx.exe** | 传统音频工具（NHL 07 时代声音转换工具） | 免费软件 | ASF ↔ WAV 音频格式转换（EA XA 编解码器） |
| **ffmpeg.exe** | [FFmpeg](https://ffmpeg.org/) | LGPL 2.1+ | 备用音频转换与 WAV 格式调整 |

## 构建工具链

- **构建系统**：qmake（Qt 6.10.1）
- **编译器**：Microsoft Visual C++ (MSVC) 2022 (v14.44)
- **Windows SDK**：10.0.26100.0
- **安装包制作**：[Inno Setup](https://jrsoftware.org/isinfo.php) 6.7.3
- **部署工具**：windeployqt（Qt 部署工具）
- **目标平台**：Windows x64（仅 64 位）

## Windows API 集成

- Win32 API（`user32.lib`）— 单实例应用程序互斥锁
- UAC 清单文件 — 管理员权限提升
- Win32 子系统链接标志（`/SUBSYSTEM:WINDOWS`）— 隐藏控制台窗口

## 支持的文件格式

| 格式 | 描述 | 读写 |
|------|------|------|
| MUS | NFS Carbon 音乐包容器文件 | 读/写 |
| MPF | NFS Carbon 音乐补丁文件 | 读/写 |
| ASF | EA SCHL/XA 压缩音频流 | 读/写 |
| WAV | 未压缩 PCM 音频（44100Hz, 16-bit） | 读/写 |

---

## 鸣谢

本软件的诞生离不开以下项目和个人的贡献，是他们让 NFSC/MW 的音乐 Modding 成为可能：

### PFDataCompiler 贡献者

- **[xan1242](https://github.com/xan1242)** — MPFmaster 的原作者，NFS 音乐包处理的奠基工具。没有 xan1242 的逆向工程工作，本软件不可能存在。
- **[BlueSkyWestSide (bluesky-dev12)](https://github.com/bluesky-dev12/PFDataCompiler)** — PFDataCompiler 的维护者，修复编译问题，添加 Event ID 生成功能，使工具能够为 Carbon 编译和工作。
- **Hickle** — 工具开发协助。
- **Nicknine** — mpf2txt.py / mpftotext.py MPF 查看工具的作者。
- **Pasta** — Reformed Modding 社区的 MPF 教程。
- **Shad** — 帮助简化 Modding 教程。

### Qt 框架

- The Qt Company 的 [Qt 框架](https://www.qt.io/) — 卓越的跨平台应用程序框架，使构建此 UI 成为可能。

### FFmpeg

- [FFmpeg](https://ffmpeg.org/) 项目提供了业界标准的多媒体处理工具包，作为备用音频转换器。

### 第三方工具

- **sx.exe**（Sound eXchange / EA XA 转换器）。

### 社区

- **BlackBox Need For Speed 社区**，数十年的 Modding 热情让极品飞车 Carbon 保持活力。

---

## 许可

### 粉丝工具免责声明

本软件是为极品飞车（Need for Speed）游戏创建修改（Mod）的非官方粉丝工具。本软件与 Electronic Arts Inc.（EA）、EA Canada、EA Black Box 或其任何子公司/合作伙伴无任何关联，也不受其认可或批准。所有提及的商标——包括 Need for Speed、NFS Carbon、EA、Black Box 及相关标志——均为其各自所有者的财产。

用户有责任确保在使用本工具修改游戏文件之前合法拥有 Need for Speed: Carbon 的副本。


### 风险免责书

本软件按“现状”提供，使用者须自行承担包括但不限于存档损坏、数据丢失、系统异常等全部固有风险；在适用法律允许的最大范围内，开发者不对因使用或无法使用本软件所产生的任何直接、间接、附带、衍生或惩罚性损害（含利润损失、数据恢复成本等）承担任何赔偿责任，继续操作即视为你明示放弃就此主张任何惩罚性赔偿之权利。

### 第三方许可证

- **MPFmaster.exe** 和 **MPFmaster_GenerateEvent.exe** 依据 MIT 许可证分发（版权所有 (c) 2022 Lovro Plese / xan1242，BlueSkyWestSide 修改）。完整的 MIT 许可证文本见下文。
- **FFmpeg** 依据 GNU Lesser General Public License（LGPL）2.1 版或更高版本分发。本发行版中包含的 FFmpeg 二进制文件未经修改。FFmpeg 源代码可在 https://ffmpeg.org/download.html 获取。

#### MIT 许可证（PFDataCompiler / MPFmaster）

```
MIT License

Copyright (c) 2022 Lovro Plese

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
