#include "analyser.h"

#include <climits>

#include<iostream>
int level=0; //层级，全局时为0
namespace miniplc0 {
	std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyser::Analyse() {
		auto err = analyseProgram();
		if (err.has_value())
			return std::make_pair(std::vector<Instruction>(), err);
		else
			return std::make_pair(_instructions, std::optional<CompilationError>());
	}
	//<C0-program> ::= {<variable-declaration>}{<function-definition>}
    std::optional<CompilationError> Analyser::analyseC0Program()
    {
	    auto err = analyseVariableDeclaration();
        if (err.has_value())
            return err;
        auto err = analyseFunctionDeclaration();
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

<function-call> ::=
    <identifier> '(' [<expression-list>] ')'
<expression-list> ::=
    <expression>{','<expression>}
     */
    std::optional<CompilationError> Analyser::analyseFunctionDeclaration()
    {
        auto next = nextToken();
        //预读
        if (!next.has_value()||
            (next.value().GetType() != TokenType::VOID&&
             next.value().GetType() != TokenType::INT))
        {
            unreadToken();
            return {};
        }
        //<type-specifier> void or int
        auto ret=next.value().GetType();
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
        //函数名加入常量表，记录常量表中的index
        int nameindex=addCONST(next);
        //<parameter-clause>
        auto err=analyseParameterClause();
        if (err.has_value())
            return err;
        //<compound-statement>

        err=analyseStatementSequence();
        if (err.has_value())
            return err;
    }
    //<parameter-clause> ::=
    //    '(' [<parameter-declaration-list>] ')'
    //<parameter-declaration-list> ::=
    //    <parameter-declaration>{','<parameter-declaration>}
    //<parameter-declaration> ::=
    //    [<const-qualifier>]<type-specifier><identifier>
    std::optional<CompilationError> Analyser::analyseParameterClause(){

        // '('
        auto next=nextToken();
        if(!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBracket);
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
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidParameterDeclaration);
            TokenType  type=next.value().GetType();
            //<identifier>
            next = nextToken();
            std::string str = next.value().GetValueString();
            if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

            /*
             * 处理参数声明
             */

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
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrFuctionBracket);

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
            TokenType  type=next.value().GetType();

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
                if (next.has_value() && next.value().GetType() == TokenType::EQUAL_SIGN)
                {
                    // '<表达式>'
                    auto err = analyseExpression();
                    if (err.has_value())
                        return err;
                    /*
                     * 声名一个初始化变量
                     */
                    continue;
                }
                // ','
                if (next.has_value() && next.value().GetType() == TokenType::COMMA) {
                    /*
                     * 声名一个未初始化变量
                     */
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
        auto err=analyseStatementSequence();
        if (err.has_value())
            return err;
        // '}'
        auto next=nextToken();
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
                    auto err=analyseCompoundStatement();
                    if (err.has_value())
                        return err;
                    break;
                case IF:
                    auto err=analyseConditionStatement();
                    if (err.has_value())
                        return err;
                    break;
                case WHILE:
                    auto err=analyseLoopStatement();
                    if (err.has_value())
                        return err;
                    break;
                case PRINT:
                    auto err=analysePrintStatement();
                    if (err.has_value())
                        return err;
                    break;
                case SCAN:
                    auto err=analyseScanStatement();
                    if (err.has_value())
                        return err;
                    break;
                case IDENTIFIER:
                    auto err=analyseAssignmentStatement();
                    if (err.has_value())
                        return err;
                    break;
                case VOID:
                case INT:
                    auto err=analyseFunctionCall();
                    if (err.has_value())
                        return err;
                    break;
                case RETURN:
                    auto err=analyseReturnStatement();
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
                auto err=analyseCompoundStatement();
                if (err.has_value())
                    return err;
                break;
            case IF:
                auto err=analyseConditionStatement();
                if (err.has_value())
                    return err;
                break;
            case WHILE:
                auto err=analyseLoopStatement();
                if (err.has_value())
                    return err;
                break;
            case PRINT:
                auto err=analysePrintStatement();
                if (err.has_value())
                    return err;
                break;
            case SCAN:
                auto err=analyseScanStatement();
                if (err.has_value())
                    return err;
                break;
            case IDENTIFIER:
                auto err=analyseAssignmentStatement();
                if (err.has_value())
                    return err;
                break;
            case VOID:
            case INT:
                auto err=analyseFunctionCall();
                if (err.has_value())
                    return err;
                break;
            case RETURN:
                auto err=analyseReturnStatement();
                if (err.has_value())
                    return err;
                break;

            default:
                break;
        }
        return {};
    }
    /*
     * 条件表达式
     * <condition> ::=
     * <expression>[<relational-operator><expression>]
     */
    std::optional<CompilationError>Analyser::analyseConditionExp()
    {

    }
    /*
     * 条件语句
     * <condition-statement> ::=
     * 'if' '(' <condition> ')' <statement> ['else' <statement>]
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
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //statement
        auto err=analyseStatement();
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
        auto err=analyseStatement();
        if (err.has_value())
            return err;

    }
    //循环语句
    /*
     * <loop-statement> ::=
     * 'while' '(' <condition> ')' <statement>
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
        auto err=analyseConditionExp();
        if (err.has_value())
            return err;
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string str=next.value().GetValueString();
        /*
         * 输入某个变量的值
         */
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoBracket);
        //statement
        auto err= analyseStatement();
        if (err.has_value())
            return err;
        //';
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        return {};
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
        auto err=analyseExpressionList();
        if (err.has_value())
            return err;
        /*
         * 输入某个变量的值
         */
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
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
        int index,level;
        getIndex(str,&level,&index);
        _instructions.emplace_back(Operation::LOADA,level,index);
        _instructions.emplace_back(Operation::ISCAN,0);
        _instructions.emplace_back(Operation::ISTORE,0);

        /*
         * 输入某个变量的值
         */
        //')'
        next=nextToken();
        if (!next.has_value()||next.value().GetType()!=LEFT_BRACKET)
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
        int index=getFunctionIndex(str);
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
        if(prefix==-1)
            _instructions.emplace_back(Operation::RET,0);
        else if(prefix==1)
            _instructions.emplace_back(Operation::IRET,0);
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
			if(type==MINUS_SIGN)
            _instructions.emplace_back(Operation::ISUB, 0);
			else if(type==PLUS_SIGN)
                _instructions.emplace_back(Operation::IADD, 0);
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
        getIndex(str,&level,&index);
        _instructions.emplace_back(Operation::LOADA,level,index);
        _instructions.emplace_back(Operation::ISCAN,0);
        _instructions.emplace_back(Operation::ISTORE,0);

        //';'
        next=nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        return {};
	}

	// <输出语句> ::= 'print' '(' <表达式> ')' ';'
	std::optional<CompilationError> Analyser::analyseOutputStatement() {
		// 如果之前 <语句序列> 的实现正确，这里第一个 next 一定是 TokenType::PRINT
		auto next = nextToken();

		// '('
		next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidPrint);

		// <表达式>
		auto err = analyseExpression();
		if (err.has_value())
			return err;

		// ')'
		next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidPrint);

		// ';'
		next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

		// 生成相应的指令 print
		_instructions.emplace_back(Operation::IPRINT, 0);
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
                _instructions.emplace_back(Operation::IDIV, 0);
            else if (type == TokenType::MULTIPLICATION_SIGN)
                _instructions.emplace_back(Operation::IMUL, 0);
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
		    case TokenType::UNSIGNED_INTEGER: {
                int32_t num;
                num = std::any_cast<int>(next.value().GetValue());
                //进常量表
                addCONST(next);
                break;
                //数字直接入栈
            }
            case TokenType::IDENTIFIER:{
                        std::string str =next.value().GetValueString();
                        if(!isDeclared(str))
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);

                        if(isUninitializedVariable(str))
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
                        int index,level;
                        getIndex(str,&index,&level);
                        _instructions.emplace_back(Operation::LOADA, level,index);
                        _instructions.emplace_back(Operation::ILOAD, 0);
                        //利用标识符找到常量、变量在栈中的索引，利用load指令载入identifi的值
                        break;
                }
                case TokenType ::{
                /*
                 * 函数call
                 */
                auto err=analyseFunctionCall();
                if (err.has_value())
                    return err;
                break;
            }
			// 这里和 <语句序列> 类似，需要根据预读结果调用不同的子程序
			// 但是要注意 default 返回的是一个编译错误
		default:
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
		}

		// 取负
		if (prefix == -1)
			_instructions.emplace_back(Operation::INEG, 0);
		return {};
	}

	std::optional<Token> Analyser::nextToken() {
        if (_offset == _tokens.size())
            return {};
        // 考虑到 _tokens[0..._offset-1] 已经被分析过了
        // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
        _current_pos = _tokens[_offset].GetEndPos();
        return _tokens[_offset++];
    }

    bool Analyser::InitialToken(std::string &str){
	    int index=getIndex(str);
	    if(!Analyser::isUninitializedVariable(str))
	        return false;

	    _uninitialized_vars.erase(str);
	    //消去在未初始化中这一声名
	    _vars[str]=index;
	    return true;
	}
	void Analyser::unreadToken() {
		if (_offset == 0)
			DieAndPrint("analyser unreads token from the begining.");
		_current_pos = _tokens[_offset - 1].GetEndPos();
		_offset--;
	}

	void Analyser::_add(const Token& tk, std::map<std::string, int32_t>& mp) {
		if (tk.GetType() != TokenType::IDENTIFIER)
			DieAndPrint("only identifier can be added to the table.");
		mp[tk.GetValueString()] = _nextTokenIndex;
		_nextTokenIndex++;
	}

	void Analyser::addVariable(const Token& tk) {
		_add(tk, _vars);
	}
    void Analyser::addConstant(const Token& tk) {
        _add(tk, _consts);
    }
	void Analyser::addCONST(const Token& tk) {
        if(tk.GetType()==IDENTIFIER)//string
        {
            std::string str=tk.GetValueString();
            _CONSTS.push_back(make_pair(str,1));
        }
        else if(tk.GetType()==INT)
        {
            int32_t num;
            num = std::any_cast<int>(next.value().GetValue());
            std:: string str=std::to_string(num);
            _CONSTS.push_back(make_pair(str,0));
        }
	}
    bool isFunctionDeclared(const std::string&){
        _funcs.
    }
	void Analyser::addUninitializedVariable(const Token& tk) {
		_add(tk, _uninitialized_vars);
	}
    void addFuntion(std::string name,int level,int para){

    }
	int32_t Analyser::getIndex(const std::string& s) {
		if (_uninitialized_vars.find(s) != _uninitialized_vars.end())
			return _uninitialized_vars[s];
		else if (_vars.find(s) != _vars.end())
			return _vars[s];
		else
			return _consts[s];
	}

	bool Analyser::isDeclared(const std::string& s) {
		return isConstant(s) || isUninitializedVariable(s) || isInitializedVariable(s);
	}

	bool Analyser::isUninitializedVariable(const std::string& s) {
		return _uninitialized_vars.find(s) != _uninitialized_vars.end();
	}
	bool Analyser::isInitializedVariable(const std::string&s) {
		return _vars.find(s) != _vars.end();
	}

	bool Analyser::isConstant(const std::string&s) {
		return _consts.find(s) != _consts.end();
	}
}