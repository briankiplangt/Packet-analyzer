# 🌐 Packet Analyzer 2026 - Network Analysis Tool

A professional network packet analysis application built with Qt 6.5.3 and C++, featuring real-time packet capture, protocol analysis, and modern QML interface.

## 🚀 Features

- **Real-time Packet Capture**: Live network traffic monitoring
- **Protocol Analysis**: Deep packet inspection for HTTP, HTTPS, DNS, TCP, UDP, ICMP
- **Modern UI**: Sleek QML interface with dark theme and animations
- **Advanced Filtering**: BPF syntax support for precise packet filtering
- **Statistics Dashboard**: Network performance metrics and analytics
- **Security Analysis**: Threat detection and security monitoring
- **Multi-interface Support**: Capture from multiple network interfaces
- **Bandwidth Monitoring**: Real-time network utilization tracking
- **Network Topology**: Visual network mapping and device discovery

## 🛠️ Tech Stack

- **Framework**: Qt 6.5.3 (C++ & QML)
- **Language**: C++17
- **UI**: QML with Material Design
- **Database**: SQLite for packet storage
- **Networking**: Raw sockets, WinPcap/libpcap support
- **Build System**: CMake & Visual Studio 2022
- **Architecture**: Modern C++ with RAII and smart pointers

## 📁 Project Structure

```
PacketAnalyzer2026/
├── src/                       # C++ Source Code
│   ├── core/                  # Core packet capture engine
│   ├── database/              # Database management
│   ├── models/                # Data models
│   ├── protocols/             # Protocol parsers
│   ├── security/              # Security analysis
│   ├── performance/           # Performance optimization
│   └── audit/                 # Audit logging
├── ui/                        # QML User Interface
│   ├── components/            # Reusable UI components
│   └── dialogs/               # Dialog windows
├── database/                  # Database schemas
├── complete_interface.qml     # Main QML interface
├── CMakeLists.txt            # CMake build configuration
├── resources.qrc             # Qt resource file
└── main.cpp                  # Application entry point
```

## 🚀 Getting Started

### Prerequisites

- **Qt 6.5.3** with MSVC 2019 64-bit
- **Visual Studio 2022** or **Qt Creator**
- **CMake 3.16+**
- **Windows 10/11** (Administrator privileges for packet capture)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/briankiplangt/packet-analyzer-2026.git
   cd packet-analyzer-2026
   ```

2. **Setup Qt Environment**
   ```bash
   # Ensure Qt 6.5.3 is installed and in PATH
   # Set Qt6_DIR environment variable if needed
   ```

3. **Build with CMake**
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build . --config Debug
   ```

4. **Build with Visual Studio**
   ```bash
   # Open PacketAnalyzer2026_GUI.sln in Visual Studio 2022
   # Build -> Build Solution
   ```

5. **Run the Application**
   ```bash
   # Run as Administrator for packet capture
   ./PacketAnalyzer2026_Production.exe
   ```

## 🎯 Usage

### Basic Operations

1. **Start Capture**: Click the capture button to begin monitoring
2. **Select Interface**: Choose network interface from dropdown
3. **Apply Filters**: Use BPF syntax for targeted packet capture
4. **Analyze Packets**: Click packets to view detailed information
5. **View Statistics**: Access network performance metrics

### Advanced Features

- **Protocol Analysis**: Deep inspection of HTTP, TLS, DNS traffic
- **Security Monitoring**: Detect suspicious network activity
- **Bandwidth Analysis**: Monitor network utilization in real-time
- **Packet Export**: Save captured data for further analysis
- **Custom Filters**: Create complex filtering rules

## 🔧 Configuration

### Network Interfaces
The application automatically detects available network interfaces. For best results:
- Run as Administrator for raw socket access
- Ensure network drivers are up to date
- Configure Windows Firewall if needed

### Filters
Use Berkeley Packet Filter (BPF) syntax:
```
tcp port 80                    # HTTP traffic
tcp port 443                   # HTTPS traffic
udp port 53                    # DNS queries
host 192.168.1.1               # Specific IP
net 192.168.1.0/24             # Subnet traffic
```

## 🏗️ Architecture

### Core Components

- **PacketCaptureEngine**: Real-time packet capture and processing
- **DatabaseManager**: SQLite database operations
- **ProtocolAnalyzers**: HTTP, DNS, TCP/UDP parsers
- **SecurityAnalyzer**: Threat detection algorithms
- **QML Interface**: Modern user interface

### Design Patterns

- **Observer Pattern**: Real-time data updates
- **Factory Pattern**: Protocol parser creation
- **Singleton Pattern**: Database connections
- **MVVM**: QML data binding

## 🎨 User Interface

- **Dark Theme**: Professional appearance
- **Material Design**: Modern UI components
- **Responsive Layout**: Adapts to different screen sizes
- **Real-time Updates**: Live data visualization
- **Interactive Charts**: Network statistics graphs
- **Glass Morphism**: Modern visual effects

## 🔒 Security Features

- **Encrypted Traffic Detection**: Identify TLS/SSL connections
- **Anomaly Detection**: Unusual traffic patterns
- **Port Scanning Detection**: Security threat monitoring
- **Data Privacy**: Local processing, no cloud dependencies
- **Privilege Management**: Secure access control

## 📊 Performance

- **High-speed Capture**: Handles high-volume network traffic
- **Efficient Storage**: Optimized database operations
- **Memory Management**: Smart packet buffering
- **Multi-threading**: Parallel processing for better performance
- **Resource Monitoring**: System resource optimization

## 🔧 Build Requirements

### Windows
- Visual Studio 2022 with MSVC v143
- Qt 6.5.3 MSVC 2019 64-bit
- CMake 3.16 or later
- Windows SDK 10.0.19041.0 or later

### Dependencies
- Qt6Core, Qt6Gui, Qt6Qml, Qt6Quick
- SQLite3
- WinPcap/Npcap for packet capture

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Follow C++ and Qt coding standards
4. Add tests for new features
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License.

## 👨‍💻 Author

**Brian Kiplang'at**
- GitHub: [@briankiplangt](https://github.com/briankiplangt)

## 🙏 Acknowledgments

- Qt Framework for excellent C++/QML integration
- WinPcap/Npcap for packet capture capabilities
- Material Design for UI inspiration

---

*Professional network analysis tool for modern network monitoring and security.*