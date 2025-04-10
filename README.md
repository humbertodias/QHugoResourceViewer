[![CD](https://github.com/humbertodias/hugo-resource-viewer/actions/workflows/cd.yml/badge.svg)](https://github.com/humbertodias/hugo-resource-viewer/actions/workflows/cd.yml)
![GitHub all releases](https://img.shields.io/github/downloads/humbertodias/hugo-resource-viewer/total)


# Hugo Resource Viewer

Tool cross-platform for viewing [Hugo](https://en.wikipedia.org/wiki/List_of_Hugo_video_games) resources games.

| FORMAT | SUPPORTED |
|--------|-----------|
| raw,wav,pal    | ✅        |
| blk,cbr,col,lbm,lzp,pbr,pc,til    | ❌        |


## Installation

Ubuntu/Debian

```bash
sudo apt install cmake ninja-build flex bison autoconf-archive
```

## Build
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
## Run

```bash
GTK_PATH="" ./build/hugo-resource-viewer
```

![image](https://github.com/user-attachments/assets/82119de7-8e6d-40a1-bcf9-079984ee071c)
Tested using **HUGO.DAT** from [Hugo 5 DOS - 1997](https://www.myabandonware.com/game/hugo-5-tqc)

## References
* https://vcpkg.io