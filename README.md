[![CD](https://github.com/humbertodias/QHugoResourceViewer/actions/workflows/cd.yml/badge.svg)](https://github.com/humbertodias/QHugoResourceViewer/actions/workflows/cd.yml)
![GitHub all releases](https://img.shields.io/github/downloads/humbertodias/QHugoResourceViewer/total)


# QHugoResourceViewer

Tool for viewing [Hugo](https://en.wikipedia.org/wiki/List_of_Hugo_video_games) resources games.

| FORMAT | SUPPORTED |
|--------|-----------|
| raw,wav,pal    | ✅        |
| blk,cbr,col,lbm,lzp,pbr,pc,til    | ❌        |


## Installation

Ubuntu/Debian

```bash
sudo apt install qt6-base-dev qt6-tools-dev qt6-multimedia-dev cmake ninja-build
```

## Build
Clone, Setup and Build
```bash
git clone https://github.com/humbertodias/QHugoResourceViewer.git
cd QHugoResourceViewer
cmake -Bbuild
cmake --build build
```
## Run

```bash
GTK_PATH="" ./build/QHugoResourceViewer
```

![image](https://github.com/user-attachments/assets/82119de7-8e6d-40a1-bcf9-079984ee071c)
Tested using **HUGO.DAT** from [Hugo 5 DOS - 1997](https://www.myabandonware.com/game/hugo-5-tqc)
