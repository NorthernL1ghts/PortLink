# PortLink - Real-Time Video and Data Handling

## Overview

PortLink is an innovative proof-of-concept project focused on developing a secure and scalable system for real-time video stream and data handling. This project aims to provide a foundation for robust surveillance solutions with potential applications across various industries. PortLink emphasizes security, performance, and cross-platform compatibility.

## Features

- **Real-Time Video and Audio Capture**: Captures video and audio streams from locally connected webcams and microphones.
- **Cross-Platform Compatibility**: Supports Windows, Linux, and MacOS operating systems.
- **Secure Data Handling**: End-to-end encryption ensures the confidentiality and integrity of transmitted data.
- **Modular Design**: The CMake build system enables easy integration of new components and customization.
- **Build Automation**: Scripts are provided to automate project generation for each supported platform.
- **Full GitHub Integration**: Includes workflows, code of conduct, and issue reporting.
- **Comprehensive Documentation**: Detailed documentation is available.
- **Future-Proof Architecture**: Designed for future expansion with planned support for wireless cameras, CCTV systems, and AI-powered analytics.

## Code Stack

- **C++ 20**: The core of PortLink is implemented in C++20 for high performance and low-level control.
- **Python**: A Python backend is planned for future integration of Machine Learning (ML) and Artificial Intelligence (AI) capabilities.
- **CMake**: The project uses CMake for cross-platform build automation.

## Architecture

- **Video and Audio Capture**: Captures real-time streams from local devices.
- **Data Handling & Encryption**: Securely processes and encrypts video and audio data.
- **Build System**: CMake manages the build process for different platforms, with automation scripts provided.
- **GitHub Integration**: Workflows and processes for contributions and issue management.
- **Documentation**: Project documentation for developers and users.
- **Future Enhancements**: The system is designed to incorporate ML/AI features and expanded device support in future phases.

## Getting Started

### Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (e.g., GCC, Clang, MSVC)
- Python 3.x (for future ML/AI features)

### Build Instructions

1. Clone the repository:

    ```bash
    git clone https://github.com/NorthernL1ghts/PortLink.git
    cd PortLink
    ```

2. Generate build files using the provided scripts:

    **Windows:**

    ```bash
    Scripts/Win-GenerateProject.bat
    ```

    **Linux / MacOS:**

    ```bash
    Scripts/GenerateProject.sh
    ```

3. Build the project using your platform's build tools (e.g., Visual Studio for Windows, make for Linux/MacOS).

## Contributing

Contributions are welcome! To contribute to PortLink, please follow these steps:

1. Fork the repository from [https://github.com/NorthernL1ghts/PortLink](https://github.com/NorthernL1ghts/PortLink)
2. Create a new branch for your feature or bug fix.
3. Commit your changes with clear and concise commit messages.
4. Push your changes to your fork.
5. Submit a pull request.

Please refer to our [Code of Conduct](.github/ISSUE_TEMPLATE/CODE_OF_CONDUCT.md) for guidelines on contributing to this project.

## Reporting Bugs

If you encounter any bugs or issues, please submit a bug report.

## Documentation

Detailed documentation can be found in the **Documentation** directory.

## TODO

See **TODO.md** for the current development roadmap.

## License

This project is licensed under the MIT License. (Replace with the actual license file)
