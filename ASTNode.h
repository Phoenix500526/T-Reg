#ifndef TREG_ASTNODE_H_
#define TREG_ASTNODE_H_

#include <memory>
#include <set>
#include <vector>

namespace treg {
namespace detail {

using Range = std::pair<int, int>;

struct Input {
  Input();
  bool IsEnd() const;
  bool IsAny() const;
  bool Accept(int ch) const;

  friend bool operator==(const Input& a, const Input& b);
  friend bool operator<(const Input& a, const Input& b);

  bool exclude_;
  std::vector<Range> ranges_;
  std::set<int> chars_;
};

class ASTNode {
 public:
  using PositionNodes = std::set<const ASTNode*>;
  virtual ~ASTNode();

  virtual bool Nullable() const = 0;
  virtual PositionNodes FirstPos() const = 0;
  virtual PositionNodes LastPos() const = 0;
  const PositionNodes& FollowPos() const;

  virtual void CalcFollowPos() const = 0;
  virtual Input GetInput() const { return Input(); }
  void AddFollowPos(const PositionNodes& follows) const;

  // for debug
  virtual void Print(int indent) const {}

  static const int kEnd;
  static const int kAny;

 private:
  mutable PositionNodes followPos_;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

class CharNode : public ASTNode {
 public:
  explicit CharNode(int c);

  void Print(int indent) const override;
  bool Nullable() const override;
  PositionNodes FirstPos() const override;
  PositionNodes LastPos() const override;

  void CalcFollowPos() const override;
  Input GetInput() const override;

 private:
  int char_;
};

class RepeateNode : public ASTNode {
 public:
  explicit RepeateNode(ASTNodePtr node);

  void Print(int indent) const override;
  bool Nullable() const override;
  PositionNodes FirstPos() const override;
  PositionNodes LastPos() const override;
  void CalcFollowPos() const override;

  void SetRepeatCount(int low, int high);

 private:
  ASTNodePtr node_;
  Range repeat_;
};

class RangeNode : public ASTNode {
 public:
  explicit RangeNode(bool exclude);

  void Print(int indent) const override;
  bool Nullable() const override;
  PositionNodes FirstPos() const override;
  PositionNodes LastPos() const override;
  void CalcFollowPos() const override;
  Input GetInput() const override;

  void AddChar(int c);
  void AddRange(int low, int high);

 private:
  const bool exclude_;
  std::vector<Range> ranges_;
  std::set<int> chars_;
};

class ConcatNode : public ASTNode {
 public:
  ConcatNode();

  void Print(int indent) const override;
  void AddNode(ASTNodePtr node);

  bool Nullable() const override;
  PositionNodes FirstPos() const override;
  PositionNodes LastPos() const override;
  void CalcFollowPos() const override;

 private:
  ASTNodePtr left_;
  ASTNodePtr right_;
};

class AlterNode : public ASTNode {
 public:
  AlterNode();

  void Print(int indent) const override;
  void AddNode(ASTNodePtr node);

  bool Nullable() const override;
  PositionNodes FirstPos() const override;
  PositionNodes LastPos() const override;
  void CalcFollowPos() const override;

 private:
  ASTNodePtr left_;
  ASTNodePtr right_;
};

// Kleene Star:*
class KleeneNode : public ASTNode {
 public:
  explicit KleeneNode(ASTNodePtr node);

  void Print(int indent) const override;

  bool Nullable() const override;
  PositionNodes FirstPos() const override;
  PositionNodes LastPos() const override;
  void CalcFollowPos() const override;

 private:
  ASTNodePtr kleene_;
};

}  // detail
}  // treg

#endif  // TREG_ASTNODE_H_