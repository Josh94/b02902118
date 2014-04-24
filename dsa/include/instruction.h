#pragma once

#include <vector>
#include <string>

struct inst_t {
  enum class type_t {
    pow, imul, idiv, add, sub, setle,
    push, access, closure, apply, ret, jz
  };
  // components of an instruction
  type_t code;
  std::string s_data;
  std::vector<int> i_data;

  std::string toString() const;
  // parse the given string into an instruction
  static inst_t parse(const std::string& repr);
};
