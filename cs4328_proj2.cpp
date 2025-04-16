#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

///////////////////
// DECK OF CARDS //
///////////////////
// This class will represent a deck of cards. It will have a vector of integers representing the cards in the deck.
// The cards will be represented by integers from 0 to 51, where 0-12 are clubs, 13-25 are hearts, 26-38 are spades, and 39-51 are diamonds.
// The rank can be determined by the modulo of the card number by 4, and the suite can be determined by the integer division of the card number by 13.
class DECK_OF_CARDS { 
public:
    vector<int> DECK;
    vector<int> DISCARD_PILE;
    
    DECK_OF_CARDS() {
        for (int i = 0; i < 52; i++) {
            DECK.push_back(i);
        }
    }

    void SHUFFLE_DECK() {
        srand(time(NULL));

        // Put all cards from the discard pile back into the deck
        for (int i = 0; i < DISCARD_PILE.size(); i++) {
            DECK.push_back(DISCARD_PILE[i]);
        }
        DISCARD_PILE.clear();

        // Shuffle the entire deck
        for (int i = 0; i < DECK.size(); i++) {
            int j = rand() % DECK.size();
            swap(DECK[i], DECK[j]);
        }
    }

    void PRINT_DECK() {
        for (int i = 0; i < DECK.size(); i++) {
            cout << DECK[i] << " ";
        }
        cout << endl;
    }

    int DEAL_TOP_CARD() {
        int topCard = DECK[0];
        DECK.erase(DECK.begin());
        DISCARD_PILE.push_back(topCard);
        return topCard;
    }

    // Might need to add non-destructive/additional versions of these functions
    // but this is definitely a good start for the deck of cards class
};


////////////
// PLAYER //
////////////
class PLAYER {
    // NOT SURE ABOUT THIS TONIGHT WILL WORK ON LATER

};









int main() {
    
    return 0;
}