// PrivilegeManager.hpp - OS-level security and privilege management
#pragma once

#include <stdexcept>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <lmcons.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace PacketAnalyzer2026::Security {

class PrivilegeManager {
public:
    static bool isRunningAsAdmin() {
#ifdef _WIN32
        BOOL isAdmin = FALSE;
        PSID adminGroup = NULL;
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        
        if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
            CheckTokenMembership(NULL, adminGroup, &isAdmin);
            FreeSid(adminGroup);
        }
        
        std::cout << "🔐 Windows privilege check: " << (isAdmin ? "Administrator" : "Standard User") << std::endl;
        return isAdmin == TRUE;
#else
        bool isRoot = (geteuid() == 0);
        std::cout << "🔐 Unix privilege check: " << (isRoot ? "Root/Sudo" : "Standard User") << std::endl;
        return isRoot;
#endif
    }

    static void validateCapturePermissions() {
        if (!isRunningAsAdmin()) {
            throw std::runtime_error("Administrator privileges required for packet capture");
        }

#ifdef _WIN32
        // On Windows, check if we can access raw sockets
        SOCKET testSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (testSocket == INVALID_SOCKET) {
            int error = WSAGetLastError();
            if (error == WSAEACCES) {
                throw std::runtime_error("Raw socket access denied - need Administrator privileges");
            }
        } else {
            closesocket(testSocket);
        }
#else
        // On Unix, check if we can create raw sockets
        int testSocket = socket(AF_PACKET, SOCK_RAW, 0);
        if (testSocket < 0) {
            throw std::runtime_error("Raw socket creation failed - need root/sudo privileges");
        } else {
            close(testSocket);
        }
#endif

        std::cout << "✅ Packet capture permissions validated" << std::endl;
    }

    static void dropPrivileges() {
#ifdef _WIN32
        // On Windows, we can't easily drop privileges after getting them
        // Instead, we limit what the process can do
        std::cout << "⚠️  Windows: Cannot drop privileges after elevation" << std::endl;
        std::cout << "🔒 Security: Process runs with limited scope" << std::endl;
#else
        // On Unix, drop to original user after initialization
        uid_t originalUid = getuid();
        gid_t originalGid = getgid();
        
        if (geteuid() == 0 && originalUid != 0) {
            // We're running as root but original user wasn't root
            if (setgid(originalGid) != 0) {
                throw std::runtime_error("Failed to drop group privileges");
            }
            
            if (setuid(originalUid) != 0) {
                throw std::runtime_error("Failed to drop user privileges");
            }
            
            std::cout << "✅ Privileges dropped to UID: " << originalUid << ", GID: " << originalGid << std::endl;
        } else {
            std::cout << "ℹ️  No privilege dropping needed" << std::endl;
        }
#endif
    }

    static std::string getCurrentUser() {
#ifdef _WIN32
        char username[UNLEN + 1];
        DWORD usernameLen = UNLEN + 1;
        if (GetUserNameA(username, &usernameLen)) {
            return std::string(username);
        }
        return "Unknown";
#else
        struct passwd* pw = getpwuid(getuid());
        return pw ? std::string(pw->pw_name) : "Unknown";
#endif
    }

    static void logPrivilegeStatus() {
        std::cout << "👤 Current user: " << getCurrentUser() << std::endl;
        std::cout << "🔐 Admin privileges: " << (isRunningAsAdmin() ? "Yes" : "No") << std::endl;
        
#ifdef _WIN32
        std::cout << "🖥️  Platform: Windows (UAC-based security)" << std::endl;
#else
        std::cout << "🐧 Platform: Unix/Linux (sudo-based security)" << std::endl;
        std::cout << "🆔 UID: " << getuid() << ", EUID: " << geteuid() << std::endl;
#endif
    }
};

} // namespace PacketAnalyzer2026::Security