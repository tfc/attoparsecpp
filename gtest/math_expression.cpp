#include <attoparsecpp/math_expression.hpp>

#include <gtest/gtest.h>

using namespace apl;

TEST(MathExpressionParser, AddOpParserEmptyString) {
  const auto r{run_parser(add_op, "")};
  EXPECT_FALSE(!!r.first);
}

TEST(MathExpressionParser, AddOpParserNonOpString) {
  const auto r{run_parser(add_op, "a")};
  EXPECT_FALSE(!!r.first);
}

TEST(MathExpressionParser, AddOpParserPlusString) {
  const auto r{run_parser(add_op, "+")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ((*r.first)(100, 10), 110);
}

TEST(MathExpressionParser, AddOpParserMinusString) {
  const auto r{run_parser(add_op, "-")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ((*r.first)(100, 10), 90);
}

TEST(MathExpressionParser, MulOpParserEmptyString) {
  const auto r{run_parser(mul_op, "")};
  EXPECT_FALSE(!!r.first);
}

TEST(MathExpressionParser, MulOpParserNonOpString) {
  const auto r{run_parser(mul_op, "a")};
  EXPECT_FALSE(!!r.first);
}

TEST(MathExpressionParser, MulOpParserMultiplicationString) {
  const auto r{run_parser(mul_op, "*")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ((*r.first)(10, 2), 20);
}

TEST(MathExpressionParser, MulOpParserDivisionString) {
  const auto r{run_parser(mul_op, "/")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ((*r.first)(10, 2), 5);
}

TEST(MathExpressionParser, ExprParserEmptyString) {
  const auto r{run_parser(expr, "")};
  EXPECT_FALSE(!!r.first);
}

TEST(MathExpressionParser, ExprParserSingleInteger) {
  const auto r{run_parser(expr, "123")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 123);
}

TEST(MathExpressionParser, ExprParserTwoIntsSum) {
  const auto r{run_parser(expr, "123 + 456")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 579);
}

TEST(MathExpressionParser, ExprParserFiveIntsSumAndSub) {
  const auto r{run_parser(expr, "1 + 2 + 3 - 4 - 5")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, -3);
}

TEST(MathExpressionParser, ExprParserTwoIntsProduct) {
  const auto r{run_parser(expr, "10 * 5")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 50);
}

TEST(MathExpressionParser, ExprParserFiveIntsProductAndDiv) {
  const auto r{run_parser(expr, "1 * 2 * 3 * 4 / 2")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 12);
}

TEST(MathExpressionParser, ExprParserMultiplicationThenAddition) {
  const auto r{run_parser(expr, "2 * 3 + 5")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 11);
}

TEST(MathExpressionParser, ExprParserAdditionThenMultiplication) {
  const auto r{run_parser(expr, "2 + 3 * 5")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 17);
}

TEST(MathExpressionParser, ExprParserComplexParentheses) {
  const auto r{run_parser(expr, "((((((2))))))")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 2);
}

TEST(MathExpressionParser, ExprParserMixedSubExpressions) {
  const auto r{run_parser(expr, "(2 * 3) + 5")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 11);
}

TEST(MathExpressionParser, ExprParserAdditionWithinParentheses) {
  const auto r{run_parser(expr, "2 * (3 + 5)")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 16);
}

TEST(MathExpressionParser, ExprParserMoreComplexExpression) {
  const auto r{run_parser(expr, "1 + (2 * (4 + 3) + 12 * 12 - (6 / 3))")};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 157);
}

TEST(MathExpressionParser, ExprParserTrailingOperationSingleInt) {
  const std::string str{"123 +"};
  const auto r{run_parser(expr, str)};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 123);
  EXPECT_TRUE(r.second.at_end());
}

TEST(MathExpressionParser, ExprParserTrailingOperationTwoInts) {
  const std::string str{"123 - 0 +"};
  const auto r{run_parser(expr, str)};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 123);
  EXPECT_TRUE(r.second.at_end());
}

TEST(MathExpressionParser, ExprParserTrailingOperationComplexExpr) {
  const std::string str{"1 + (2 * (4 + 3) + 12 * 12 - (6 / 3)) +"};
  const auto r{run_parser(expr, str)};
  ASSERT_TRUE(!!r.first);
  EXPECT_EQ(r.first, 157);
  EXPECT_TRUE(r.second.at_end());
}
