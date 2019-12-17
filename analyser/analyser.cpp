#include "analyser.h"

#include <climits>

#include<iostream>

namespace miniplc0 {
    int32_t insindex=0;
    bool isret=true;//默认返回
	std::pair<std::vector<std::vector<Instruction>>, std::optional<CompilationError>> Analyser::Analyse() {
		auto err = analyseC0Program();
		if (err.has_value())
			return std::make_pair(std::vector<std::vector<Instruction>>(), err);
		else
			return std::make_pair(_program, std::optional<CompilationError>());
	}
	//<C0-program> ::= {<variable-declaration>}{<function-definition>}
    std::optional<CompilationError> Analyser::analyseC0Program()
    {
	    auto err = analyseVariableDeclaration();
        if (err.has_value())
            return err;
        //全局变量的帧尾得到确定
        Cbp=_const.end();
        Vbp=_var.end();
        Ubp=_unit_var.end();

         err = analyseFunctionDeclaration();
        if (err.has_value())
            return err;

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
    std::optional<CompilationError> Analyser::analyseFunctionDeclaration()
    {
        auto next = nextToken();
        //预读
        insindex=0;
        _nextTokenIndex=0;
        isret=true;
        _program.emplace_back(_instructions);
        _instructions.empty();
        //新的函数
        if (!next.has_value()||
            (next.value().GetType() != TokenType::VOID&&
             next.value().GetType() != TokenType::INT))
        {
            unreadToken();
            return {};
        }
        //<type-specifier> void or int
        auto ret=next.value().GetType();
        if(ret==VOID)
            isret=false;
        else
            isret=true;
        //<identifier>
        next = nextToken();

        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        /*
         * 检查是否同名函数
         */
        std::string str = next.value().GetValueString();
        if(isFunctionDeclared(str))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
        Token fun=next.value();
        //函数名加入常量表，记录常量表中的index
        addCONST(fun);
        /*
         * 相当于进入一个新的块
         */


        //<parameter-clause>
        auto perr=analyseParameterClause();
        if (perr.second.has_value())
            return perr.second;
        addFuntion(str,1,perr.first);
        //<compound-statement>
        auto err=analyseCompoundStatement();
        if (err.has_value())
            return err;
        /*
         * 退出一个块
         */
        popCurrentLevel();
    }
    //<parameter-clause> ::=
    //    '(' [<parameter-declaration-list>] ')'
    //<parameter-declaration-list> ::=
    //    <parameter-declaration>{','<parameter-declaration>}
    //<parameter-declaration> ::=
    //    [<const-qualifier>]<type-specifier><identifier>
    std::pair<int32_t,std::optional<CompilationError>> Analyser::analyseParameterClause(){

        // '('
        auto next=nextToken();
        int num=0;
        if(!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_pair(-1,std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBracket));
        //[<parameter-declaration-list>]
        while(1)
        {
            //预读

            bool isconst=false;
            auto next = nextToken();
            //空声明
            if (!next.has_value()||(next.value().GetType() != TokenType::CONST&&next.value().GetType() != TokenType::VOID&&next.value().GetType() != TokenType::INT))
            {
                unreadToken();
                return std::make_pair(0,std::optional<CompilationError>());
            }
            // 'const' 可选
            num++;
            if (next.value().GetType() == TokenType::CONST)
            {
                next=nextToken();
                isconst=true;
            }
            //<type-specifier>         ::= <simple-type-specifier>
            //<simple-type-specifier>  ::= 'void'|'int'
            //<const-qualifier>        ::= 'const'
            if (!next.has_value() ||(
                next.value().GetType() != TokenType::VOID&&
                next.value().GetType() != TokenType::INT))
                return std::make_pair(-1,std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidParameterDeclaration));
            //<identifier>
            next = nextToken();
            std::string str = next.value().GetValueString();
            if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
                return std::make_pair(-1,std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier));
            Token var=next.value();
            /*
             * 处理参数声明,此处不需要指令因为call时会自己载入。
             */
            if(isconst== false)
                addVariable(var);
            else
                addConstant(var);
            //‘,’ 存在则继续读，不是,就退出
            next = nextToken();
            if (!next.has_value() || next.value().GetType() != TokenType::COMMA)
            {
                unreadToken();
                break;
            }


        }
        //')'
        if(!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
            return std::make_pair(-1,std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBracket));
        return std::make_pair(num,std::optional<CompilationError>());
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
		while(1) {
            // 预读？
            bool isconst=false;
            auto next = nextToken();
            //空声明
            if (!next.has_value()||(next.value().GetType() != TokenType::CONST&&next.value().GetType() != TokenType::VOID&&next.value().GetType() != TokenType::INT))
            {
                unreadToken();
                return {};
            }
            // 'const' 可选
            if (next.value().GetType() == TokenType::CONST)
            {
                next=nextToken();
                isconst=true;
            }

            //<type-specifier>         ::= <simple-type-specifier>
            //<simple-type-specifier>  ::= 'void'|'int'
            //<const-qualifier>        ::= 'const'
            if (!next.has_value() ||(
            next.value().GetType() != TokenType::VOID&&
            next.value().GetType() != TokenType::INT))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);

            //<init-declarator-list> ::=
            //    <init-declarator>{','<init-declarator>}
            //<init-declarator> ::=
            //    <identifier>[<initializer>]
            //<initializer> ::=
            //    '='<expression>
            while(1) {
                next = nextToken();
                std::string str = next.value().GetValueString();
                if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
                if (isDeclared(next.value().GetValueString()))
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
                Token var = next.value();
                //查看是否需要初始化
                next = nextToken();
                // '=' 需要
                if(!next.has_value()||
                        (next.value().GetType() != TokenType::EQUAL_SIGN&&
                                next.value().GetType() != TokenType::COMMA))
                {
                    //不是一个变量声名，可能是一个函数定义。需要回溯
                    if(isconst==true)//有const一定是变量声名，返回错误
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
                    else
                    {
                        unreadToken();//'('
                        unreadToken();//id
                        unreadToken();//type
                        return {};
                    }

                }
                if (next.has_value() && next.value().GetType() == TokenType::EQUAL_SIGN)
                {
                    // '<表达式>'
                    auto err = analyseExpression();
                    if (err.has_value())
                        return err;

                    /*
                     * 声名一个初始化变量
                     */
                    if(isconst==true)
                        addConstant(var);
                    else
                        addVariable(var);
                    continue;
                }
                // ','
                if (next.has_value() && next.value().GetType() == TokenType::COMMA) {
                    /*
                     * 声名一个未初始化变量，初始化为0
                     */
                    addUninitializedVariable(var);
                    _instructions.emplace_back(Operation::IPUSH,0);
                    insindex+=5;
                    continue;
                }
                    unreadToken();
                break;

                }



            // ';'
            next = nextToken();
            if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);


        }
		return {};
	}

	// <compound-statement> ::=
    //    '{' {<variable-declaration>} <statement-seq> '}'

	std::optional<CompilationError> Analyser::analyseCompoundStatement() {
        // '{‘
        auto next=nextToken();
        if(!next.has_value()||next.value().GetType()!=LEFT_BRACE)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBrace);
        // {<variable-declaration>}
        auto err=analyseVariableDeclaration();
        if (err.has_value())
            return err;
        // <statement-seq>
        err=analyseStatementSequence();
        if (err.has_value())
            return err;
        // '}'
        next=nextToken();
        if(!next.has_value()||next.value().GetType()!=RIGHT_BRACE)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBrace);

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
    std::optional<CompilationError> Analyser::analyseStatementSequence()
    {
        while(1)
        {
            auto next = nextToken();
            if (!next.has_value())
                return {};
            unreadToken();
            if (next.value().GetType() != TokenType::IDENTIFIER &&
                next.value().GetType() != TokenType::PRINT &&
                next.value().GetType() != TokenType::LEFT_BRACE &&
                next.value().GetType() != TokenType::IF &&
                next.value().GetType() != TokenType::WHILE &&
                next.value().GetType() != TokenType::SCAN &&
                next.value().GetType() != TokenType::VOID &&
                next.value().GetType() != TokenType::INT &&
                next.value().GetType() != TokenType::RETURN) {
                return {};
            }
            std::optional<CompilationError> err;
            switch (next.value().GetType()) {
                case SEMICOLON:
                    nextToken();
                    break;
                case LEFT_BRACE:
                     err=analyseCompoundStatement();
                    if (err.has_value())
                        return err;
                    break;
                case IF:
                     err=analyseConditionStatement();
                    if (err.has_value())
                        return err;
                    break;
                case WHILE:
                     err=analyseLoopStatement();
                    if (err.has_value())
                        return err;
                    break;
                case PRINT:
                     err=analysePrintStatement();
                    if (err.has_value())
                        return err;
                    break;
                case SCAN:
                     err=analyseScanStatement();
                    if (err.has_value())
                        return err;
                    break;
                case IDENTIFIER:
                     err=analyseAssignmentStatement();
                    if (err.has_value())
                        return err;
                    break;
                case VOID:
                case INT:
                     err=analyseFunctionCall();
                    if (err.has_value())
                        return err;
                    break;
                case RETURN:
                     err=analyseReturnStatement();
                    if (err.has_value())
                        return err;
                    break;

                default:
                    break;
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
    std::optional<CompilationError>Analyser:: analyseStatement(){
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
            next.value().GetType() != TokenType::RETURN) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrinvalidStatement);
        }
        std::optional<CompilationError> err;
        switch (next.value().GetType()) {
            case SEMICOLON:
                nextToken();
                break;
            case LEFT_BRACE:
                 err=analyseCompoundStatement();
                if (err.has_value())
                    return err;
                break;
            case IF:
                 err=analyseConditionStatement();
                if (err.has_value())
                    return err;
                break;
            case WHILE:
                 err=analyseLoopStatement();
                if (err.has_value())
                    return err;
                break;
            case PRINT:
                 err=analysePrintStatement();
                if (err.has_value())
                    return err;
                break;
            case SCAN:
                 err=analyseScanStatement();
                if (err.has_value())
                    return err;
                break;
            case IDENTIFIER:
                 err=analyseAssignmentStatement();
                if (err.has_value())
                    return err;
                break;
            case VOID:
            case INT:
                 err=analyseFunctionCall();
                if (err.has_value())
                    return err;
                break;
            case RETURN:
                 err=analyseReturnStatement();
                if (err.has_value())
                    return err;
                break;

            default:
                break;
        }
        return {};
    }
    /*
     * 条件表达式  其他 true ==0 false
     * <condition> ::=
     * <expression>[<relational-operator><expression>]
     */
    std::optional<CompilationError>Analyser::analyseConditionExp()
    {
        auto err= analyseExpression();
        if (err.has_value())
            return err;
        while(1) {
            auto next = nextToken();
            if (!next.has_value() ||
                (next.value().GetType() != GREATER_THAN_SIGN &&
                 next.value().GetType() != LESS_THAN_SIGN &&
                 next.value().GetType() != GRT_EQU_SIGN &&
                 next.value().GetType() != LES_EQU_SIGN &&
                 next.value().GetType() != IS_EQU_SIGN &&
                 next.value().GetType() != NOT_EQU_SIGN  )) {
                unreadToken();
                return {};
            }

            auto err= analyseExpression();
            if (err.has_value())
                return err;
            // >
            if(next.value().GetType() == GREATER_THAN_SIGN){
                // -1 0 1  -> 0 0 any false false  true （第一个值大的是1，cmp与>逻辑相同）
                _instructions.emplace_back(Operation::ICMP,0);
            }
            // <
            else if(next.value().GetType() == LESS_THAN_SIGN){
                //乘以-1取反  -1 0 1 -> any 0 0 true false false
                _instructions.emplace_back(Operation::ICMP,0);
                _instructions.emplace_back(Operation::INEG,0);
            }
            // >= finish
            else if(next.value().GetType() == GRT_EQU_SIGN){
                // -1 0 1 -> 0 any any false  true true
                _instructions.emplace_back(Operation::ICMP,0);
                _instructions.emplace_back(Operation::IPUSH,1);
                _instructions.emplace_back(Operation::IADD,0);
            }
            // <= finish
            else if(next.value().GetType() == LES_EQU_SIGN){
                //符号取反后+1  -1 0 1 -> any any 0 true true false
                _instructions.emplace_back(Operation::ICMP,0);
                _instructions.emplace_back(Operation::IPUSH,1);
                _instructions.emplace_back(Operation::ISUB,0);
            }
            // ==
            else if(next.value().GetType() == IS_EQU_SIGN){
                _instructions.emplace_back(Operation::ICMP,0);
                // -1 0 1 -> 0 any 0 false true false
            }
            // != finish
            else if(next.value().GetType() == NOT_EQU_SIGN){
                _instructions.emplace_back(Operation::ISUB,0);

                // -1 0 1 -> any 0 any true false true
            }
        }
    }
    /*
     * 条件语句
     * <condition-statement> ::=
     * 'if' '(' <condition> ')' <statement> ['else' <statement>]
     * je s1
     * s0:
     * ifstmt
     * s1:
     * elsestmt
     */
    std::optional<CompilationError>Analyser:: analyseConditionStatement(){
        auto next=nextToken();
        if (!next.has_value()||next.value().GetType()!=IF)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteConditionStatement);
        //'(
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //condition
        auto err=analyseConditionExp();
        if (err.has_value())
            return err;

        _instructions.emplace_back(Operation::JE,0);
        insindex+=3;
        int s1=_instructions.size()-1;
        //s1意义如上
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //statement
         err=analyseStatement();
        if (err.has_value())
            return err;
        //'else'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=ELSE)
        {
            unreadToken();
            return {};
        }
        //statement
        _instructions[s1].SetX(insindex);
        //回填地址
        err=analyseStatement();
        if (err.has_value())
            return err;

    }
    //循环语句
    /*
     * <loop-statement> ::=
     * 'while' '(' <condition> ')' <statement>
     * 1求值<condition>
     * 2如果<condition>是false，跳转到步骤5
     *  S0:
     *  JNE S1
     *  [statement]
     *  JMP S0
     *  S1:
     *  ...
     * 3控制进入while的代码块并顺序执行
     * 4控制达到while代码块的尾部时，跳转到步骤1控制
     * 5跳过while结构，执行之后的代码块
     */
    std::optional<CompilationError> Analyser::analyseLoopStatement(){
        auto next=nextToken();
        if (!next.has_value()||next.value().GetType()!=WHILE)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteWhileStatement);
        //'('
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //<condition>
        int s0=insindex,s1;
        auto err=analyseConditionExp();
        if (err.has_value())
            return err;
        _instructions.emplace_back(Operation::JNE,0);
        insindex++;
        int jne=_instructions.size()-1;
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);

        //statement
       err= analyseStatement();
        if (err.has_value())
            return err;
        _instructions.emplace_back(Operation::JMP,s0);
        insindex++;
        //回填jne s1
        s1=insindex;
        _instructions[jne].SetX(s1);


    }
    //输出语句
    /*
     * <print-statement> ::= 'print' '(' [<printable-list>] ')' ';'
     * <printable-list>  ::= <printable> {',' <printable>}
     * <printable> ::= <expression>
     */
    std::optional<CompilationError> Analyser::analysePrintStatement(){
        auto next=nextToken();
        if (!next.has_value()||next.value().GetType()!=PRINT)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompletePrintStatement);
        //'('
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //<printable-list>
        next=nextToken();
        auto err=analyseExpression();
        if (err.has_value())
            return err;
        _instructions.emplace_back(Operation::IPRINT,0);
        insindex+=1;
        //,
        while(1)
        {
            next=nextToken();
            if (!next.has_value() || next.value().GetType() != TokenType::COMMA)
            {
                unreadToken();
                return{};
            }
            auto err=analyseExpression();
            if (err.has_value())
                return err;
            _instructions.emplace_back(Operation::IPRINT,0);
            insindex+=1;
        }

        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //';
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        return {};
    }
    //扫描语句
    /*
     * <scan-statement>  ::= 'scan' '(' <identifier> ')' ';'
     */
    std::optional<CompilationError> Analyser::analyseScanStatement(){
        auto next=nextToken();
        if (!next.has_value()||next.value().GetType()!=SCAN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteScanStatement);
        //'('
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //identtifier
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string str=next.value().GetValueString();
        /*
         * 查找变量的地址，然后保存。
         */
        int index,level;
        std::pair<int32_t ,int32_t > p=getIndex(str);
        index=p.first;
        level=p.second;
        _instructions.emplace_back(Operation::LOADA,level,index);//7
        _instructions.emplace_back(Operation::ISCAN,0);//1
        _instructions.emplace_back(Operation::ISTORE,0);//1
        insindex+=9;
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //';
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        return {};
    }
    //函数调用语句
    /*
     * <function-call> ::=
     * <identifier> '(' [<expression-list>] ')'
     */
    std::optional<CompilationError>Analyser:: analyseFunctionCall(){
        auto next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string str=next.value().GetValueString();

        //函数是否被声明成变量
        if(isDeclared(str))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVariableCall);
        //函数是否被声明过
        if(!isFunctionDeclared(str))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
        int index=getFuncIndex(str);
        //'('
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //[<expression-list>]
        auto err=analyseExpressionList();
        if (err.has_value())
            return err;

        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //';
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

        _instructions.emplace_back(Operation::CALL,index);
        insindex+=3;

        return {};
    }
    //返回语句
    /*
     * <return-statement> ::= 'return' [<expression>] ';'
     */
    std::optional<CompilationError>Analyser:: analyseReturnStatement(){
        auto next=nextToken();
        if (!next.has_value()||next.value().GetType()!=RETURN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteReturnStatement);
        next=nextToken();
        int prefix=-1;
        //预读一下判断是否有exp的存在
        if(!next.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteReturnStatement);
        if ( next.value().GetType() != TokenType::SEMICOLON)
        {
            unreadToken();
            auto err=analyseExpression();
            if (err.has_value())
                return err;
            prefix=1;
        }
        //';'
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        if(prefix==-1&&isret==false)
        {
            _instructions.emplace_back(Operation::POPN,_nextTokenIndex);//5
            _instructions.emplace_back(Operation::RET,0);//1
            insindex+=6;
        }
        else if(prefix==1&&isret==true)
        {_instructions.emplace_back(Operation::IRET,0);
        insindex+=1;}
        else
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrReturn);
        return {};

    }

	// <表达式> ::= <项>{<加法型运算符><项>}
	std::optional<CompilationError> Analyser::analyseExpression() {
		// <项>
		auto err = analyseItem();
		if (err.has_value())
			return err;

		// {<加法型运算符><项>}
		while (true) {
			// 预读
			auto next = nextToken();
			if (!next.has_value())
				return {};
			auto type = next.value().GetType();
			if (type != TokenType::PLUS_SIGN && type != TokenType::MINUS_SIGN) {
				unreadToken();
				return {};
			}

			// <项>
			err = analyseItem();
			if (err.has_value())
				return err;

			// 根据结果生成指令
			if(type==MINUS_SIGN) {
                _instructions.emplace_back(Operation::ISUB, 0);
                insindex+=1;
            }
			else if(type==PLUS_SIGN){
                _instructions.emplace_back(Operation::IADD, 0);
                insindex+=1;
			}

		}
		return {};
	}
	/*
	 * <表达式list>
	 * <expression-list> ::=
	 * <expression>{','<expression>}
	 */
    std::optional<CompilationError> Analyser::analyseExpressionList(){
        while(1)
        {
            auto err=analyseExpression();
            if (err.has_value())
                return err;
            auto next=nextToken();
            if (!next.has_value()||next.value().GetType()!=COMMA)
            {
                unreadToken();
                return {};
            }
        }
        return {};
    }
	// <赋值语句> ::= <标识符>'='<表达式>';'

	std::optional<CompilationError> Analyser::analyseAssignmentStatement() {
	    auto next=nextToken();
		// <标识符>
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		// 标识符声明过吗？
		std::string str=next.value().GetValueString();
        if(!Analyser::isDeclared(str))
        {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
        }
		// 标识符是常量吗？
		if(Analyser::isConstant(str))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrAssignToConstant);
		next =nextToken();

		//'='
        if (!next.has_value() || next.value().GetType() != TokenType::EQUAL_SIGN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidAssignment);


        //调用exp子程序
        auto err = analyseExpression();
        if (err.has_value())
            return err;
        /*exp子程序会将算好的值存在栈顶，只要我们计算identi的索引，然后使用sto指令就可以实现赋值
         * 但是要分清楚是未初始化的还是已经初始化的变量。
         * if(isUninitializedVariable(str))
         *    InitialToken(str);
         * 给未初始化的变量赋值，则需要把符号从未初始化表中删除，加入到初始化表中
         *
         * 寻找该变量在栈中存储的位置
         * 然后修改变量的值。
         */
        int index,level;
        std::pair<int32_t ,int32_t > p=getIndex(str);
        index=p.first;
        level=p.second;
        _instructions.emplace_back(Operation::LOADA,level,index);//7
        _instructions.emplace_back(Operation::ISCAN,0);//1
        _instructions.emplace_back(Operation::ISTORE,0);//1
        insindex+=9;
        //';'
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        return {};
	}


	// <项> :: = <因子>{ <乘法型运算符><因子> }

	std::optional<CompilationError> Analyser::analyseItem() {
		// 可以参考 <表达式> 实现
		//因子
        auto err = analyseFactor();
        if (err.has_value())
            return err;
		//{ <乘法型运算符><因子> }
        while (true) {
            // 预读
            auto next = nextToken();
            if (!next.has_value())
                return {};
            auto type = next.value().GetType();
            if (type != TokenType::DIVISION_SIGN && type != TokenType::MULTIPLICATION_SIGN) {
                unreadToken();
                return {};
            }

            //因子
            err = analyseFactor();
            if (err.has_value())
                return err;

            // 根据结果生成指令
            if (type == TokenType::DIVISION_SIGN)
            {
                _instructions.emplace_back(Operation::IDIV, 0);
                insindex+=1;
            }
            else if (type == TokenType::MULTIPLICATION_SIGN)
            {
                insindex+=1;
                _instructions.emplace_back(Operation::IMUL, 0);
            }
        }
		return {};
	}

	// <因子> ::= [<单目符号>]( <标识符> | <无符号整数> | '('<表达式>')' | <函数> )

	std::optional<CompilationError> Analyser::analyseFactor() {
		// [<符号>]
		auto next = nextToken();
		auto prefix = 1;
		if (!next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
		if (next.value().GetType() == TokenType::PLUS_SIGN)
			prefix = 1;
		else if (next.value().GetType() == TokenType::MINUS_SIGN) {
			prefix = -1;
		}
		else
			unreadToken();

		// 预读
		next = nextToken();
		if (!next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
		switch (next.value().GetType()) {
		    case TokenType::LEFT_BRACKET: {
                auto err = analyseExpression();
                if (err.has_value())
                    return err;
                next = nextToken();
                if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
                break;
            }
		    case TokenType::INTEGER: {
                int32_t num;
                num = std::any_cast<int>(next.value().GetValue());
                //进常量表
                addCONST(next.value());
                _instructions.emplace_back(Operation::LOADC, _CONSTS.size()-1);
                insindex+=2;
                break;
                //数字直接入栈
            }
            case TokenType::IDENTIFIER:{
                        std::string str =next.value().GetValueString();
                        //先看是否是变量
                        if(isDeclared(str)) {
                            if (isUninitializedVariable(str))
                                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
                            std::pair<int32_t,int32_t> p=getIndex(str);
                            int index=p.first, level=p.second;
                            _instructions.emplace_back(Operation::LOADA, level, index);//7
                            _instructions.emplace_back(Operation::ILOAD, 0);//1
                            insindex += 8;
                            //利用标识符找到常量、变量在栈中的索引，利用load指令载入identifi的值
                        }
                        //函数调用
                        else if(isFunctionDeclared(str))
                        {
                            auto err=analyseFunctionCall();
                            if (err.has_value())
                                return err;
                            break;
                        }
                        else
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
                        break;
                }
            default:return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrinvalidStatement);
            }
			// 这里和 <语句序列> 类似，需要根据预读结果调用不同的子程序
			// 但是要注意 default 返回的是一个编译错误



		// 取负
		if (prefix == -1)
        {
		    _instructions.emplace_back(Operation::INEG, 0);
            insindex+=1;
        }
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
    /*bool Analyser::InitialToken(std::string &str){
	    int index=getIndex(str);
	    if(!Analyser::isUninitializedVariable(str))
	        return false;

	    _uninitialized_vars.erase(str);
	    //消去在未初始化中这一声名
	    _vars[str]=index;
	    return true;
	}*/


    //常量表、函数表操作
	void Analyser::addCONST(const Token& tk) {
        if(tk.GetType()==IDENTIFIER)//string
        {
            std::string str=tk.GetValueString();
            _CONSTS.push_back(make_pair(str,1));
            _constant[str]=_CONSTS.size()-1;
        }
        else if(tk.GetType()==INTEGER)
        {
            int32_t num;
            num = std::any_cast<int>(tk.GetValue());
            std:: string str=std::to_string(num);
            _CONSTS.push_back(make_pair(str,0));
            _constant[str]=_CONSTS.size()-1;
        }
	}
    bool Analyser::isFunctionDeclared(const std::string& s){
        return _function.find(s) != _function.end();
    }

    void Analyser::addFuntion(std::string name,int level,int para){
        int nameindex;
        getConstIndex(name,nameindex);
        Function f(nameindex,para,level);
        _funcs.emplace_back(f);
        _function[name]=_funcs.size()-1;
    }
    void Analyser::getConstIndex(const std::string& s,int &index){
        index=_constant[s];

    }
    int32_t Analyser::getFuncIndex(const std::string& s){
        return _function[s];
    }
    //变量表

	bool Analyser::isDeclared(const std::string& s) {
		return isConstant(s) || isUninitializedVariable(s) || isInitializedVariable(s);
	}
	bool Analyser::isUninitializedVariable(const std::string& s) {
        return _find(s,_unit_var)!=-1;
	}
	bool Analyser::isInitializedVariable(const std::string&s) {
        return _find(s,_var)!=-1;
	}
	bool Analyser::isConstant(const std::string&s) {
        return _find(s,_const)!=-1;
	}
    //底层操作，添加，查找
    void Analyser::_add(const Token& tk, std::vector<std::pair<std::string,int32_t>>& sk) {
        if (tk.GetType() != TokenType::IDENTIFIER)
            DieAndPrint("only identifier can be added to the table.");
        std::string s=tk.GetValueString();
        std::pair<std::string,int32_t> p(s,_nextTokenIndex);
        _nextTokenIndex++;
        sk.push_back(p);
    }
    int32_t Analyser::_find(const std::string& s,std::vector<std::pair<std::string,int32_t>>& sk)
    {
        for(std::pair<std::string,int32_t> &p:sk)
        {
            if(p.first==s)
                return p.second;
        }
        return -1;
    }
    int32_t Analyser::_findLocal(const std::string& s)
    {
        auto beg=Cbp;
        for(;beg!=_const.end();beg++)
        {
            if((*beg).first==s)
                return (*beg).second;
        }
        beg=Vbp;
        for(;beg!=_var.end();beg++) {
            if ((*beg).first == s)
                return (*beg).second;
        }
        beg=Ubp;
        for(;beg!=_unit_var.end();beg++) {
            if ((*beg).first == s)
                return (*beg).second;
        }
        return -1;
    }
    int32_t Analyser::_findGlobal(const std::string& s)
    {
        auto beg=_const.begin();
        for(;beg!=Cbp;beg++)
        {
            if((*beg).first==s)
                return (*beg).second;
        }
        beg=_var.begin();
        for(;beg!=Vbp;beg++) {
            if ((*beg).first == s)
                return (*beg).second;
        }
        beg=_unit_var.begin();
        for(;beg!=Ubp;beg++) {
            if ((*beg).first == s)
                return (*beg).second;
        }
        return -1;
    }

    std::pair<int32_t,int32_t> Analyser::getIndex(const std::string& s) {
	    std::pair<int32_t ,int32_t > p(-1,-1);
	    //先找局部变量
	    int index=-1;
	    if((index=_findLocal(s))!=-1)
	        p.second=0;
	    else if((index=_findGlobal(s))!=-1)
            p.second=1;
	    p.first=index;
	    return p;
    }
    //添加变量、常量、未初始化变量
    void Analyser::addVariable(const Token& tk) {
        _add(tk, _var);
    }
    void Analyser::addConstant(const Token& tk) {
        _add(tk, _const);
    }
    void Analyser::addUninitializedVariable(const Token& tk) {
        _add(tk, _unit_var);
    }


    //高级操作，对帧栈的操作
   /* void InitStack(){
        Vpre=_var.begin();
        Upre=_unit_var.begin();
        Cpre=_const.begin();
    }*/
    //进入一个新块。将pre指针指向当前 prepre=pre,pre=top;top++
    void Analyser::loadNewLevel(){
	    Vbp=_var.end();
	    Cbp=_const.end();
	    Ubp=_unit_var.end();
    }
    //弹出一个块。
    void Analyser::popCurrentLevel(){
	    _var.erase(Vbp,_var.end());
        _const.erase(Cbp,_const.end());
        _unit_var.erase(Ubp,_unit_var.end());

    }

}