#include "Regex.h"
#include <gtest/gtest.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using treg::Regex;

TEST(RegexSimpleTest, OrTest) {
  Regex r("a|b");

  EXPECT_TRUE(r.Match("a"));
  EXPECT_TRUE(r.Match("b"));

  EXPECT_FALSE(r.Match("ab"));
  EXPECT_FALSE(r.Match("ba"));
  EXPECT_FALSE(r.Match(""));
}

TEST(RegexSimpleTest, KleeneTest) {
  Regex r("a*a*");

  EXPECT_TRUE(r.Match("a"));
  EXPECT_TRUE(r.Match("aaaa"));
  EXPECT_TRUE(r.Match(""));
}

TEST(RegexSimpleTest, KleeneAndParentTest) {
  Regex r("(a*)*");

  EXPECT_TRUE(r.Match("a"));
  EXPECT_TRUE(r.Match("aa"));
  EXPECT_TRUE(r.Match(""));
}

TEST(RegexSimpleTest, PlusAndParentTest) {
  Regex t("(ab)+");

  EXPECT_TRUE(t.Match("ab"));
  EXPECT_TRUE(t.Match("abab"));

  EXPECT_FALSE(t.Match(""));
  EXPECT_FALSE(t.Match("a"));
}

TEST(RegexSimpleTest, QuestionMarkAndParentTest) {
  Regex t("(ab)?");

  EXPECT_TRUE(t.Match(""));
  EXPECT_TRUE(t.Match("ab"));

  EXPECT_FALSE(t.Match("abab"));
}

TEST(RegexSimpleTest, DoubleSlashAndPlusTest) {
  Regex t("a\\+");
  EXPECT_TRUE(t.Match("a+"));
  EXPECT_TRUE(!t.Match(""));
  EXPECT_FALSE(t.Match("a"));
}

TEST(RegexSimpleTest, KleeneAndDotTest) {
  Regex t("a.*b");

  EXPECT_TRUE(t.Match("ab"));
  std::cerr << "Testing Start ...\n";
  EXPECT_TRUE(t.Match("abb"));
  std::cerr << "Testing Over ...\n";
  EXPECT_TRUE(t.Match("abbb"));

  EXPECT_FALSE(t.Match("a"));
  EXPECT_FALSE(t.Match("b"));
  EXPECT_FALSE(t.Match("bb"));
  EXPECT_FALSE(t.Match(""));
}

TEST(RegexSimpleTest, KleeneAndOrTest) {
  {
    Regex r("(ab|bc|cd)*");

    EXPECT_TRUE(r.Match("ab"));
    EXPECT_TRUE(r.Match("bc"));
    EXPECT_TRUE(r.Match("abbccd"));
    EXPECT_TRUE(r.Match(""));

    EXPECT_FALSE(r.Match("ac"));
  }
  {
    Regex r("(a|b|c)*b*");

    EXPECT_TRUE(r.Match("b"));
    EXPECT_TRUE(r.Match("bbb"));
    EXPECT_TRUE(r.Match("bbbabc"));
    EXPECT_TRUE(r.Match(""));
  }
}

TEST(RegexSimpleTest, BracketsAndOrTest) {
  Regex t("([^ab]|a)f");

  EXPECT_TRUE(t.Match("af"));
  EXPECT_TRUE(t.Match("cf"));
  EXPECT_FALSE(t.Match(""));
  EXPECT_FALSE(t.Match("bf"));
}

TEST(RegexComprehensiveTest, CompTest) {
  // testing * and | and ()
  {
    Regex r("((ab*|ac*)*|ad*)*");

    EXPECT_TRUE(r.Match("a"));
    EXPECT_TRUE(r.Match(""));
    EXPECT_TRUE(r.Match("abad"));
    EXPECT_FALSE(r.Match("abcd"));
  }

  // testing [^] and *
  {
    Regex t("[^abc]*");
    EXPECT_TRUE(t.Match("xyz"));
    EXPECT_FALSE(t.Match("a"));
    EXPECT_FALSE(t.Match("b"));
    EXPECT_FALSE(t.Match("abc"));
    EXPECT_FALSE(t.Match("fuckabc"));
  }
  // testing [^] and | and ()
  {
    Regex t("12[^a-ch-ij]*");

    EXPECT_TRUE(t.Match("12xyz"));
    EXPECT_TRUE(t.Match("12"));
    EXPECT_FALSE(t.Match("12abcd"));
    EXPECT_FALSE(t.Match(""));
  }
  {
    Regex r("((ab*|ac*)*|ad*|ef)*");

    EXPECT_TRUE(r.Match("abbacadaaefa"));
    EXPECT_TRUE(r.Match(""));

    //
    EXPECT_TRUE(r.Compile("x(a(b|c))*(d|e)"));

    EXPECT_TRUE(r.Match("xabacd"));
    EXPECT_TRUE(r.Match("xd"));

    EXPECT_FALSE(r.Match("xde"));
  }

  {
    Regex r("(a|b)*abb");

    EXPECT_TRUE(r.Match("abb"));
    EXPECT_TRUE(r.Match("aabb"));
    EXPECT_TRUE(r.Match("ababb")) << "why ababb not match (a|b)*abb?";

    EXPECT_FALSE(r.Match("ab"));
    EXPECT_FALSE(r.Match(""));
  }
  {
    Regex r("x(a(b|c))*(d|e)");

    EXPECT_TRUE(r.Match("xacabe"));
    EXPECT_TRUE(!r.Match("xbcbe"));
    EXPECT_TRUE(r.Match("xe"));

    EXPECT_FALSE(r.Match(""));
    EXPECT_FALSE(r.Match("xbd"));
  }
  {
    Regex t("a\\+[bc].*e?f+");
    EXPECT_TRUE(t.Match("a+cbf"));
    EXPECT_TRUE(t.Match("a+bceff"));
  }
}
