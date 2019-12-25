#include "analyser.h"

#include <climits>

#include<iostream>
#include <sstream>

namespace miniplc0 {
    int insindex=0;
    std::vector<miniplc0::Instruction> _instructions;
    TokenType returntype=NULL_TOKEN;
    std::pair<Program, std::optional<CompilationError>> Analyser::Analyse() {
        auto err = analyseC0Program();
        if (err.has_value())
            return std::make_pair(Program(), err);
        else {
            auto Pro = Program(_CONSTS, _funcs, _program);
            return std::make_pair(Pro, std::optional<CompilationError>());
        }
    }

    //<C0-program> ::= {<variable-declaration>}{<function-definition>}
    std::optional<CompilationError> Analyser::analyseC0Program() {
        isGlabol = true;
        auto err = analyseVariableDeclaration();
        if (err.has_value())
            return err;
        //全局变量的帧尾得到确定
        InitStack();
        err = analyseFunctionDeclaration();
        if (err.has_value())
            return err;
        auto next = nextToken();

        if (next.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoEnd);
        if(!isFunctionDeclared("main"))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoMain);
        return {};
    }


    /*
     <function-definition> ::=
        <type-specifier><identifier><parameter-clause><compound-statement>
<parameter-clause> ::=
    '(' [<parameter-declaration-list>] ')'
<parameter-declaration-list> ::=
    <parameter-declaration>{','<parameter-declaration>}
<parameter-declaration> ::=
    [<const-qualifier>]<type-specifier><identifier>

     */
    std::optional<CompilationError> Analyser::analyseFunctionDeclaration() {
        while (1) {
            auto next = nextToken();
            _program.emplace_back(_instructions);
            _instructions.erase(_instructions.begin(), _instructions.end());
            if (!next.has_value())
                return {};
            if ((next.value().GetType() != TokenType::VOID && next.value().GetType() != TokenType::INT &&
                 next.value().GetType() != TokenType::CHAR)) {
                unreadToken();
                return {};
            }
            //预读
            insindex = 0;
            _nextTokenIndex = 0;
            //新的函数

            //<type-specifier> void or int or char
            auto ret = next.value().GetType();
            returntype=ret;
            //<identifier>
            next = nextToken();
            if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
            /*
             * 检查是否同名函数
             */
            std::string str = next.value().GetValueString();
            if (isFunctionDeclared(str))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            Token fun = next.value();
            //函数名加入常量表，记录常量表中的index
            addCONST(fun);
            //<parameter-clause>
            isGlabol = false;
            loadNewLevel();

            auto perr = analyseParameterClause();
            addFuntion(str, 1, perr.first, ret);
            if (perr.second.has_value())
                return perr.second;
            //<compound-statement>
            auto err = analyseCompoundStatement();
            if (err.has_value())
                return err;
        }
    }

    //<parameter-clause> ::=
    //    '(' [<parameter-declaration-list>] ')'
    //<parameter-declaration-list> ::=
    //    <parameter-declaration>{','<parameter-declaration>}
    //<parameter-declaration> ::=
    //    [<const-qualifier>]<type-specifier><identifier>
    std::pair<std::vector<TokenType>, std::optional<CompilationError>> Analyser::analyseParameterClause() {

        // '('
        auto next = nextToken();
        int num = 0;
        std::vector<TokenType> paras;
        if (!next.has_value() || next.value().GetType() != LEFT_BRACKET)
            return std::make_pair(paras,
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBracket));
        //[<parameter-declaration-list>]
        while (1) {
            //预读

            bool isconst = false;
            next = nextToken();
            //空声明
            if (!next.has_value() ||
                (next.value().GetType() != TokenType::RIGHT_BRACKET && next.value().GetType() != TokenType::CONST &&
                 next.value().GetType() != TokenType::INT && next.value().GetType() != TokenType::CHAR))
                return std::make_pair(paras, std::make_optional<CompilationError>(_current_pos,
                                                                                  ErrorCode::ErrInvalidParameterDeclaration));
            if (next.value().GetType() == TokenType::RIGHT_BRACKET)
                return std::make_pair(paras, std::optional<CompilationError>());
            // 'const' 可选
            num++;
            //<type-specifier>         ::= <simple-type-specifier>
            //<simple-type-specifier>  ::= 'void'|'int'|'char
            //<const-qualifier>        ::= 'const'
            if (next.value().GetType() == CONST) {
                isconst = true;
                next = nextToken();
            }
            if (!next.has_value() ||
                (next.value().GetType() != TokenType::INT && next.value().GetType() != TokenType::CHAR))
                return std::make_pair(paras, std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidParameterDeclaration));
            //<identifier>
            TokenType type = next.value().GetType();
            paras.emplace_back(type);
            next = nextToken();
            std::string str = next.value().GetValueString();
            if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
                return std::make_pair(paras,
                                      std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier));
            if (!canbeDeclaredInThisPart(str))
                return std::make_pair(paras, std::make_optional<CompilationError>(_current_pos,
                                                                                  ErrorCode::ErrDuplicateDeclaration));
            Token var = next.value();
            /*
             * 处理参数声明,此处不需要指令因为call时会自己载入。
             */
            if (isconst == false)
                addVariable(var, type);
            else
                addConstant(var, type);
            //‘,’ 存在则继续读，不是,就退出
            next = nextToken();
            if (!next.has_value() || next.value().GetType() != TokenType::COMMA)
                break;
        }
        //')'
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
            return std::make_pair(paras,
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBracket));
        return std::make_pair(paras, std::optional<CompilationError>());
    }


    // <变量声明> ::= {<变量声明语句>}
    //<variable-declaration> ::=
    //    [<const-qualifier>]<type-specifier><init-declarator-list>';'
    //<init-declarator-list> ::=
    //    <init-declarator>{','<init-declarator>}
    //<init-declarator> ::=
    //    <identifier>[<initializer>]
    //<initializer> ::=
    //    '='<expression>
    std::optional<CompilationError> Analyser::analyseVariableDeclaration() {
        // 变量声明语句可能有一个或者多个
        while (1) {
            // 预读？
            bool isconst = false;
            auto next = nextToken();
            //空声明
            if (!next.has_value())
                return {};
            if ((next.value().GetType() != TokenType::CONST &&next.value().GetType() != TokenType::VOID &&next.value().GetType() != TokenType::CHAR &&
                     next.value().GetType() != TokenType::INT)) {
                unreadToken();
                return {};
            }
            // 'const' 可选
            if (next.value().GetType() == TokenType::CONST) {
                next = nextToken();
                isconst = true;
            }

            //<type-specifier>         ::= <simple-type-specifier>
            //<simple-type-specifier>  ::= 'void'|'int'
            //<const-qualifier>        ::= 'const'
            if (!next.has_value() || (
                    next.value().GetType() != TokenType::INT && next.value().GetType() != TokenType::CHAR))
                {
                unreadToken();
                return {};
                }
            TokenType type = next.value().GetType();
            //<init-declarator-list> ::=
            //    <init-declarator>{','<init-declarator>}
            //<init-declarator> ::=
            //    <identifier>[<initializer>]
            //<initializer> ::=
            //    '='<expression>
            while (1) {
                next = nextToken();
                std::string str = next.value().GetValueString();

                if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {

                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
                }
                if (!canbeDeclaredInThisPart(next.value().GetValueString()))
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
                Token var = next.value();
                next = nextToken();
                if (!next.has_value() ||
                    (next.value().GetType() != TokenType::EQUAL_SIGN && next.value().GetType() != TokenType::COMMA &&
                     next.value().GetType() != TokenType::SEMICOLON)) {
                    //不是一个变量声名，可能是一个函数定义。需要回溯
                    if (isconst == true)//有const一定是变量声名，返回错误
                        return std::make_optional<CompilationError>(_current_pos,
                                                                    ErrorCode::ErrInvalidVariableDeclaration);
                    else {
                        unreadToken();//'('
                        unreadToken();//id
                        unreadToken();//type
                        return {};
                    }
                }

                if (next.value().GetType() == TokenType::EQUAL_SIGN) {

                    // '<表达式>'
                    auto err = analyseExpression();
                    if (err.second.has_value())
                        return err.second;
                    auto rettype=err.first.value()->generation();
                    if(rettype==VOID)
                        return std::make_optional<CompilationError>(_current_pos,
                                                                    ErrorCode::ErrVOIDBEVALUE);
                    if(rettype==INT&&type==CHAR)
                    {
                        insindex++;
                        _instructions.emplace_back(I2C,0);

                    }

                    next = nextToken();
                    /*
                     * 声名一个初始化变量
                     */
                    if (isconst == true)
                        addConstant(var, type);
                    else
                        addVariable(var, type);

                } else {
                    if (isconst == true)
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrAssignToConstant);
                    addUninitializedVariable(var, type);
                    _instructions.emplace_back(Operation::IPUSH, 0);
                    insindex += 5;
                }
                // ','
                if (next.value().GetType() == TokenType::COMMA) {
                    /*
                     * 声名一个未初始化变量，初始化为0
                     */
                    continue;
                }
                break;

            }
            // ';'
            if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
            }
        }
        return {};
    }

    // <compound-statement> ::=
    //    '{' {<variable-declaration>} <statement-seq> '}'

    std::optional<CompilationError> Analyser::analyseCompoundStatement() {
        // '{‘
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != LEFT_BRACE)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBrace);
        // {<variable-declaration>}
        auto err = analyseVariableDeclaration();
        if (err.has_value())
            return err;
        // <statement-seq>
        err = analyseStatementSequence();
        if (err.has_value())
            return err;
        // '}'
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACE)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBrace);
        popCurrentLevel();
        return {};
    }

    //<statement-seq> ::=
    //	{<statement>}
    //<statement> ::=
    //     '{' <statement-seq> '}'
    //    |<condition-statement>
    //    |<loop-statement>
    //    |<jump-statement>
    //    |<print-statement>
    //    |<scan-statement>
    //    |<assignment-expression>';'
    //    |<function-call>';'
    //    |';'
    //
    //<jump-statement> ::= <return-statement>
    //<return-statement> ::= 'return' [<expression>] ';'
    std::optional<CompilationError> Analyser::analyseStatementSequence() {
        while (1) {
            auto err = analyseStatement();
            if (err.has_value()) {
                if (err.value().GetCode() == ErrinvalidStatement)
                    return {};
                else
                    return err;
            }
        }
        return {};


    }

    // <单语句>
    //<statement> ::=
    //     '{' <statement-seq> '}'
    //    |<condition-statement>
    //    |<loop-statement>
    //    |<jump-statement>
    //    |<print-statement>
    //    |<scan-statement>
    //    |<assignment-expression>';'
    //    |<function-call>';'
    //    |';'
    std::optional<CompilationError> Analyser::analyseStatement() {
        auto next = nextToken();
        if (!next.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrinvalidStatement);
        unreadToken();
        if (next.value().GetType() != TokenType::IDENTIFIER &&
            next.value().GetType() != TokenType::PRINT &&
            next.value().GetType() != TokenType::LEFT_BRACE &&
            next.value().GetType() != TokenType::IF &&
            next.value().GetType() != TokenType::WHILE &&
            next.value().GetType() != TokenType::SCAN &&
            next.value().GetType() != TokenType::VOID &&
            next.value().GetType() != TokenType::INT &&
            next.value().GetType() != TokenType::RETURN &&
            next.value().GetType() != TokenType::SEMICOLON) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrinvalidStatement);
        }
        std::optional<CompilationError> err;
        switch (next.value().GetType()) {
            case SEMICOLON:
                nextToken();
                return {};
            case LEFT_BRACE:
                loadNewLevel();
                err = analyseCompoundStatement();
                if (err.has_value())
                    return err;
                return {};
            case IF:
                err = analyseConditionStatement();
                if (err.has_value())
                    return err;
                return {};
            case WHILE:
                err = analyseLoopStatement();
                if (err.has_value())
                    return err;
                return {};
            case VOID:
            case INT:
                err = analyseVariableDeclaration();
                if (err.has_value())
                    return err;
                return {};
            case PRINT:
                err = analysePrintStatement();
                if (err.has_value())
                    return err;
                return {};
            case SCAN:
                err = analyseScanStatement();
                if (err.has_value())
                    return err;
                return {};
            case IDENTIFIER: {
                std::string s = next.value().GetValueString();
                if (isDeclared(s)) {

                    err = analyseAssignmentStatement();
                    if (err.has_value())
                        return err;
                    //;
                    next = nextToken();
                    if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
                    return {};
                }
                if (isFunctionDeclared(s)) {
                   auto err = analyseFunctionCall();
                    if (err.second.has_value())
                        return err.second;
                     err.first.value()->generation();

                    //;
                    next = nextToken();
                    if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
                    }
                    return {};
                }
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
                break;
            }
            case RETURN:
                err = analyseReturnStatement();
                if (err.has_value())
                    return err;
                return {};

            default:
                break;
        }
        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrinvalidStatement);
    }

    /*
     * 条件表达式  其他 true ==0 false
     * <condition> ::=
     * <expression>[<relational-operator><expression>]
     */
    std::optional<CompilationError> Analyser::analyseConditionExp() {
        auto err = analyseExpression();
        if (err.second.has_value())
            return err.second;
        err.first.value()->generation();
        auto next = nextToken();
        if (!next.has_value() ||
            (next.value().GetType() != GREATER_THAN_SIGN &&
             next.value().GetType() != LESS_THAN_SIGN &&
             next.value().GetType() != GRT_EQU_SIGN &&
             next.value().GetType() != LES_EQU_SIGN &&
             next.value().GetType() != IS_EQU_SIGN &&
             next.value().GetType() != NOT_EQU_SIGN)) {
            unreadToken();
            insindex+=3;
            _instructions.emplace_back(Operation::JNE, insindex + 3);

            return {};
        }

        err = analyseExpression();
        if (err.second.has_value())
            return err.second;
        err.first.value()->generation();
        _instructions.emplace_back(Operation::ISUB, 0);
        insindex += 1;
        insindex += 3;
        // >
        if (next.value().GetType() == GREATER_THAN_SIGN)
            _instructions.emplace_back(Operation::JG, insindex + 3);
            // <
        else if (next.value().GetType() == LESS_THAN_SIGN)
            _instructions.emplace_back(Operation::JL, insindex + 3);

            // >= finish
        else if (next.value().GetType() == GRT_EQU_SIGN)
            _instructions.emplace_back(Operation::JGE, insindex + 3);
            // <= finish
        else if (next.value().GetType() == LES_EQU_SIGN)
            _instructions.emplace_back(Operation::JLE, insindex + 3);
            // ==
        else if (next.value().GetType() == IS_EQU_SIGN)
            _instructions.emplace_back(Operation::JE, insindex + 3);
            // != finish
        else if (next.value().GetType() == NOT_EQU_SIGN)
            _instructions.emplace_back(Operation::JNE, insindex + 3);

        return std::optional<CompilationError>();
        return {};
    }

    /*
     * 条件语句
     * <condition-statement> ::=
     * 'if' '(' <condition> ')' <statement> ['else' <statement>]
     * JCOND s0
     * jmp s1
     * s0:
     * ifstmt
     * s1:
     * elsestmt
     */
    std::optional<CompilationError> Analyser::analyseConditionStatement() {
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != IF)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteConditionStatement);
        //'(
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //condition
        auto err = analyseConditionExp();
        if (err.has_value())
            return err;

        _instructions.emplace_back(Operation::JMP, 0);//1
        insindex += 3;
        int jmp = _instructions.size() - 1;
        //s1意义如上
        //')'
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //statement
        err = analyseStatement();
        if (err.has_value())
            return err;
        //'else'
        next = nextToken();
        _instructions[jmp].SetX(insindex);
        if (!next.has_value() || next.value().GetType() != ELSE) {
            unreadToken();
            return {};
        }
        //statement
        //回填地址
        err = analyseStatement();
        if (err.has_value())
            return err;
        return {};
    }
    //循环语句
    /*
     * <loop-statement> ::=
     * 'while' '(' <condition> ')' <statement>
     * 1求值<condition>
     * 2如果<condition>是false，跳转到步骤5
     *  jcond s0
     *
     *  jmp s1
     *  s0:0
     *  [statement]
     *  JMP S0
     *  S1:
     *  ...
     * 3控制进入while的代码块并顺序执行
     * 4控制达到while代码块的尾部时，跳转到步骤1控制
     * 5跳过while结构，执行之后的代码块
     */
    std::optional<CompilationError> Analyser::analyseLoopStatement() {
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != WHILE)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteWhileStatement);
        //'('
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //<condition>
        int s0 = insindex, s1;
        auto err = analyseConditionExp();
        if (err.has_value())
            return err;
        _instructions.emplace_back(Operation::JMP, 0);
        insindex += 3;
        int jne = _instructions.size() - 1;
        //')'
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);

        //statement
        err = analyseStatement();
        if (err.has_value())
            return err;
        _instructions.emplace_back(Operation::JMP, s0);
        insindex += 3;
        //回填jne s1
        s1 = insindex;
        _instructions[jne].SetX(s1);
        return {};

    }
    //输出语句
    /*
     * <print-statement> ::= 'print' '(' [<printable-list>] ')' ';'
     * <printable-list>  ::= <printable> {',' <printable>}
     * <printable> ::= <expression>
     */
    std::optional<CompilationError> Analyser::analysePrintStatement() {
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != PRINT)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompletePrintStatement);
        //'('
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //<printable-list>

        while (1) {
            next = nextToken();
            if (!next.has_value())
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidPrint);
            if (next.value().GetType() == TokenType::STRING_LIT || next.value().GetType() == TokenType::CHAR_LIT) {
                if (!isBeenLoad(next.value()))
                    addCONST(next.value());
                _instructions.emplace_back(LOADC, getConstIndex(next.value()));
                insindex += 3;
            } else {
                unreadToken();
                auto err = analyseExpression();
                if (err.second.has_value())
                    return err.second;
            }
            next = nextToken();
            _instructions.emplace_back(IPRINT, 0);
            insindex += 1;
            if (!next.has_value() || next.value().GetType() != COMMA) {
                break;
            }
        }
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

        return {};

    }
    //扫描语句
    /*
     * <scan-statement>  ::= 'scan' '(' <identifier> ')' ';'
     */
    std::optional<CompilationError> Analyser::analyseScanStatement() {
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != SCAN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteScanStatement);
        //'('
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //identtifier
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string str = next.value().GetValueString();
        if (!isDeclared(str))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
        /*
         * 查找变量的地址，然后保存。
         */
        int index, level;
        std::pair<int32_t, int32_t> p = getIndex(str);
        index = p.first;
        level = p.second;
        _instructions.emplace_back(Operation::LOADA, level, index);//7
        _instructions.emplace_back(Operation::ISCAN, 0);//1
        _instructions.emplace_back(Operation::ISTORE, 0);//1
        insindex += 9;
        //')'
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //';
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        return {};
    }
    //函数调用语句
    /*
     * <function-call> ::=
     * <identifier> '(' [<expression-list>] ')'
     */
    std::pair<std::optional<Analyser::Call*>, std::optional<CompilationError>> Analyser::analyseFunctionCall() {
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_pair(std::optional<Analyser::Call*>(),
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier));
        std::string str = next.value().GetValueString();

        //函数是否被声明成变量
        if (isDeclared(str))
            return std::make_pair(std::optional<Analyser::Call*>(),
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVariableCall));
        //函数是否被声明过
        if (!isFunctionDeclared(str))
            return std::make_pair(std::optional<Analyser::Call*>(),
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared));
        int index = getFuncIndex(str);
        Function function = getFunc(index);
        index--;
        std::vector<Expression *> exps;
        //'('
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != LEFT_BRACKET)
            return std::make_pair(std::optional<Analyser::Call*>(),
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket));
        //[<expression-list>]
        next = nextToken();
        if (next.has_value() && next.value().GetType() == RIGHT_BRACKET) {
            if (function.getParaSize() != 0)
                return std::make_pair(std::optional<Analyser::Call*>(),
                                      std::make_optional<CompilationError>(_current_pos,
                                                                           ErrorCode::ErrWrongCallParameter));
            return std::make_pair(new Call(NULL_TOKEN, function, exps, index), std::optional<CompilationError>());
        }
        unreadToken();
        auto err = analyseExpressionList();
        if (err.second.has_value())
            return std::make_pair(std::optional<Analyser::Call*>(), err.second);
        exps = err.first.value();
        //')'
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
            return std::make_pair(std::optional<Analyser::Call*>(),
                                  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket));
        return std::make_pair(new Call(NULL_TOKEN, function, exps, index), std::optional<CompilationError>());
    }
    //返回语句
    /*
     * <return-statement> ::= 'return' [<expression>] ';'
     */
    std::optional<CompilationError> Analyser::analyseReturnStatement() {
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != RETURN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteReturnStatement);
        next = nextToken();
        int prefix = -1;
        //预读一下判断是否有exp的存在
        if (!next.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteReturnStatement);
        TokenType  type;
        if (next.value().GetType() != TokenType::SEMICOLON) {
            unreadToken();
            auto err = analyseExpression();
            if (err.second.has_value())
                return err.second;
            type = err.first.value()->generation();
            prefix = 1;
            next = nextToken();
        }
        //';'

        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        if (prefix == -1 && returntype == VOID) {
            _instructions.emplace_back(Operation::POPN, _nextTokenIndex);//5
            _instructions.emplace_back(Operation::RET, 0);//1
            insindex += 6;
        } else if (prefix == 1 && returntype != VOID) {
            if(returntype==CHAR &&type==INT)
            {
                insindex++;
                _instructions.emplace_back(Operation::I2C, 0);
            }
            _instructions.emplace_back(Operation::IRET, 0);
            insindex += 1;
        } else
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrReturn);
        return {};

    }

    // <表达式> ::= <项>{<加法型运算符><项>}
    std::pair<std::optional<Analyser::Expression*>, std::optional<CompilationError>> Analyser::analyseExpression() {
        // <项>
        std::vector<TokenType> add;
        std::vector<Item > items;
        auto err = analyseItem();
        if (err.second.has_value())
            return std::make_pair(std::optional<Expression*>(), err.second);
        items.emplace_back(err.first.value());
        // {<加法型运算符><项>}
        while (1) {
            // 预读
            auto next = nextToken();
            if (!next.has_value())
                return std::make_pair(new Expression(TokenType::NULL_TOKEN, add, items), std::optional<CompilationError>());
            auto type = next.value().GetType();
            if (type != TokenType::PLUS_SIGN && type != TokenType::MINUS_SIGN) {
                unreadToken();
                return std::make_pair(new Expression(TokenType::NULL_TOKEN, add, items), std::optional<CompilationError>());
            }
            // <项>
            err = analyseItem();
            if (err.second.has_value())
                return std::make_pair(std::optional<Expression*>(), err.second);
            items.emplace_back((err.first.value()));
            // 根据结果生成指令
            add.emplace_back(type);
        }
    }

    /*
     * <表达式list>
     * <expression-list> ::=
     * <expression>{','<expression>}
     */
    std::pair<std::optional<std::vector<Analyser::Expression *> >, std::optional<CompilationError>>
    Analyser::analyseExpressionList() {
        std::vector<Analyser::Expression *> exps;
        while (1) {

            auto err = analyseExpression();
            if (err.second.has_value())
                return std::make_pair(std::optional<std::vector<Analyser::Expression *>>(), err.second);
            exps.emplace_back(err.first.value());
            auto next = nextToken();
            if (!next.has_value() || next.value().GetType() != COMMA) {
                unreadToken();
                return std::make_pair(exps, std::optional<CompilationError>());
            }
        }
        return {};
    }
    // <赋值语句> ::= <标识符>'='<表达式>

    std::optional<CompilationError> Analyser::analyseAssignmentStatement() {
        auto next = nextToken();
        // <标识符>
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        // 标识符声明过吗？
        std::string str = next.value().GetValueString();
        if (!Analyser::isDeclared(str)) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
        }
        // 标识符是常量吗？
        if (Analyser::isConstant(str))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrAssignToConstant);
        Var var = getVar(str);

        next = nextToken();

        //'='
        if (!next.has_value() || next.value().GetType() != TokenType::EQUAL_SIGN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidAssignment);

        int index, level;
        index = var.getIndex();
        level = var.isGlobal1();
        _instructions.emplace_back(Operation::LOADA, level, index);//7
        insindex += 7;
        //调用exp子程序
        auto err = analyseExpression();
        if (err.second.has_value())
            return err.second;
        auto rettype=err.first.value()->generation();
        if(rettype==VOID)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVOIDBEVALUE);
        if(var.getType()==CHAR&&rettype==INT)
        {
            insindex++;
            _instructions.emplace_back(Operation::I2C, 0);
        }
        _instructions.emplace_back(Operation::ISTORE, 0);//1
        insindex += 1;

        return {};
    }



    std::pair<std::optional<TokenType>, std::optional<CompilationError>> Analyser::analyseConvert(){
        auto next=nextToken();
        TokenType Convert=NULL_TOKEN;
        if(!next.has_value())
            return std::make_pair(std::optional<TokenType >(), std::make_optional<CompilationError>(_current_pos,
                                                                                              ErrorCode::ErrIncompleteExpression));
        if(next.value().GetType()==LEFT_BRACKET)
        {
            next= nextToken();
            if(!next.has_value())
                return std::make_pair(std::optional<TokenType>(), std::make_optional<CompilationError>(_current_pos,
                                                                                                  ErrorCode::ErrIncompleteExpression));
            if(next.value().GetType()==VOID)
                return std::make_pair(std::optional<TokenType>(), std::make_optional<CompilationError>(_current_pos,
                                                                                                  ErrorCode::ErrConvertToVOID));
            if(next.value().GetType()!=CHAR&&next.value().GetType()!=INT)
            {
                unreadToken();
                unreadToken();
                return {};
            }
            Convert=next.value().GetType();
            next= nextToken();
            if(!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
                return std::make_pair(std::optional<TokenType >(), std::make_optional<CompilationError>(_current_pos,
                                                                                                  ErrorCode::ErrNoBracket));
        }
        else
        {
            unreadToken();
            return {};
        }
        return  std::make_pair(Convert, std::optional<CompilationError>());
    }
    // <项> :: = <因子>{ <乘法型运算符><因子> }
    std::pair<std::optional<Analyser::Item>, std::optional<CompilationError>> Analyser::analyseItem() {
        // 可以参考 <表达式> 实现
        //因子
        std::vector<Factor *> factors;
        std::vector<TokenType> mul;
        auto Cerr =analyseConvert();
        if (Cerr.second.has_value())
            return std::make_pair(std::optional<Item>(), Cerr.second);

        auto err = analyseFactor();
        if (err.second.has_value())
            return std::make_pair(std::optional<Item>(), err.second);

        if(Cerr.first.has_value()&&Cerr.first.value()!=NULL_TOKEN)
            err.first.value()->convert=Cerr.first.value();
        factors.emplace_back(err.first.value());
        //{ <乘法型运算符><因子> }
        while (true) {
            // 预读
            auto next = nextToken();
            if (!next.has_value())
                return std::make_pair(Item(factors, mul), std::optional<CompilationError>());
            auto type = next.value().GetType();
            if (type != TokenType::DIVISION_SIGN && type != TokenType::MULTIPLICATION_SIGN) {
                unreadToken();
                return std::make_pair(Item(factors,mul),std::optional<CompilationError>());
            }

            Cerr =analyseConvert();
            if (Cerr.second.has_value())
                return std::make_pair(std::optional<Item>(), Cerr.second);
            //因子
            err = analyseFactor();
            if (err.second.has_value())
                return std::make_pair(std::optional<Item>(), err.second);
            if(Cerr.first.has_value()&&Cerr.first.value()!=NULL_TOKEN)
                err.first.value()->convert=Cerr.first.value();
            factors.emplace_back(err.first.value());
            // 根据结果生成指令
            mul.emplace_back(type);
        }
        return {};
    }

    // <因子> ::= [<单目符号>]( <标识符> | <无符号整数> | '('<表达式>')' | <函数> | char_lit)

    std::pair<std::optional<Analyser::Factor*>, std::optional<CompilationError>> Analyser::analyseFactor() {
        // [<符号>]
        auto next = nextToken();
        TokenType prefix;
        if (!next.has_value())
            return std::make_pair(std::optional<Analyser::Factor*>(), std::make_optional<CompilationError>(_current_pos,
                                                                                                          ErrorCode::ErrIncompleteExpression));
        if (next.value().GetType() == TokenType::PLUS_SIGN)
            prefix = PLUS_SIGN;
        else if (next.value().GetType() == TokenType::MINUS_SIGN) {
            prefix = MINUS_SIGN;
        } else
            unreadToken();

        // 预读
        next = nextToken();
        if (!next.has_value())
            return std::make_pair(std::optional<Analyser::Factor*>(), std::make_optional<CompilationError>(_current_pos,
                                                                                                          ErrorCode::ErrIncompleteExpression));
        switch (next.value().GetType()) {
            case TokenType::LEFT_BRACKET: {
                auto err = analyseExpression();
                if (err.second.has_value())
                    return std::make_pair(std::optional<Analyser::Factor*>(), err.second);
                err.first.value()->sign = prefix;
                next = nextToken();
                if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
                    return std::make_pair(std::optional<Analyser::Factor*>(),
                                          std::make_optional<CompilationError>(_current_pos,
                                                                               ErrorCode::ErrIncompleteExpression));
                break;
            }
            case TokenType::INTEGER: {
                //进常量表
                uint32_t k;
                if (!isBeenLoad(next.value())) {
                    addCONST(next.value());
                    k = _CONSTS.size() - 1;
                } else
                    k = getConstIndex(next.value());
                return std::make_pair(new Integer(prefix, k), std::optional<CompilationError>());
            }
            case TokenType::CHAR_LIT: {
                //进常量表
                uint32_t k;
                if (!isBeenLoad(next.value())) {
                    addCONST(next.value());
                    k = _CONSTS.size() - 1;
                } else
                    k = getConstIndex(next.value());
                return std::make_pair(new Char(prefix, k), std::optional<CompilationError>());
                //数字直接入栈
            }
            case TokenType::IDENTIFIER: {
                std::string str = next.value().GetValueString();
                //先看是否是变量
                if (isDeclared(str)) {
                    Var var = getVar(str);
                    return std::make_pair(new Variable(prefix, var), std::optional<CompilationError>());
                    //利用标识符找到常量、变量在栈中的索引，利用load指令载入identifi的值
                }
                    //函数调用
                else if (isFunctionDeclared(str)) {
                    unreadToken();
                    auto err = analyseFunctionCall();
                    if (err.second.has_value())
                        return std::make_pair(std::optional<Analyser::Factor*>(), err.second);
                    return std::make_pair(err.first, std::optional<CompilationError>());
                    break;
                } else
                    return std::make_pair(std::optional<Factor*>(), std::make_optional<CompilationError>(_current_pos,
                                                                                                        ErrorCode::ErrNotDeclared));
                break;
            }
            default:
                return std::make_pair(std::optional<Factor*>(), std::make_optional<CompilationError>(_current_pos,
                                                                                                    ErrorCode::ErrinvalidExpression));
        }
        // 这里和 <语句序列> 类似，需要根据预读结果调用不同的子程序
        // 但是要注意 default 返回的是一个编译错误

        return {};
    }

    std::optional<miniplc0::Token> Analyser::nextToken() {
        if (_offset == _tokens.size())
            return {};
        // 考虑到 _tokens[0..._offset-1] 已经被分析过了
        // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
        _current_pos = _tokens[_offset].GetEndPos();
        return _tokens[_offset++];
    }

    void Analyser::unreadToken() {
        if (_offset == 0)
            DieAndPrint("analyser unreads token from the begining.");
        _current_pos = _tokens[_offset - 1].GetEndPos();
        _offset--;
    }


    //常量表、函数表操作
    void Analyser::addCONST(const Token &tk) {
        if (tk.GetType() == STRING_LIT || tk.GetType() == IDENTIFIER)//string
        {
            std::string str = tk.GetValueString();
            _CONSTS.push_back(make_pair(str, 1));
            _constant[str] = _CONSTS.size();
        } else if (tk.GetType() == INTEGER || tk.GetType() == CHAR_LIT) {
            int32_t num;
            num = std::any_cast<int>(tk.GetValue());
            std::string str = std::to_string(num);
            _CONSTS.push_back(make_pair(str, 0));
            _constant[str] = _CONSTS.size();
        }
    }

    bool Analyser::isFunctionDeclared(const std::string &s) {
        return _function.find(s) != _function.end();
    }

    void Analyser::addFuntion(std::string name, int level, std::vector<TokenType> &paras, TokenType &ret) {
        int nameindex = getConstIndex(Token(TokenType::IDENTIFIER, name));
        Function f(nameindex, level, paras, ret);
        _funcs.emplace_back(f);
        _function[name] = _funcs.size();
    }

    //返回索引+1
    int32_t Analyser::getConstIndex(const Token &tk) {
        if (tk.GetType() == STRING_LIT || tk.GetType() == IDENTIFIER)//string
        {
            return _constant[tk.GetValueString()] - 1;
        } else if (tk.GetType() == INTEGER || tk.GetType() == CHAR_LIT) {
            int32_t num;
            num = std::any_cast<int>(tk.GetValue());
            std::string str = std::to_string(num);
            return _constant[str] - 1;
        }
    }

    int32_t Analyser::getFuncIndex(const std::string &s) {
        return _function[s];
    }

    Function Analyser::getFunc(int32_t index) {
        return _funcs[index - 1];
    }

    bool Analyser::isBeenLoad(Token &token) {
        std::string s;
        if (token.GetType() == TokenType::INTEGER || token.GetType() == TokenType::CHAR_LIT) {
            int num = std::any_cast<std::int32_t>(token.GetValue());
            std::stringstream ss;
            ss << num;
            ss >> s;
        } else
            s = token.GetValueString();
        return isCONST(s);

    }

    bool Analyser::isCONST(std::string basicString) {
        return _constant[basicString] != 0;
    }
//返回 实际的索引+1

    //变量表

    bool Analyser::isDeclared(const std::string &s) {
        return _findLocal(s).getIndex() != 0 || _findGlobal(s).getIndex() != 0;
    }

    bool Analyser::canbeDeclaredInThisPart(const std::string &s) {
        if (isGlabol == true) {
            return _findGlobal(s).getIndex() == 0;
        } else {
            return _find(s, *_var).getIndex() == 0;
        }
    }

    bool Analyser::isUninitializedVariable(const std::string &s) {//1
        auto p = _findLocal(s);
        auto g = _findGlobal(s);
        return (p.getIndex() != 0 && p.isUnit1()) || (g.getIndex() != 0 && g.isUnit1());
    }

    bool Analyser::isInitializedVariable(const std::string &s) {//2
        auto p = _findLocal(s);
        auto g = _findGlobal(s);
        return (p.getIndex() != 0 && !p.isConst1() && !p.isUnit1()) ||
               (g.getIndex() != 0 && !g.isConst1() && !g.isUnit1());
    }

    bool Analyser::isConstant(const std::string &s) {//0
        auto p = _findLocal(s);
        auto g = _findGlobal(s);
        return (p.getIndex() != 0 && p.isConst1()) || (g.getIndex() != 0 && g.isConst1());
    }

    //底层操作，添加，查找
    void Analyser::_add(const Token &tk, std::map<std::string, Var> &sk, const TokenType &type, const bool &isConst,
                        const bool &isUnit) {
        if (tk.GetType() != TokenType::IDENTIFIER)
            DieAndPrint("only identifier can be added to the table.");
        std::string s = tk.GetValueString();
        Var var(++_nextTokenIndex, type, isConst, isUnit, false);
        Var gvar(++_nextGTokenIndex, type, isConst, isUnit, true);
        if (isGlabol == false)
            sk[s] = var;
        else
            sk[s] = gvar;

    }

    Var Analyser::_find(const std::string &s, std::map<std::string, Var> &sk) {
        return sk[s];
    }

    Var Analyser::_findLocal(const std::string &s) {
        if (_find(s, *_var).getIndex() != 0)
            return _find(s, *_var);
        for (int i = _var_table.size() - 1; i >= 0; i--) {
            if (_find(s, (*(_var_table[i]))).getIndex() != 0)
                return _find(s, (*(_var_table[i])));
        }
        return Var();
    }

    Var Analyser::_findGlobal(const std::string &s) {
        if (_find(s, (g_var)).getIndex() != 0)
            return _find(s, (g_var));
        return Var();
    }

    std::pair<int32_t, int32_t> Analyser::getIndex(const std::string &s) {
        std::pair<int32_t, int32_t> p(-1, -1);
        //先找局部变量
        int index = -1;
        if ((index = _findLocal(s).getIndex()) != 0)
            p.second = 0;
        else if ((index = _findGlobal(s).getIndex()) != 0)
            p.second = 1;
        p.first = index - 1;
        return p;
    }

    Var Analyser::getVar(const std::string &s) {
        auto var = _findLocal(s), gvar = _findGlobal(s);
        if (var.getIndex() != 0)
            return var;
        if (gvar.getIndex() != 0)
            return var;
        return Var();
    }

    //添加变量、常量、未初始化变量
    void Analyser::addVariable(const Token &tk, const TokenType &type) {
        if (isGlabol == true)
            _add(tk, g_var, type, false, false);
        else
            _add(tk, (*_var), type, false, false);
    }

    void Analyser::addConstant(const Token &tk, const TokenType &type) {
        if (isGlabol == true)
            _add(tk, g_var, type, true, false);
        else
            _add(tk, (*_var), type, true, false);
    }

    void Analyser::addUninitializedVariable(const Token &tk, const TokenType &type) {

        if (isGlabol == true)
            _add(tk, g_var, type, false, true);
        else
            _add(tk, (*_var), type, false, true);
    }


    //高级操作，对帧栈的操作
    void Analyser::InitStack() {
        _var = new std::map<std::string, Var>();
    }

    //进入一个新块。将pre指针指向当前 prepre=pre,pre=top;top++
    void Analyser::loadNewLevel() {
        _var_table.emplace_back(_var);
        _var = new std::map<std::string, Var>();

    }

    //弹出一个块。
    void Analyser::popCurrentLevel() {
        (*_var).clear();
        _var = _var_table.back();
        _var_table.pop_back();
    }


    TokenType Analyser::getType(std::string &var) {
        getIndex(var);
    }


}