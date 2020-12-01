#ifndef TREG_DFA_H_
#define TREG_DFA_H_

#include <map>
#include <memory>
#include <set>
#include <string>

namespace treg {
namespace detail {

class ASTNode;
struct Input;

class DFA {
 public:
  ~DFA() = default;
  bool Match(const std::string& pattern) const;
  bool Match(const char* pattern) const;
  bool Match(const char* pattern, std::size_t len) const;
  bool Construct(ASTNode* root);
  struct State {
    explicit State(int id = -1);
    ~State() = default;

    std::string DebugString() const;

    void AddTranslation(const Input& input, State* dstState);
    std::set<State*> NextState(int input) const;

    int id;
    bool isFinal;
    std::map<Input, State*> transition;
  };

 private:
  using States = std::map<int, std::unique_ptr<State>>;
  static bool _Match(State* cur, const char* pattern, std::size_t len);
  States dfa_;
  State* _GetState(int id) const;
};

}  // detail
}  // treg
#endif  // TREG_DFA_H_