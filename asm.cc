#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <climits>
#include "scanner.h"
using namespace std;

/*
 * C++ Starter code for CS241 A3
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */
bool ifID(vector<Token>& tokens, map<string, int>& table, int pc);

bool ifWORD(vector<Token>& tokens, map<string, int>& table, int pc);

bool checkInt(string n);

bool checkHex(string n);

bool reg3(vector<Token>& tokens, string op);

bool reg2(vector<Token>& tokens, string op);

bool reg1(vector<Token>& tokens, string op);

bool ls(vector<Token>& tokens, string op);

bool branch(vector<Token>& tokens, string op, map<string, int>& table, int pc);

bool jump(vector<Token>& tokens, string op);


int main() {
  std::string line;
  map<string, int> table;
  vector<vector<Token>> tokens;
  int pc = -1;
  bool cor;
  try {
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);
      if (tokenLine.empty()) {
        continue;
      }
      for (int i = 0; i < (int)tokenLine.size();) {
        if (tokenLine[i].getKind() == Token::LABEL) {
          string key = tokenLine[i].getLexeme();
          key = key.substr(0, key.length() - 1);
          if (table.find(key) != table.end()) {
            cerr << "ERROR: " << "Label " << key << " already exists." << endl;
            return 1;
          }
          else {
            table.insert(pair<string, int>(key, pc + 1));
            tokenLine.erase(tokenLine.begin());
          }
        }
        else {
          pc++;
          tokens.push_back(tokenLine);
          break;
        }
      }
    }
    pc = 0;
    for (int j = 0; j < (int)tokens.size(); ++j) {
      if (tokens[j][0].getKind() == Token::ID) {
        cor = ifID(tokens[j], table, pc);
      }
      else if (tokens[j][0].getKind() == Token::WORD) {
        cor = ifWORD(tokens[j], table, pc);
      }
      else {
        cerr << "ERROR: " << "First token of line " << pc << " is invalid." << endl;
        return 1;
      }

      if (cor == false) {
        return 1;
      }
      pc++;
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}

bool ifID(vector<Token>& tokens, map<string, int>& table, int pc) {
  bool cor;
  string opcode = tokens[0].getLexeme();
  if (opcode == "add" || opcode == "sub" || opcode == "slt" || opcode == "sltu") {
    cor = reg3(tokens, opcode);
    if (!cor) {
      return false;
    }
  }
  else if (opcode == "mult" || opcode == "multu" || opcode == "div" || opcode == "divu") {
    cor = reg2(tokens, opcode);
    if (!cor) {
      return false;
    }
  }
  else if (opcode == "mfhi" || opcode == "mflo" || opcode == "lis") {
    cor = reg1(tokens, opcode);
    if (!cor) {
      return false;
    }
  }
  else if (opcode == "lw" || opcode == "sw") {
    cor = ls(tokens, opcode);
    if (!cor) {
      return false;
    }
  }
  else if (opcode == "beq" || opcode == "bne") {
    cor = branch(tokens, opcode, table, pc);
    if (!cor) {
      return false;
    }
  }
  else if (opcode == "jr" || opcode == "jalr") {
    cor = jump(tokens, opcode);
    if (!cor) {
      return false;
    }
  }
  else {
    cerr << "ERROR: " << "Instruction of line " << pc << " is invalid." << endl;
    return false;
  }
  return true;
}

bool reg3(vector<Token>& tokens, string op) {
  int s, t, d, instr;
  unsigned char c;
  string reg;
  if (tokens.size() != 6 || tokens[1].getKind() != Token::REG || tokens[2].getKind() != Token::COMMA || tokens[3].getKind() != Token::REG || tokens[4].getKind() != Token::COMMA || tokens[5].getKind() != Token::REG) {
    cerr << "ERROR: " << "Format of instruction " << op << " is invalid." << endl;
    return false;
  }
  for (int i = 1; i <= 5; i += 2) {
    reg = tokens[i].getLexeme().substr(1);
    while (reg != "" && reg[0] == '0') {
      reg = reg.substr(1);
    }
    if (reg.length() > 2 || (reg != "" && stoi(reg) > 31)) {
      cerr << "ERROR: " << "Token " << i << "of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  d = stoi(tokens[1].getLexeme().substr(1));
  s = stoi(tokens[3].getLexeme().substr(1));
  t = stoi(tokens[5].getLexeme().substr(1));
  if (op == "add") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 32;
  }
  else if (op == "sub") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 34;
  }
  else if (op == "slt") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 42;
  }
  else if (op == "sltu") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 43;
  }
  c = instr >> 24;
  cout << c;
  c = instr >> 16;
  cout << c;
  c = instr >> 8;
  cout << c;
  c = instr;
  cout << c;
  return true;
}

bool reg2(vector<Token>& tokens, string op) {
  int s, t, d = 0, instr;
  unsigned char c;
  string reg;
  if (tokens.size() != 4 || tokens[1].getKind() != Token::REG || tokens[2].getKind() != Token::COMMA || tokens[3].getKind() != Token::REG) {
    cerr << "ERROR: " << "Format of instruction " << op << " is invalid." << endl;
    return false;
  }
  for (int i = 1; i <= 3; i += 2) {
    reg = tokens[i].getLexeme().substr(1);
    while (reg != "" && reg[0] == '0') {
      reg = reg.substr(1);
    }
    if (reg.length() > 2 || (reg != "" && stoi(reg) > 31)) {
      cerr << "ERROR: " << "Token " << i << "of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  s = stoi(tokens[1].getLexeme().substr(1));
  t = stoi(tokens[3].getLexeme().substr(1));
  if (op == "mult") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 24;
  }
  else if (op == "multu") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 25;
  }
  else if (op == "div") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 26;
  }
  else if (op == "divu") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 27;
  }
  c = instr >> 24;
  cout << c;
  c = instr >> 16;
  cout << c;
  c = instr >> 8;
  cout << c;
  c = instr;
  cout << c;
  return true;
}

bool reg1(vector<Token>& tokens, string op) {
  int s = 0, t = 0, d, instr;
  unsigned char c;
  string reg;
  if (tokens.size() != 2 || tokens[1].getKind() != Token::REG) {
    cerr << "ERROR: " << "Format of instruction " << op << " is invalid." << endl;
    return false;
  }
  reg = tokens[1].getLexeme().substr(1);
  while (reg != "" && reg[0] == '0') {
    reg = reg.substr(1);
  }
  if (reg.length() > 2 || (reg != "" && stoi(reg) > 31)) {
    cerr << "ERROR: " << "Token 1 of instructon "  << op << " is out of range." << endl;
    return false;
  }
  d = stoi(reg);
  if (op == "mfhi") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 16;
  }
  else if (op == "mflo") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 18;
  }
  else if (op == "lis") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 20;
  }
  c = instr >> 24;
  cout << c;
  c = instr >> 16;
  cout << c;
  c = instr >> 8;
  cout << c;
  c = instr;
  cout << c;
  return true;
}

bool ls(vector<Token>& tokens, string op) {
  int s, t, imme, instr;
  unsigned char c;
  string reg;
  if (tokens.size() != 7 || tokens[1].getKind() != Token::REG || tokens[2].getKind() != Token::COMMA || (tokens[3].getKind() != Token::INT && tokens[3].getKind() != Token::HEXINT) || tokens[4].getKind() != Token::LPAREN || tokens[5].getKind() != Token::REG || tokens[6].getKind() != Token::RPAREN) {
    cerr << "ERROR: " << "Format of instruction " << op << " is invalid." << endl;
    return false;
  }
  for (int i = 1; i <= 5; i += 4) {
    reg = tokens[i].getLexeme().substr(1);
    while (reg != "" && reg[0] == '0') {
      reg = reg.substr(1);
    }
    if (reg.length() > 2 || (reg != "" && stoi(reg) > 31)) {
      cerr << "ERROR: " << "Token " << i << " of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  s = stoi(tokens[5].getLexeme().substr(1));
  t = stoi(tokens[1].getLexeme().substr(1));
  string immediate = tokens[3].getLexeme();
  if (immediate.length() > 6) {
    cerr << "ERROR: " << "Token 3 of instructon "  << op << " is out of range." << endl;
    return false;
  }
  if (tokens[3].getKind() == Token::INT) {
    imme = stoi(immediate);
    if (imme < -32768 || imme > 32767) {
      cerr << "ERROR: " << "Token 3 of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  else {
    imme = stoi(immediate, nullptr, 16);
  }
  if (op == "lw") {
    instr =  (35<<26) | (s<<21) | (t<<16) | (imme & 0xFFFF);
  }
  else if (op == "sw") {
    instr =  (43<<26) | (s<<21) | (t<<16) | (imme & 0xFFFF);
  }
  c = instr >> 24;
  cout << c;
  c = instr >> 16;
  cout << c;
  c = instr >> 8;
  cout << c;
  c = instr;
  cout << c;
  return true;
}

bool branch(vector<Token>& tokens, string op, map<string, int>& table, int pc) {
  int s, t, imme, instr;
  unsigned char c;
  string reg;
  if (tokens.size() != 6 || tokens[1].getKind() != Token::REG || tokens[2].getKind() != Token::COMMA || tokens[3].getKind() != Token::REG || tokens[4].getKind() != Token::COMMA || (tokens[5].getKind() != Token::INT && tokens[5].getKind() != Token::HEXINT && tokens[5].getKind() != Token::ID)) {
    cerr << "ERROR: " << "Format of instruction " << op << " is invalid." << endl;
    return false;
  }
  for (int i = 1; i <= 3; i += 2) {
    reg = tokens[i].getLexeme().substr(1);
    while (reg != "" && reg[0] == '0') {
      reg = reg.substr(1);
    }
    if (reg.length() > 2 || (reg != "" && stoi(reg) > 31)) {
      cerr << "ERROR: " << "Token " << i << "of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  s = stoi(tokens[1].getLexeme().substr(1));
  t = stoi(tokens[3].getLexeme().substr(1));
  string immediate = tokens[5].getLexeme();
  if (tokens[5].getKind() != Token::ID && immediate.length() > 6) {
    cerr << "ERROR: " << "Token 5 of instructon "  << op << " is out of range." << endl;
    return false;
  }
  if (tokens[5].getKind() == Token::INT) {
    imme = stoi(immediate);
    if (imme < -32768 || imme > 32767) {
      cerr << "ERROR: " << "Token 5 of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  else if (tokens[5].getKind() == Token::HEXINT) {
    imme = stoi(immediate, nullptr, 16);
  }
  else {
    if (table.find(immediate) == table.end()) {
      cerr << "ERROR: " << "Lable in the instruction " << op << " at line " << pc << " does not exist." << endl;
      return false;
    }
    imme = table[immediate] - pc - 1;
    if (imme < -32768 || imme > 32767) {
      cerr << "ERROR: " << "Token 5 of instructon "  << op << " is out of range." << endl;
      return false;
    }
  }
  if (op == "beq") {
    instr =  (4<<26) | (s<<21) | (t<<16) | (imme & 0xFFFF);
  }
  else if (op == "bne") {
    instr =  (5<<26) | (s<<21) | (t<<16) | (imme & 0xFFFF);
  }
  c = instr >> 24;
  cout << c;
  c = instr >> 16;
  cout << c;
  c = instr >> 8;
  cout << c;
  c = instr;
  cout << c;
  return true;
}

bool jump(vector<Token>& tokens, string op) {
  int s, t = 0, d = 0, instr;
  unsigned char c;
  string reg;
  if (tokens.size() != 2 || tokens[1].getKind() != Token::REG) {
    cerr << "ERROR: " << "Format of instruction " << op << " is invalid." << endl;
    return false;
  }
  reg = tokens[1].getLexeme().substr(1);
  while (reg != "" && reg[0] == '0') {
    reg = reg.substr(1);
  }
  if (reg.length() > 2 || (reg != "" && stoi(reg) > 31)) {
    cerr << "ERROR: " << "Token 1 of instructon "  << op << " is out of range." << endl;
    return false;
  }
  s = stoi(reg);
  if (op == "jr") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 8;
  }
  else if (op == "jalr") {
    instr =  (0<<26) | (s<<21) | (t<<16) | (d<<11) | 9;
  }
  c = instr >> 24;
  cout << c;
  c = instr >> 16;
  cout << c;
  c = instr >> 8;
  cout << c;
  c = instr;
  cout << c;
  return true;
}

bool ifWORD(vector<Token>& tokens, map<string, int>& table, int pc) {
  bool inrange;
  int instr;
  unsigned int instru;
  unsigned char c;
  if (tokens.size() != 2) {
    cerr << "ERROR: " << "Format of instruction .word at line " << pc << " is invalid." << endl;
    return false;
  }
  if (tokens[1].getKind() == Token::INT) {
    inrange = checkInt(tokens[1].getLexeme());
    if (!inrange) {
      cerr << "ERROR: " << "Int value of instruction .word at line " << pc << " is out of range." << endl;
      return false;
    }
    if (tokens[1].getLexeme()[0] == '-') {
      instr = stoi(tokens[1].getLexeme());
    }
    else {
      instru = stoll(tokens[1].getLexeme());
    }
  }
  else if (tokens[1].getKind() == Token::HEXINT) {
    inrange = checkHex(tokens[1].getLexeme());
    if (!inrange) {
      cerr << "ERROR: " << "Hex value of instruction .word at line " << pc << " is out of range." << endl;
      return false;
    }
    else {
      instr = stoll(tokens[1].getLexeme(), nullptr, 16);
    }
  }
  else {
    if (table.find(tokens[1].getLexeme()) == table.end()) {
      cerr << "ERROR: " << "Lable in the instruction .word at line " << pc << " does not exist." << endl;
      return false;
    }
    else {
      instru = table[tokens[1].getLexeme()] * 4;
    }
  }
  if ((tokens[1].getKind() == Token::INT && tokens[1].getLexeme()[0] == '-') || tokens[1].getKind() == Token::HEXINT) {
    c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr;
    cout << c;
    return true;
  }
  else {
    c = instru >> 24;
    cout << c;
    c = instru >> 16;
    cout << c;
    c = instru >> 8;
    cout << c;
    c = instru;
    cout << c;
    return true;
  }
}

bool checkInt(string n) {
  bool neg = false;
  int res;
  if (n[0] == '-') {
    n = n.substr(1);
    neg = true;
  }
  while (n != "" && n[0] == '0') {
    n = n.substr(1);
  }
  if (n == "" || n.length() < 10) {
    return true;
  }
  if (neg && n.length() == 10) {
    if (n[0] < '2') {
      return true;
    }
    else if (n[0] > '2') {
      return false;
    }
    else {
      res = stoi(n.substr(1));
      if (res <= 147483648) {
        return true;
      }
      else {
        return false;
      }
    }
  }
  else if (n.length() == 10) {
    if (n[0] < '4') {
      return true;
    }
    else if (n[0] > '4') {
      return false;
    }
    else {
      res = stoi(n.substr(1));
      if (res <= 294967295) {
        return true;
      }
      else {
        return false;
      }
    }
  }
  return false;
}

bool checkHex(string n) {
  if (n.length() > 2 && n.substr(0, 2) == "0x") {
    n = n.substr(2);
  }
  if (n.length() <= 8) {
    return true;
  }
  return false;
}