#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <stack>
#include <map>

#include "hw3_1.h"
#include "instruction.h"
using namespace std;


map<string,int> mymap;



void def_map(){

       mymap["@"]=6;	
	mymap["^"]=5;
	mymap["*"]=4;
	mymap["/"]=4;
	mymap["+"]=3;
	mymap["-"]=3;
	mymap["<="]=2;
		return;
}

int to_int(string a){
stringstream buffer(a);
int value;
buffer>>value;
return value;
}

int main() {
	def_map();
  list<inst_t> insts(Parser(std::cin).parse());
  for (const inst_t& inst : insts)
    std::cout << inst.toString() << std::endl;
  return 0;
}

list<inst_t> Parser::parseExpr() {
	string tok;
	string op;
	stack<string> mystack;
	list<inst_t> exp;
	string::const_iterator nxt;
 	for(;this->hasMore();this->nextToken()){
	tok=this->peekToken();
	nxt=tok.cbegin();
	if(tok==")" || tok=="then" || tok=="else"){
		break;
	}
	else if(tok=="(")
	{
		this->nextToken();
	list<inst_t> insts(this->parseTopExpr());
	exp.splice(exp.end(), insts);
	}
	else if(isdigit(*nxt)){
     	exp.push_back({inst_t::type_t::push, "", {to_int(tok)}});
    	} 
	else if (isalpha(*nxt) or *nxt=='_') { // [_a-zA-z][_a-zA-z0-9]*
      	exp.push_back({inst_t::type_t::access, tok, {}});
	} 
	else if(mystack.empty()){
		mystack.push(tok);
		}
	else if(tok=="^" and mystack.top()=="^"){
	mystack.push(tok);

	}
	else if(mymap[mystack.top()] >= mymap[tok]){
	
		while(mymap[mystack.top()] >= mymap[tok]){
			op=mystack.top();
			if(op=="@")
		exp.push_back({inst_t::type_t::apply, "", {}});
   		
			else if(op=="^")
		exp.push_back({inst_t::type_t::pow, "", {}});
   		
			else if(op=="*")
		exp.push_back({inst_t::type_t::imul, "", {}});
   		
			else if(op=="/")
		exp.push_back({inst_t::type_t::idiv, "", {}});
   		
			else if(op=="+")
		exp.push_back({inst_t::type_t::add, "", {}});
   		
			else if(op=="-")
		exp.push_back({inst_t::type_t::sub, "", {}});
   		
			
			else if(op=="<=")
		exp.push_back({inst_t::type_t::setle, "", {}});
   		
		mystack.pop();
		if(mystack.empty())
			break;
		}
		mystack.push(tok);
	}
		else{
		mystack.push(tok);
		}

	}
	while(not mystack.empty()){
			op=mystack.top();
			mystack.pop();
		
			if(op=="@")
		exp.push_back({inst_t::type_t::apply, "", {}});
   		
			else if(op=="^")
		exp.push_back({inst_t::type_t::pow, "", {}});
   		
			else if(op=="*")
		exp.push_back({inst_t::type_t::imul, "", {}});
   		
			else if(op=="/")
		exp.push_back({inst_t::type_t::idiv, "", {}});
   		
			else if(op=="+")
		exp.push_back({inst_t::type_t::add, "", {}});
   		
			else if(op=="-")
		exp.push_back({inst_t::type_t::sub, "", {}});
   		
			
			else if(op=="<=")
		exp.push_back({inst_t::type_t::setle, "", {}});
   		
			}	
		


		

  return exp;
  // TODO: implement your algorithm here
}

// Below are the utility and provided functions.
// You can safely skip all of them
list<inst_t> Parser::parseTopExpr() {
  const auto eat = [this](const char *s) -> bool {
    if (not this->hasMore()) return false;
    if (s!=nullptr and this->peekToken() != s) return false;
    this->nextToken();
    return true;
  };
  if (not this->hasMore())
    throw std::runtime_error("empty input");
  if (eat("if")) {
    list<inst_t> con(this->parseExpr());

    if (not eat("then")) throw std::runtime_error("expecting 'then'");
    list<inst_t> th(this->parseExpr());
    th.push_back({inst_t::type_t::ret, "", {}});

    if (not eat("else")) throw std::runtime_error("expecting 'else'");
    list<inst_t> el(this->parseExpr());
    el.push_back({inst_t::type_t::ret, "", {}});

    list<inst_t> buf;
    buf.splice(buf.end(), con);
    buf.push_back({inst_t::type_t::jz, "", {static_cast<int>(th.size()),
                                            static_cast<int>(th.size() + el.size())}});
    buf.splice(buf.end(), th);
    buf.splice(buf.end(), el);
    return buf;
  } else if (eat("\\")) {
    string id = this->nextToken();
    if (not eat("->")) throw std::runtime_error("expecting '->'");
    list<inst_t> body(this->parseTopExpr());
    body.push_back({inst_t::type_t::ret, "", {}});

    list<inst_t> buf;
    buf.push_back({inst_t::type_t::closure, id, {static_cast<int>(body.size())}});
    buf.splice(buf.end(), body);
    return buf;
  } else {
    return this->parseExpr();
  }
}

static list<string> desugar(
  vector<string>::const_iterator& cur,
  const vector<string>::const_iterator& end)
{
  list<string> tokens;
  if (*cur == "let") {
    // syntactic extension: let
    // syntax: topexpr ::= let identifier [x1 x2 ... xn] = e in body
    //   where e, body : topexpr
    // transformed to: (\identifier -> body) (e)
    //                or (\identifier -> body) (\x1 x2 ... xn -> e)
    ++cur; //eat 'let'
    tokens.insert(tokens.end(), {"(", "(", "\\", *cur, "->"});
    ++cur; //eat identifier
    vector<string> params;
    while (*cur != "=") {
      params.emplace_back(*cur);
      ++cur;
    }
    ++cur; //eat '='

    list<string> expr(desugar(cur, end));

    ++cur; //eat 'in'
    tokens.splice(tokens.end(), desugar(cur, end));
    tokens.insert(tokens.end(), {")", "@", "("});
    for (const string& param : params)
      tokens.insert(tokens.end(), {"\\", param, "->"});
    tokens.splice(tokens.end(), expr);
    tokens.insert(tokens.end(), {")", ")"});

  } else if (*cur == "\\") {
    // syntactic extension: multivariate function
    // syntax: topexpr ::= \ x1 x2 x3 ... xn -> body
    //   where body : topexpr
    // transformed to: \ x1 -> \ x2 -> \ x3 -> ... \ xn -> body
    ++cur;
    while (cur!=end and *cur!="->") {
      tokens.insert(tokens.end(), {"\\", *cur, "->"});
      ++cur;
    }
    ++cur;
    tokens.splice(tokens.end(), desugar(cur, end));
  } else if (*cur == "if") {
    tokens.emplace_back("if");
    ++cur; //eat 'if'
    tokens.splice(tokens.end(), desugar(cur, end));

    tokens.emplace_back("then");
    ++cur; //eat 'then'
    tokens.splice(tokens.end(), desugar(cur, end));

    tokens.emplace_back("else");
    ++cur; //eat 'else'
    tokens.splice(tokens.end(), desugar(cur, end));
  } else {
    // syntactic extension: function application
    // syntax: expr ::= expr expr
    // transformed to: expr @ expr
    bool prev_sym = true, curr_sym = false;
    for (; cur!=end and *cur!=")" and *cur!="then" and *cur!="else" and *cur!="in"; ++cur) {
      list<string> token_buf;
      if (*cur=="@" || *cur=="^" || *cur=="*" || *cur=="/"
        || *cur=="+" || *cur=="-" || *cur=="<=")
      {
        token_buf.emplace_back(*cur);
        curr_sym = true;
      } else if (*cur == "(") {
        token_buf.emplace_back("(");
        ++cur; //eat '('
        token_buf.splice(token_buf.end(), desugar(cur, end));
        token_buf.emplace_back(")");
        curr_sym = false;
      } else {
        token_buf.emplace_back(*cur);
        curr_sym = false;
      }
      if (not prev_sym and not curr_sym)
        tokens.emplace_back("@");
      tokens.splice(tokens.end(), token_buf);
      prev_sym = curr_sym;
    }
  }
  return tokens;
}

list<inst_t> Parser::parse() {
  vector<string> token_buf;
  Tokenizer tokenizer(this->input_stream);
  while (tokenizer.hasMore())
    token_buf.emplace_back(tokenizer.nextToken());
  vector<string>::const_iterator pos(token_buf.cbegin());
  list<string> des_tokens(desugar(pos, token_buf.cend()));
  this->tokens.clear();
  this->tokens.insert(this->tokens.end(), des_tokens.begin(), des_tokens.end());
  this->cur = this->tokens.cbegin();
  return this->parseTopExpr();
}

string Parser::peekToken() {
  if (not this->hasMore())
    throw std::runtime_error("Parser::peekToken(): going over the end");
  return *this->cur;
}

string Parser::nextToken() {
  if (not this->hasMore())
    throw std::runtime_error("Parser::nextToken(): going over the end");
  string token(*this->cur);
  ++this->cur;
  return token;
}
