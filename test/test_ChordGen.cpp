#include <unity.h>
#include <ChordGenerator.h>


void test_midi_num() {
  ChordGenerator chordGen(Scale{Notes::C, Mode::Major}, 3);
  TEST_ASSERT_EQUAL(60, chordGen.getNoteNum(0));
}

void test_scale_midi_num() {
  ChordGenerator chordGen(Scale{Notes::D, Mode::Major}, 4);
  TEST_ASSERT_EQUAL(76, chordGen.getNoteNum(1));
}



int main() {
  UNITY_BEGIN();
  RUN_TEST(test_midi_num);
  RUN_TEST(test_scale_midi_num);
  UNITY_END();
}
