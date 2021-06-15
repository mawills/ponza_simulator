#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

enum card {
  ARBOR_ELF = 'a',
  UTOPIA_SPRAWL = 'b',
  BIRDS_OF_PARADISE = 'c',
  PILLAGE = 'd',
  BLOOD_MOON = 'e',
  MAGUS_OF_THE_MOON = 'f',
  FOREST = 'g',
  MOUNTAIN = 'h',
  STOMPING_GROUND = 'i',
  FETCHLAND = 'j',
  COLORLESS_LAND = 'k'
};

// Configuration
const int DECK_SIZE = 60;
const int STARTING_HAND_SIZE = 7;
const int MAX_MULLIGANS = 3;
const int NUM_TRIALS = 100000;
const int NUM_ARBOR_ELF = 4;
const int NUM_UTOPIA_SPRAWL = 4;
const int NUM_BOP = 2;
const int NUM_PILLAGE = 4;
const int NUM_BLOOD_MOON = 4;
const int NUM_MAGUS_OF_THE_MOON = 2;
const int NUM_FOREST = 8;
const int NUM_MOUNTAIN = 1;
const int NUM_STOMPING_GROUND = 2;
const int NUM_FETCHLAND = 8;
const int NUM_COLORLESS_LAND = 1;

// Returns an unshuffled deck of DECK_SIZE containing the cards from config
vector<char> initializeDeck()
{
  vector<char> deck;
  for(int i=0; i < NUM_ARBOR_ELF; i++) {
    deck.push_back(card::ARBOR_ELF);
  }

  for(int i=0; i < NUM_UTOPIA_SPRAWL; i++) {
    deck.push_back(card::UTOPIA_SPRAWL);
  }

  for(int i=0; i < NUM_BOP; i++) {
    deck.push_back(card::BIRDS_OF_PARADISE);
  }

  for(int i=0; i < NUM_PILLAGE; i++) {
    deck.push_back(card::PILLAGE);
  }

  for(int i=0; i < NUM_BLOOD_MOON; i++) {
    deck.push_back(card::BLOOD_MOON);
  }

  for(int i=0; i < NUM_MAGUS_OF_THE_MOON; i++) {
    deck.push_back(card::MAGUS_OF_THE_MOON);
  }

  for(int i=0; i < NUM_FOREST; i++) {
    deck.push_back(card::FOREST);
  }

  for(int i=0; i < NUM_MOUNTAIN; i++) {
    deck.push_back(card::MOUNTAIN);
  }

  for(int i=0; i < NUM_STOMPING_GROUND; i++) {
    deck.push_back(card::STOMPING_GROUND);
  }

  for(int i=0; i < NUM_FETCHLAND; i++) {
    deck.push_back(card::FETCHLAND);
  }

  for(int i=0; i < NUM_COLORLESS_LAND; i++) {
    deck.push_back(card::COLORLESS_LAND);
  }

  while (deck.size() < DECK_SIZE) {
    deck.push_back(' ');
  }

  return deck;
}

// Returns true if the given hand will allow a turn-2 land destruction spell
bool verifyHand(vector<char> hand)
{
  std::map<char,int> count;
  for (int i= 0; i < STARTING_HAND_SIZE; ++i) {
    count[hand[i]]++;
  }

  const bool isRampSpellAvailable = (
    count[card::ARBOR_ELF] +
    count[card::UTOPIA_SPRAWL] +
    count[card::BIRDS_OF_PARADISE] > 0);
  if (!isRampSpellAvailable) return false;

  const bool isGreenLandAvailable = (
    count[card::FOREST] +
    count[card::STOMPING_GROUND] +
    count[card::FETCHLAND] > 0);
  if (!isGreenLandAvailable) return false;

  const bool isLandDestructionAvailable = (
    count[card::PILLAGE] +
    count[card::BLOOD_MOON] +
    count[card::MAGUS_OF_THE_MOON] > 0);
  if (!isLandDestructionAvailable) return false;

  const int numLands =
    count[card::FOREST] +
    count[card::MOUNTAIN] +
    count[card::STOMPING_GROUND] +
    count[card::FETCHLAND] +
    count[card::COLORLESS_LAND];
  if (numLands < 2) return false;

  int numRedSources = (
    count[card::MOUNTAIN] +
    count[card::STOMPING_GROUND] +
    count[card::FETCHLAND] +
    count[card::BIRDS_OF_PARADISE] +
    count[card::UTOPIA_SPRAWL]);
  
  // Arbor Elf can count for a red source as long as a forest-land can tap for red
  if (count[card::STOMPING_GROUND] + count[card::FETCHLAND] > 0 && count[card::ARBOR_ELF] > 0) {
    numRedSources++;
  }

  // If your only LD is Pillage, you will need two red sources
  if (count[card::BLOOD_MOON] == 0 &&
      count[card::MAGUS_OF_THE_MOON] == 0 &&
      count[card::PILLAGE] > 0 &&
      numRedSources < 2) return false;
  if (numRedSources < 1) return false;

  return true;
}

void runTests()
{
  cout << "Running tests..." << '\n';

  vector<char> test1 = { card::FOREST, card::ARBOR_ELF, card::STOMPING_GROUND, card::BLOOD_MOON };
  (verifyHand(test1) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + blood moon + forest + stomping ground\n";

  vector<char> test2 = { card::FOREST, card::ARBOR_ELF };
  (verifyHand(test2) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + forest\n";

  vector<char> test3 = { card::FOREST, card::BLOOD_MOON };
  (verifyHand(test3) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "forest + blood moon\n";

  vector<char> test4 = { card::UTOPIA_SPRAWL, card::BLOOD_MOON };
  (verifyHand(test4) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "utopia sprawl + blood moon\n";

  vector<char> test5 = { card::FOREST, card::ARBOR_ELF, card::FOREST, card::BLOOD_MOON };
  (verifyHand(test5) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + blood moon + forest + forest\n";

  vector<char> test6 = { card::ARBOR_ELF, card::PILLAGE, card::FOREST, card::MOUNTAIN };
  (verifyHand(test6) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + pillage + forest + mountain\n";

  vector<char> test7 = { card::ARBOR_ELF, card::PILLAGE, card::FOREST, card::FETCHLAND };
  (verifyHand(test7) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + pillage + forest + fetchland\n";

  vector<char> test8 = { card::ARBOR_ELF, card::BIRDS_OF_PARADISE, card::PILLAGE, card::STOMPING_GROUND };
  (verifyHand(test8) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + birds + pillage + stomping ground\n";

  vector<char> test9 = { card::ARBOR_ELF, card::MAGUS_OF_THE_MOON, card::FETCHLAND, card::FETCHLAND };
  (verifyHand(test9) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + magus + fetchland + fetchland\n";

  vector<char> test10 = { card::UTOPIA_SPRAWL, card::PILLAGE, card::STOMPING_GROUND, card::STOMPING_GROUND };
  (verifyHand(test10) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "utopia sprawl + pillage + stomping ground + stomping ground\n";

  cout << "Tests complete." << '\n';
}

int main()
{
  srand(unsigned(time(0)));

  runTests();

  vector<char> deck = initializeDeck();
  vector<char> hand;
  int goodHands = 0;
  for (int i=0; i < NUM_TRIALS; i++) {
    int countMulligan = 0;
    while (countMulligan <= MAX_MULLIGANS) {
      random_shuffle( deck.begin(), deck.end() );

      // Draw cards
      for (int i= 0; i < STARTING_HAND_SIZE - countMulligan; i++) {
        hand.push_back(deck[i]);
      }

      // If the hand is good, move on to next trial. If not, mulligan.
      if (verifyHand(hand)) {
        goodHands++;
        hand.clear();
        break;
      } else {
        countMulligan++;
        hand.clear();
      }
    }
  }

  double percent = 100.0 * goodHands / NUM_TRIALS;
  cout << goodHands << '/' << NUM_TRIALS << " games with good starting hands. (" << percent << "%)";

  return 0;
}
