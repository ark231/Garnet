#ifndef GARNET_LIBS_AST_VISITOR_VISITOR
#define GARNET_LIBS_AST_VISITOR_VISITOR

namespace Garnet {
namespace ast {
class FunctionDecl;
class VariableDecl;
class TypeDecl;
class ErrorNode;
class ErrorSentence;
class ErrorExpression;
class ErrorStatement;
class BinaryOperator;
class VariableReference;
class SignedIntegerLiteral;
class UnsignedIntegerLiteral;
class FloatingPointLiteral;
class FunctionCall;
class CompilationUnit;
class FunctionDecl;
class VariableDecl;
class TypeDecl;
class FunctionDef;
class VariableDeclStatement;
class ReturnStatement;
class Block;
class LoopStatement;
class BreakStatement;
class IfStatement;
class Visitor {
   public:
    virtual ~Visitor() = default;

    virtual void visit(const ast::FunctionDecl*) = 0;
    virtual void visit(const ast::VariableDecl*) = 0;
    virtual void visit(const ast::TypeDecl*) = 0;
    virtual void visit(const ast::ErrorNode*) = 0;
    virtual void visit(const ast::ErrorSentence*) = 0;
    virtual void visit(const ast::ErrorExpression*) = 0;
    virtual void visit(const ast::ErrorStatement*) = 0;
    virtual void visit(const ast::BinaryOperator*) = 0;
    virtual void visit(const ast::VariableReference*) = 0;
    virtual void visit(const ast::SignedIntegerLiteral*) = 0;
    virtual void visit(const ast::UnsignedIntegerLiteral*) = 0;
    virtual void visit(const ast::FloatingPointLiteral*) = 0;
    virtual void visit(const ast::FunctionCall*) = 0;
    virtual void visit(const ast::CompilationUnit*) = 0;
    virtual void visit(const ast::FunctionDef*) = 0;
    virtual void visit(const ast::VariableDeclStatement*) = 0;
    virtual void visit(const ast::ReturnStatement*) = 0;
    virtual void visit(const ast::Block*) = 0;
    virtual void visit(const ast::LoopStatement*) = 0;
    // virtual void visit(const ast::BreakStatement*) = 0;
    virtual void visit(const ast::IfStatement*) = 0;
};
}  // namespace ast
};  // namespace Garnet
#endif
