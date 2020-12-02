#include "Regex.h"
#include <string.h>
#include "ASTBuilder.h"

namespace treg {

Regex::Regex(const char* regex) { Compile(regex); }
Regex::Regex(const std::string& regex) { Compile(regex); }

bool Regex::Compile(const char* regex) { return Compile(regex, strlen(regex)); }

bool Regex::Compile(const char* regex, std::size_t len) {
  detail::ASTBuilder astBuilder;
  if (!astBuilder.Build(regex, len)) return false;
  return dfa_.Construct(astBuilder.GetASTRoot());
}

bool Regex::Compile(const std::string& regex) {
  return Compile(regex.data(), regex.size());
}

bool Regex::Match(const char* pattern) const { return dfa_.Match(pattern); }

bool Regex::Match(const std::string& pattern) const {
  return dfa_.Match(pattern.c_str(), pattern.size());
}

}  // treg