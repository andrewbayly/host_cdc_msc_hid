/**
TODO: 
  - goal - make a function which computes a factorial using recursion - DONE!
    - if
    - * 
  - implement back-space - MOSTLY DONE :)
  - show a cursor - DONE!
  - long string crashed program - DONE!
  - hitting return breaks the program completely! - DONE!
  - save/load/clear
  - functions don't appear e.g. (def! foo (fn* (a) a)) - just evaluates to blank
  - memory leaks
  - () should eval to nil
  - figure out how to get my code into github

**/

/***
(defun square (x) (* x x) ) - DONE!
    (def! square (fn* (x) (* x x)))

defines a function and stores the definition in code
  - create "code" structure - map from string to Node* - DONE!
  - implement deepCopy on Node. - DONE!
  - in EVAL ("defun") special form:
    - create two copies of expressions
    - get name from expression and set property in code
    - get name, params, body from expression and create "def!" expression and evaluate it.

(code) - DONE!
returns a list of the functions which have been defined in code
    - implement special form
    - implement pretty print 

(code foo) - DONE!
returns the definition of foo from code
  
(code-names) - DONE!
returns a list of the names of the functions which have been defined in code

(undef foo)
removes the definition of foo from code

(save foobar)
saves the function definitions currently in code into a file called foobar.lsp

(load foobar)
clears the definitions currently in code and then loads the definitions from the file called foobar.lsp

(insert <name> <address> <content>)
(append <name> <address> <content>)
insert or append the content at the address in function definition foo

(remove <name> <address>)
remove the expression at the address in function <name>

Example: 

> (code fact)
(defun fact (x) 
  (if (= x 1) 1
    (fact (- x 1))
  )
)

> (remove fact (3 3))
(defun fact (x) 
  ( if (= x 1) 1 )
)

> (append fact (3 2) ((* x (fact (- x 1)))))
(defun fact (x) 
  ( if (= x 1) 1 
    (* x (fact (- x 1)))
  )
)

***/


/**
  primitives: 
    + - * = % code-names code def! defun if fn* cons

  TODO: 
    car cdr quote

**/


extern "C" int foo();
extern "C" void print(char ch); 
extern "C" void mal_key_pressed(char ch); 
extern "C" void mal_init(void);

#include <string>
#include <regex>
#include <vector>
#include <unordered_map>

using namespace std;

int foo(){ 
  try{ 
    throw 44; 
  }
  catch(int e){ 
  }
  return 0;
}

void mal_print(char ch){ 
  print(ch); 
}

void print_string(std::string s){ 
  for(unsigned int i = 0; i < s.length(); i++){ 
    mal_print(s[i]); 
  }
}



std::string input; 

bool is_balanced(std::string s){ 
  int count_open = 0; 
  int count_close = 0; 
  
  for(unsigned int i = 0; i < s.length(); i++){ 
    char ch = s[i]; 
    if(ch == '('){ 
      count_open++; 
    }
    if(ch == ')'){ 
      count_close++; 
    }
  }
  return (count_open == count_close);  
}

/**
void join(const vector<string>& v, char c, string& s) {

   s.clear();

   for (vector<string>::const_iterator p = v.begin();
        p != v.end(); ++p) {
      s += *p;
      if (p != v.end() - 1)
        s += c;
   }
}
**/

//=====================================================================================
//       READ   
//=====================================================================================

std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}

#define NODE_LIST 0
#define NODE_BOOL 1
#define NODE_NUMBER 2
#define NODE_NIL 3
#define NODE_SYMBOL 4
#define NODE_FUNCTION 4

class Node { 

public: 
  Node(int _type){
    type = _type;  
  }
   
  ~Node(){
  } 

  string toString(){ 
    if(this->type == NODE_BOOL){ 
      if(this->value == 0)
        return string("false"); 
      else
        return string("true"); 
    }
    else if(this->type == NODE_SYMBOL){ 
      return this->symbol; 
    }
    else if(this->type == NODE_LIST){ 
      string s = "(";
      for(unsigned int i = 0; i < this->list.size(); i++){ 
        s += this->list[i]->toString();
        if(i < this->list.size()-1)
          s += " ";  
      } 
      s += ")"; 
      return s;
    }
    else if(this->type == NODE_NIL){ 
      return "nil"; 
    }
    else if(this->type == NODE_NUMBER){ 
      return to_string(this->value ); 
    }
    else if(this->type == NODE_FUNCTION){ 
      return string("<function>"); 
    }
    else
      return "ERROR!"; 
  }
  
  string toPrettyString(string indent){ 
    string str = toString(); 
    if((str.size() <= 12) || (type != NODE_LIST))
      return (indent + str); 
      
    string pretty = indent + string("(\r"); 
    for(unsigned int i = 0; i < list.size(); i++){
      pretty += list[i]->toPrettyString(indent + string(" "));
      pretty += string("\r");
    }  
    
    pretty += (indent + string(")"));
    return pretty;       
  }
  
  int type; 
  vector<Node*> list; 
  int value;
  string symbol; 
  
  Node* deepCopy(){ 
    Node* that = new Node(type); 
    that->value = value; 
    that->symbol = symbol; 
    for(unsigned int i = 0; i < list.size(); i++){ 
      that->list.push_back(list[i]->deepCopy()); 
    }
    return that;   
  }  
  
  virtual Node* call(vector<Node*>* args){
    return NULL;  
  }
  
  bool isTrue(){ 
    return !(type == NODE_NIL || (type == NODE_BOOL && value == 0)); 
  }
  
  bool isSpecial(string name){ 
    return type == NODE_SYMBOL && symbol.compare(name)==0; 
  }
}; 

class AddFunction : public Node { 

public: 
  AddFunction(): Node(NODE_FUNCTION)
  { 
  }   

  virtual Node* call(vector<Node*>* args){
    Node* pNode = new Node(NODE_NUMBER); 
    pNode->value = args->at(0)->value + args->at(1)->value; 
    return pNode;   
  }
};

class SubtractFunction : public Node { 

public: 
  SubtractFunction(): Node(NODE_FUNCTION)
  { 
  }   

  virtual Node* call(vector<Node*>* args){
    Node* pNode = new Node(NODE_NUMBER); 
    pNode->value = args->at(0)->value - args->at(1)->value; 
    return pNode;   
  }
};

class MultiplyFunction : public Node { 

public: 
  MultiplyFunction(): Node(NODE_FUNCTION)
  { 
  }   

  virtual Node* call(vector<Node*>* args){
    Node* pNode = new Node(NODE_NUMBER); 
    pNode->value = args->at(0)->value * args->at(1)->value; 
    return pNode;   
  }
};

class ModuloFunction : public Node { 

public: 
  ModuloFunction(): Node(NODE_FUNCTION)
  { 
  }   

  virtual Node* call(vector<Node*>* args){
    Node* pNode = new Node(NODE_NUMBER); 
    pNode->value = args->at(0)->value % args->at(1)->value; 
    return pNode;   
  }
};

class EqualsFunction : public Node { 

public: 
  EqualsFunction(): Node(NODE_FUNCTION)
  { 
  }   

  virtual Node* call(vector<Node*>* args){
    Node* pNode = new Node(NODE_BOOL); 
    pNode->value = ( args->at(0)->value == args->at(1)->value ) ? 1 : 0 ; 
    return pNode;   
  }
};


bool isNumber(string s)
{
  char* p;
  strtol(s.c_str(), &p, 10);
  return *p == 0;
}

Node* categorize(string s){ 
  //TODO: add nil and integer
 
  Node* pNode; 
 
  if(isNumber(s)){ 
    pNode = new Node(NODE_NUMBER); 
    pNode->value = stoi(s); 
    return pNode; 
  } else if(s.compare(string("false"))== 0){ 
    pNode = new Node(NODE_BOOL); 
    pNode->value = 0; 
    return pNode; 
  }  
  else if(s.compare(string("true"))== 0){ 
    pNode = new Node(NODE_BOOL); 
    pNode->value = 1; 
    return pNode; 
  }
  else if(s.compare(string("nil"))== 0){ 
    pNode = new Node(NODE_NIL); 
    pNode->value = 0; 
    return pNode; 
  }
  else { 
    pNode = new Node(NODE_SYMBOL); 
    pNode->symbol = s; 
    return pNode; 
  } 
}

Node* parenthesize(vector<string>* tokens, Node* pList){ 
  if(tokens->size() == 0){ 
    Node* pNode = pList->list.back(); 
    pList->list.pop_back();  
    return pNode; 
  }
  
  string token = tokens->at(0); 
  tokens->erase(tokens->begin()); 
  
  if(token.compare(string("(")) == 0){ 
    pList->list.push_back(parenthesize(tokens, new Node(NODE_LIST))); 
    return parenthesize(tokens, pList); 
  }
  else if(token.compare(string(")")) == 0){
    return pList; 
  }
  else { 
    pList->list.push_back(categorize(token)); 
    return parenthesize(tokens, pList);
  }
}

Node* READ(string s){ 

  s = regex_replace(s, regex("(\\()"), " ( ");
  s = regex_replace(s, regex("(\\))"), " ) ");
  s = regex_replace(s, regex("(\\r)"), " ");

  std::vector<std::string> tokenized = split(s, ' ');
  tokenized.pop_back(); //hack - apparently split has an extra blank on the end!

  if(tokenized.size() == 0){ 
    throw string("Empty!"); 
  }

  return parenthesize(&tokenized, new Node(NODE_LIST));  
}

//=====================================================================================
//       EVAL   
//=====================================================================================

//------------------------------------------------------------------------------
//  Env
//------------------------------------------------------------------------------

class Env { 
public: 
  Env(Env* _outer){ 
    outer = _outer; 
  }  

  Env(Env* _outer, vector<string>* binds, vector<Node*>* exprs){ 
    for(unsigned int i = 0; i < binds->size(); i++){ 
      set(binds->at(i), exprs->at(i)); 
    } 
 
    outer = _outer; 
  }  
  
  void set(string key, Node* value){ 
    data[key] = value; 
  }
  
  Env* find(string key){ 
    if(data.find(key) != data.end()){ 
      return this; 
    }else{ 
      if(outer == NULL){ 
        return NULL; 
      }
      return outer->find(key); 
    }
  }
  
  Node* get(string key){ 
    Env* pEnv = find(key); 
    if(pEnv == NULL){ 
      throw string("Not Found!"); 
    }
    return pEnv->data[key]; 
  }
  
  unordered_map<string, Node*> data; 
  
  Env* outer; 
}; 

Env repl_env(NULL); 

//------------------------------------------------------------------------------
//  Code
//------------------------------------------------------------------------------

class Code { 
public: 
  Code(){ 
  }

  void set(string key, Node* value){ 
    data[key] = value; 
  }
  
  Node* get(string key){ 
    return data[key]; 
  }
  
  //returns all the nodes inside an enclosing list node
  Node* getAll(){ 
    Node* pRet = new Node(NODE_LIST); 
          
    //get all keys from the map: 
    vector<string> keys;
    keys.reserve(data.size());
    
    for(auto k : data){ 
      keys.push_back(k.first); 
    }
    
    //sort them:
    sort(keys.begin(), keys.end()); 
    
    for(unsigned int i = 0; i < keys.size(); i++){ 
      Node* value = data[keys[i]]; 
      pRet->list.push_back(value); 
    } 
    
    return pRet; 
  }
  
  unordered_map<string, Node*> data; 
}; 

Code code; 

//forward:
Node* EVAL(Node* pNode, Env* pEnv); 

class AnonymousFunction : public Node { 

public: 
  AnonymousFunction(Env* _env, vector<string>* _binds, Node* _body ): Node(NODE_FUNCTION)
  { 
    env = _env; 
    binds = _binds; 
    body = _body; 
  }   

  Env* env; 
  vector<string>* binds; 
  Node* body;  

  virtual Node* call(vector<Node*>* args){
    Env* pNewEnv = new Env(env, binds, args); 
    return EVAL(body, pNewEnv); 
  
  //  Node* pNode = new Node(NODE_NUMBER); 
  //  pNode->value = args->at(0)->value - args->at(1)->value; 
  //  return pNode;   
  }
};


Node* eval_ast(Node* ast, Env* pEnv){ 
  if(ast->type == NODE_SYMBOL){ 
    //if(pEnv->contains(ast->symbol)){ 
      return pEnv->get(ast->symbol);
    //} else { 
    //  throw "cannot find symbol";
    //}
  } else if(ast->type == NODE_LIST){ 
    Node* pRet = new Node(NODE_LIST); 
    for(unsigned int i = 0; i < ast->list.size(); i++){ 
      pRet->list.push_back(EVAL(ast->list[i], pEnv));
    }
    return pRet;   
  } else { 
    return ast; 
  }
}

Node* EVAL(Node* pNode, Env* pEnv){
  if(pNode->type == NODE_LIST){ 
    if(pNode->list.size() == 0){ 
      return pNode; 
    } else {
      //specials and primitives: 
      if(pNode->list[0]->isSpecial(string("code-names"))){ 
        Node* pList = new Node(NODE_LIST); 
        for(auto k : code.data){ 
          Node* pCodeName = new Node(NODE_SYMBOL);
          pCodeName->symbol = k.first;  
          pList->list.push_back(pCodeName);  
        }
        return pList; 
      }
      else if(pNode->list[0]->isSpecial(string("cons"))){ 
        Node* pRet = EVAL(pNode->list[2], pEnv)->deepCopy(); 
        Node* pVal = EVAL(pNode->list[1], pEnv); 
        pRet->list.push_back(pVal); 
        return pRet;         
      }
      else if(pNode->list[0]->isSpecial(string("quote"))){ 
        return pNode->list[1]; 
      }
      else if(pNode->list[0]->isSpecial(string("code"))){ 
        if(pNode->list.size() == 2){ 
          return code.get(pNode->list[1]->symbol);
        }
        else{
          return code.getAll();   
        }
      }
      else if(pNode->list[0]->isSpecial(string("def!"))){ 
        string symbol = pNode->list[1]->symbol; 
        Node* value = EVAL(pNode->list[2], pEnv); 
        pEnv->set(symbol, value);
        return value;   
      }
      else if(pNode->list[0]->isSpecial(string("defun"))){
        
        //get name of function:
        Node* pNode1 = pNode->deepCopy(); 
        string name = pNode1->list[1]->symbol; 
        
        //set in code: 
        code.set(name, pNode1); 
        
        //def!: 
        
        //(defun square (x) (* x x) ) 
        //(def! square (fn* (x) (* x x)))

        Node* params = pNode->list[2]; 
        Node* body = pNode->list[3]; 
        
        Node* pNode2 = new Node(NODE_LIST); 
        Node* def = new Node(NODE_SYMBOL); 
        def->symbol = string("def!"); 
        pNode2->list.push_back(def); 
        
        Node* name2 = new Node(NODE_SYMBOL); 
        name2->symbol = name; 
        pNode2->list.push_back(name2); 
                
        Node* function = new Node(NODE_LIST); 
        Node* fn = new Node(NODE_SYMBOL); 
        fn->symbol = string("fn*"); 
        function->list.push_back(fn); 
        function->list.push_back(params); 
        function->list.push_back(body); 
        
        pNode2->list.push_back(function); 
                 
        return EVAL(pNode2, pEnv); 
       
      }
      else if(pNode->list[0]->isSpecial(string("if"))){
        Node* test = EVAL(pNode->list[1], pEnv);
        if(test->isTrue()){ 
          return EVAL(pNode->list[2], pEnv);   
        }else{
          if(pNode->list.size() == 3){ 
            return new Node(NODE_NIL); 
          }
          else{ 
            return EVAL(pNode->list[3], pEnv); 
          }
        }
      }else if(pNode->list[0]->isSpecial(string("fn*"))){
        Node* argsList = pNode->list[1]; 
        Node* body = pNode->list[2];  
        vector<string>* pBinds = new vector<string>(); 
        for(unsigned int i = 0; i < argsList->list.size(); i++){ 
          pBinds->push_back(argsList->list.at(i)->symbol); 
        }
        return( new AnonymousFunction(pEnv, pBinds, body));  
      } else { 
        Node* pEvaluated = eval_ast(pNode, pEnv); 
        vector<Node*> args;
        for(unsigned int i = 1; i < pEvaluated->list.size(); i++){ 
          args.push_back(pEvaluated->list[i]); 
        }
        return pEvaluated->list[0]->call(&args);
      }  
    }
  } else { 
    return eval_ast(pNode, pEnv); 
  }

  return pNode; 
}

std::string PRINT(Node* pNode){ 
  return pNode->toPrettyString(string("")); 
}

std::string rep(std::string str){ 
  //TODO: Add try catch around this:
  try{ 
    return PRINT(EVAL(READ(str), &repl_env));
  }catch(string e){ 
    return e;
  }catch(const std::runtime_error& re){
    return re.what();
  }catch(const std::exception& ex){
    return ex.what();
  }catch(...){
    return string("Unknown Error!");  
  }  
}

void mal_key_pressed(char ch){ 
//  print_string(to_string((int)ch)); 

  mal_print(ch); 

  if(ch == 8){ 
    if(input.length() > 0)
      input.pop_back(); 
    return; 
  }

  input.push_back(ch); 
  if(is_balanced(input) && input[input.length()-1] == '\r'){ 
    std::string output = rep(input);
    input.clear();  
    print_string(output);
    print_string(std::string("\r > "));
  }  
}

void mal_init(){ 
  std::string s("*** Welcome to Lisp!!! ***\r > "); 
  print_string(s); 

//  string s2("(((ppp)))"); 
//  s2 = regex_replace(s2, regex("(\\()"), " ( ");
//  print_string(s2);

  repl_env.set("+", new AddFunction()); 
  repl_env.set("-", new SubtractFunction()); 
  repl_env.set("*", new MultiplyFunction()); 
  repl_env.set("=", new EqualsFunction()); 

  repl_env.set("%", new ModuloFunction()); 
  
  
}




