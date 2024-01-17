#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Forward declarations of classes used in the AST
class AST;
class Expr;
class Goal;
class Factor;
class Assignment;
class Declaration;
class Loop;
class BE;
class Condition;
class BinaryOp;

// ASTVisitor class defines a visitor pattern to traverse the AST
class ASTVisitor
{
public:
    // Virtual visit functions for each AST node type
    virtual void visit(AST &) {}           // Visit the base AST node
    virtual void visit(Expr &) {}          // Visit the expression node
    virtual void visit(Goal &) = 0;        // Visit the group of expressions node
    virtual void visit(Factor &) = 0;      // Visit the factor nodes
    virtual void visit(Assignment &) = 0;  // Visit the assignment expression node
    virtual void visit(Declaration &) = 0; // Visit the variable declaration node
    virtual void visit(Loop &) = 0;        // Visit the Loop node
    virtual void visit(BE &) = 0;          // Visit the BE node
    virtual void visit(Condition &) = 0;   // Visit the Condition node
    virtual void visit(BinaryOp &) = 0;    // Visit the binary operation node
};

// AST class serves as the base class for all AST nodes
class AST
{
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor &V) = 0; // Accept a visitor for traversal
};

// Expr class represents an expression in the AST
class Expr : public AST
{
public:
    Expr() {}
};

// Goal class represents a group of expressions in the AST
class Goal : public Expr
{
    using ExprVector = llvm::SmallVector<Expr *>;

private:
    ExprVector exprs; // Stores the list of expressions

public:
    Goal(llvm::SmallVector<Expr *> exprs) : exprs(exprs) {}

    void setExprs(llvm::SmallVector<Expr *> expressions) {
        exprs = expressions;
    }

    llvm::SmallVector<Expr *> getExprs() { return exprs; }

    ExprVector::const_iterator begin() { return exprs.begin(); }

    ExprVector::const_iterator end() { return exprs.end(); }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

// Factor class represents a factor in the AST (either an identifier or a number)
class Factor : public Expr
{
public:
    enum ValueKind
    {
        Ident,
        Number
    };

private:
    ValueKind Kind;      // Stores the kind of factor (identifier or number)
    llvm::StringRef Val; // Stores the value of the factor

public:
    Factor(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}

    ValueKind getKind() { return Kind; }

    llvm::StringRef getVal() { return Val; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

// BinaryOp class represents a binary operation in the AST (plus, minus, multiplication, division)
class BinaryOp : public Expr
{
public:
    enum Operator
    {
        Plus,
        Minus,
        Mul,
        Div,
        Power,
        Remain,
        Or,
        And,
        Equal_equal,
        Not_equal,
        More_equal,
        Less_equal,
        Less,
        More
    };

private:
    Expr *Left;  // Left-hand side expression
    Expr *Right; // Right-hand side expression
    Operator Op; // Operator of the binary operation

public:
    BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R) {}

    Expr *getLeft() { return Left; }

    Expr *getRight() { return Right; }

    Operator getOperator() { return Op; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

// Assignment class represents an assignment expression in the AST
class Assignment : public Expr
{
private:
    Factor *Left; // Left-hand side factor (identifier)
    Expr *Right;  // Right-hand side expression

public:
    Assignment(Factor *L, Expr *R) : Left(L), Right(R) {}

    Factor *getLeft() { return Left; }

    Expr *getRight() { return Right; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

// Declaration class represents a variable declaration with an initializer in the AST
class Declaration : public Expr
{
    using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
    using ExprVector = llvm::SmallVector<Expr *>;

    VarVector Vars;     // Stores the list of variables
    ExprVector Numbers; // Stores the list of numbers
                        // Expression serving as the initializer

public:
    Declaration(llvm::SmallVector<llvm::StringRef, 8> Vars, llvm::SmallVector<Expr *> Numbers) : Vars(Vars), Numbers(Numbers) {}

    VarVector::const_iterator begin() { return Vars.begin(); }

    VarVector::const_iterator end() { return Vars.end(); }

    ExprVector::const_iterator begin_values() { return Numbers.begin(); }

    ExprVector::const_iterator end_values() { return Numbers.end(); }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

// BE class represents a variable Begin End with an initializer in the AST
class BE : public Expr
{

    using ExprVector = llvm::SmallVector<Assignment *>;

private:
    ExprVector assigns; // Stores the list of expressions

public:
    BE(llvm::SmallVector<Assignment *> assigns) : assigns(assigns) {}

    ExprVector::const_iterator begin() { return assigns.begin(); }

    ExprVector::const_iterator end() { return assigns.end(); }

    llvm::SmallVector<Assignment *> getAssigns() { return assigns; }


    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};
class Loop : public Expr
{
    Expr *E; // Expression serving as the initializer
    BE *B;   // Begin end  serving as the initializer

public:
    Loop(Expr *E, BE *B) : E(E), B(B) {}

    Expr *getExpr() { return E; }

    BE *getBE() { return B; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};
class Condition : public Expr
{

    using ExprVector = llvm::SmallVector<Expr *>;
    using BEVector = llvm::SmallVector<BE *>;


private:
    ExprVector exprs; // Stores the list of expressions
    BEVector bes;   // Stores the list of bes

public:
    Condition(llvm::SmallVector<Expr *> exprs, llvm::SmallVector<BE *> bes) : exprs(exprs), bes(bes) {}

    ExprVector::const_iterator exprs_begin() { return exprs.begin(); }

    ExprVector::const_iterator exprs_end() { return exprs.end(); }

    llvm::SmallVector<BE *> getAllBes() { return bes; }

    BEVector::const_iterator bes_begin() { return bes.begin(); }

    BEVector::const_iterator bes_end() { return bes.end(); }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

#endif