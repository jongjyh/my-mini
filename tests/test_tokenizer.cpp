#include "catch2/catch.hpp"
#include "tokenizer/tokenizer.h"
#include "fmt/core.h"

#include <sstream>
#include <vector>
#include <analyser/analyser.h>

// 下面是示例如何书写测试用例
TEST_CASE("Test hello world.") {

	std::string input = 
		"int main (){}\n";
	std::stringstream ss;
	ss.str(input);
	miniplc0::Tokenizer tkz(ss);
	std::vector<miniplc0::Token> output = {};
	auto result = tkz.AllTokens();


    miniplc0::Analyser analyser(result.first);
    auto p = analyser.Analyse();
	if (result.second.has_value()) {
		FAIL();
	}
	REQUIRE( (result.first == output) );
}