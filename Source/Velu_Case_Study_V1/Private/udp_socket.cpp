/*********************************************************************************************************************/
#include "udp_socket.hpp"
#include <winsock2.h>   
#include <ws2tcpip.h>   
#include <algorithm>    
#include <sstream>      
#include <string>       

/*********************************************************************************************************************/
using namespace qs::libs;

/*********************************************************************************************************************/
UdpSocket::UdpSocket() : socket_(INVALID_SOCKET), select_(), receiver_(), winsock_initialized_(false) {}

/*********************************************************************************************************************/
UdpSocket::~UdpSocket()
{
    if (socket_ != INVALID_SOCKET)
    {
        closesocket(socket_);
    }
    deinitializeWinsock();
}

/*********************************************************************************************************************/
UdpSocket::UdpSocket(UdpSocket&& other) noexcept
{
    std::swap(socket_, other.socket_);
    select_ = other.select_;
    receiver_ = other.receiver_;
    winsock_initialized_ = other.winsock_initialized_;
}

/*********************************************************************************************************************/
UdpSocket& UdpSocket::operator=(UdpSocket&& other) noexcept
{
    if (this != &other)
    {
        if (socket_ != INVALID_SOCKET)
        {
            closesocket(socket_);
        }
        std::swap(socket_, other.socket_);
        select_ = other.select_;
        receiver_ = other.receiver_;
        winsock_initialized_ = other.winsock_initialized_;
    }
    return *this;
}

/*********************************************************************************************************************/
bool UdpSocket::initSocket(std::string& error_string) noexcept
{
    if (!initializeWinsock(error_string))
    {
        error_string = "Failed to initialize Winsock: " + error_string;
        return false;
    }

    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_ == INVALID_SOCKET)
    {
        error_string = "Error creating socket: " + std::to_string(WSAGetLastError());
        return false;
    }
    return true;
}

/*********************************************************************************************************************/
int UdpSocket::sendData(const std::vector<uint8_t>& data) const noexcept
{
    return sendData(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}

/*********************************************************************************************************************/
int UdpSocket::sendData(const char* buffer, int size) const noexcept
{
    return sendto(socket_, buffer, size, 0, reinterpret_cast<const sockaddr*>(&receiver_), static_cast<int>(sizeof(receiver_)));
}

/*********************************************************************************************************************/
int UdpSocket::readData(char* data, int length) noexcept
{
    static constexpr timeval NO_TIMEOUT{};
    if (isSocketReadable(NO_TIMEOUT))
    {
        return recvfrom(socket_, data, length, 0, nullptr, nullptr);
    }
    else
    {
        WSASetLastError(WSAEWOULDBLOCK);
        return SOCKET_ERROR;
    }
}

/*********************************************************************************************************************/
// New function: Try to receive and parse AircraftState data from socket buffer
std::optional<case_study::AircraftState> UdpSocket::TryGet()
{
    constexpr int buffer_size = sizeof(case_study::AircraftState);
    char buffer[buffer_size] = { 0 };

    int bytes_received = readData(buffer, buffer_size);

    if (bytes_received == buffer_size)
    {
        // Assume the data layout exactly matches AircraftState (packed struct)
        case_study::AircraftState state;
        memcpy(&state, buffer, sizeof(case_study::AircraftState));
        return state;
    }
    // No valid data received
    return std::nullopt;
}

/*********************************************************************************************************************/
bool UdpSocket::setMulticastConfig(const std::string& interface_ip, uint8_t ttl, bool enable_loopback,
    std::string& error_string) const noexcept
{
    char loopback_option = enable_loopback ? 1 : 0;
    struct in_addr local_interface {};

    if (!interface_ip.empty() && inet_pton(AF_INET, interface_ip.c_str(), &local_interface) <= 0)
    {
        error_string = "Failed to convert IP address for multicast interface";
        return false;
    }

    if (!interface_ip.empty() &&
        setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char*>(&local_interface), sizeof(local_interface)) < 0)
    {
        error_string = "Failed to set multicast interface: [" + std::to_string(WSAGetLastError()) + "]";
        return false;
    }

    if (setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<const char*>(&ttl), sizeof(ttl)) < 0)
    {
        error_string = "Failed to set TTL for multicast: [" + std::to_string(WSAGetLastError()) + "]";
        return false;
    }

    if (setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_LOOP, &loopback_option, sizeof(loopback_option)) < 0)
    {
        error_string = "Failed to enable multicast loopback: [" + std::to_string(WSAGetLastError()) + "]";
        return false;
    }

    return true;
}

/*********************************************************************************************************************/
bool UdpSocket::setUdpSendIpPort(const std::string& ip, uint16_t port, std::string& error_string) noexcept
{
    receiver_.sin_family = AF_INET;
    receiver_.sin_port = htons(port);

    int inet_pton_result = inet_pton(AF_INET, ip.c_str(), &receiver_.sin_addr);
    if (inet_pton_result == 0)
    {
        error_string = "Failed to set the UDP send ip and port: ip address is not valid: [" + ip + "]";
        return false;
    }
    else if (inet_pton_result == -1)
    {
        error_string = "Failed to set the UDP send ip and port: [" + std::to_string(WSAGetLastError()) + "]";
        return false;
    }
    return true;
}

/*********************************************************************************************************************/
bool UdpSocket::setUdpListenPort(uint16_t port, std::string& error_string) const noexcept
{
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces

    if (bind(socket_, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == SOCKET_ERROR)
    {
        std::ostringstream error;
        error << "Failed bind() call on port " << port;
        error_string = error.str();
        return false;
    }
    return true;
}

/*********************************************************************************************************************/
int32_t UdpSocket::getRxBytesInBuffer() const noexcept
{
    unsigned long bytes_available = 0;
    if (ioctlsocket(socket_, FIONREAD, &bytes_available) == SOCKET_ERROR)
    {
        return -1;
    }
    return static_cast<int32_t>(std::clamp(bytes_available, 0UL, static_cast<unsigned long>(INT32_MAX)));
}

/*********************************************************************************************************************/
bool UdpSocket::isSocketReadable(timeval timeout) noexcept
{
    FD_ZERO(&select_);
    FD_SET(socket_, &select_);
    return select(0, &select_, nullptr, nullptr, &timeout) > 0;
}

/*********************************************************************************************************************/
bool UdpSocket::initializeWinsock(std::string& error_string) noexcept
{
    if (winsock_initialized_)
    {
        return true;
    }

    WSADATA wsa_data;
    int startup_return = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (startup_return != 0)
    {
        std::stringstream error;
        error << "WSAStartup failed. WSAStartup returned [" << startup_return << "]";
        error_string = error.str();
        return false;
    }

    winsock_initialized_ = true;
    return true;
}

/*********************************************************************************************************************/
void UdpSocket::deinitializeWinsock() noexcept
{
    if (winsock_initialized_)
    {
        WSACleanup();
        winsock_initialized_ = false;
    }
}
