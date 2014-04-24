#pragma once

#include <fstream>
#include <list>
#include <vector>
#include <string>
#include "tokenizer.h"   // `Tokenizer` is defined here
#include "instruction.h" // `inst_t` is defined here

class Parser {
  std::vector<std::string> tokens;
  std::istream& input_stream;
  std::vector<std::string>::const_iterator cur;
public:
  /*
  @param input_stream: an input `istream` (e.g. cin)
  @description: create a parser with input `input_stream`
  */
  Parser(std::istream& _input_stream)
    : input_stream(_input_stream), cur(this->tokens.cend()) {}
  /*
  @description:
    Parse the input into SVM instructions.
  */
  std::list<inst_t> parse();
private:
  /*
  @description: test if there are any tokens
  */
  bool hasMore() { return this->cur != this->tokens.end(); }

  /*
  @descrpition: peek the next token in the buffer
  */
  std::string peekToken();

  /*
  @description: remove the next token from the buffer and return it
  */
  std::string nextToken();

  /*
  @return: the compiled instructions
  @description: the provided function that handles `topexpr`
  */
  std::list<inst_t> parseTopExpr();

  /*
  @return: the compiled instructions
  @description:
    the function you need to implement. This function
    should handle `expr` and call `topexpr` when needed.
  */
  std::list<inst_t> parseExpr();
};
