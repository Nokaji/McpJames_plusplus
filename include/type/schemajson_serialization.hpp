#pragma once
#include "schemajson.hpp"
#include <nlohmann/json.hpp>

namespace mcp {
namespace type {
// ============================================================================
// JSON Serialization for Enums
// ============================================================================

NLOHMANN_JSON_SERIALIZE_ENUM(Role, {
    {Role::User, "user"},
    {Role::Assistant, "assistant"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(LoggingLevel, {
    {LoggingLevel::Debug, "debug"},
    {LoggingLevel::Info, "info"},
    {LoggingLevel::Notice, "notice"},
    {LoggingLevel::Warning, "warning"},
    {LoggingLevel::Error, "error"},
    {LoggingLevel::Critical, "critical"},
    {LoggingLevel::Alert, "alert"},
    {LoggingLevel::Emergency, "emergency"}
})

// ============================================================================
// JSON Serialization for Base Structures
// ============================================================================

inline void to_json(nlohmann::json& j, const BaseMetadata& b) {
    j = nlohmann::json{{"name", b.name}};
    if (b.title) j["title"] = *b.title;
}

inline void from_json(const nlohmann::json& j, BaseMetadata& b) {
    j.at("name").get_to(b.name);
    if (j.contains("title")) b.title = j.at("title").get<std::string>();
}

inline void to_json(nlohmann::json& j, const Annotations& a) {
    j = nlohmann::json::object();
    if (a.audience) j["audience"] = *a.audience;
    if (a.lastModified) j["lastModified"] = *a.lastModified;
    if (a.priority) j["priority"] = *a.priority;
}

inline void from_json(const nlohmann::json& j, Annotations& a) {
    if (j.contains("audience")) a.audience = j.at("audience").get<std::vector<Role>>();
    if (j.contains("lastModified")) a.lastModified = j.at("lastModified").get<std::string>();
    if (j.contains("priority")) a.priority = j.at("priority").get<double>();
}

// ============================================================================
// JSON Serialization for Content Blocks
// ============================================================================

inline void to_json(nlohmann::json& j, const TextContent& t) {
    j = nlohmann::json{
        {"type", "text"},
        {"text", t.text}
    };
    if (t.annotations) j["annotations"] = *t.annotations;
    if (t._meta) j["_meta"] = *t._meta;
}

inline void from_json(const nlohmann::json& j, TextContent& t) {
    j.at("text").get_to(t.text);
    if (j.contains("annotations")) t.annotations = j.at("annotations").get<Annotations>();
    if (j.contains("_meta")) t._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

inline void to_json(nlohmann::json& j, const ImageContent& i) {
    j = nlohmann::json{
        {"type", "image"},
        {"data", i.data},
        {"mimeType", i.mimeType}
    };
    if (i.annotations) j["annotations"] = *i.annotations;
    if (i._meta) j["_meta"] = *i._meta;
}

inline void from_json(const nlohmann::json& j, ImageContent& i) {
    j.at("data").get_to(i.data);
    j.at("mimeType").get_to(i.mimeType);
    if (j.contains("annotations")) i.annotations = j.at("annotations").get<Annotations>();
    if (j.contains("_meta")) i._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

inline void to_json(nlohmann::json& j, const AudioContent& a) {
    j = nlohmann::json{
        {"type", "audio"},
        {"data", a.data},
        {"mimeType", a.mimeType}
    };
    if (a.annotations) j["annotations"] = *a.annotations;
    if (a._meta) j["_meta"] = *a._meta;
}

inline void from_json(const nlohmann::json& j, AudioContent& a) {
    j.at("data").get_to(a.data);
    j.at("mimeType").get_to(a.mimeType);
    if (j.contains("annotations")) a.annotations = j.at("annotations").get<Annotations>();
    if (j.contains("_meta")) a._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

// ============================================================================
// JSON Serialization for Tool Structures
// ============================================================================

inline void to_json(nlohmann::json& j, const ToolAnnotations& t) {
    j = nlohmann::json::object();
    if (t.title) j["title"] = *t.title;
    if (t.readOnlyHint) j["readOnlyHint"] = *t.readOnlyHint;
    if (t.destructiveHint) j["destructiveHint"] = *t.destructiveHint;
    if (t.idempotentHint) j["idempotentHint"] = *t.idempotentHint;
    if (t.openWorldHint) j["openWorldHint"] = *t.openWorldHint;
}

inline void from_json(const nlohmann::json& j, ToolAnnotations& t) {
    if (j.contains("title")) t.title = j.at("title").get<std::string>();
    if (j.contains("readOnlyHint")) t.readOnlyHint = j.at("readOnlyHint").get<bool>();
    if (j.contains("destructiveHint")) t.destructiveHint = j.at("destructiveHint").get<bool>();
    if (j.contains("idempotentHint")) t.idempotentHint = j.at("idempotentHint").get<bool>();
    if (j.contains("openWorldHint")) t.openWorldHint = j.at("openWorldHint").get<bool>();
}

inline void to_json(nlohmann::json& j, const InputSchema& i) {
    j = nlohmann::json{{"type", "object"}};
    if (i.properties) j["properties"] = *i.properties;
    if (i.required) j["required"] = *i.required;
}

inline void from_json(const nlohmann::json& j, InputSchema& i) {
    if (j.contains("properties")) i.properties = j.at("properties").get<std::map<std::string, nlohmann::json>>();
    if (j.contains("required")) i.required = j.at("required").get<std::vector<std::string>>();
}

inline void to_json(nlohmann::json& j, const OutputSchema& o) {
    j = nlohmann::json{{"type", "object"}};
    if (o.properties) j["properties"] = *o.properties;
    if (o.required) j["required"] = *o.required;
}

inline void from_json(const nlohmann::json& j, OutputSchema& o) {
    if (j.contains("properties")) o.properties = j.at("properties").get<std::map<std::string, nlohmann::json>>();
    if (j.contains("required")) o.required = j.at("required").get<std::vector<std::string>>();
}

inline void to_json(nlohmann::json& j, const Tool& t) {
    j = nlohmann::json{
        {"name", t.name},
        {"inputSchema", t.inputSchema}
    };
    if (t.title) j["title"] = *t.title;
    if (t.description) j["description"] = *t.description;
    if (t.outputSchema) j["outputSchema"] = *t.outputSchema;
    if (t.annotations) j["annotations"] = *t.annotations;
    if (t._meta) j["_meta"] = *t._meta;
}

inline void from_json(const nlohmann::json& j, Tool& t) {
    j.at("name").get_to(t.name);
    j.at("inputSchema").get_to(t.inputSchema);
    if (j.contains("title")) t.title = j.at("title").get<std::string>();
    if (j.contains("description")) t.description = j.at("description").get<std::string>();
    if (j.contains("outputSchema")) {
        t.outputSchema = j.at("outputSchema").get<OutputSchema>();
    }
    if (j.contains("annotations")) t.annotations = j.at("annotations").get<ToolAnnotations>();
    if (j.contains("_meta")) t._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

// ============================================================================
// JSON Serialization for Resource Structures
// ============================================================================

inline void to_json(nlohmann::json& j, const Resource& r) {
    j = nlohmann::json{
        {"name", r.name},
        {"uri", r.uri}
    };
    if (r.title) j["title"] = *r.title;
    if (r.description) j["description"] = *r.description;
    if (r.mimeType) j["mimeType"] = *r.mimeType;
    if (r.size) j["size"] = *r.size;
    if (r.annotations) j["annotations"] = *r.annotations;
    if (r._meta) j["_meta"] = *r._meta;
}

inline void from_json(const nlohmann::json& j, Resource& r) {
    j.at("name").get_to(r.name);
    j.at("uri").get_to(r.uri);
    if (j.contains("title")) r.title = j.at("title").get<std::string>();
    if (j.contains("description")) r.description = j.at("description").get<std::string>();
    if (j.contains("mimeType")) r.mimeType = j.at("mimeType").get<std::string>();
    if (j.contains("size")) r.size = j.at("size").get<int64_t>();
    if (j.contains("annotations")) r.annotations = j.at("annotations").get<Annotations>();
    if (j.contains("_meta")) r._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

// ============================================================================
// JSON Serialization for Prompt Structures
// ============================================================================

inline void to_json(nlohmann::json& j, const PromptArgument& p) {
    j = nlohmann::json{{"name", p.name}};
    if (p.title) j["title"] = *p.title;
    if (p.description) j["description"] = *p.description;
    if (p.required) j["required"] = *p.required;
}

inline void from_json(const nlohmann::json& j, PromptArgument& p) {
    j.at("name").get_to(p.name);
    if (j.contains("title")) p.title = j.at("title").get<std::string>();
    if (j.contains("description")) p.description = j.at("description").get<std::string>();
    if (j.contains("required")) p.required = j.at("required").get<bool>();
}

inline void to_json(nlohmann::json& j, const Prompt& p) {
    j = nlohmann::json{{"name", p.name}};
    if (p.title) j["title"] = *p.title;
    if (p.description) j["description"] = *p.description;
    if (p.arguments) j["arguments"] = *p.arguments;
    if (p._meta) j["_meta"] = *p._meta;
}

inline void from_json(const nlohmann::json& j, Prompt& p) {
    j.at("name").get_to(p.name);
    if (j.contains("title")) p.title = j.at("title").get<std::string>();
    if (j.contains("description")) p.description = j.at("description").get<std::string>();
    if (j.contains("arguments")) p.arguments = j.at("arguments").get<std::vector<PromptArgument>>();
    if (j.contains("_meta")) p._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

// ============================================================================
// JSON Serialization for Implementation
// ============================================================================

inline void to_json(nlohmann::json& j, const Implementation& i) {
    j = nlohmann::json{
        {"name", i.name},
        {"version", i.version}
    };
    if (i.title) j["title"] = *i.title;
}

inline void from_json(const nlohmann::json& j, Implementation& i) {
    j.at("name").get_to(i.name);
    j.at("version").get_to(i.version);
    if (j.contains("title")) i.title = j.at("title").get<std::string>();
}

// ============================================================================
// JSON Serialization for Capabilities
// ============================================================================

inline void to_json(nlohmann::json& j, const ClientCapabilities& c) {
    j = nlohmann::json::object();
    if (c.roots) {
        nlohmann::json roots_obj = nlohmann::json::object();
        if (c.roots->listChanged) roots_obj["listChanged"] = *c.roots->listChanged;
        j["roots"] = roots_obj;
    }
    if (c.sampling) j["sampling"] = nlohmann::json::object();
    if (c.elicitation) j["elicitation"] = nlohmann::json::object();
    if (c.experimental) j["experimental"] = *c.experimental;
}

inline void from_json(const nlohmann::json& j, ClientCapabilities& c) {
    if (j.contains("roots")) {
        ClientCapabilities::RootsCapability roots;
        if (j["roots"].contains("listChanged")) {
            roots.listChanged = j["roots"]["listChanged"].get<bool>();
        }
        c.roots = roots;
    }
    if (j.contains("sampling")) c.sampling = ClientCapabilities::SamplingCapability{};
    if (j.contains("elicitation")) c.elicitation = ClientCapabilities::ElicitationCapability{};
    if (j.contains("experimental")) c.experimental = j.at("experimental").get<std::map<std::string, nlohmann::json>>();
}

inline void to_json(nlohmann::json& j, const ServerCapabilities& s) {
    j = nlohmann::json::object();
    
    if (s.prompts) {
        nlohmann::json prompts_obj = nlohmann::json::object();
        if (s.prompts->listChanged) prompts_obj["listChanged"] = *s.prompts->listChanged;
        j["prompts"] = prompts_obj;
    }
    
    if (s.resources) {
        nlohmann::json resources_obj = nlohmann::json::object();
        if (s.resources->listChanged) resources_obj["listChanged"] = *s.resources->listChanged;
        if (s.resources->subscribe) resources_obj["subscribe"] = *s.resources->subscribe;
        j["resources"] = resources_obj;
    }
    
    if (s.tools) {
        nlohmann::json tools_obj = nlohmann::json::object();
        if (s.tools->listChanged) tools_obj["listChanged"] = *s.tools->listChanged;
        j["tools"] = tools_obj;
    }
    
    if (s.logging) j["logging"] = nlohmann::json::object();
    if (s.completions) j["completions"] = nlohmann::json::object();
    if (s.experimental) j["experimental"] = *s.experimental;
}

inline void from_json(const nlohmann::json& j, ServerCapabilities& s) {
    if (j.contains("prompts")) {
        ServerCapabilities::PromptsCapability prompts;
        if (j["prompts"].contains("listChanged")) {
            prompts.listChanged = j["prompts"]["listChanged"].get<bool>();
        }
        s.prompts = prompts;
    }
    
    if (j.contains("resources")) {
        ServerCapabilities::ResourcesCapability resources;
        if (j["resources"].contains("listChanged")) {
            resources.listChanged = j["resources"]["listChanged"].get<bool>();
        }
        if (j["resources"].contains("subscribe")) {
            resources.subscribe = j["resources"]["subscribe"].get<bool>();
        }
        s.resources = resources;
    }
    
    if (j.contains("tools")) {
        ServerCapabilities::ToolsCapability tools;
        if (j["tools"].contains("listChanged")) {
            tools.listChanged = j["tools"]["listChanged"].get<bool>();
        }
        s.tools = tools;
    }
    
    if (j.contains("logging")) s.logging = ServerCapabilities::LoggingCapability{};
    if (j.contains("completions")) s.completions = ServerCapabilities::CompletionsCapability{};
    if (j.contains("experimental")) s.experimental = j.at("experimental").get<std::map<std::string, nlohmann::json>>();
}

// ============================================================================
// JSON Serialization for Request/Response Structures
// ============================================================================

inline void to_json(nlohmann::json& j, const InitializeRequest::Params& p) {
    j = nlohmann::json{
        {"protocolVersion", p.protocolVersion},
        {"capabilities", p.capabilities},
        {"clientInfo", p.clientInfo}
    };
}

inline void from_json(const nlohmann::json& j, InitializeRequest::Params& p) {
    j.at("protocolVersion").get_to(p.protocolVersion);
    j.at("capabilities").get_to(p.capabilities);
    j.at("clientInfo").get_to(p.clientInfo);
}

inline void to_json(nlohmann::json& j, const InitializeResult& r) {
    j = nlohmann::json{
        {"protocolVersion", r.protocolVersion},
        {"capabilities", r.capabilities},
        {"serverInfo", r.serverInfo}
    };
    if (r.instructions) j["instructions"] = *r.instructions;
    if (r._meta) j["_meta"] = *r._meta;
}

inline void from_json(const nlohmann::json& j, InitializeResult& r) {
    j.at("protocolVersion").get_to(r.protocolVersion);
    j.at("capabilities").get_to(r.capabilities);
    j.at("serverInfo").get_to(r.serverInfo);
    if (j.contains("instructions")) r.instructions = j.at("instructions").get<std::string>();
    if (j.contains("_meta")) r._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

inline void to_json(nlohmann::json& j, const ListToolsResult& r) {
    j = nlohmann::json{{"tools", r.tools}};
    if (r.nextCursor) j["nextCursor"] = *r.nextCursor;
    if (r._meta) j["_meta"] = *r._meta;
}

inline void from_json(const nlohmann::json& j, ListToolsResult& r) {
    j.at("tools").get_to(r.tools);
    if (j.contains("nextCursor")) r.nextCursor = j.at("nextCursor").get<std::string>();
    if (j.contains("_meta")) r._meta = j.at("_meta").get<std::map<std::string, nlohmann::json>>();
}

// Add more serialization functions as needed...
} // namespace type
} // namespace mcp