#include <ChordGenerator.h>
#include <unity.h>

void test_midi_num() {
  ChordGenerator chordGen(Scale{Notes::C, Mode::Major}, 3);
  TEST_ASSERT_EQUAL(60, chordGen.getNoteNum(0));
}

void test_scale_midi_num() {
  ChordGenerator chordGen(Scale{Notes::D, Mode::Major}, 4);
  TEST_ASSERT_EQUAL(76, chordGen.getNoteNum(1));
}

void test_chord_in_scale() {
  ChordGenerator chordGen(Scale{Notes::D, Mode::Major}, 4);
  Chord actual = chordGen.getMidiChord(0);

  Chord expected = Chord({74, 78, 81});

  TEST_ASSERT_EQUAL_INT_ARRAY(expected.notes.data(), actual.notes.data(), 3);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_midi_num);
  RUN_TEST(test_scale_midi_num);
  RUN_TEST(test_chord_in_scale);
  UNITY_END();
}
