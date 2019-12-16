#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"

#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <vector>
#include <stack>
#include <cstddef> // for std::size_t

namespace miniplc0 {

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}),_program({}), _current_pos(0, 0),
			_function({}),_constant({}),_CONSTS({}),_funcs({}),
			_unit_var({}), _var({}), _const({}),
			_nextTokenIndex(0),_nextConstIndex(0),_nextFuncIndex(0)
			{}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
		std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyse();
	private:
		// 所有的递归子程序
        std::optional<CompilationError> analyseC0Program();

		// <变量声明>
		std::optional<CompilationError> analyseVariableDeclaration();
		// 函数声名
        std::optional<CompilationError> analyseFunctionDeclaration()
		// <语句序列>
		std::optional<CompilationError> analyseStatementSequence();
        // <单语句>
        std::optional<CompilationError> analyseStatement();
        //复合语句
        std::optional<CompilationError> Analyser::analyseCompoundStatement()
        //<参数列表>
        std::optional<CompilationError> analyseParameterClause();
		// <表达式>
		std::optional<CompilationError> analyseExpression();
		//<表达式list>
        std::optional<CompilationError> analyseExpressionList();
		// <赋值语句>
		std::optional<CompilationError> analyseAssignmentStatement();

		// <项>
		std::optional<CompilationError> analyseItem();
		// <因子>

		std::optional<CompilationError> analyseFactor();
		//{}块
		std::optional<CompilationError> analyseCompoundStatement();
		//条件语句
        std::optional<CompilationError> analyseConditionStatement();
        //循环语句
        std::optional<CompilationError> analyseLoopStatement();
        //输出语句
        std::optional<CompilationError> analysePrintStatement();
        //扫描语句
        std::optional<CompilationError> analyseScanStatement();
        //函数调用语句
        std::optional<CompilationError> analyseFunctionCall();
        //返回语句
        std::optional<CompilationError> analyseReturnStatement();
        //条件表达式
        std::optional<CompilationError> analyseConditionExp();
		// Token 缓冲区相关操作

		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();

		// 下面是符号表相关操作

		// helper function
		void _add(const Token&, std::vector<std::pair<std::string,int32_t>>& sk);
        int32_t _find(const std::string& ,std::vector<std::pair<std::string,int32_t>>& );
        int32_t _findLocal(const std::string& );
        int32_t _findGlobal(const std::string& );
		// 添加变量、常量、未初始化的变量
		//添加函数表 ，返回索引
        void Analyser::addFuntion(std::string name,int level,int para);
        void addVariable(const Token&);
		void addConstant(const Token&);
		void addUninitializedVariable(const Token&);
		//添加字面量 返回表的索引
        int addCONST(const Token& tk);
		// 是否被声明过
		bool isDeclared(const std::string& );
        bool isFunctionDeclared(const std::string& );
		// 是否是未初始化的变量
		bool isUninitializedVariable(const std::string& );
		// 是否是已初始化的变量
		bool isInitializedVariable(const std::string& );
		// 是否是常量
		bool isConstant(const std::string& s);

		void getConstIndex(const std::string& s,int &index);
        void getFuncIndex(const std::string& s,int &index);

        //符号表管理
        void loadNewLevel();//将pre指针指向当前 prepre=pre,pre=top;top++
        void popCurrentLevel();//top=pre ,pre=prepre
        void loadOne(const string& s);//top++
        pair<int32_t,int32_t>  getIndex(const std::string&);
        // 获得 {变量，常量} 在栈上的偏移


	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _instructions;
		std::vector<std::vector<Instruction>> _program;
		std::pair<uint64_t, uint64_t> _current_pos;

		// 为了简单处理，我们直接把符号表耦合在语法分析里
		// 变量                   示例
		// _uninitialized_vars    int a;
		// _vars                  int a=1;
		// _consts                const a=1;
		/*
		std::map<std::string, int32_t> _uninitialized_vars;
		std::map<std::string, int32_t> _vars;
        std::map<std::string, int32_t> _consts;
        */
        std::map<std::string, int32_t> _function;
        std::map<std::string, int32_t> _constant;
		std::vector<std::pair<std::string,int>> _CONSTS;
		std::vector<Function> _funcs;
		// 下一个 token 在栈的偏移
		int32_t _nextTokenIndex;
        int32_t _nextFuncIndex;
        int32_t _nextConstIndex;
        bool InitialToken(std::string &str);
        std::vector<std::pair<std::string,int32_t>> _var;
        std::vector<std::pair<std::string,int32_t>> _unit_var;
        std::vector<std::pair<std::string,int32_t>> _const;
        std::vector<std::pair<std::string,int32_t>>::iterator Vbp;
        std::vector<std::pair<std::string,int32_t>>::iterator Ubp;
        std::vector<std::pair<std::string,int32_t>>::iterator Cbp;
    };
	class Function
    {
    public:
        Function(int nameindex,int para,int level) : _nameindex(nameindex), _para(para),_level(level) {}
    public:
	    int nameindex;
	    int para;
	    int level;

    };
}