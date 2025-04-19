#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

//////////////////
// INSTRUCTIONS //
//////////////////
//
// 1 Overview
// In this project, we are going to build the game “Greasy Cards”. Greasy Cards is a card game played by
// n players in which players play cards using a single deck of cards while eating an infinite supply of bagged
// potato chips. Each bag contains m potato chips.
// The game is composed of n rounds. In each round, one of the players will serve as the dealer and will be
// responsible for (1) shuffling the deck of cards, (2) choosing and displaying a single chosen at random called
// the “Greasy Card”, (3) dealing a single card to each player, (4) opening the first bag of potato chips, and
// (5) waiting for the round to end. In round 1, player 1 will be the dealer, in round 2, player 2 will be the
// dealer and so on. Players eat a random number of chips between [1 and 5] after playing and before it would
// be their turn again. Only one player can grab potato chips from the bag at a time. Whenever a player
// discovers that there are no potato chips left, he/she opens another bag.
// In the first round, player 1 will start by shuffling the deck of cards, drawing one card to be the “Greasy
// Card” and giving each player one card. Player 1 will then place the remaining deck of cards on the table
// and signals that the round begins. In this round, players in a round-robin fashion and starting with player
// 2 will draw a card from the deck and compares his/her hand to the Greasy Card. If none of the cards in
// his/her hand match the Greasy Card, the player will discard one card at random by placing it at the end of
// the deck and the next player proceeds. If there is a match (e.g., two kings, two queens, two jacks or any two
// numbers of the same value), then the player will show their hand, declaring his/herself as the winner and
// the round ends. At the end of the first round, player 1 will signal the following round to start (i.e., player 2
// will be the dealer). A game ends when the n rounds are completed.
//
// 2 Implementation
// This project is to implemented in C using POSIX threads. You can check:
// https://hpc-tutorials.llnl.gov/posix/
// for a tutorial on the POSIX thread library. The main function should create n threads – one for each player.
// Notice that we want to keep the threads synchronized and to protect any shared objects (e.g., deck of cards,
// log file, printing to the console, eating from the bag of chips). At the end of every round, each player must
// print a message to the console if they won or lost this round (e.g., “Player 2 lost round 4” or “Player 3 won
// round 1”, etc.). The main program takes 3 arguments: a seed for the random number generation (which
// will be used in shuffling and in discarding cards and deciding how many chips to be eaten), the number of
// players, the number of potato chips in a bag).
//
// 3 The Output
// All players will write into a common log file each action they take. The log file should be able to describe
// exactly what is happening at each step of the game. The log file should look something like this:
//
// 4 SUBMISSION
// PLAYER 2: hand 5
// PLAYER 2: draws 7
// PLAYER 2: discards 7 at random
// PLAYER 2: hand 5
// DECK: contents of the deck, separated by spaces (e.g., 1 2 3)
// BAG: 17 Chips left
// PLAYER 4: eats 3 chips
// BAG: 14 Chips left
// PLAYER 3: hand J
// ...
// The final messages for a round (say round 3) should look something like:
// ...
// PLAYER 2: hand 3
// PLAYER 2: draws 6
// PLAYER 2: hand (3,6) <> Greasy card is 6
// PLAYER 2: wins round 3
// PLAYER 6: lost round 3
// PLAYER 5: lost round 3
// PLAYER 1: lost round 3
// PLAYER 4: lost round 3
// PLAYER 3: Round ends
// ... 




// Global variables with default values
int SEED = 0;
int NUM_CHIPS_PER_BAG = 30;
int NUM_PLAYERS = 6;
int GREASY_CARD = 0;
int CURRENT_ROUND = 0;
int CURRENT_TURN = 0;
int TOTAL_CHIPS_EATEN = 0;

bool ROUND_COMPLETE = false;

ofstream LOG_FILE;


class PLAYER;
vector<PLAYER> PLAYERS;

class DECK_OF_CARDS;
class BAG_OF_CHIPS;


// Mutexes
pthread_mutex_t DECK_MUTEX;
pthread_mutex_t CHIPS_MUTEX;
pthread_mutex_t LOG_MUTEX;
pthread_mutex_t CONSOLE_MUTEX;
pthread_mutex_t TURN_MUTEX;

// Condition variables
pthread_cond_t TURN_COND;
pthread_cond_t ROUND_COND;

void LOG(const string &message) {
    pthread_mutex_lock(&LOG_MUTEX);
    pthread_mutex_lock(&CONSOLE_MUTEX);
    cout << message << endl;
    LOG_FILE << message << endl;
    pthread_mutex_unlock(&CONSOLE_MUTEX);
    pthread_mutex_unlock(&LOG_MUTEX);
}



///////////////////
// DECK OF CARDS //
///////////////////
// This class will represent a deck of cards. It will have a vector of integers representing the cards in the deck.
// The cards will be represented by integers from 0 to 51, where 0-12 are clubs, 13-25 are hearts, 26-38 are spades, and 39-51 are diamonds.
// The rank can be determined by the modulo of the card number by 4, and the suite can be determined by the integer division of the card number by 13.
class DECK_OF_CARDS {
private:
    DECK_OF_CARDS() = delete; // Prevent instantiation of this class

public:
    static vector<int> DECK;

    static void INIT_DECK() {
        DECK = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52};
    }

    static void SHUFFLE_DECK() {
        // Shuffle the entire deck
        for (int i = 0; i < DECK.size(); i++) {
            int j = rand() % DECK.size();
            swap(DECK[i], DECK[j]);
        }
    }

    static void PRINT_DECK() {
        for (int i = 0; i < DECK.size(); i++) {
            cout << DECK[i] << " ";
        }
        cout << endl;
    }

    static int DEAL_TOP_CARD() {
        int TOP_CARD = DECK[0];
        DECK.erase(DECK.begin());
        return TOP_CARD;
    }

    static void DISCARD(int CARD) {
        // Discard the card by putting it at the end of the deck
        DECK.push_back(CARD);
    }
};


//////////////////
// BAG OF CHIPS //
//////////////////
class BAG_OF_CHIPS {
private:
    BAG_OF_CHIPS() = delete; // Prevent instantiation of this class
    static int NUM_CHIPS;

public:
    static void INIT_BAG_OF_CHIPS() {
        NUM_CHIPS = NUM_CHIPS_PER_BAG;
    }

    static void OPEN_NEW_BAG() {
        NUM_CHIPS = NUM_CHIPS_PER_BAG;
    }

    static void TAKE_CHIPS(int NUM_CHIPS_TAKEN) {
        NUM_CHIPS -= NUM_CHIPS_TAKEN;
        TOTAL_CHIPS_EATEN += NUM_CHIPS_TAKEN;
    }

    static int GET_NUM_CHIPS() {
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
    int PLAYER_ID = 0;

    bool WON_ROUND = false;

public:
    // Constructor
    PLAYER(int PLAYER_ID) : PLAYER_ID(PLAYER_ID) {};


    // Card number -> string representation
    /*
    string CARD_TO_STRING(int CARD) {
        string SUITE = "CHSD";
        string RANK = "A23456789TJQK";
        return RANK[CARD % 13] + string(1, SUITE[CARD / 13]);
    }
    */
   string CARD_TO_STRING(int CARD) {
       return to_string(CARD % 13 + 1);
   }

   int GET_CHIPS() {
       return CHIPS_EATEN;
   }

    int GET_WINS() {
         return NUM_WINS;
    }

    void PLAY() {
        while (CURRENT_ROUND < NUM_PLAYERS - 1) {
            pthread_mutex_lock(&TURN_MUTEX);
            // Ensure only the correct dealer resets the round
            if (ROUND_COMPLETE && PLAYER_ID == CURRENT_ROUND % NUM_PLAYERS) { // Dealer resets the round
                LOG("\nDEALER: PLAYER " + to_string(PLAYER_ID + 1));
                LOG("\nCURRENT_ROUND: " + to_string(CURRENT_ROUND + 1) + " < NUM_PLAYERS: " + to_string(NUM_PLAYERS) + " ? ==> " + to_string(CURRENT_ROUND < NUM_PLAYERS));
                ROUND_COMPLETE = false; // Reset the round flag
                CURRENT_ROUND++;        // Increment the round counter
                LOG("\nROUND " + to_string(CURRENT_ROUND + 1) + " begins" + "\n" + "\n" + "----------------------------------\n" + "\n");
                pthread_cond_broadcast(&TURN_COND); // Notify all players
                pthread_cond_broadcast(&ROUND_COND); // Notify all players
            }
            pthread_mutex_unlock(&TURN_MUTEX);

            while (!ROUND_COMPLETE) {
                if (CURRENT_TURN == PLAYER_ID) {
                    pthread_mutex_lock(&DECK_MUTEX);
                    if (CURRENT_ROUND % NUM_PLAYERS == PLAYER_ID) {
                        // Dealer logic: shuffle deck, set greasy card, and deal cards
                        DECK_OF_CARDS::SHUFFLE_DECK();
                        GREASY_CARD = DECK_OF_CARDS::DEAL_TOP_CARD();
                        LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": Greasy card is " + CARD_TO_STRING(GREASY_CARD));

                        for (int i = 0; i < NUM_PLAYERS; i++) {
                            PLAYERS[i].CURRENT_CARD = DECK_OF_CARDS::DEAL_TOP_CARD();
                            LOG("PLAYER " + to_string(i + 1) + ": dealt " + CARD_TO_STRING(PLAYERS[i].CURRENT_CARD));
                        }

                        LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": starts round " + to_string(CURRENT_ROUND + 1));
                    }
                    pthread_mutex_unlock(&DECK_MUTEX);

                    // Pass the turn to the next player
                    CURRENT_TURN = (CURRENT_TURN + 1) % NUM_PLAYERS;
                    pthread_cond_broadcast(&TURN_COND);
                }

                pthread_mutex_lock(&TURN_MUTEX);

                // Wait for the player's turn or round completion
                while (CURRENT_TURN != PLAYER_ID && !ROUND_COMPLETE) {
                    pthread_cond_wait(&TURN_COND, &TURN_MUTEX);
                }

                if (ROUND_COMPLETE) {
                    pthread_mutex_lock(&DECK_MUTEX);
                    DECK_OF_CARDS::DISCARD(CURRENT_CARD);
                    pthread_mutex_unlock(&DECK_MUTEX);
                    pthread_mutex_unlock(&TURN_MUTEX);
                    break;
                }

                // Player's turn logic...
                pthread_mutex_lock(&DECK_MUTEX);
                int TEMP_CARD = DECK_OF_CARDS::DEAL_TOP_CARD();
                pthread_mutex_unlock(&DECK_MUTEX);

                LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": hand " + CARD_TO_STRING(CURRENT_CARD));
                LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": draws " + CARD_TO_STRING(TEMP_CARD));
                // Check for a match with the Greasy Card
                if (CURRENT_CARD % 13 == GREASY_CARD % 13 || TEMP_CARD % 13 == GREASY_CARD % 13) {
                    WON_ROUND = true;
                    NUM_WINS++;
                    LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": hand (" + CARD_TO_STRING(CURRENT_CARD) +
                        ") <> The Greasy card was " + CARD_TO_STRING(GREASY_CARD));
                    LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": wins round " + to_string(CURRENT_ROUND + 1));

                    // Discard the current card and the temporary card
                    pthread_mutex_lock(&DECK_MUTEX);
                    DECK_OF_CARDS::DISCARD(CURRENT_CARD);
                    DECK_OF_CARDS::DISCARD(TEMP_CARD);
                    pthread_mutex_unlock(&DECK_MUTEX);

                    ROUND_COMPLETE = true;
                    pthread_cond_broadcast(&TURN_COND);
                    pthread_cond_broadcast(&ROUND_COND);
                    pthread_mutex_unlock(&TURN_MUTEX);
                    break;
                }

                // Discard either the current card or the temporary card
                pthread_mutex_lock(&DECK_MUTEX);
                if (rand() % 2 == 0) {
                    DECK_OF_CARDS::DISCARD(CURRENT_CARD);
                    LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": discards " + CARD_TO_STRING(CURRENT_CARD) + " at random");
                    CURRENT_CARD = TEMP_CARD;
                } else {
                    DECK_OF_CARDS::DISCARD(TEMP_CARD);
                    LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": discards " + CARD_TO_STRING(TEMP_CARD) + " at random");
                }
                pthread_mutex_unlock(&DECK_MUTEX);

                // Eat chips
                pthread_mutex_lock(&CHIPS_MUTEX);
                int CHIPS_TO_EAT = rand() % 5 + 1;
                if (BAG_OF_CHIPS::GET_NUM_CHIPS() < CHIPS_TO_EAT) {
                    BAG_OF_CHIPS::OPEN_NEW_BAG();
                    LOG("BAG: New bag opened");
                }
                BAG_OF_CHIPS::TAKE_CHIPS(CHIPS_TO_EAT);
                pthread_mutex_unlock(&CHIPS_MUTEX);
                CHIPS_EATEN += CHIPS_TO_EAT;

                LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": eats " + to_string(CHIPS_TO_EAT) + " chips");
                LOG("BAG: " + to_string(BAG_OF_CHIPS::GET_NUM_CHIPS()) + " Chips left");

                // Pass the turn to the next player
                CURRENT_TURN = (CURRENT_TURN + 1) % NUM_PLAYERS;
                pthread_cond_broadcast(&TURN_COND);

                pthread_mutex_unlock(&TURN_MUTEX);
            }

            if (!WON_ROUND) {
                LOG("PLAYER " + to_string(PLAYER_ID + 1) + ": lost round " + to_string(CURRENT_ROUND + 1));
                // Wait for the next round to start
                // How would I do this? I need to wait for the dealer to reset the round
            }
        }
    }
};







vector<int> DECK_OF_CARDS::DECK;
int BAG_OF_CHIPS::NUM_CHIPS = 0;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <seed> <num_players> <num_chips>" << endl;
        return 1;
    }

    SEED = atoi(argv[1]);
    NUM_PLAYERS = atoi(argv[2]);
    NUM_CHIPS_PER_BAG = atoi(argv[3]);

    srand(SEED);

    LOG_FILE.open("log.txt", ios::out);
    if (!LOG_FILE.is_open()) {
        cout << "Error opening log file" << endl;
        return -1;
    }
    

    DECK_OF_CARDS::INIT_DECK();
    BAG_OF_CHIPS::INIT_BAG_OF_CHIPS();

    // Initialize mutexes and condition variables
    pthread_mutex_init(&DECK_MUTEX, NULL);
    pthread_mutex_init(&CHIPS_MUTEX, NULL);
    pthread_mutex_init(&TURN_MUTEX, NULL);
    pthread_cond_init(&TURN_COND, NULL);
    pthread_cond_init(&ROUND_COND, NULL);
    pthread_mutex_init(&LOG_MUTEX, NULL);
    pthread_mutex_init(&CONSOLE_MUTEX, NULL);

    // Initialize players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        PLAYERS.push_back(PLAYER(i));
    }

    // Create threads for each player
    vector<pthread_t> PLAYER_THREADS(NUM_PLAYERS);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        pthread_create(&PLAYER_THREADS[i], NULL, [](void *arg) -> void * {
            PLAYER *P = (PLAYER *)arg;
            P->PLAY();
            return NULL;
        }, (void *)&PLAYERS[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_PLAYERS; i++) {
        pthread_join(PLAYER_THREADS[i], NULL);
    }


    // Print final results
    LOG("\nFinal Results:");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        LOG("Player " + to_string(i + 1) + ": " + to_string(PLAYERS[i].GET_CHIPS()) + " chips, " + to_string(PLAYERS[i].GET_WINS()) + " wins");
    }
    LOG("Total chips eaten: " + to_string(TOTAL_CHIPS_EATEN));

    // Cleanup
    pthread_mutex_destroy(&DECK_MUTEX);
    pthread_mutex_destroy(&CHIPS_MUTEX);    
    pthread_mutex_destroy(&TURN_MUTEX);
    pthread_cond_destroy(&TURN_COND);
    pthread_cond_destroy(&ROUND_COND);
    pthread_mutex_destroy(&LOG_MUTEX);
    pthread_mutex_destroy(&CONSOLE_MUTEX);

    

    // Close log file
    if (LOG_FILE.is_open()) {
        LOG_FILE.close();
    }

    return 0;
}