[![CD](https://github.com/humbertodias/QHugoResourceViewer/actions/workflows/cd.yml/badge.svg)](https://github.com/humbertodias/QHugoResourceViewer/actions/workflows/cd.yml)
![GitHub all releases](https://img.shields.io/github/downloads/humbertodias/QHugoResourceViewer/total)


# QHugoResourceViewer

Tool for viewing [Hugo](https://en.wikipedia.org/wiki/List_of_Hugo_video_games) resources games.
Tested with [Hugo 5 DOS - 1997](https://www.myabandonware.com/game/hugo-5-tqc)


![image](https://github.com/user-attachments/assets/82119de7-8e6d-40a1-bcf9-079984ee071c)



Current state: 

| FORMAT | SUPPORTED |
|--------|-----------|
| RAW    | ✅        |
| WAV    | ✅        |
| PAL    | ✅        |
| BLK    | ❌        |
| CBR    | ❌        |
| COL    | ❌        |
| LBM    | ❌        |
| LZP    | ❌        |
| PBR    | ❌        |
| PC     | ❌        |
| TIL    | ❌        |


## Dependencies

Ubuntu/Debian

```bash
sudo apt install qt6-base-dev qt6-tools-dev qt6-multimedia-dev cmake ninja-build
```

## Clone, Setup and Build

```bash
git clone https://github.com/humbertodias/QHugoResourceViewer.git
cd QHugoResourceViewer
cmake -Bbuild
cmake --build build
```
## Running the Application

```bash
GTK_PATH="" ./build/QHugoResourceViewer
```

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for more details.
