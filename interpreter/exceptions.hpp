#ifndef WOMUYURO_INTERPRETER_EXCEPTIONS
#define WOMUYURO_INTERPRETER_EXCEPTIONS
#include <stdexcept>
namespace WomuYuro::interpreter {
class InterpreterError : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
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
}  // namespace WomuYuro::interpreter
#endif
