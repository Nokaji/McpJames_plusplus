#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <variant>
#include <future>
#include <chrono>
#include <optional>
#include "../transport/transport.hpp"
#include <nlohmann/json.hpp>
namespace mcp {
namespace type {

struct HttpConfig {
    std::string baseUrl;
    std::map<std::string, std::string> headers;
    int timeoutMs = 30000;
    bool verifySSL = true;
};

struct WebSocketConfig {
    std::string url;
    std::map<std::string, std::string> headers;
    int timeoutMs = 30000;
    bool verifySSL = true;
};

struct SseConfig {
    std::string url;
    std::string sseEndpoint = "/sse";
    std::string messageEndpoint = "/message";
    std::map<std::string, std::string> headers;
    int timeoutMs = 30000;
    bool verifySSL = true;
    int reconnectDelayMs = 3000;
    int maxRetries = -1;
    std::string lastEventId;
};

enum class ConnectionStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    ERROR,
    RECONNECTING
};

struct McpMessage {
    std::string id;
    std::string method;
    std::map<std::string, std::string> params;
    std::string result;
    std::string error;
    bool isRequest = true;
};

struct McpTransportConfig {
    enum class TransportType {
        HTTP,
        WEBSOCKET,
        SSE
    } type = TransportType::SSE;

    std::variant<HttpConfig, WebSocketConfig, SseConfig> config;

    NLOHMANN_JSON_SERIALIZE_ENUM(TransportType, {
        {TransportType::HTTP, "HTTP"},
        {TransportType::WEBSOCKET, "WEBSOCKET"},
        {TransportType::SSE, "SSE"}
    })

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(McpTransportConfig, type, config)
};

struct McpServerConfig {
    std::string name;
    std::string description;
    bool autoReconnect = true;
    int maxRetries = 3;
    int retryDelayMs = 1000;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(McpServerConfig, name, description, autoReconnect, maxRetries, retryDelayMs)
};

using ConnectionCallback = std::function<void(const std::string& serverId, ConnectionStatus status)>;
using MessageCallback = std::function<void(const std::string& serverId, const McpMessage& message)>;
using ErrorCallback = std::function<void(const std::string& serverId, const std::string& error)>;

struct McpServerInfo {
    std::string id;
    McpServerConfig config;
    std::unique_ptr<Transport> transport = nullptr;
    std::unique_ptr<McpTransportConfig> transportConfigJson = nullptr;
    ConnectionStatus status = ConnectionStatus::DISCONNECTED;
    std::chrono::steady_clock::time_point lastConnected;
    int retryCount = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(McpServerInfo, id, config, transport, transportConfigJson, status, lastConnected, retryCount)

    static McpServerInfo fromJson(const nlohmann::json& j) {
        McpServerInfo info;
        j.get_to(info);
        return info;
    }
};
}
};