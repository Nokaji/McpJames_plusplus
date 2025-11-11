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
#include <nlohmann/json.hpp>

namespace mcp {
// Forward declaration
class Transport;

namespace type {

struct HttpConfig {
    std::string baseUrl;
    std::map<std::string, std::string> headers;
    int timeoutMs = 30000;
    bool verifySSL = true;
};

// JSON (de)serialization helpers for HttpConfig
inline void to_json(nlohmann::json &j, const HttpConfig &c) {
    j = nlohmann::json{
        {"baseUrl", c.baseUrl},
        {"headers", c.headers},
        {"timeoutMs", c.timeoutMs},
        {"verifySSL", c.verifySSL}
    };
}
inline void from_json(const nlohmann::json &j, HttpConfig &c) {
    c.baseUrl = j.value("baseUrl", "");
    c.headers = j.value("headers", std::map<std::string, std::string>{});
    c.timeoutMs = j.value("timeoutMs", 30000);
    c.verifySSL = j.value("verifySSL", true);
}

struct WebSocketConfig {
    std::string url;
    std::map<std::string, std::string> headers;
    int timeoutMs = 30000;
    bool verifySSL = true;
};

inline void to_json(nlohmann::json &j, const WebSocketConfig &c) {
    j = nlohmann::json{
        {"url", c.url},
        {"headers", c.headers},
        {"timeoutMs", c.timeoutMs},
        {"verifySSL", c.verifySSL}
    };
}
inline void from_json(const nlohmann::json &j, WebSocketConfig &c) {
    c.url = j.value("url", "");
    c.headers = j.value("headers", std::map<std::string, std::string>{});
    c.timeoutMs = j.value("timeoutMs", 30000);
    c.verifySSL = j.value("verifySSL", true);
}

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

inline void to_json(nlohmann::json &j, const SseConfig &c) {
    j = nlohmann::json{
        {"url", c.url},
        {"sseEndpoint", c.sseEndpoint},
        {"messageEndpoint", c.messageEndpoint},
        {"headers", c.headers},
        {"timeoutMs", c.timeoutMs},
        {"verifySSL", c.verifySSL},
        {"reconnectDelayMs", c.reconnectDelayMs},
        {"maxRetries", c.maxRetries},
        {"lastEventId", c.lastEventId}
    };
}
inline void from_json(const nlohmann::json &j, SseConfig &c) {
    c.url = j.value("url", "");
    c.sseEndpoint = j.value("sseEndpoint", "/sse");
    c.messageEndpoint = j.value("messageEndpoint", "/message");
    c.headers = j.value("headers", std::map<std::string, std::string>{});
    c.timeoutMs = j.value("timeoutMs", 30000);
    c.verifySSL = j.value("verifySSL", true);
    c.reconnectDelayMs = j.value("reconnectDelayMs", 3000);
    c.maxRetries = j.value("maxRetries", -1);
    c.lastEventId = j.value("lastEventId", "");
}

enum class ConnectionStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    ERROR,
    RECONNECTING
};

NLOHMANN_JSON_SERIALIZE_ENUM(ConnectionStatus, {
    {ConnectionStatus::DISCONNECTED, "DISCONNECTED"},
    {ConnectionStatus::CONNECTING,   "CONNECTING"},
    {ConnectionStatus::CONNECTED,    "CONNECTED"},
    {ConnectionStatus::ERROR,        "ERROR"},
    {ConnectionStatus::RECONNECTING, "RECONNECTING"}
})

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

    // Removed NLOHMANN_DEFINE_TYPE_INTRUSIVE because std::variant is not directly supported.
};

// Custom JSON (de)serialization for McpTransportConfig
inline void to_json(nlohmann::json &j, const McpTransportConfig &mtc) {
    j["type"] = mtc.type; // uses enum serializer
    std::visit([&](auto const &cfg) {
        j["config"] = cfg; // relies on to_json for concrete config type
    }, mtc.config);
}

inline void from_json(const nlohmann::json &j, McpTransportConfig &mtc) {
    mtc.type = j.at("type").get<McpTransportConfig::TransportType>();
    // Detect transport type and deserialize accordingly
    if (!j.contains("config")) {
        // Provide default config depending on type
        switch (mtc.type) {
            case McpTransportConfig::TransportType::HTTP:
                mtc.config = HttpConfig{}; return;
            case McpTransportConfig::TransportType::WEBSOCKET:
                mtc.config = WebSocketConfig{}; return;
            case McpTransportConfig::TransportType::SSE:
                mtc.config = SseConfig{}; return;
        }
    }
    const auto &cjson = j.at("config");
    switch (mtc.type) {
        case McpTransportConfig::TransportType::HTTP: {
            mtc.config = cjson.get<HttpConfig>();
            break;
        }
        case McpTransportConfig::TransportType::WEBSOCKET: {
            mtc.config = cjson.get<WebSocketConfig>();
            break;
        }
        case McpTransportConfig::TransportType::SSE: {
            mtc.config = cjson.get<SseConfig>();
            break;
        }
    }
}

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
};

// Custom JSON (de)serialization for McpServerInfo that skips non-serializable members
inline void to_json(nlohmann::json& j, const McpServerInfo& info) {
    j = nlohmann::json{
        {"id", info.id},
        {"config", info.config},
        {"status", info.status},
        {"retryCount", info.retryCount}
    };
    if (info.transportConfigJson) {
        j["transportConfigJson"] = *info.transportConfigJson;
    }
    // 'transport' and 'lastConnected' intentionally omitted
}

inline void from_json(const nlohmann::json& j, McpServerInfo& info) {
    info.id = j.at("id").get<std::string>();
    if (j.contains("config")) {
        info.config = j.at("config").get<McpServerConfig>();
    }
    if (j.contains("status")) {
        info.status = j.at("status").get<ConnectionStatus>();
    }
    if (j.contains("retryCount")) {
        info.retryCount = j.at("retryCount").get<int>();
    }
    if (j.contains("transportConfigJson") && !j.at("transportConfigJson").is_null()) {
        auto tcfg = j.at("transportConfigJson").get<McpTransportConfig>();
        info.transportConfigJson = std::make_unique<McpTransportConfig>(std::move(tcfg));
    }
    // 'transport' and 'lastConnected' intentionally not deserialized
};
}
};