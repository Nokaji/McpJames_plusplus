#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

namespace mcp {

// ============================================================================
// Common Types and Enums
// ============================================================================

using RequestId = std::variant<std::string, int64_t>;
using ProgressToken = std::variant<std::string, int64_t>;
using Cursor = std::string;

enum class Role {
    User,
    Assistant
};

enum class LoggingLevel {
    Debug,
    Info,
    Notice,
    Warning,
    Error,
    Critical,
    Alert,
    Emergency
};

// ============================================================================
// Base Metadata Structure
// ============================================================================

struct BaseMetadata {
    std::string name;
    boost::optional<std::string> title;
};

// ============================================================================
// Annotations
// ============================================================================

struct Annotations {
    boost::optional<std::vector<Role>> audience;
    boost::optional<std::string> lastModified;  // ISO 8601 formatted string
    boost::optional<double> priority;  // 0.0 to 1.0
};

// ============================================================================
// Content Blocks
// ============================================================================

struct TextContent {
    std::string type = "text";
    std::string text;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct ImageContent {
    std::string type = "image";
    std::string data;  // base64-encoded
    std::string mimeType;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct AudioContent {
    std::string type = "audio";
    std::string data;  // base64-encoded
    std::string mimeType;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct ResourceLink {
    std::string type = "resource_link";
    std::string uri;
    std::string name;
    boost::optional<std::string> title;
    boost::optional<std::string> description;
    boost::optional<std::string> mimeType;
    boost::optional<int64_t> size;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct TextResourceContents {
    std::string uri;
    std::string text;
    boost::optional<std::string> mimeType;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct BlobResourceContents {
    std::string uri;
    std::string blob;  // base64-encoded
    boost::optional<std::string> mimeType;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct EmbeddedResource {
    std::string type = "resource";
    std::variant<TextResourceContents, BlobResourceContents> resource;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

using ContentBlock = std::variant<TextContent, ImageContent, AudioContent, ResourceLink, EmbeddedResource>;

// ============================================================================
// Schema Definitions
// ============================================================================

struct StringSchema {
    std::string type = "string";
    boost::optional<std::string> description;
    boost::optional<std::string> title;
    boost::optional<std::string> format;  // "uri", "email", "date", "date-time"
    boost::optional<int64_t> minLength;
    boost::optional<int64_t> maxLength;
};

struct NumberSchema {
    std::string type;  // "number" or "integer"
    boost::optional<std::string> description;
    boost::optional<std::string> title;
    boost::optional<double> minimum;
    boost::optional<double> maximum;
};

struct BooleanSchema {
    std::string type = "boolean";
    boost::optional<std::string> description;
    boost::optional<std::string> title;
    boost::optional<bool> defaultValue;
};

struct EnumSchema {
    std::string type = "string";
    std::vector<std::string> enumValues;
    boost::optional<std::vector<std::string>> enumNames;
    boost::optional<std::string> description;
    boost::optional<std::string> title;
};

using PrimitiveSchemaDefinition = std::variant<StringSchema, NumberSchema, BooleanSchema, EnumSchema>;

// ============================================================================
// Tool Structures
// ============================================================================

struct ToolAnnotations {
    boost::optional<std::string> title;
    boost::optional<bool> readOnlyHint;
    boost::optional<bool> destructiveHint;
    boost::optional<bool> idempotentHint;
    boost::optional<bool> openWorldHint;
};

struct InputSchema {
    std::string type = "object";
    boost::optional<std::map<std::string, nlohmann::json>> properties;
    boost::optional<std::vector<std::string>> required;
};

struct OutputSchema {
    std::string type = "object";
    boost::optional<std::map<std::string, nlohmann::json>> properties;
    boost::optional<std::vector<std::string>> required;
};

struct Tool : BaseMetadata {
    boost::optional<std::string> description;
    InputSchema inputSchema;
    boost::optional<OutputSchema> outputSchema;
    boost::optional<ToolAnnotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

// ============================================================================
// Prompt Structures
// ============================================================================

struct PromptArgument : BaseMetadata {
    boost::optional<std::string> description;
    boost::optional<bool> required;
};

struct Prompt : BaseMetadata {
    boost::optional<std::string> description;
    boost::optional<std::vector<PromptArgument>> arguments;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct PromptMessage {
    Role role;
    ContentBlock content;
};

struct PromptReference : BaseMetadata {
    std::string type = "ref/prompt";
};

// ============================================================================
// Resource Structures
// ============================================================================

struct Resource : BaseMetadata {
    std::string uri;
    boost::optional<std::string> description;
    boost::optional<std::string> mimeType;
    boost::optional<int64_t> size;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct ResourceTemplate : BaseMetadata {
    std::string uriTemplate;  // RFC 6570
    boost::optional<std::string> description;
    boost::optional<std::string> mimeType;
    boost::optional<Annotations> annotations;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct ResourceTemplateReference {
    std::string type = "ref/resource";
    std::string uri;
};

// ============================================================================
// Sampling and Model Structures
// ============================================================================

struct SamplingMessage {
    Role role;
    std::variant<TextContent, ImageContent, AudioContent> content;
};

struct ModelHint {
    boost::optional<std::string> name;
};

struct ModelPreferences {
    boost::optional<std::vector<ModelHint>> hints;
    boost::optional<double> costPriority;  // 0.0 to 1.0
    boost::optional<double> speedPriority;  // 0.0 to 1.0
    boost::optional<double> intelligencePriority;  // 0.0 to 1.0
};

// ============================================================================
// Root Structures
// ============================================================================

struct Root {
    std::string uri;  // Must start with file://
    boost::optional<std::string> name;
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

// ============================================================================
// Capabilities
// ============================================================================

struct ClientCapabilities {
    struct RootsCapability {
        boost::optional<bool> listChanged;
    };
    
    struct SamplingCapability {
        // Empty for now
    };
    
    struct ElicitationCapability {
        // Empty for now
    };
    
    boost::optional<RootsCapability> roots;
    boost::optional<SamplingCapability> sampling;
    boost::optional<ElicitationCapability> elicitation;
    boost::optional<std::map<std::string, nlohmann::json>> experimental;
};

struct ServerCapabilities {
    struct PromptsCapability {
        boost::optional<bool> listChanged;
    };
    
    struct ResourcesCapability {
        boost::optional<bool> listChanged;
        boost::optional<bool> subscribe;
    };
    
    struct ToolsCapability {
        boost::optional<bool> listChanged;
    };
    
    struct LoggingCapability {
        // Empty for now
    };
    
    struct CompletionsCapability {
        // Empty for now
    };
    
    boost::optional<PromptsCapability> prompts;
    boost::optional<ResourcesCapability> resources;
    boost::optional<ToolsCapability> tools;
    boost::optional<LoggingCapability> logging;
    boost::optional<CompletionsCapability> completions;
    boost::optional<std::map<std::string, nlohmann::json>> experimental;
};

// ============================================================================
// Implementation Info
// ============================================================================

struct Implementation : BaseMetadata {
    std::string version;
};

// ============================================================================
// JSON-RPC Base Structures
// ============================================================================

struct Result {
    boost::optional<std::map<std::string, nlohmann::json>> _meta;
};

struct JSONRPCRequest {
    std::string jsonrpc = "2.0";
    RequestId id;
    std::string method;
    
    struct Params {
        boost::optional<std::map<std::string, nlohmann::json>> _meta;
        boost::optional<ProgressToken> progressToken;
        std::map<std::string, nlohmann::json> data;
    };
    
    boost::optional<Params> params;
};

struct JSONRPCResponse {
    std::string jsonrpc = "2.0";
    RequestId id;
    nlohmann::json result;
};

struct JSONRPCError {
    std::string jsonrpc = "2.0";
    RequestId id;
    
    struct Error {
        int code;
        std::string message;
        boost::optional<nlohmann::json> data;
    };
    
    Error error;
};

struct JSONRPCNotification {
    std::string jsonrpc = "2.0";
    std::string method;
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

// ============================================================================
// Request/Response Structures
// ============================================================================

// Initialize
struct InitializeRequest {
    std::string method = "initialize";
    
    struct Params {
        std::string protocolVersion;
        ClientCapabilities capabilities;
        Implementation clientInfo;
    };
    
    Params params;
};

struct InitializeResult : Result {
    std::string protocolVersion;
    ServerCapabilities capabilities;
    Implementation serverInfo;
    boost::optional<std::string> instructions;
};

// List Tools
struct ListToolsRequest {
    std::string method = "tools/list";
    
    struct Params {
        boost::optional<Cursor> cursor;
    };
    
    boost::optional<Params> params;
};

struct ListToolsResult : Result {
    std::vector<Tool> tools;
    boost::optional<Cursor> nextCursor;
};

// Call Tool
struct CallToolRequest {
    std::string method = "tools/call";
    
    struct Params {
        std::string name;
        boost::optional<std::map<std::string, nlohmann::json>> arguments;
    };
    
    Params params;
};

struct CallToolResult : Result {
    std::vector<ContentBlock> content;
    boost::optional<bool> isError;
    boost::optional<nlohmann::json> structuredContent;
};

// List Prompts
struct ListPromptsRequest {
    std::string method = "prompts/list";
    
    struct Params {
        boost::optional<Cursor> cursor;
    };
    
    boost::optional<Params> params;
};

struct ListPromptsResult : Result {
    std::vector<Prompt> prompts;
    boost::optional<Cursor> nextCursor;
};

// Get Prompt
struct GetPromptRequest {
    std::string method = "prompts/get";
    
    struct Params {
        std::string name;
        boost::optional<std::map<std::string, std::string>> arguments;
    };
    
    Params params;
};

struct GetPromptResult : Result {
    std::vector<PromptMessage> messages;
    boost::optional<std::string> description;
};

// List Resources
struct ListResourcesRequest {
    std::string method = "resources/list";
    
    struct Params {
        boost::optional<Cursor> cursor;
    };
    
    boost::optional<Params> params;
};

struct ListResourcesResult : Result {
    std::vector<Resource> resources;
    boost::optional<Cursor> nextCursor;
};

// Read Resource
struct ReadResourceRequest {
    std::string method = "resources/read";
    
    struct Params {
        std::string uri;
    };
    
    Params params;
};

struct ReadResourceResult : Result {
    std::vector<std::variant<TextResourceContents, BlobResourceContents>> contents;
};

// Subscribe/Unsubscribe Resource
struct SubscribeRequest {
    std::string method = "resources/subscribe";
    
    struct Params {
        std::string uri;
    };
    
    Params params;
};

struct UnsubscribeRequest {
    std::string method = "resources/unsubscribe";
    
    struct Params {
        std::string uri;
    };
    
    Params params;
};

// List Resource Templates
struct ListResourceTemplatesRequest {
    std::string method = "resources/templates/list";
    
    struct Params {
        boost::optional<Cursor> cursor;
    };
    
    boost::optional<Params> params;
};

struct ListResourceTemplatesResult : Result {
    std::vector<ResourceTemplate> resourceTemplates;
    boost::optional<Cursor> nextCursor;
};

// Completion
struct CompleteRequest {
    std::string method = "completion/complete";
    
    struct Params {
        std::variant<PromptReference, ResourceTemplateReference> ref;
        
        struct Argument {
            std::string name;
            std::string value;
        };
        
        Argument argument;
        
        struct Context {
            boost::optional<std::map<std::string, std::string>> arguments;
        };
        
        boost::optional<Context> context;
    };
    
    Params params;
};

struct CompleteResult : Result {
    struct Completion {
        std::vector<std::string> values;
        boost::optional<int64_t> total;
        boost::optional<bool> hasMore;
    };
    
    Completion completion;
};

// Logging
struct SetLevelRequest {
    std::string method = "logging/setLevel";
    
    struct Params {
        LoggingLevel level;
    };
    
    Params params;
};

// Sampling
struct CreateMessageRequest {
    std::string method = "sampling/createMessage";
    
    struct Params {
        std::vector<SamplingMessage> messages;
        boost::optional<std::string> systemPrompt;
        int64_t maxTokens;
        boost::optional<ModelPreferences> modelPreferences;
        boost::optional<std::vector<std::string>> stopSequences;
        boost::optional<double> temperature;
        boost::optional<std::string> includeContext;  // "none", "thisServer", "allServers"
        boost::optional<nlohmann::json> metadata;
    };
    
    Params params;
};

struct CreateMessageResult : Result {
    Role role;
    std::variant<TextContent, ImageContent, AudioContent> content;
    std::string model;
    boost::optional<std::string> stopReason;
};

// Elicitation
struct ElicitRequest {
    std::string method = "elicitation/create";
    
    struct Params {
        std::string message;
        
        struct RequestedSchema {
            std::string type = "object";
            std::map<std::string, PrimitiveSchemaDefinition> properties;
            boost::optional<std::vector<std::string>> required;
        };
        
        RequestedSchema requestedSchema;
    };
    
    Params params;
};

struct ElicitResult : Result {
    std::string action;  // "accept", "decline", "cancel"
    boost::optional<std::map<std::string, std::variant<std::string, double, bool>>> content;
};

// List Roots
struct ListRootsRequest {
    std::string method = "roots/list";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

struct ListRootsResult : Result {
    std::vector<Root> roots;
};

// Ping
struct PingRequest {
    std::string method = "ping";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

// ============================================================================
// Notifications
// ============================================================================

struct InitializedNotification {
    std::string method = "notifications/initialized";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

struct ProgressNotification {
    std::string method = "notifications/progress";
    
    struct Params {
        ProgressToken progressToken;
        double progress;
        boost::optional<double> total;
        boost::optional<std::string> message;
    };
    
    Params params;
};

struct CancelledNotification {
    std::string method = "notifications/cancelled";
    
    struct Params {
        RequestId requestId;
        boost::optional<std::string> reason;
    };
    
    Params params;
};

struct LoggingMessageNotification {
    std::string method = "notifications/message";
    
    struct Params {
        LoggingLevel level;
        nlohmann::json data;
        boost::optional<std::string> logger;
    };
    
    Params params;
};

struct ResourceUpdatedNotification {
    std::string method = "notifications/resources/updated";
    
    struct Params {
        std::string uri;
    };
    
    Params params;
};

struct ResourceListChangedNotification {
    std::string method = "notifications/resources/list_changed";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

struct ToolListChangedNotification {
    std::string method = "notifications/tools/list_changed";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

struct PromptListChangedNotification {
    std::string method = "notifications/prompts/list_changed";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

struct RootsListChangedNotification {
    std::string method = "notifications/roots/list_changed";
    boost::optional<std::map<std::string, nlohmann::json>> params;
};

// ============================================================================
// Utility Type Aliases
// ============================================================================

using EmptyResult = Result;

} // namespace mcp
