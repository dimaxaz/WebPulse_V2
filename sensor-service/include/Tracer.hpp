#pragma once

#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <string>
#include <memory>

class Tracer {
public:
    explicit Tracer(const std::string& service_name);
    
    std::shared_ptr<opentelemetry::trace::Span> startSpan(
        const std::string& name,
        const std::unordered_map<std::string, std::string>& attributes = {}
    );

    void addEvent(
        const std::shared_ptr<opentelemetry::trace::Span>& span,
        const std::string& name,
        const std::unordered_map<std::string, std::string>& attributes = {}
    );

    void setError(
        const std::shared_ptr<opentelemetry::trace::Span>& span,
        const std::string& error_message
    );

private:
    std::shared_ptr<opentelemetry::trace::TracerProvider> provider_;
    std::shared_ptr<opentelemetry::trace::Tracer> tracer_;
}; 