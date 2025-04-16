#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

// Global variables with default values
int SEED = 0;
int NUM_CHIPS_PER_BAG = 30;
int NUM_PLAYERS = 5;

///////////////////
// DECK OF CARDS //
///////////////////
// This class will represent a deck of cards. It will have a vector of integers representing the cards in the deck.
// The cards will be represented by integers from 0 to 51, where 0-12 are clubs, 13-25 are hearts, 26-38 are spades, and 39-51 are diamonds.
// The rank can be determined by the modulo of the card number by 4, and the suite can be determined by the integer division of the card number by 13.
static class DECK_OF_CARDS { 
public:
    vector<int> DECK = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                        40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52};

    vector<int> DISCARD_PILE;


    void SHUFFLE_DECK() {
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
        int TOP_CARD = DECK[0];
        DECK.erase(DECK.begin());
        DISCARD_PILE.push_back(TOP_CARD);
        return TOP_CARD;
    }


    // Might need to add non-destructive/additional versions of these functions
    // but this is definitely a good start for the deck of cards class
};


//////////////////
// BAG OF CHIPS //
//////////////////
static class BAG_OF_CHIPS {
    int NUM_CHIPS = NUM_CHIPS_PER_BAG;


public:
    void OPEN_NEW_BAG() {
        NUM_CHIPS = NUM_CHIPS_PER_BAG;
    }

    void TAKE_CHIPS(int NUM_CHIPS_TAKEN) {
        NUM_CHIPS -= NUM_CHIPS_TAKEN;
    }

    int GET_NUM_CHIPS() {
        return NUM_CHIPS;
    }
};


////////////
// PLAYER //
////////////
class PLAYER {
    int CURRENT_CARD = 0;
    int CHIPS_EATEN = 0;
    int NUM_WINS = 0;

    bool IS_DEALER = false;


    void EAT_CHIPS(int NUM_CHIPS_EATEN) {
        CHIPS_EATEN += NUM_CHIPS_EATEN;
    }

    // NEED TO HANDLE WHEN CURRENT_CARD IS 0 (NO CARD)
    // NEED TO HANDLE WHEN CURRENT_CARD IS 0 (NO CARD)
    // NEED TO HANDLE WHEN CURRENT_CARD IS 0 (NO CARD)
    int TAKE_NEW_CARD(int CARD) {
        int TEMP = CARD;
        CURRENT_CARD = CARD;
        return TEMP;
    }

    // NEED TO HANDLE WHEN THERE IS A MATCH (RETURN 0)
    // NEED TO HANDLE WHEN THERE IS A MATCH (RETURN 0)
    // NEED TO HANDLE WHEN THERE IS A MATCH (RETURN 0)
    int CHECK_FOR_MATCH(int CARD_1) {
        if (CARD_1 % 4 == CURRENT_CARD % 4) {
        NUM_WINS++;
            return 0;
        } else {
            // 50 / 50 chance of keeping current card
            if (rand() % 2 == 0) {
                int TEMP = CURRENT_CARD;
                CURRENT_CARD = CARD_1;
                return TEMP;
            } else {
                return CARD_1;
            }
        }
    }



    // ACTUAL TURN FUNCTIONS
    void WAIT_FOR_TURN() {
        // Wait for the turn to be over
        // This will be a mutex lock or a condition variable
    }

    void TAKE_TURN() {
        // Take the turn
        // This will be a mutex lock or a condition variable
    }

    void END_OF_ROUND() {
        // End of the round
        // This will be a mutex lock or a condition variable
    }
};









int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <seed> <num_players> <num_chips>" << endl;
        return 1;
    }

    SEED = atoi(argv[1]);
    NUM_PLAYERS = atoi(argv[2]);
    NUM_CHIPS_PER_BAG = atoi(argv[3]);

    srand(SEED);

    // Mutexes
    pthread_mutex_t deck_mutex;
    pthread_mutex_t chips_mutex;
    pthread_mutex_t log_mutex;
    pthread_mutex_t console_mutex;


    /*
    The main function should create n threads – one for each player.
    Notice that we want to keep the threads synchronized and to protect any shared objects (e.g., deck of cards,
    log file, printing to the console, eating from the bag of chips). At the end of every round, each player must
    print a message to the console if they won or lost this round (e.g., “Player 2 lost round 4” or “Player 3 won
    round 1”, etc.). The main program takes 3 arguments: a seed for the random number generation (which
    will be used in shuffling and in discarding cards and deciding how many chips to be eaten), the number of
    players, the number of potato chips in a bag).
    */



    
    return 0;
}