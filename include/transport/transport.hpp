#pragma once
#include <string>
#include <functional>
#include <variant>
#include "../type/mcp_type.hpp"

namespace mcp {

class Transport {
public:
    using MessageHandler = std::function<void(const std::string&)>;

    using Config = std::variant<type::HttpConfig, type::SseConfig, type::WebSocketConfig>;

    virtual ~Transport() = default;

    virtual void send(const std::string& message) = 0;
    virtual void start(MessageHandler onMessage) = 0;
    virtual void stop() = 0;

    // Retourne le type/identifiant de configuration du transport
    virtual Config getConfig() const = 0;
};

}