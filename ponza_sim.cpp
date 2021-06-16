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
  BIRDS_OF_PARADISE,
  IGNOBLE_HIERARCH,
  PILLAGE,
  BLOOD_MOON,
  MAGUS_OF_THE_MOON,
  STONE_RAIN,
  FOREST,
  MOUNTAIN,
  STOMPING_GROUND,
  FETCHLAND,
  COLORLESS_LAND,
  FILLER,
};

class Configuration
{
  public:
    const int DECK_SIZE = 60;
    const int STARTING_HAND_SIZE = 7;
    const int MAX_MULLIGANS = 2;
    const int NUM_TRIALS = 10000;
    const map<card, int> DECKLIST =
    {
      { card::ARBOR_ELF, 4 },
      { card::UTOPIA_SPRAWL, 4 },
      { card::BIRDS_OF_PARADISE, 2 },
      { card::IGNOBLE_HIERARCH, 0},
      { card::PILLAGE, 4 },
      { card::BLOOD_MOON, 4 },
      { card::MAGUS_OF_THE_MOON, 2 },
      { card::STONE_RAIN, 0 },
      { card::FOREST, 7 },
      { card::MOUNTAIN, 1 },
      { card::STOMPING_GROUND, 3 },
      { card::FETCHLAND, 8 },
      { card::COLORLESS_LAND, 1 },
    };
};

class Deck
{
  public:
  vector<card> cards;

  Deck (map<card, int> decklist, int deckSize) {
    vector<card> a;
    for (auto const& x : decklist) {
      for (int i=0; i < x.second; i++) {
        a.push_back(x.first);
      }
    }

    while (a.size() < (unsigned)deckSize) {
      a.push_back(card::FILLER);
    }

    cards = a;
  };

  void shuffle() {
    random_shuffle( this->cards.begin(), this->cards.end() );
  };

};

class Hand
{
  public:
  vector<card> cards;

  Hand(vector<card> a) {
    cards = a;
  };

  // Returns true if the given hand will allow a turn-2 land destruction spell
  bool verifyHand()
  {
    map<char,int> count;
    for (auto const card : this->cards) {
      count[card]++;
    }

    const bool isRampSpellAvailable = (
      count[card::ARBOR_ELF] +
      count[card::UTOPIA_SPRAWL] +
      count[card::IGNOBLE_HIERARCH] +
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
      count[card::STONE_RAIN] +
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
      count[card::IGNOBLE_HIERARCH] +
      count[card::UTOPIA_SPRAWL]);

    // Arbor Elf can count for a red source as long as a forest-land can tap for red
    if (count[card::STOMPING_GROUND] + count[card::FETCHLAND] > 0 && count[card::ARBOR_ELF] > 0) {
      numRedSources++;
    }

    // If your only LD is Pillage, you will need two red sources
    if (count[card::BLOOD_MOON] == 0 &&
        count[card::MAGUS_OF_THE_MOON] == 0 &&
        count[card::STONE_RAIN] == 0 &&
        count[card::PILLAGE] > 0 &&
        numRedSources < 2) return false;
    if (numRedSources < 1) return false;

    return true;
  };

  void drawCards(Deck deck, int n) {
    for (int i= 0; i < n; i++) {
        this->cards.push_back(deck.cards[i]);
      }
  };

};

void runTests()
{
  cout << "Running tests..." << '\n';

  Hand test1({ card::FOREST, card::ARBOR_ELF, card::STOMPING_GROUND, card::BLOOD_MOON });
  (test1.verifyHand() == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + blood moon + forest + stomping ground\n";

  Hand test2({ card::FOREST, card::ARBOR_ELF });
  (test2.verifyHand() == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + forest\n";

  Hand test3({ card::FOREST, card::BLOOD_MOON });
  (test3.verifyHand() == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "forest + blood moon\n";

  Hand test4({ card::UTOPIA_SPRAWL, card::BLOOD_MOON });
  (test4.verifyHand() == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "utopia sprawl + blood moon\n";

  Hand test5({ card::FOREST, card::ARBOR_ELF, card::FOREST, card::BLOOD_MOON });
  (test5.verifyHand() == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + blood moon + forest + forest\n";

  Hand test6({ card::ARBOR_ELF, card::PILLAGE, card::FOREST, card::MOUNTAIN });
  (test6.verifyHand() == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + pillage + forest + mountain\n";

  Hand test7({ card::ARBOR_ELF, card::PILLAGE, card::FOREST, card::FETCHLAND });
  (test7.verifyHand() == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + pillage + forest + fetchland\n";

  Hand test8({ card::ARBOR_ELF, card::BIRDS_OF_PARADISE, card::PILLAGE, card::STOMPING_GROUND });
  (test8.verifyHand() == false) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + birds + pillage + stomping ground\n";

  Hand test9({ card::ARBOR_ELF, card::MAGUS_OF_THE_MOON, card::FETCHLAND, card::FETCHLAND });
  (test9.verifyHand() == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "arbor elf + magus + fetchland + fetchland\n";

  Hand test10({ card::UTOPIA_SPRAWL, card::PILLAGE, card::STOMPING_GROUND, card::STOMPING_GROUND });
  (test10.verifyHand() == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "utopia sprawl + pillage + stomping ground + stomping ground\n";

  Hand test11({ card::IGNOBLE_HIERARCH, card::STONE_RAIN, card::FOREST, card::COLORLESS_LAND });
  (test11.verifyHand() == true) ? (cout << "ok: ") : (cout << "fail: ");
  cout << "ignoble hierarch + stone rain + forest + colorless land\n";

  cout << "Tests complete." << '\n';
}

int main()
{
  srand(unsigned(time(0)));
  Configuration config;
  Deck deck(config.DECKLIST, config.DECK_SIZE);
  Hand hand({});

  // runTests();

  int countSuccessfulTrials = 0;
  for (int i=0; i < config.NUM_TRIALS; i++) {
    int countMulligans = 0;
    while (countMulligans <= config.MAX_MULLIGANS) {
      deck.shuffle();
      hand.drawCards(deck, config.STARTING_HAND_SIZE - countMulligans);

      // If the hand is good, move on to next trial. If not, mulligan.
      if (hand.verifyHand()) {
        countSuccessfulTrials++;
        hand.cards.clear();
        break;
      } else {
        countMulligans++;
        hand.cards.clear();
      }
    }
  }

  double percent = 100.0 * countSuccessfulTrials / config.NUM_TRIALS;
  cout << countSuccessfulTrials << '/' << config.NUM_TRIALS << " games with good starting hands. (" << percent << "%)";

  return 0;
}
