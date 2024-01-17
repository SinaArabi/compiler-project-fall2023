#include "AST.h"
#include "iostream"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;

  class OptimizeAST : public ASTVisitor
  {
    bool right_alive = false;
    llvm::SmallVector<Expr *> exprs;
    llvm::SmallVector<llvm::StringRef> live_var;
    llvm::SmallVector<llvm::StringRef> live_exprs;
    public:
    OptimizeAST(){
      live_var.push_back(llvm::StringRef("result"));
      live_exprs.push_back(llvm::StringRef("result"));
    }


    
    virtual void visit(Condition &node) override{}
    virtual void visit(Loop &node) override{}
    virtual void visit(BE &node) override{}


    virtual void visit(Goal &Node) override
    {
      exprs = Node.getExprs();
      for (auto I = exprs.end(), E = exprs.begin(); I != E; --I)
      {
        (*I)->accept(*this);
        if(!right_alive){
          exprs.erase(I);
        }
      }
      Node.setExprs(exprs);
    };
    // Visit function for Factor nodes
    virtual void visit(Factor &Node) override
    {
      if(right_alive){
        live_var.push_back(Node.getVal());
        live_exprs.push_back(Node.getVal());
      }
    };

 

    // Visit function for BinaryOp nodes
    virtual void visit(BinaryOp &Node) override{
      (Node.getLeft()) -> accept(*this);
      (Node.getLeft()) -> accept(*this);
    };

    virtual void visit(Condition &node) override{}
    virtual void visit(Loop &node) override{}
    virtual void visit(BE &node) override{}

    // Visit function for Assignment nodes
    virtual void visit(Assignment &Node) override
    {
      right_alive = false;
      Factor *left = Node.getLeft();
      if (left->getKind() == Factor::Ident)
      {
        auto itr = find(live_var.begin(), live_var.end(), left->getVal());
        if (itr != live_var.end()){
          right_alive = true;
          live_var.erase(itr);
          if (Node.getRight())
            Node.getRight()->accept(*this);
        }
      }
    };

    virtual void visit(Declaration &Node) override
    {
      right_alive = false;
      llvm::StringRef left = *Node.begin();
      auto itr = find(live_var.begin(), live_var.end(), left);
      
      if(itr != live_var.end()){
        right_alive = true;
        live_var.erase(itr);
        if (Node.begin_values())
          (* Node.begin_values())->accept(*this);
      }

    
      if (find(live_exprs.begin(), live_exprs.end(), left) == live_exprs.end()){
        right_alive = true;
      }
      else{
        right_alive = false;
      }
    };



    
  };



class Optimization{ 
  public:
  void optimize(AST *Tree)
  {
    OptimizeAST Opt_tree;
    Tree->accept(Opt_tree);
  }
};