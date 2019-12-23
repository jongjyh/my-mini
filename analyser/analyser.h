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
    extern int insindex;
    extern std::vector<miniplc0::Instruction> _instructions;
    class Var
    {
    private:
        int32_t _index;
        TokenType _type;
        bool isConst;
        bool isUnit;
        bool isGlobal;
    public:
        bool isGlobal1() const {
            return isGlobal;
        }

        bool isConst1() const {
            return isConst;
        }

        bool isUnit1() const {
            return isUnit;
        }

        void setIsConst(bool isConst) {
            Var::isConst = isConst;
        }

        void setIsUnit(bool isUnit) {
            Var::isUnit = isUnit;
        }

    public:
        void setIndex(int32_t index) {
            _index = index;
        }

        void setType(TokenType type) {
            _type = type;
        }

        int32_t getIndex() const {
            return _index;
        }

        TokenType getType() const {
            return _type;
        }

    public:
        Var(int32_t index, TokenType type, bool isConst, bool isUnit, bool isGlobal) : _index(index), _type(type),
                                                                                       isConst(isConst), isUnit(isUnit),
                                                                                       isGlobal(isGlobal) {}

        Var(){_type=TokenType ::LEFT_BRACE;_index=0;}
    };
    class Function
    {
    public:
        Function(int nameindex, int level, const std::vector<TokenType> &paras, TokenType ret) : nameindex(nameindex),
                                                                                                 level(level),
                                                                                                 paras(paras),
                                                                                                 ret(ret) {}

    public:
        int nameindex;
        int level;
        std::vector<TokenType> paras;
        TokenType ret;

        TokenType getRet() const {
            return ret;
        }

        void setRet(TokenType ret) {
            Function::ret = ret;
        }

        void insertPara(TokenType type)
        {
            paras.emplace_back(type);
        }
        int32_t getParaSize()
        {
            return paras.size();
        }

        const std::vector<TokenType> &getParas() const {
            return paras;
        }
    };
    class Program{
    public:
        Program(std::vector<std::pair<std::string,int>> _CONSTS,std::vector<Function> _funcs,std::vector<std::vector<Instruction>> _program):
        _CONSTS(_CONSTS),_funcs(_funcs),_program(_program){}
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
			: _tokens(std::move(v)), _offset(0),_program({}), _current_pos(0, 0),
			_function({}),_constant({}),_CONSTS({}),_funcs({}),_var(nullptr),
			_nextTokenIndex(0),_nextConstIndex(0),_nextFuncIndex(0)
			{
		    //初始化

			}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;
        struct Factor;
        struct Item;
        struct Expression;
        struct Call;
        struct Char;
        struct Integer;
        struct Variable;
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
        std::pair<std::vector<TokenType>,std::optional<CompilationError>> analyseParameterClause();
		// <表达式>
        std::pair<std::optional<Expression*>,std::optional<CompilationError>> analyseExpression();
		//<表达式list>
        std::pair<std::optional<std::vector<Expression*>> ,std::optional<CompilationError>> analyseExpressionList();
		// <赋值语句>
		std::optional<CompilationError> analyseAssignmentStatement();

		// <项>
        std::pair<std::optional<Item>,std::optional<CompilationError>> analyseItem();
		// <因子>

        std::pair<std::optional<Factor*>,std::optional<CompilationError>> analyseFactor();
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
        std::pair<std::optional<Call*>,std::optional<CompilationError>> analyseFunctionCall();
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
        void
        _add(const Token &, std::map<std::string, Var> &, const TokenType &, const bool &,const bool &);
        Var _find(const std::string& ,std::map<std::string, Var>& );
        TokenType getType(std::string& );
        Var _findLocal(const std::string& );
        Var _findGlobal(const std::string& );
		// 添加变量、常量、未初始化的变量
		//添加函数表 ，返回索引
        void addFuntion(std::string ,int  ,std::vector<TokenType>&,TokenType&);
        void addVariable(const Token&,const TokenType&);
		void addConstant(const Token&,const TokenType&);
		void addUninitializedVariable(const Token&,const TokenType&);
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


        // 获得 {变量，常量} 在栈上的偏移
        bool isBeenLoad(Token &token);

        bool isCONST(std::string basicString);


	private:
		std::vector<Token> _tokens;
		std::size_t _offset;

		std::vector<std::vector<Instruction>> _program;
		std::pair<uint64_t, uint64_t> _current_pos;

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
        std::map<std::string, Var>* _var;
        std::map<std::string, Var> g_var;//仅做全局变量为空时迭代器所指的地方
        std::vector<std::string> localVars;
        std::vector<std::map<std::string, Var>*> _var_table;

        bool isGlabol;

        void deleteVar(std::string basicString);

        Function getFunc(int32_t index);

        bool canbeDeclaredInThisPart(const std::string &s);


        Var getVar(const std::string &s);

	public:

	    struct Item { //* /
            std::vector<Factor*> factors;
            std::vector<TokenType> mul;

            Item(const std::vector<Factor *> &factors, const std::vector<TokenType> &mul) : factors(factors),
                                                                                            mul(mul) {}

            TokenType generation()
            {
                if(mul.size()==0)
                {
                    return factors[0]->generation();
                }
                for(int i=0;i<mul.size();i++)
                {
                    factors[i]->generation();
                    factors[i+1]->generation();
                    if(mul[i]==DIVISION_SIGN)
                    {
                        _instructions.emplace_back(Operation::IDIV, 0);
                        insindex+=1;
                    }
                    else if (mul[i]==MULTIPLICATION_SIGN)
                    {
                        insindex+=1;
                        _instructions.emplace_back(Operation::IMUL, 0);
                    }
                }
                return INT;
            }
        };
        struct Factor {
            TokenType  sign;

            Factor(TokenType sign) : sign(sign) {}

            virtual  TokenType generation(){
                if(sign==MINUS_SIGN)
                {
                    insindex+=1;
                    _instructions.emplace_back(Operation::INEG, 0);
                }
                return TokenType ::NULL_TOKEN;
            }
        };
        struct Variable : Factor {
            Var var;

            Variable(TokenType sign, const Var &var) : Factor(sign), var(var) {}

            TokenType generation()
            {
                int level=var.isGlobal1(),index=var.getIndex()-1;
                _instructions.emplace_back(Operation::LOADA, level, index);//7
                _instructions.emplace_back(Operation::ILOAD, 0);//1
                insindex += 8;
                Factor::generation();
                return var.getType();
            }
        };
        struct Integer : Factor {
            int32_t index;

            Integer(TokenType sign, int32_t index) : Factor(sign),  index(index) {}

            TokenType generation()
            {
                _instructions.emplace_back(Operation::LOADC, index);
                insindex+=3;
                Factor::generation();
                return INT;
            }
        };
        struct Expression : Factor {
            std::vector<TokenType> add;
            std::vector<Item> items;

            Expression(TokenType sign, const std::vector<TokenType> &add, const std::vector<Item > &items) : Factor(
                    sign), add(add), items(items) {}

            TokenType  generation(){
                if(items.size()==1)
                {
                    Factor::generation();
                    return items[0].generation();
                }
                for(int i=0;i<add.size();i++)
                {
                    items[i].generation();
                    items[i+1].generation();
                    if(add[i]==MINUS_SIGN)
                    {
                        _instructions.emplace_back(Operation::ISUB, 0);
                        insindex+=1;
                    }
                    else if (add[i]==PLUS_SIGN)
                    {
                        insindex+=1;
                        _instructions.emplace_back(Operation::IADD, 0);
                    }
                }
                Factor::generation();
                return INT;
            }
        };
        struct Call : Factor {
            Function function;
            std::vector<Expression*> exps;
            int index;
            Call(TokenType sign, const Function &function, const std::vector<Expression *> &exps) : Factor(sign),
                                                                                                    function(function),
                                                                                                    exps(exps) {}

            Call(TokenType sign, const Function &function, const std::vector<Expression *> &exps, int index) : Factor(
                    sign), function(function), exps(exps), index(index) {}

            TokenType generation(){
                auto para=function.getParas();
                for(int i=0;i<exps.size();i++)
                {
                    auto type = exps[i]->generation();
                }
                Factor::generation();

                _instructions.emplace_back(Operation::CALL,index);
                return function.getRet();
            }
        };
        struct Char : Factor {
            int32_t index;
            Char(TokenType sign, int32_t index) : Factor(sign), index(index) {}
            TokenType generation()
            {
                _instructions.emplace_back(Operation::LOADC, index);
                insindex+=3;
                Factor::generation();
                return CHAR;
            }
        };
    };

}