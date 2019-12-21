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
    class Function
    {
    public:
        Function(int nameindex,int para,int level) : nameindex(nameindex), para(para),level(level) {}
    public:
        int nameindex;
        int para;
        int level;

    };
    class Program{
    public:
        Program(std::vector<std::pair<std::string,int>> _CONSTS,std::vector<Function> _funcs,std::vector<std::vector<Instruction>> _program):
        _CONSTS(_CONSTS),_funcs(_funcs),_program(_program){

        }
        Program(){}
        std::vector<std::pair<std::string,int>> getConstList(){
            return _CONSTS;
        }
        std::vector<Function> getFunctionList(){
            return _funcs;
        }
        std::vector<Instruction> getBeginCode(){
            return _program[0];
        }
        std::vector<std::vector<Instruction>> getProgramList(){
            return _program;
            //下标从1开始
        }
    private:
        std::vector<std::pair<std::string,int>> _CONSTS;
        std::vector<Function> _funcs;
        std::vector<std::vector<Instruction>> _program;
    };
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
			{
		    //初始化

			}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
        std::pair<Program, std::optional<CompilationError>> Analyse();
	private:
		// 所有的递归子程序
        std::optional<CompilationError> analyseC0Program();

		// <变量声明>
		std::optional<CompilationError> analyseVariableDeclaration();
		// 函数声名
        std::optional<CompilationError> analyseFunctionDeclaration();
		// <语句序列>
		std::optional<CompilationError> analyseStatementSequence();
        // <单语句>
        std::optional<CompilationError> analyseStatement();
        //复合语句
        //<参数列表>
        std::pair<int32_t,std::optional<CompilationError>> analyseParameterClause();
		// <表达式>
		std::optional<CompilationError> analyseExpression();
		//<表达式list>
        std::pair<int32_t ,std::optional<CompilationError>> analyseExpressionList();
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
		void _add(const Token&, std::map<std::string, int32_t>& sk);
        int32_t _find(const std::string& ,std::map<std::string, int32_t>& );
        std::pair<int32_t,int32_t> _findLocal(const std::string& );
        int32_t _findGlobal(const std::string& );
		// 添加变量、常量、未初始化的变量
		//添加函数表 ，返回索引
        void addFuntion(std::string name,int level,int para);
        void addVariable(const Token&);
		void addConstant(const Token&);
		void addUninitializedVariable(const Token&);
		//添加字面量 返回表的索引
        void addCONST(const Token&);
		// 是否被声明过
		bool isDeclared(const std::string& );
        bool isFunctionDeclared(const std::string& );
		// 是否是未初始化的变量
		bool isUninitializedVariable(const std::string& );
		// 是否是已初始化的变量
		bool isInitializedVariable(const std::string& );
		// 是否是常量
		bool isConstant(const std::string& );

		int32_t getConstIndex(const Token&);
        int32_t getFuncIndex(const std::string& );

        //符号表管理
        void InitStack();
        void loadNewLevel();//将pre指针指向当前 prepre=pre,pre=top;top++
        void popCurrentLevel();//top=pre ,pre=prepre
        void loadOne(const std::string& s);//top++
        std::pair<int32_t,int32_t>  getIndex(const std::string&);

        std::vector<std::pair<std::string,int32_t>>::iterator getGlobalVarEnd();
        std::vector<std::pair<std::string,int32_t>>::iterator getGlobalUnitEnd();

        std::vector<std::pair<std::string,int32_t>>::iterator getGlobalConstEnd();

        // 获得 {变量，常量} 在栈上的偏移
        bool isBeenLoad(Token &token);

        bool isCONST(std::string basicString);

        uint32_t getCONSTindex(Token token);

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
        int32_t _nextGTokenIndex;
        bool InitialToken(std::string &str);
        std::map<std::string, int32_t>* _var;
        std::map<std::string, int32_t>* _unit_var;
        std::map<std::string, int32_t>* _const;
        std::map<std::string, int32_t> g_var;//仅做全局变量为空时迭代器所指的地方
        std::map<std::string, int32_t> g_unit_var;
        std::map<std::string, int32_t> g_const;
        std::vector<std::string> localVars;
        std::vector<std::map<std::string, int32_t>*> _var_table;
        std::vector<std::map<std::string, int32_t>*> _unit_table;
        std::vector<std::map<std::string, int32_t>*> _const_table;
        bool isGlabol;

        void deleteVar(std::string basicString);

        Function getFunc(int32_t index);

        bool canbeDeclared(const std::string &s);

        std::optional<CompilationError> analyseSingleLineComment();

        std::optional<CompilationError> analyseMultiLineComment();
    };

}