/*********************************************************************************************************************/
#pragma once

/*********************************************************************************************************************/
#include "aircraft_state.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <optional>
#include <vector>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

/*********************************************************************************************************************/
namespace qs::libs
{

    /*********************************************************************************************************************/
    class UdpSocket
    {
    public:
        UdpSocket();
        ~UdpSocket();
        UdpSocket(UdpSocket const& other) = delete;
        UdpSocket(UdpSocket&& other) noexcept;
        UdpSocket& operator=(UdpSocket const& other) = delete;
        UdpSocket& operator=(UdpSocket&& other) noexcept;

        bool initSocket(std::string& error_string) noexcept;
        int sendData(std::vector<uint8_t> const& data) const noexcept;
        int sendData(char const* buffer, int size) const noexcept;
        int readData(char* data, int length) noexcept;

        /// Tries to receive and parse AircraftState data from the socket.
        /// Returns std::optional<AircraftState> with data if successful, std::nullopt otherwise.
        std::optional<case_study::AircraftState> TryGet();

        bool setMulticastConfig(std::string const& interface_ip, uint8_t ttl, bool enable_loopback,
            std::string& error_string) const noexcept;

        bool setUdpSendIpPort(std::string const& ip, uint16_t port, std::string& error_string) noexcept;
        bool setUdpListenPort(uint16_t port, std::string& error_string) const noexcept;

        int32_t getRxBytesInBuffer() const noexcept;
        bool isSocketReadable(timeval timeout) noexcept;

    private:
        SOCKET      socket_;
        fd_set      select_;
        sockaddr_in receiver_;
        bool        winsock_initialized_;

        bool initializeWinsock(std::string& error_string) noexcept;
        void deinitializeWinsock() noexcept;
    };

} // namespace qs::libs
