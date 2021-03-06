#include "math_utils/MathExpression.h"
#include "math_utils/angles.h"
#include "math_utils/math_utils.h"
#include <gtest/gtest.h>

#define TEST_EXPRESSION(a) EXPECT_EQ((a), meval::EvaluateMathExpression(#a))

TEST(MathExpressions, operatorRecognition){
  EXPECT_TRUE(meval::ContainsOperators("+"));
  EXPECT_TRUE(meval::ContainsOperators("-"));
  EXPECT_TRUE(meval::ContainsOperators("/"));
  EXPECT_TRUE(meval::ContainsOperators("*"));
  EXPECT_FALSE(meval::ContainsOperators("1234567890qwertyuiop[]asdfghjkl;'zxcvbnm,._=?8"));
}

TEST(MathExpressions, basicOperations){
  EXPECT_EQ(5, meval::EvaluateMathExpression("2+3"));
  EXPECT_EQ(5, meval::EvaluateMathExpression("2 + 3"));
  EXPECT_EQ(10, meval::EvaluateMathExpression("20/2"));
  EXPECT_EQ(-4, meval::EvaluateMathExpression("6 - 10"));
  EXPECT_EQ(24, meval::EvaluateMathExpression("6 * 4"));
}

TEST(MathExpressions, complexOperations){
  TEST_EXPRESSION(((3 + 4) / 2.0) + 10);
  TEST_EXPRESSION(7 * (1 + 2 + 3 - 2 + 3.4) / 12.7);
  TEST_EXPRESSION((1 + 2 + 3) - (8.0 / 10));
}

TEST(MathExpressions, UnaryMinus){
  TEST_EXPRESSION(-5);
}

TEST(MathExpressions, badInput){
  //TODO - figure out what good error behavior is and test for it properly
  //EXPECT_EQ(0, meval::EvaluateMathExpression("4.1.3 - 4.1"));
  //EXPECT_EQ(0, meval::EvaluateMathExpression("4.1.3"));
}

TEST(MathUtils, basicOperations){
  EXPECT_EQ(math_utils::clamp<int>(-10, 10, 20), 10);
  EXPECT_EQ(math_utils::clamp<int>(15, 10, 20), 15);
  EXPECT_EQ(math_utils::clamp<int>(25, 10, 20), 20);
}

TEST(MathUtils, shortestDistanceWithLimits){
  double shortest_angle;
  bool result = math_utils::shortest_angular_distance_with_limits(-0.5, 0.5,-0.25,0.25,shortest_angle);
  EXPECT_TRUE(!result);

  result = math_utils::shortest_angular_distance_with_limits(-0.5, 0.5,0.25,0.25,shortest_angle);
  EXPECT_TRUE(!result);

  result = math_utils::shortest_angular_distance_with_limits(-0.5, 0.5,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(result);
  EXPECT_NEAR(shortest_angle, -2*M_PI+1.0,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(0.5, 0.5,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(result);
  EXPECT_NEAR(shortest_angle, 0,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(0.5, 0,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(!result);
  EXPECT_NEAR(shortest_angle, -0.5,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(-0.5, 0,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(!result);
  EXPECT_NEAR(shortest_angle, 0.5,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(-0.2,0.2,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(!result);
  EXPECT_NEAR(shortest_angle, -2*M_PI+0.4,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(0.2,-0.2,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(!result);
  EXPECT_NEAR(shortest_angle,2*M_PI-0.4,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(0.2,0,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(!result);
  EXPECT_NEAR(shortest_angle,2*M_PI-0.2,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(-0.2,0,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(!result);
  EXPECT_NEAR(shortest_angle,-2*M_PI+0.2,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(-0.25,-0.5,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(result);
  EXPECT_NEAR(shortest_angle,-0.25,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(-0.25,0.5,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(result);
  EXPECT_NEAR(shortest_angle,-2*M_PI+0.75,1e-6);

  result = math_utils::shortest_angular_distance_with_limits(-0.2500001,0.5,0.25,-0.25,shortest_angle);
  EXPECT_TRUE(result);
  EXPECT_NEAR(shortest_angle,-2*M_PI+0.5+0.2500001,1e-6);

}

int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
