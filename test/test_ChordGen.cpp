#include <unity.h>

int add(int a, int b) {
  return a + b;
}

void test_add() {
  TEST_ASSERT_EQUAL(4, add(2, 2));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_add);
  UNITY_END();
}
