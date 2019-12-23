#include "argparse.hpp"
#include "fmt/core.h"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "fmts.hpp"

#include <iostream>
#include <fstream>
#include "analyser/analyser.h"


void printOperation(miniplc0::Instruction &instruction);

std::vector<miniplc0::Token> _tokenize(std::istream &input) {
    miniplc0::Tokenizer tkz(input);
    auto p = tkz.AllTokens();
    if (p.second.has_value()) {
        fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
        exit(2);
    }
    return p.first;
}

void Tokenize(std::istream &input, std::ostream &output) {
    auto v = _tokenize(input);
    for (auto &it : v)
        output << fmt::format("{}\n", it);
    return;
}

void Binary(std::istream &input, std::ostream &output) {
    auto tks = _tokenize(input);

    miniplc0::Analyser analyser(tks);
    auto p = analyser.Analyse();
    if (p.second.has_value()) {
        fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
        exit(2);
    }
    auto v = p.first;
    std::vector<std::pair<std::string, int>> Consts = v.getConstList();
    std::vector<miniplc0::Function> funlist = v.getFunctionList();
    std::vector<miniplc0::Instruction> beginCode = v.getBeginCode();
    std::vector<std::vector<miniplc0::Instruction>> program = v.getProgramList();
    output <<fmt::format("# magic\n");
    fmt::print("{:08x}",0x43303a29);
    fmt::print("\n# version\n");
    fmt::print("{:08x}",0x1);
    fmt::print("\n# constants_count\n");
    fmt::print("{:04x}",Consts.size());
    for(int i=0;i<Consts.size();i++)
    {
        fmt::print("\n# constants[{}]\n",i);
        int type,length;
        if(Consts.at(i).second==0)
            type=1;//INT
        else
            type=0;//String
        //type 8
        fmt::print("{:02x}\n",type);
        //length 16
        fmt::print("{:04x}\n",Consts.at(i).first.length());
        //value 一个char 8位
        if(type==1)
        {
            std::stringstream ss;
            ss<<Consts.at(i).first;
            int32_t num;
            ss>>num;
            fmt::print("{:08x}\n",num);
        }
        else{
            for(auto it:Consts.at(i).first)
                fmt::print("{:02x} ",it);
            fmt::print("\n");
        }

    }
    fmt::print("\n# start_code\n");
    for(auto it:beginCode)
        printOperation(it);
    fmt::print("\n# functions_count\n");
    for(int i=0;i<funlist.size();i++)
    {
        fmt::print("# function[{}]\n",i);
        //nameindex 16
        fmt::print("{:04x}\n",funlist[i].nameindex);
        //params_len 16
        fmt::print("{:04x}\n",funlist[i].getParaSize());
        //level 16
        fmt::print("{:04x}\n",funlist[i].level);
        //ins_count 16
        fmt::print("{:04x}\n",program[i].size());
        fmt::print("function[{]].instructions\n",i);
        for(auto it:program[i])
        {
            printOperation(it);
        }
    }
}

void printOperation(miniplc0::Instruction &instruction) {
    int ope,x,y,z;
    switch(instruction.GetOperation())
    {
        case miniplc0::ILOAD:ope=0x10;
        case miniplc0::ISTORE:ope=0x20;
        case miniplc0::IADD:ope=0x30;
        case miniplc0::ISUB:ope=0x34;
        case miniplc0::IMUL:ope=0x38;
        case miniplc0::INEG:ope=0x40;
        case miniplc0::IDIV:ope=0x3c;
        case miniplc0::RET:ope=0x88;
        case miniplc0::IRET:ope=0x89;
        case miniplc0::IPRINT:ope=0xa0;
        case miniplc0::ISCAN:ope=0xb0;
        case miniplc0::POP:ope=0x04;
        case miniplc0::I2C:ope=0x62;
            fmt::print("{:04x}\n",ope);
            break;
        case miniplc0::CALL:ope=0x80;
        case miniplc0::POPN:ope=0x06;
        case miniplc0::LOADC:ope=0x06;
        case miniplc0::IPUSH:ope=0x02;
        case miniplc0::JE:ope=0x71;
        case miniplc0::JNE:ope=0x72;
        case miniplc0::JMP:ope=0x70;
        case miniplc0::JL:ope=0x73;
        case miniplc0::JLE:ope=0x76;
        case miniplc0::JG:ope=0x75;
        case miniplc0::JGE:ope=0x74;
            fmt::print("{:04x} {:04x}\n",ope,instruction.GetX());
            break;
        case miniplc0::LOADA:
            fmt::print("{:04x} {:04x} {:04x}\n",ope,instruction.GetX(),instruction.GetY());
            break;
    }
}

void Analyse(std::istream &input, std::ostream &output) {
    auto tks = _tokenize(input);

    miniplc0::Analyser analyser(tks);
    auto p = analyser.Analyse();
    if (p.second.has_value()) {
        fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
        exit(2);
    }
    auto v = p.first;
    std::vector<std::pair<std::string, int>> Consts = v.getConstList();
    std::vector<miniplc0::Function> funlist = v.getFunctionList();
    std::vector<miniplc0::Instruction> beginCode = v.getBeginCode();
    std::vector<std::vector<miniplc0::Instruction>> program = v.getProgramList();
    output << fmt::format(".constants:\n");
    for (int i = 0; i < Consts.size(); i++) {
        std::string INT = "I", STR = "S", type, value = Consts[i].first;
        if (Consts[i].second == 0)
            type = INT;
        else
            type = STR;
        output << fmt::format("\t{} {} {}\n", i, type, value);
    }
    output << fmt::format("\n");
    output << fmt::format(".start:\n");
    for (int i = 0; i < beginCode.size(); i++) {
        output << fmt::format("\t{} {}\n", i, beginCode[i]);
    }
    output << fmt::format("\n");
    output << fmt::format(".functions:\n");

    for (int i = 0; i < funlist.size(); i++) {
        output << fmt::format("\t{} {} {} {}\n", i, funlist[i].nameindex, funlist[i].getParaSize(), funlist[i].level);
    }

    output << fmt::format("\n");
    for (int i = 1; i < program.size(); i++) {
        auto it = program[i];
        output << fmt::format(".F{}:\n", i - 1);
        for (int j = 0; j < it.size(); j++) {
            output << fmt::format("\t{} {}\n", j, it[j]);
        }
    }

    return;
}

int main(int argc, char **argv) {
    argparse::ArgumentParser program("c0");
    program.add_argument("input")
            .help("speicify the file to be compiled.");
    program.add_argument("-t")
            .default_value(false)
            .implicit_value(true)
            .help("perform tokenization for the input file.");
    program.add_argument("-s")
            .default_value(false)
            .implicit_value(true)
            .help("perform syntactic analysis for the input file.");
    program.add_argument("-c")
            .default_value(false)
            .implicit_value(true)
            .help("perform syntactic analysis for the input file.");
    program.add_argument("-o", "--output")
            .required()
            .default_value(std::string("-"))
            .help("specify the output file.");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err) {
        fmt::print(stderr, "{}\n\n", err.what());
        program.print_help();
        exit(2);
    }

    auto input_file = program.get<std::string>("input");
    auto output_file = program.get<std::string>("--output");
    std::istream *input;
    std::ostream *output;
    std::ifstream inf;
    std::ofstream outf;
    if (input_file != "-") {
        inf.open(input_file, std::ios::in);
        if (!inf) {
            fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
            exit(2);
        }
        input = &inf;
    } else
        input = &std::cin;
    if (output_file != "-") {
        outf.open(output_file, std::ios::out | std::ios::trunc);
        if (!outf) {
            fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
            exit(2);
        }
        output = &outf;
    } else
        output = &std::cout;
    if (program["-t"] == true && program["-s"] == true) {
        fmt::print(stderr, "You can only perform tokenization or syntactic analysis at one time.");
        exit(2);
    }
    if (program["-t"] == true) {
        Tokenize(*input, *output);
    } else if (program["-s"] == true) {
        Analyse(*input, *output);
    } else if (program["-c"] == true) {
        Binary(*input, *output);
    } else {
        fmt::print(stderr, "You must choose tokenization or syntactic analysis.");
        exit(2);
    }
    return 0;
}

