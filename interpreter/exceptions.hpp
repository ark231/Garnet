#ifndef GARNET_INTERPRETER_EXCEPTIONS
#define GARNET_INTERPRETER_EXCEPTIONS
#include <boost/stacktrace.hpp>
#include <stdexcept>
namespace Garnet::interpreter {
class InterpreterError : public std::runtime_error {
    boost::stacktrace::stacktrace trace_;

   public:
    InterpreterError(std::string message) : std::runtime_error(message) { trace_ = boost::stacktrace::stacktrace(); }
    boost::stacktrace::stacktrace trace() const { return trace_; }
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
}  // namespace Garnet::interpreter
#endif
