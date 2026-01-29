// ModernProtocolParser.hpp - Parse modern protocols (HTTP/2, QUIC, WebSocket)
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

namespace PacketAnalyzer2026::Protocols {

struct HTTP2Frame {
    uint32_t length;
    uint8_t type;
    uint8_t flags;
    uint32_t streamId;
    std::vector<uint8_t> payload;
};

struct QUICPacket {
    uint32_t version;
    bool isLongHeader;
    uint8_t packetType;
    std::vector<uint8_t> payload;
};

struct WebSocketFrame {
    bool fin;
    uint8_t opcode;
    bool masked;
    uint64_t payloadLength;
    std::vector<uint8_t> payload;
};

class ModernProtocolParser {
public:
    static std::string detectModernProtocol(const uint8_t* data, size_t size, uint16_t port) {
        if (!data || size < 4) return "Unknown";

        // HTTP/2 detection (port 80/443 with HTTP/2 magic)
        if ((port == 80 || port == 443) && size >= 24) {
            // HTTP/2 connection preface: "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n"
            const char* http2_preface = "PRI * HTTP/2.0";
            if (std::memcmp(data, http2_preface, 14) == 0) {
                return "HTTP/2";
            }
            
            // HTTP/2 frame format detection
            if (isHTTP2Frame(data, size)) {
                return "HTTP/2";
            }
        }

        // QUIC detection (UDP, specific patterns)
        if (port == 443 || port == 80) {
            if (isQUICPacket(data, size)) {
                return "QUIC";
            }
        }

        // WebSocket detection (after HTTP upgrade)
        if ((port == 80 || port == 443) && isWebSocketFrame(data, size)) {
            return "WebSocket";
        }

        // gRPC detection (HTTP/2 with specific content-type)
        if ((port == 80 || port == 443) && size > 20) {
            // Look for gRPC content-type in HTTP/2 headers
            std::string dataStr(reinterpret_cast<const char*>(data), std::min(size, size_t(100)));
            if (dataStr.find("application/grpc") != std::string::npos) {
                return "gRPC";
            }
        }

        return "Standard";
    }

    static HTTP2Frame parseHTTP2Frame(const uint8_t* data, size_t size) {
        HTTP2Frame frame{};
        
        if (size < 9) return frame; // Minimum frame size

        // Parse frame header (9 bytes)
        frame.length = (data[0] << 16) | (data[1] << 8) | data[2];
        frame.type = data[3];
        frame.flags = data[4];
        frame.streamId = ((data[5] & 0x7F) << 24) | (data[6] << 16) | (data[7] << 8) | data[8];

        // Parse payload if available
        if (size > 9 && frame.length > 0) {
            size_t payloadSize = std::min(static_cast<size_t>(frame.length), size - 9);
            frame.payload.assign(data + 9, data + 9 + payloadSize);
        }

        std::cout << "📦 HTTP/2 Frame parsed - Type: " << static_cast<int>(frame.type) 
                  << ", Length: " << frame.length << ", Stream: " << frame.streamId << std::endl;

        return frame;
    }

    static QUICPacket parseQUIC(const uint8_t* data, size_t size) {
        QUICPacket packet{};
        
        if (size < 1) return packet;

        uint8_t firstByte = data[0];
        packet.isLongHeader = (firstByte & 0x80) != 0;

        if (packet.isLongHeader && size >= 5) {
            // Long header format
            packet.version = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
            packet.packetType = (firstByte & 0x30) >> 4;
        } else {
            // Short header format
            packet.version = 0; // Version not present in short header
            packet.packetType = 0;
        }

        // Parse payload (simplified)
        if (size > (packet.isLongHeader ? 5 : 1)) {
            size_t payloadStart = packet.isLongHeader ? 5 : 1;
            packet.payload.assign(data + payloadStart, data + size);
        }

        std::cout << "🚀 QUIC Packet parsed - Version: 0x" << std::hex << packet.version 
                  << ", Long Header: " << packet.isLongHeader << std::dec << std::endl;

        return packet;
    }

    static WebSocketFrame parseWebSocket(const uint8_t* data, size_t size) {
        WebSocketFrame frame{};
        
        if (size < 2) return frame;

        uint8_t firstByte = data[0];
        uint8_t secondByte = data[1];

        frame.fin = (firstByte & 0x80) != 0;
        frame.opcode = firstByte & 0x0F;
        frame.masked = (secondByte & 0x80) != 0;

        uint8_t payloadLen = secondByte & 0x7F;
        size_t headerSize = 2;

        if (payloadLen == 126 && size >= 4) {
            frame.payloadLength = (data[2] << 8) | data[3];
            headerSize = 4;
        } else if (payloadLen == 127 && size >= 10) {
            frame.payloadLength = 0;
            for (int i = 0; i < 8; i++) {
                frame.payloadLength = (frame.payloadLength << 8) | data[2 + i];
            }
            headerSize = 10;
        } else {
            frame.payloadLength = payloadLen;
        }

        if (frame.masked) {
            headerSize += 4; // Masking key
        }

        // Parse payload if available
        if (size > headerSize && frame.payloadLength > 0) {
            size_t payloadSize = std::min(static_cast<size_t>(frame.payloadLength), size - headerSize);
            frame.payload.assign(data + headerSize, data + headerSize + payloadSize);
        }

        std::cout << "🌐 WebSocket Frame parsed - Opcode: " << static_cast<int>(frame.opcode) 
                  << ", Masked: " << frame.masked << ", Length: " << frame.payloadLength << std::endl;

        return frame;
    }

private:
    static bool isHTTP2Frame(const uint8_t* data, size_t size) {
        if (size < 9) return false;
        
        // Check if it looks like an HTTP/2 frame
        uint32_t length = (data[0] << 16) | (data[1] << 8) | data[2];
        uint8_t type = data[3];
        
        // Valid frame types: 0-10 in HTTP/2 spec
        return length <= 16384 && type <= 10; // Max frame size and valid type
    }

    static bool isQUICPacket(const uint8_t* data, size_t size) {
        if (size < 1) return false;
        
        uint8_t firstByte = data[0];
        
        // QUIC packets have specific bit patterns
        if ((firstByte & 0x80) != 0) {
            // Long header - check version
            if (size >= 5) {
                uint32_t version = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
                return version != 0; // Version 0 is version negotiation
            }
        } else {
            // Short header - less reliable detection
            return (firstByte & 0x40) != 0; // Fixed bit must be set
        }
        
        return false;
    }

    static bool isWebSocketFrame(const uint8_t* data, size_t size) {
        if (size < 2) return false;
        
        uint8_t firstByte = data[0];
        uint8_t opcode = firstByte & 0x0F;
        
        // Valid WebSocket opcodes: 0-2, 8-10
        return (opcode <= 2) || (opcode >= 8 && opcode <= 10);
    }
};

} // namespace PacketAnalyzer2026::Protocols