//
// Created by Jongjyh on 2019/12/13.
//
#include "tokenizer/token.h"
#include <vector>
namespace miniplc0 {
    enum class OP { VOID,ADD, SUB,MUL,DIV,PLUS,NEG,GRE,LES,GREE,LESE,NEQU,EQU };
    struct AST {
    };

    //根
    struct TypeSpecifier:AST {
        TokenType type;
        TypeSpecifier(TokenType type):type(type){}
        TokenType generation(){
            return type;
        }
    };
    struct Identifier:AST {
        std::string value;
        Identifier
        std::string generation(){
            return value;
        }
    };
    struct Integer:AST{
        Analyser::uint32_t value;
    };
    struct Const:AST{
        bool isConst;
        bool generation(){
            return isConst;
        }
    };

    struct C0AST : AST {
        std::vector<VarDecAst *> variableDeclarations;
        std::vector<FunDecAst *> functionDeclarations;
        void generation(){
            for(auto it:variableDeclarations)
                it->generation();
            for(auto it:functionDeclarations)
                it->generation();
        }
    };
    struct DecAST :AST{};
    struct StmtAST : AST {};
    struct ExprAST:AST{};
    //list
    struct ListAST: AST{};
    struct InitDeclaratorListAST:ListAST{
        std::vector<InitDeclaratorAST*> initDeclaratorAst;
        void generation(){
            for (auto it:initDeclaratorAst)
                it->generation();
        }

    };
    struct InitDeclaratorAST:AST{
        Identifier *identifier;
        ExprAST *exp;
        void generation(){
            exp->generation();
        }
    };
    //declaration
    struct VarDecAst : DecAST {
        Const *consts;
        TypeSpecifier* typeSpecifier;
        InitDeclaratorListAST *initDeclaratorList;
        void generation(){
            init
        }
    };

    struct FunDecAst : DecAST {
        TypeSpecifier *typeSpecifier;
        Identifier *identifier;
        ParameterClause *parameterClause;
        CompoundStmtAST *compoundStatement;
        void generation(){
            /*
             * 保存
             */
        }
    };

    struct ParameterClause{
        ParameterDecListAST* listAst;
        void generation(){
            listAst->generation();
        }
    }:AST;
    struct ParameterDecListAST:ListAST{
        std::vector<ParameterDecAST*> paraList;
        void generation(){
            for(auto it:paraList)
                it->generation();
        }
    };
    struct ParameterDecAST:DecAST{
        Const *consts;
        TypeSpecifier *type;
        Identifier *identifier;
        void generation(){

        }
    };

    //Statement

    struct ConditionStmtAST : StmtAST {
        ExprAST *condtion;
        StmtAST *thenStmt;
        StmtAST *elseStmt;
        void generation(){

        }
    };

    struct LoopStmtAST:StmtAST{
        ExprAST *condtion;
        StmtAST *loopStmt;
        void generation(){

        }

    };
    struct JumpStmtAST:StmtAST{
        Const *consts;
        void generation(){

        }

    };
    struct PrintStmtAST:StmtAST{
        std::vector<Identifier*> list;
        void generation(){

        }
    };
    struct ScanStmtAST:StmtAST{
        Identifier *id;
        void generation(){

        }
    };
    struct CallStmtAST:StmtAST{
        Identifier *function;
        std::vector<ExprAST*> exps;
        void generation(){

        }
    };
    struct CompoundStmtAST : StmtAST {
        std::vector<VarDecAst *> decs;
        std::vector<StmtAST *> stmts; // StmtAST* stmts[];
        void generation(){

        }
    }
    struct AssignmentExpStmtAST:StmtAST{
        Identifier *id;
        Const *consts;
        TypeSpecifier *type;
        ExprAST *exp;
        /*值*/
        void generation(){

        }
    };
    struct SemicolonStmtAST : StmtAST {
        void generation(){

        }
    }

    struct ConditionExpr:ExprAST{
        ExprAST *l,*r;
        OP op;
        void generation(){

        }
    };
    struct AddExp:ExprAST{
        ExprAST *l,*r;
        OP op;
        void generation(){

        }
    };
    struct MulExp:ExprAST{
        ExprAST *l,*r;
        OP op;
    };
    struct UnaryExp:ExprAST{
        OP op;
        Integer *integer;
        Identifier *identifier;
        StmtAST *fun;
        UnaryExp(OP op, Integer *integer):op(op),integer(integer){}
        UnaryExp(OP op, Identifier *identifier):op(op),identifier(identifier){}
        UnaryExp(OP op, StmtAST *fun):op(op),fun(fun){}
        void generation(){
            if(integer!= nullptr)
                integer->g
        }
    };
}