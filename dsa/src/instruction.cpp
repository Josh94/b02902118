#include <cctype>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include "instruction.h"

using std::string;
using std::vector;
using std::get;

// add an hash instance for the enum class inst_t::type_t
// simply casts the (strongly typed) enum to its underlying type
// http://stackoverflow.com/questions/18837857/
namespace std {
  template<>
  struct hash< ::inst_t::type_t> {
    typedef ::inst_t::type_t argument_type;
    typedef std::underlying_type<argument_type>::type underlying_type;
    typedef std::hash<underlying_type>::result_type result_type;
    result_type operator()(const argument_type& key) const {
      std::hash<underlying_type> hash_fn;
      return hash_fn(static_cast<underlying_type>(key));
    }
  };
}

inst_t inst_t::parse(const string& repr) {
  struct inst_data_t { type_t op; bool has_sdata; int idata_cnt; };
  static const std::unordered_map<string, inst_data_t> str_inst = {
    {"pow", {type_t::pow, false, 0}}, {"imul", {type_t::imul, false, 0}},
    {"idiv", {type_t::idiv, false, 0}}, {"add", {type_t::add, false, 0}},
    {"sub", {type_t::sub, false, 0}}, {"setle", {type_t::setle, false, 0}},
    {"push", {type_t::push, false, 1}}, {"access", {type_t::access, true, 0}},
    {"closure", {type_t::closure, true, 1}}, {"apply", {type_t::apply, false, 0}},
    {"ret", {type_t::ret, false, 0}}, {"jz", {type_t::jz, false, 2}}
  };
  std::istringstream input(repr);
  string inst, s_data;
  vector<int> i_data;
  input >> inst;
  for (char& ch : inst)
    ch = static_cast<char>(std::tolower(ch));
  if (str_inst.find(inst) == str_inst.end())
    throw std::runtime_error("inst_t::parse(): unknown instruction " + inst);
  const inst_data_t& format = str_inst.at(inst);
  if (format.has_sdata and not (input >> s_data))
    throw std::runtime_error("inst_t::parse(): cannot read s_data of " + inst);
  for (int i = 0, k; i < format.idata_cnt; ++i) {
    if (not (input >> k))
      throw std::runtime_error("inst_t::parse(): cannot read i_data of " + inst);
    i_data.push_back(k);
  }
  return inst_t { format.op, s_data, i_data };
}

string inst_t::toString() const {
  static const std::unordered_map<type_t, string> inst_str = {
      {type_t::pow, "pow"}, {type_t::imul, "imul"}, {type_t::idiv, "idiv"},
      {type_t::add, "add"}, {type_t::sub, "sub"},   {type_t::setle, "setle"},
      {type_t::push, "push"}, {type_t::access, "access"},
      {type_t::closure, "closure"}, {type_t::apply, "apply"},
      {type_t::ret, "ret"}, {type_t::jz, "jz"}
  };
  string res = inst_str.at(this->code);
  if (this->s_data.size())
    res += " " + this->s_data;
  for (int k : this->i_data)
    res += " " + std::to_string(k);
  return res;
}
