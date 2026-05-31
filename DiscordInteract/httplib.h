//==============================================================================================
// Include Only Once
//==============================================================================================
#pragma once
//==============================================================================================
// Defines
//==============================================================================================
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//==============================================================================================
// Libraries To Include
//==============================================================================================
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <functional>
#include <iostream>
#include <vector>
//==============================================================================================
#pragma comment(lib, "Ws2_32.lib")
//==============================================================================================
// Declare Namespace and Core Structures for Minimal HTTP Server Functionality
//==============================================================================================
namespace httplib {
    struct Request {
        std::string body;
    };
    struct Response {
        int status = 200;
        void set_content(const std::string& content, const std::string& content_type) {}
    };
    class Server {
    private:
        std::function<void(const Request&, Response&)> post_handler;
    public:
        Server() {}
        void Post(const std::string& pattern, std::function<void(const Request&, Response&)> handler) {
            post_handler = handler;
        }
        bool listen(const std::string& host, int port) {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
            SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (listenSocket == INVALID_SOCKET) {
                WSACleanup();
                return false;
            }
            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            inet_pkey_fallback(host, &serverAddr.sin_addr);
            if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                closesocket(listenSocket);
                WSACleanup();
                return false;
            }
            if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
                closesocket(listenSocket);
                WSACleanup();
                return false;
            }
            // Allocate a massive heap buffer (512 KB) to prevent large JSON packets from getting truncated
            const size_t BUFFER_SIZE = 1024 * 512;
            std::vector<char> recvBuf(BUFFER_SIZE);
            while (true) {
                SOCKET clientSocket = accept(listenSocket, NULL, NULL);
                if (clientSocket != INVALID_SOCKET) {
                    std::fill(recvBuf.begin(), recvBuf.end(), 0);
                    int bytesRecv = recv(clientSocket, recvBuf.data(), static_cast<int>(BUFFER_SIZE) - 1, 0);

                    if (bytesRecv > 0) {
                        std::string requestStr(recvBuf.data());
                        size_t bodyPos = requestStr.find("\r\n\r\n");

                        if (bodyPos != std::string::npos && post_handler) {
                            Request req;
                            req.body = requestStr.substr(bodyPos + 4);
                            Response res;

                            std::cout << "[SERVER] Inbound packet intercepted! Payload size: " << req.body.length() << " bytes.\n";

                            post_handler(req, res);

                            std::string http_response =
                                "HTTP/1.1 202 Accepted\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 8\r\n"
                                "Connection: close\r\n\r\n"
                                "Accepted";
                            send(clientSocket, http_response.c_str(), (int)http_response.length(), 0);
                        }
                    }
                    closesocket(clientSocket);
                }
            }
            closesocket(listenSocket);
            WSACleanup();
            return true;
        }
    private:
        void inet_pkey_fallback(const std::string& host, in_addr* addr) {
            if (host == "0.0.0.0") {
                addr->s_addr = INADDR_ANY;
            }
            else {
                if (inet_pton(AF_INET, host.c_str(), addr) != 1) {
                    addr->s_addr = INADDR_NONE;
                }
            }
        }
    };
}
//==============================================================================================
// End of httplib.h
//==============================================================================================