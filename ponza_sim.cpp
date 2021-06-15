#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

vector<char> initializeDeck(int deckSize, int numLands, int numLandDestruction, int numRamp) {
  vector<char> deck;
  for(int i=0; i < numRamp; i++) {
    deck.push_back('R');
  }

  for(int i=0; i < numLandDestruction; i++) {
    deck.push_back('D');
  }

  for(int i=0; i < numLands; i++) {
    deck.push_back('L');
  }

  for(int i= numRamp + numLandDestruction + numLands; i != deckSize; i++) {
    deck.push_back(' ');
  }

  return deck;
}

int main()
{
  srand(unsigned(time(0)));

  // Configuration
  int DECK_SIZE = 60;
  int STARTING_HAND_SIZE = 7;
  int MAX_MULLIGANS = 3;
  int NUM_TRIALS = 10000;
  int NUM_RAMP = 11;
  int NUM_LD = 12;
  int NUM_LANDS = 21;

  int goodHands = 0;
  for(int i=0; i < NUM_TRIALS; i++) {
    vector<char> deck = initializeDeck(DECK_SIZE, NUM_LANDS, NUM_LD, NUM_RAMP);
    vector<char> hand;
    int countMulligan = 0;
    
    while(countMulligan <= MAX_MULLIGANS) {
      int countRamp = 0;
      int countLD = 0;
      int countLands = 0;

      random_shuffle( deck.begin(), deck.end() );

      // Draw cards
      for(int i= 0; i < STARTING_HAND_SIZE - countMulligan; i++) {
        hand.push_back(deck[i]);
      }

      // Count the number of ramp, LD, and lands in the hand
      for(int i= 0; i < STARTING_HAND_SIZE; i++) {
        if (hand[i] == 'R') countRamp++;
        if (hand[i] == 'D') countLD++;
        if (hand[i] == 'L') countLands++;
      }

      // If the hand is good, move on to next trial. If not, mulligan.
      if (countRamp >= 1 && countLD >= 1 && countLands >= 2) {
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
