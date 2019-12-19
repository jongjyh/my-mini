#include "3rd_party/fmt/include/fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
	template<>
	struct formatter<miniplc0::ErrorCode> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::ErrorCode &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::ErrNoError:
				name = "No error.";
				break;
			case miniplc0::ErrStreamError:
				name = "Stream error.";
				break;
			case miniplc0::ErrEOF:
				name = "EOF";
				break;
			case miniplc0::ErrInvalidInput:
				name = "The input is invalid.";
				break;
			case miniplc0::ErrInvalidIdentifier:
				name = "Identifier is invalid";
				break;
			case miniplc0::ErrIntegerOverflow:
				name = "The integer is too big(int64_t).";
				break;
			case miniplc0::ErrNoBegin:
				name = "The program should start with 'begin'.";
				break;
			case miniplc0::ErrNoEnd:
				name = "Program ending is wrong!";
				break;
			case miniplc0::ErrNeedIdentifier:
				name = "Need an identifier here.";
				break;
			case miniplc0::ErrConstantNeedValue:
				name = "The constant need a value to initialize.";
				break;
			case miniplc0::ErrNoSemicolon:
				name = "Zai? Wei shen me bu xie fen hao.";
				break;
			case miniplc0::ErrInvalidVariableDeclaration:
				name = "The declaration is invalid.";
				break;
			case miniplc0::ErrIncompleteExpression:
				name = "The expression is incomplete.";
				break;
			case miniplc0::ErrNotDeclared:
				name = "The variable or constant must be declared before being used.";
				break;
			case miniplc0::ErrAssignToConstant:
				name = "Trying to assign value to a constant.";
				break;
			case miniplc0::ErrDuplicateDeclaration:
				name = "The variable or constant has been declared.";
				break;
			case miniplc0::ErrNotInitialized:
				name = "The variable has not been initialized.";
				break;
			case miniplc0::ErrInvalidAssignment:
				name = "The assignment statement is invalid.";
				break;
			case miniplc0::ErrInvalidPrint:
				name = "The output statement is invalid.";
				break;
			case miniplc0::ErrRelationSymbol:
                name = "The output statement is invalid.";
                break;
			case miniplc0::ErrLeadingZero:
                name = "can't not have leading Zero.";
                break;
            case miniplc0::ErrFuctionBracket:
                name = "Function bracket() can't not match";
                break;
            case miniplc0::ErrInvalidParameterDeclaration:
                name = "Invalid Parameter Declaration";
                break;
            case miniplc0::ErrFuctionBrace:
                name = "Function brace can't not fetch";
                break;
            case miniplc0::ErrIncompleteConditionStatement:
                name = "Incomplete ConditionStatement!";
                break;
            case miniplc0::ErrNoBracket:
                name = "Your bracket can't not match";
                break;
            case miniplc0::ErrinvalidStatement:
                name = "Invalid Statement!";
                break;
            case miniplc0::ErrIncompleteScanStatement:
                name = "Incomplete ScanStatement!";
                break;
            case miniplc0::ErrIncompleteReturnStatement:
                name = "Incomplete ReturnStatement!";
                break;
            case miniplc0::ErrIncompletePrintStatement:
                name = "Incomplete PrintStatement!";
                break;
            case miniplc0::ErrVariableCall://变量当函数调用
                name = "It should be a var but you used it like function.";
                break;
            case miniplc0::ErrReturn://fanhuicuowu
                name = "Wrong return type.";
                break;
            case miniplc0::ErrIncompleteWhileStatement:
                name = "Incomplete WhileStatement!";
                break;
			}
			return format_to(ctx.out(), name);
		}
	};

	template<>
	struct formatter<miniplc0::CompilationError> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::CompilationError &p, FormatContext &ctx) {
			return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second, p.GetCode());
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Token> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Token &p, FormatContext &ctx) {
			return format_to(ctx.out(),
				"Line: {} Column: {} Type: {} Value: {}",
				p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
		}
	};

	template<>
	struct formatter<miniplc0::TokenType> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::TokenType &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::NULL_TOKEN:
				name = "NullToken";
				break;
            case miniplc0::VOID:
                name = "void";
                break;
            case miniplc0::CHAR:
                name = "char";
                break;
            case miniplc0::DOUBLE:
                name = "double";
                break;
            case miniplc0::STRUCT:
                name = "struct";
                break;
            case miniplc0::INT:
			    name = "int";
			    break;
			    case miniplc0::INTEGER:
			        name = "integer";
			        break;
			case miniplc0::IDENTIFIER:
				name = "Identifier";
				break;
			case miniplc0::CONST:
				name = "Const";
				break;
			case miniplc0::IF:
                name = "if";
                break;
			case miniplc0::ELSE:
                name = "else";
                break;
			case miniplc0::SWITCH:
                name = "switch";
                break;
			case miniplc0::CASE:
                name = "case";
                break;
			case miniplc0::DEFAULT:
                name = "default";
                break;
			case miniplc0::WHILE:
                name = "while";
                break;
			case miniplc0::FOR:
                name = "for";
                break;
			case miniplc0::DO:
                name = "do";
                break;
			case miniplc0::RETURN:
                name = "return";
                break;
			case miniplc0::BREAK:
                name = "break";
                break;
			case miniplc0::CONTINUE:
                name = "continue";
                break;
			case miniplc0::PRINT:
                name = "print";
                break;
			case miniplc0::SCAN:
                name = "scan";
                break;

			case miniplc0::PLUS_SIGN:
				name = "PlusSign";
				break;
			case miniplc0::MINUS_SIGN:
				name = "MinusSign";
				break;
			case miniplc0::MULTIPLICATION_SIGN:
				name = "MultiplicationSign";
				break;
			case miniplc0::DIVISION_SIGN:
				name = "DivisionSign";
				break;
			case miniplc0::EQUAL_SIGN:
				name = "EqualSign";
				break;

			case miniplc0::GREATER_THAN_SIGN://>
                name = "GreaterThanSign";
                    break;
			case miniplc0::LESS_THAN_SIGN://<
                name = "LessthanSign";
                    break;
            case miniplc0::GRT_EQU_SIGN:// >=
                name = "GreEquSign";
                    break;
            case miniplc0::LES_EQU_SIGN:// <=
                name = "LesEquSign";
                    break;
            case miniplc0::IS_EQU_SIGN:// ==
                name = "IsEquSign";
                    break;
            case miniplc0::NOT_EQU_SIGN:// !=
                name = "NotEquSign";
                    break;
            case miniplc0::COMMA:// ,
                name = "Comma";
                    break;
            case miniplc0::SEMICOLON:// ;
                name = "Semicolon";
                    break;
            case miniplc0::LEFT_BRACKET://(
                name = "LeftBracket";
                    break;
            case miniplc0::RIGHT_BRACKET://)
                name = "RightBracket";
                    break;
            case miniplc0::LEFT_BRACE:
                name = "LeftBrace";
                    break;
            case miniplc0::RIGHT_BRACE:
                name = "RightBrace";
                    break;

			}
			return format_to(ctx.out(), name);
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Operation> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Operation &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::ILL:
				name = "ILL";
				break;
			case miniplc0::LOADA:
				name = "loada";
				break;
			case miniplc0::LOADC:
				name = "loadc";
				break;
			case miniplc0::IPUSH:
				name = "ipush";
				break;
			case miniplc0::ILOAD:
				name = "iload";
				break;
			case miniplc0::ISTORE:
				name = "istore";
				break;
			case miniplc0::IADD:
				name = "iadd";
				break;
			case miniplc0::ISUB:
				name = "isub";
				break;
			case miniplc0::IMUL:
				name = "imul";
				break;
            case miniplc0::INEG:
                name = "ineg";
                break;
            case miniplc0::IDIV:
                name = "idiv";
                break;
            case miniplc0::ICMP:
                name = "icmp";
                break;
            case miniplc0::CALL:
                name = "call";
                break;
            case miniplc0::RET:
                name = "ret";
                break;
            case miniplc0::IRET:
                name = "iret";
                break;
            case miniplc0::IPRINT:
                name = "iprint";
                break;
            case miniplc0::ISCAN:
                name = "iscan";
                break;
            case miniplc0::POPN:
                name = "popn";
                break;
             case miniplc0::POP:
                 name = "pop";
                 break;
             case miniplc0::JE:
                 name = "je";
                 break;
             case miniplc0::JNE:
                 name = "jne";
                 break;
             case miniplc0::JMP:
                 name = "jmp";
                 break;

			}
			return format_to(ctx.out(), name);
		}
	};
	template<>
	struct formatter<miniplc0::Instruction> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Instruction &p, FormatContext &ctx) {
			std::string name;
			switch (p.GetOperation())
			{
			case miniplc0::ILOAD:
			case miniplc0::ISTORE:
			case miniplc0::IADD:
			case miniplc0::ISUB:
			case miniplc0::IMUL:
            case miniplc0::INEG:
            case miniplc0::IDIV:
            case miniplc0::RET:
            case miniplc0::IRET:
            case miniplc0::IPRINT:
            case miniplc0::ISCAN:
            case miniplc0::POP:
				return format_to(ctx.out(), "{}", p.GetOperation());
			case miniplc0::CALL:
			case miniplc0::POPN:
			case miniplc0::LOADC:
            case miniplc0::IPUSH:
            case miniplc0::JE:
            case miniplc0::JNE:
            case miniplc0::JMP:
				return format_to(ctx.out(), "{} {}", p.GetOperation(), p.GetX());
			case miniplc0::LOADA:
                return format_to(ctx.out(), "{} {} {}", p.GetOperation(), p.GetX(),p.GetY());
			}
			return format_to(ctx.out(), "ILL");
		}
	};
}