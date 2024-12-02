#include "Tracer.hpp"
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/provider.h>

namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace jaeger = opentelemetry::exporter::jaeger;

Tracer::Tracer(const std::string& service_name) {
    auto exporter = std::unique_ptr<jaeger::JaegerExporter>(
        new jaeger::JaegerExporter()
    );

    auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
        new trace_sdk::SimpleSpanProcessor(std::move(exporter))
    );

    auto provider = std::shared_ptr<trace::TracerProvider>(
        new trace_sdk::TracerProvider(std::move(processor))
    );

    trace::Provider::SetTracerProvider(provider);
    
    tracer_ = provider->GetTracer(service_name);
}

std::shared_ptr<trace::Span> Tracer::startSpan(
    const std::string& name,
    const std::unordered_map<std::string, std::string>& attributes
) {
    auto span = tracer_->StartSpan(name);
    
    for (const auto& [key, value] : attributes) {
        span->SetAttribute(key, value);
    }
    
    return span;
}

void Tracer::addEvent(
    const std::shared_ptr<trace::Span>& span,
    const std::string& name,
    const std::unordered_map<std::string, std::string>& attributes
) {
    if (span) {
        span->AddEvent(name);
        for (const auto& [key, value] : attributes) {
            span->SetAttribute(key, value);
        }
    }
}

void Tracer::setError(
    const std::shared_ptr<trace::Span>& span,
    const std::string& error_message
) {
    if (span) {
        span->SetStatus(trace::StatusCode::kError, error_message);
    }
} 