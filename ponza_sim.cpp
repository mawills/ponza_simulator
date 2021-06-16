#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

enum card {
  ARBOR_ELF,
  UTOPIA_SPRAWL,
  BIRDS_OF_PARADISE ,
  PILLAGE,
  BLOOD_MOON,
  MAGUS_OF_THE_MOON,
  FOREST,
  MOUNTAIN,
  STOMPING_GROUND,
  FETCHLAND,
  COLORLESS_LAND,
  FILLER,
};

class Configuration {
  public:
    const int DECK_SIZE = 60;
    const int STARTING_HAND_SIZE = 7;
    const int MAX_MULLIGANS = 3;
    const int NUM_TRIALS = 10000;
    const map<card, int> DECKLIST =
    {
      { card::ARBOR_ELF, 4 },
      { card::UTOPIA_SPRAWL, 4 },
      { card::BIRDS_OF_PARADISE, 2 },
      { card::PILLAGE, 4 },
      { card::BLOOD_MOON, 4 },
      { card::MAGUS_OF_THE_MOON, 2 },
      { card::FOREST, 7 },
      { card::MOUNTAIN, 1 },
      { card::STOMPING_GROUND, 3 },
      { card::FETCHLAND, 8 },
      { card::COLORLESS_LAND, 1 },
    };
};

// Returns an unshuffled deck of DECK_SIZE containing the cards from config
vector<card> initializeDeck(map<card, int> decklist, int deckSize)
{
  vector<card> deck;

  for (auto const& x : decklist) {
    for (int i=0; i < x.second; i++) {
      deck.push_back(x.first);
    }
  }

  while (deck.size() < (unsigned)deckSize) {
    deck.push_back(card::FILLER);
  }

  return deck;
}

// Returns true if the given hand will allow a turn-2 land destruction spell
bool verifyHand(vector<card> hand)
{
  map<char,int> count;
  for (auto const card : hand) {
    count[card]++;
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

  vector<card> test1 = { card::FOREST, card::ARBOR_ELF, card::STOMPING_GROUND, card::BLOOD_MOON };
  (verifyHand(test1) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + blood moon + forest + stomping ground\n";

  vector<card> test2 = { card::FOREST, card::ARBOR_ELF };
  (verifyHand(test2) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + forest\n";

  vector<card> test3 = { card::FOREST, card::BLOOD_MOON };
  (verifyHand(test3) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "forest + blood moon\n";

  vector<card> test4 = { card::UTOPIA_SPRAWL, card::BLOOD_MOON };
  (verifyHand(test4) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "utopia sprawl + blood moon\n";

  vector<card> test5 = { card::FOREST, card::ARBOR_ELF, card::FOREST, card::BLOOD_MOON };
  (verifyHand(test5) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + blood moon + forest + forest\n";

  vector<card> test6 = { card::ARBOR_ELF, card::PILLAGE, card::FOREST, card::MOUNTAIN };
  (verifyHand(test6) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + pillage + forest + mountain\n";

  vector<card> test7 = { card::ARBOR_ELF, card::PILLAGE, card::FOREST, card::FETCHLAND };
  (verifyHand(test7) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + pillage + forest + fetchland\n";

  vector<card> test8 = { card::ARBOR_ELF, card::BIRDS_OF_PARADISE, card::PILLAGE, card::STOMPING_GROUND };
  (verifyHand(test8) == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + birds + pillage + stomping ground\n";

  vector<card> test9 = { card::ARBOR_ELF, card::MAGUS_OF_THE_MOON, card::FETCHLAND, card::FETCHLAND };
  (verifyHand(test9) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + magus + fetchland + fetchland\n";

  vector<card> test10 = { card::UTOPIA_SPRAWL, card::PILLAGE, card::STOMPING_GROUND, card::STOMPING_GROUND };
  (verifyHand(test10) == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "utopia sprawl + pillage + stomping ground + stomping ground\n";

  cout << "Tests complete." << '\n';
}

int main()
{
  srand(unsigned(time(0)));
  Configuration config;

  // runTests();

  vector<card> deck = initializeDeck(config.DECKLIST, config.DECK_SIZE);
  vector<card> hand;
  int goodHands = 0;
  for (int i=0; i < config.NUM_TRIALS; i++) {
    int countMulligan = 0;
    while (countMulligan <= config.MAX_MULLIGANS) {
      random_shuffle( deck.begin(), deck.end() );

      // Draw cards
      for (int i= 0; i < config.STARTING_HAND_SIZE - countMulligan; i++) {
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

  double percent = 100.0 * goodHands / config.NUM_TRIALS;
  cout << goodHands << '/' << config.NUM_TRIALS << " games with good starting hands. (" << percent << "%)";

  return 0;
}
