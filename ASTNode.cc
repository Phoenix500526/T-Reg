#include <assert.h>
#include <algorithm>
#include <iostream>
#include <iterator>

#include "ASTNode.h"

namespace treg {

namespace detail {

static const char* kTab = "    ";
const int ASTNode::kEnd = -1;
const int ASTNode::kAny = -2;

Input::Input() : exclude_(false) {}

bool Input::IsEnd() const {
  if (exclude_ || !ranges_.empty()) {
    return false;
  }
  return 1 == chars_.size() && ASTNode::kEnd == *chars_.begin();
}

bool Input::IsAny() const {
  if (exclude_ || !ranges_.empty()) {
    return false;
  }
  return 1 == chars_.size() && ASTNode::kAny == *chars_.begin();
}

bool Input::Accept(int ch) const {
  if (IsAny()) return true;
  bool found = false;
  if (chars_.count(ch) > 0) {
    found = true;
  } else {
    for (auto range : ranges_) {
      if (ch >= range.first && ch <= range.second) {
        found = true;
        break;
      }
    }
  }
  return found == !exclude_;
}

bool operator==(const Input& a, const Input& b) {
  return a.exclude_ == b.exclude_ && a.chars_ == b.chars_ &&
         a.ranges_ == b.ranges_;
}

bool operator<(const Input& a, const Input& b) {
  return a.exclude_ < b.exclude_ || a.chars_ < b.chars_ ||
         a.ranges_ < b.ranges_;
}

ASTNode::~ASTNode() {}

const ASTNode::PositionNodes& ASTNode::FollowPos() const { return followPos_; }

void ASTNode::AddFollowPos(const ASTNode::PositionNodes& follows) const {
  ASTNode::PositionNodes res;
  std::set_union(follows.begin(), follows.end(), followPos_.begin(),
                 followPos_.end(), std::inserter(res, res.begin()));
  followPos_ = std::move(res);
}

// CharNode
CharNode::CharNode(int c) : char_(c) {}

void CharNode::Print(int indent) const {
  for (int i = 0; i < indent; ++i) {
    std::cerr << kTab;
  }
  std::cerr << "CharNode";
  if (kEnd == char_)
    std::cerr << "[End]\n";
  else if (kAny == char_)
    std::cerr << "[Any]\n";
  else
    std::cerr << (char)char_ << std::endl;
}

bool CharNode::Nullable() const { return false; }

ASTNode::PositionNodes CharNode::FirstPos() const {
  return ASTNode::PositionNodes{this};
}

ASTNode::PositionNodes CharNode::LastPos() const {
  return ASTNode::PositionNodes{this};
}

void CharNode::CalcFollowPos() const {}

Input CharNode::GetInput() const {
  Input in;
  in.chars_.insert(char_);
  return in;
}

// RangeNode
RangeNode::RangeNode(bool b) : exclude_(b) {}

void RangeNode::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cerr << kTab;

  if (exclude_) std::cerr << "!";
  std::cerr << "RangeNode ";

  for (auto c : chars_) std::cerr << (char)c;
  if (!chars_.empty()) std::cerr << " ";

  for (auto pair : ranges_)
    std::cerr << "[" << (char)pair.first << "-" << (char)pair.second << "]";
  if (!chars_.empty() || !ranges_.empty()) std::cerr << std::endl;
}

bool RangeNode::Nullable() const { return chars_.empty() && ranges_.empty(); }

ASTNode::PositionNodes RangeNode::FirstPos() const {
  return ASTNode::PositionNodes{this};
}

ASTNode::PositionNodes RangeNode::LastPos() const {
  return ASTNode::PositionNodes{this};
}

void RangeNode::CalcFollowPos() const {}

void RangeNode::AddChar(int c) { chars_.insert(c); }

void RangeNode::AddRange(int low, int high) {
  if (low > high) std::swap(low, high);
  ranges_.emplace_back(low, high);
}

Input RangeNode::GetInput() const {
  Input in;
  in.chars_ = chars_;
  in.ranges_ = ranges_;
  in.exclude_ = exclude_;
  return in;
}

// ConcatNode
ConcatNode::ConcatNode() {}

void ConcatNode::AddNode(ASTNodePtr node) {
  if (!left_)
    left_ = std::move(node);
  else if (!right_)
    right_ = std::move(node);
  else
    assert(!!!"Never here, because bug never happen");
}

void ConcatNode::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cerr << kTab;

  std::cerr << "Concat... \n";

  left_->Print(indent + 1);
  right_->Print(indent + 1);
}

bool ConcatNode::Nullable() const {
  return left_->Nullable() && right_->Nullable();
}

ASTNode::PositionNodes ConcatNode::FirstPos() const {
  auto left = left_->FirstPos();
  auto right = right_->FirstPos();

  ASTNode::PositionNodes res;

  if (left_->Nullable()) {
    std::set_union(left.begin(), left.end(), right.begin(), right.end(),
                   std::inserter(res, res.begin()));
  } else {
    res.swap(left);
  }

  return res;
}

ASTNode::PositionNodes ConcatNode::LastPos() const {
  auto left = left_->LastPos();
  auto right = right_->LastPos();

  ASTNode::PositionNodes res;

  if (right_->Nullable()) {
    std::set_union(left.begin(), left.end(), right.begin(), right.end(),
                   std::inserter(res, res.begin()));
  } else {
    res.swap(right);
  }

  return res;
}

void ConcatNode::CalcFollowPos() const {
  auto leftLast = left_->LastPos();
  auto rightFirst = right_->FirstPos();

  for (auto node : leftLast) {
    node->AddFollowPos(rightFirst);
  }

  left_->CalcFollowPos();
  right_->CalcFollowPos();
}

// AlterNode
AlterNode::AlterNode() {}

void AlterNode::AddNode(std::unique_ptr<ASTNode> node) {
  if (!left_)
    left_ = std::move(node);
  else if (!right_)
    right_ = std::move(node);
  else
    assert(false);
}

void AlterNode::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cerr << kTab;

  std::cerr << "Alter... \n";

  left_->Print(indent + 1);
  right_->Print(indent + 1);
}

bool AlterNode::Nullable() const {
  return left_->Nullable() || right_->Nullable();
}

ASTNode::PositionNodes AlterNode::FirstPos() const {
  auto left = left_->FirstPos();
  auto right = right_->FirstPos();

  ASTNode::PositionNodes res;
  std::set_union(left.begin(), left.end(), right.begin(), right.end(),
                 std::inserter(res, res.begin()));

  return res;
}

ASTNode::PositionNodes AlterNode::LastPos() const {
  auto left = left_->LastPos();
  auto right = right_->LastPos();

  ASTNode::PositionNodes res;
  std::set_union(left.begin(), left.end(), right.begin(), right.end(),
                 std::inserter(res, res.begin()));

  return res;
}

void AlterNode::CalcFollowPos() const {
  left_->CalcFollowPos();
  right_->CalcFollowPos();
}

// RepeateNode
RepeateNode::RepeateNode(std::unique_ptr<ASTNode> node)
    : node_(std::move(node)) {
  repeat_.first = repeat_.second = 1;
}

void RepeateNode::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cerr << kTab;

  std::cerr << "Repeat(" << repeat_.first << "," << repeat_.second << ")... \n";
  node_->Print(indent + 1);
}

bool RepeateNode::Nullable() const { return repeat_.first == 0; }

ASTNode::PositionNodes RepeateNode::FirstPos() const {
  assert(repeat_.second > 0);
  return node_->FirstPos();
}

ASTNode::PositionNodes RepeateNode::LastPos() const {
  assert(repeat_.second > 0);
  return node_->LastPos();
}

void RepeateNode::CalcFollowPos() const {
  assert(repeat_.second > 0);
  if (repeat_.second > 1) {
    const auto first = FirstPos();
    auto lastPos = LastPos();
    for (auto n : lastPos) {
      n->AddFollowPos(first);
    }
  }

  node_->CalcFollowPos();
}

void RepeateNode::SetRepeatCount(int low, int high) {
  repeat_.first = low;
  repeat_.second = high;
}

// KleeneNode

KleeneNode::KleeneNode(std::unique_ptr<ASTNode> node)
    : kleene_(std::move(node)) {}

void KleeneNode::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cerr << kTab;

  std::cerr << "Kleene... \n";
  kleene_->Print(indent + 1);
}

bool KleeneNode::Nullable() const { return true; }

ASTNode::PositionNodes KleeneNode::FirstPos() const {
  return kleene_->FirstPos();
}

ASTNode::PositionNodes KleeneNode::LastPos() const {
  return kleene_->LastPos();
}

void KleeneNode::CalcFollowPos() const {
  const auto first = FirstPos();
  auto lastPos = LastPos();
  for (auto n : lastPos) {
    n->AddFollowPos(first);
  }

  kleene_->CalcFollowPos();
}

}  // detail

}  // treg