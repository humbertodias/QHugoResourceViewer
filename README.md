[![CD](https://github.com/humbertodias/QHugoResourceViewer/actions/workflows/cd.yml/badge.svg)](https://github.com/humbertodias/QHugoResourceViewer/actions/workflows/cd.yml)
![GitHub all releases](https://img.shields.io/github/downloads/humbertodias/QHugoResourceViewer/total)


# QHugoResourceViewer

**QHugoResourceViewer** is a tool for viewing Hugo resources (static site framework). This project is developed with **Qt6** and **CMake**, providing a graphical interface to interact with Hugo resources.

## Dependencies

Before compiling, you need to install the system dependencies. On Ubuntu/Debian, use the following command:

```bash
sudo apt install qt6-base-dev qt6-tools-dev cmake ninja-build
```

These dependencies include **Qt6** and the necessary development tools for building the application.

## Setup and Build

Follow the steps below to build the project in your local environment.

### 1. Clone the repository

First, clone the repository to your local machine:

```bash
git clone https://github.com/humbertodias/QHugoResourceViewer.git
cd QHugoResourceViewer
```

### 2. Create the Build Directory

Next, create the build directory and compile the project:

```bash
cmake -Bbuild
cmake --build build
```

This will generate the compiled project inside the `build` directory.

## Running the Application

After a successful build, you can run the application with the following command:

```bash
GTK_PATH="" ./build/QHugoResourceViewer
```

### Notes on Running:
- If Qt6 cannot find the GTK dependencies on your system, set the `GTK_PATH` environment variable to an empty string as shown above.

## Known Issues

- **Qt6 Dependencies**: If your system doesn’t have the required version of Qt6 or cannot install `qt6-base-dev` directly, consider using **Qt6 from the official repository** or through tools like **Homebrew** on macOS.

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for more details.
