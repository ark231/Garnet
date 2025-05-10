#ifndef GARNET_INTERPRETER_EXCEPTIONS
#define GARNET_INTERPRETER_EXCEPTIONS
#include <stacktrace>
#include <stdexcept>

#include "location.hpp"
namespace Garnet::interpreter {
class InterpreterError : public std::runtime_error {
    std::stacktrace trace_;
    location::SourceRegion location_;

   public:
    InterpreterError(std::string message, location::SourceRegion location)
        : std::runtime_error(message), location_(location) {
        trace_ = std::stacktrace::current();
    }
    std::stacktrace trace() const { return trace_; }
    location::SourceRegion location() const { return location_; }
    virtual ~InterpreterError() = default;
};
class ExecutionError : public InterpreterError {
   public:
    using InterpreterError::InterpreterError;
};
class CriticalExcecutionError : public ExecutionError {
   public:
    using ExecutionError::ExecutionError;
};
class NonCriticalExcecutionError : public ExecutionError {
   public:
    using ExecutionError::ExecutionError;
};
class UnImplementedError : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
class InvalidRedeclarationError : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
class SyntaxError : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
class TypeError : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
class NameError : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
class InvalidArgument : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
class AssertionError : public CriticalExcecutionError {
   public:
    using CriticalExcecutionError::CriticalExcecutionError;
};
}  // namespace Garnet::interpreter
#endif
