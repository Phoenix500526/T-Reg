#ifndef TREG_ASTBUILDER_H_
#define TREG_ASTBUILDER_H_

#include <memory>
#include <stack>
#include <string>

#include "ASTNode.h"

namespace treg {
namespace detail {

class ASTBuilder final {
  static const int kLeft = '(';
  static const int kRight = ')';
  static const int kAlter = '|';
  static const int kConcat = '&';
  static const int kKleene = '*';
  static const int kLeftRange = '[';
  static const int kRightRange = ']';
  static const int kExclude = '^';
  static const int kPlus = '+';
  static const int kQuestion = '?';
  static const int kSlash = '\\';
  static const int kDot = '.';

 public:
  enum class PriorityLevel {
    LEVELZERO = 0,
    LEVELONE,
    LEVELTWO,
    LEVELTHREE,
  };
  void Print() const;
  bool Build(const char* regex);
  bool Build(const char* regex, std::size_t len);
  bool Build(const std::string& regex);
  ASTNode* GetASTRoot() const;

 private:
  PriorityLevel _Priority(int ch) const;
  void _Reset();
  std::unique_ptr<RangeNode> _ProcessRange(const char* regex, std::size_t len,
                                           std::size_t& offset);
  bool _ProcessNode(int symbol);
  bool _AddSymbol(std::stack<int>& symbols, int newSymbol);
  bool _AddEndNode();

  std::stack<ASTNodePtr> ast_;
};

}  // detail

}  // treg

#endif  // TREG_ASTBUILDER_H_