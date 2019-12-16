#pragma once

#include <cstdint>
#include <utility>

namespace miniplc0 {

	enum Operation {
		ILL = 0,
		LOADA,//（读地址）loada 0, 0 # 加载fun栈帧（作用域层次差为0的栈帧）中相对于BP偏移为0的内存的地址入栈
		LOADC,//1
		IPUSH,// 1压入一个int入栈
		ILOAD,//0# （从地址中取值）弹出栈顶的地址值，从该地址加载一个int值，压栈该int值
		ISTORE,//0address(1), value(1)将值value存入内存地址address处。
		IADD,
		ISUB,
		IMUL,
		INEG,
		IDIV,
		ICMP,
		CALL,//1
		RET,
		IRET,
		IPRINT,
		ISCAN,
		POPN,//1
		POP,
		JE,
		JNE,
		JMP

	};
	
	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		Instruction(Operation opr, int32_t x) : _opr(opr), _x(x) {}
        Instruction(Operation opr, int32_t x,int32_t y) : _opr(opr), _x(x),_y(y) {}
		Instruction() : Instruction(Operation::ILL, 0){}
		Instruction(const Instruction& i) { _opr = i._opr; _x = i._x;_y=i._y ;}
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x == i._x&& _y == i._y; }

		Operation GetOperation() const { return _opr; }
		int32_t GetX() const { return _x; }
        int32_t GetY() const { return _y; }
        void SetX(int32_t x){  _x=x;}
	private:
		Operation _opr;
		int32_t _x;
        int32_t _y;
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
        swap(lhs._y, rhs._y);
	}

}