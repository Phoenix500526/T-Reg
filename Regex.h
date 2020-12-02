#ifndef TREG_REGEX_H_
#define TREG_REGEX_H_

#include "DFA.h"
#include "noncopyable.h"

namespace treg {

class Regex final : noncopyable {
 public:
  Regex() = default;
  Regex(const char* regex);
  Regex(const char* regex, std::size_t len);
  Regex(const std::string& regex);

  ~Regex() = default;

  bool Compile(const char* regex);
  bool Compile(const char* regex, std::size_t len);
  bool Compile(const std::string& regex);

  bool Match(const char* pattern) const;
  bool Match(const std::string& pattern) const;

 private:
  detail::DFA dfa_;
};

}  // treg

#endif  // TREG_REGEX_H_