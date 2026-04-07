#include <Sequencer.h>
#include <Clock.h>
#include <unity.h>

Clock testClock(120.0f);
Sequencer seq(testClock);

void setUp() {
  // Reset sequencer state before each test
  seq.setArpOn(false);
  seq.setStrumOn(false);
}

void test_arp_mode_up() {
  Chord chord({60, 64, 67}); // C major chord
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::Up);
  seq.setChord(chord);
  
  // Test looping: 0, 1, 2, 0, 1, 2...
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum()); // loops back
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum());
}

void test_arp_mode_down() {
  Chord chord({60, 64, 67});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::Down);
  seq.setChord(chord);
  
  // Test looping: 2, 1, 0, 2, 1, 0...
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum()); // loops back
}

void test_arp_mode_updown() {
  Chord chord({60, 64, 67, 72});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::UpDown);
  seq.setChord(chord);
  
  // Test bounce: 0, 1, 2, 3, 2, 1, 0, 1, 2...
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(72, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum()); // direction change, no repeat
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum()); // direction change again
}

void test_arp_mode_downup() {
  Chord chord({60, 64, 67, 72});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::DownUp);
  seq.setChord(chord);
  
  // Test bounce: 3, 2, 1, 0, 1, 2, 3, 2...
  TEST_ASSERT_EQUAL(72, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum()); // direction change, no repeat
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(72, seq.getNextNoteNum());
}

void test_arp_mode_converge() {
  Chord chord({60, 64, 67, 72});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::Converge);
  seq.setChord(chord);
  
  // Test converge: 0, 3, 1, 2, (reset) 0, 3...
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum()); // left (0)
  TEST_ASSERT_EQUAL(72, seq.getNextNoteNum()); // right (3)
  TEST_ASSERT_EQUAL(64, seq.getNextNoteNum()); // left (1)
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum()); // right (2)
  // Should reset and start over
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
}

void test_arp_mode_diverge() {
  Chord chord({60, 64, 67, 72});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::Diverge);
  seq.setChord(chord);
  
  // Test diverge from center: starts at middle positions
  // For 4 notes: center indices are 2, 1
  uint8_t first = seq.getNextNoteNum();
  uint8_t second = seq.getNextNoteNum();
  
  // Should start from center and move outward
  TEST_ASSERT_TRUE(first == 64 || first == 67);
}

void test_separate_timing_params() {
  seq.setStrumDenom(8);
  seq.setArpDenom(16);
  
  TEST_ASSERT_EQUAL(8, seq.getStrumDenom());
  TEST_ASSERT_EQUAL(16, seq.getArpDenom());
}

void test_mutual_exclusivity() {
  seq.setStrumOn(true);
  TEST_ASSERT_TRUE(seq.isEnabled());
  
  // Setting arp should disable strum
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::Up);
  TEST_ASSERT_TRUE(seq.isEnabled());
  
  Chord chord({60, 64, 67});
  seq.setChord(chord);
  
  // Should use arp mode, not strum
  uint8_t note = seq.getNextNoteNum();
  TEST_ASSERT_EQUAL(60, note);
}

void test_edge_case_single_note() {
  Chord chord({60});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::UpDown);
  seq.setChord(chord);
  
  // Should handle single note gracefully
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
}

void test_edge_case_two_notes() {
  Chord chord({60, 67});
  seq.setArpOn(true);
  seq.setArpMode(ArpMode::UpDown);
  seq.setChord(chord);
  
  // Should bounce: 0, 1, 0, 1...
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(60, seq.getNextNoteNum());
  TEST_ASSERT_EQUAL(67, seq.getNextNoteNum());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_arp_mode_up);
  RUN_TEST(test_arp_mode_down);
  RUN_TEST(test_arp_mode_updown);
  RUN_TEST(test_arp_mode_downup);
  RUN_TEST(test_arp_mode_converge);
  RUN_TEST(test_arp_mode_diverge);
  RUN_TEST(test_separate_timing_params);
  RUN_TEST(test_mutual_exclusivity);
  RUN_TEST(test_edge_case_single_note);
  RUN_TEST(test_edge_case_two_notes);
  UNITY_END();
}
