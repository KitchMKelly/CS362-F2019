/****************************************************************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/16/2019
* Description:	randomtestcard2.c tests the implementation of the dominion.c function minionAction().
*				Constructs the game state, randomizes the player's hand, choice1 and choice2 input,
*				and calls minionAction(). The success/failure of each test is printed to the console.
*				//TODO: switch to printing to randomTestRestults.out
****************************************************************************************************/

#include "dominion.h"
#include <stdio.h> 
#include <stdlib.h> 

# define HANDSIZE 7

struct errorState {

	//Current test variables

	//Invoking player and player's choices
	int chosenPlayer;
	int choice1;
	int choice2;

	//Track each players' initial hand size
	int startingHandSize[MAX_PLAYERS];

	//Cumulative test variables

	//Player chooses gain coins
	int gainCoins_error;
	int gainCoins_success;

	//Player chooses to discard hand and gain 4 cards.
	//All other players with over 4 cards must also
	//discard and draw 4 cards.
	int discard_error;
	int discard_success;

	//player doesn't choose either choice1 or choice2
	int nothing_happens;
};

/************************************************
* resetError()
*
* Resets/Initializes all values of errorState struct.
*
* @param:	Pointer to errorState struct.
*
* @return:	None - errorState values reset
*************************************************/
void resetError(struct errorState * error) {
	error->chosenPlayer = -1;
	error->choice1 = -1;
	error->choice2 = -1;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		error->startingHandSize[i] = -1;
	}
}

/************************************************
* setSupply()
*
* Randomly assigns the cards in the game's supply
*
* @param:	Pointer to integer array of size 10
*
* @return:	 None - integer array will contain 10 random
*			card values.
*************************************************/
void setSupply(int * k) {
	int allKingdomCards[] = { adventurer, council_room, feast,
		gardens, mine, remodel, smithy, village, baron,
		great_hall, minion, steward, tribute, ambassador,
		cutpurse, embargo, outpost, salvager, sea_hag, treasure_map };

	int allKingdomCardCount = 20;

	for (int i = 0; i < 10; i++) {
		/* Pick a random card from allKingdomCards.
		   If it's already in k, pick another.
		   If not, add it to k
		*/
		int pickAgain = 1;				//Flag used for signaling if a new card needs to be chosen

		while (pickAgain == 1) {
			//Choose a random card
			int chosenCard = rand() % 20;	//Returns 0 to 19

			//Lower pickAgain flag
			pickAgain = 0;

			//check if it's already in k
			for (int j = 0; j < i; j++) {
				//If the chosen card is found, reset pickAgain and leave loop
				if (k[j] == chosenCard) {
					pickAgain = 1;
					break;
				}
			}

			//If no cards re-raise pickAgain flag, then chosen card is added to k
			if (pickAgain == 0) {
				k[i] = chosenCard;
			}

			//Loop repeats if pickAgain = 1, otherwise selection of next card starts
		}
	}
}

/************************************************
* randomize()
*
* Randomly assigns the cards in each player's hand and deck.
* Up to 7 cards assigned to player's hand,
* and 4 cards assigned to player's deck.
* This prevents need for shuffling discard pile back into deck,
* which is beyond the scope of this randomized test.
*
* @param:	Pointer to gameState struct
* @param:	Pointer to errorState struct
* @param:	Pointer to kingdomCards array
*
* @return:	None - each player's hand will contain
*			a random amount of cards from the supply,
*			and each player's deck will contain 4 random
*			cards from the supply.
*************************************************/
void randomize(struct gameState * state, struct errorState * error, int * k) {
	int DECKSIZE = 4;
	
	//Iterate through each player, setting their hand, handCount, deck, and deckCount
	for (int i = 0; i < state->numPlayers; i++) {

		//Assign random number for player's hand size and record in errorState struct
		int handNum = rand() % (HANDSIZE + 1);		//Returns 0 to 7
		state->handCount[i] = handNum;
		error->startingHandSize[i] = handNum;

		//Set all cards in player's hand to -1
		//this prevents a random positon from accidentally containing a valid card
		for (int j = 0; j < handNum; j++) {
			state->hand[i][j] = -1;
		}

		//Assign random cards to player's hand
		for (int j = 0; j < handNum; j++) {
			//pick a random card from card array k
			int card = rand() % 10;		//Returns 0 to 9
			state->hand[i][j] = k[card];
		}

		//Assign player's deck and deckCount
		state->deckCount[i] = DECKSIZE;
		for (int j = 0; j < DECKSIZE; j++) {
			//pick a random card rom card array k
			int card = rand() % 10;
			state->deck[i][j] = k[card];
		}
	}
}

/************************************************
* clearDiscard()
*
* Clear all players' discard piles.
*
* @param:	Pointer to gameState struct.
*
* @return:	None - all values in players' discard
*			piles equal -1.
*************************************************/
void clearDiscard(struct gameState * state) {
	for (int i = 0; i < state->numPlayers; i++) {
		//Set all cards in player's discard pile to -1
		for (int j = 0; j < MAX_DECK; j++) {
			state->discard[i][j] = -1;
		}

		//Set player's discardCount to 0
		state->discardCount[i] = 0;
	}
}

/************************************************
* clearDeck()
*
* Clear all players' decks.
*
* @param:	Pointer to gameState struct.
*
* @return:	None - all values in players' discard
*			piles equal -1.
*************************************************/
void clearDeck(struct gameState * state) {
	for (int i = 0; i < state->numPlayers; i++) {
		//Set all cards in player's discard pile to -1
		for (int j = 0; j < MAX_DECK; j++) {
			state->deck[i][j] = -1;
		}

		//Set player's discardCount to 0
		state->deckCount[i] = 0;
	}
}

/************************************************
* checkAllDeckAndDiscardCounts()
*
* Compares deckCounts and discardCounts of all players
* against assumed values.
*
* Requires known deckCount and DiscardCount for comparison.
* Useful for checking if known discard action performed as
* expected.
*
* @param:	gameState struct.
* @param:	Integer representing assumed deck count.
* @param:	Integer representing assumed discard Count.
*
* @return:	Integer representing if any difference is detected
*			for any player's deckCount or discardCount
*			from the assumed values.
*			1 = Change found; 0 = No change found.
*************************************************/
int checkAllDeckAndDiscardCounts(struct gameState state, int deckCount, int discardCount) {
	for (int i = 0; i < state.numPlayers; i++) {
		if (state.deckCount[i] != deckCount || state.discardCount[i] != discardCount) {
			return 1;
		}
	}

	return 0;
}

/************************************************
* checkPlayerDeckAndDiscardCounts()
*
* Compares deckCounts and discardCounts of specified player
* against assumed values.
*
* Requires known deckCount and DiscardCount for comparison.
* Useful for checking if known discard action performed as
* expected.
*
* @param:	gameState struct.
* @param:	Integer representing assumed deck count.
* @param:	Integer representing assumed discard Count.
* @param:	Integer representing target player.
*
* @return:	Integer representing if any difference is detected
*			in target player's deckCount or discardCount
*			from the assumed values.
*			1 = Change found; 0 = No change found.
*************************************************/
int checkPlayerDeckAndDiscardCounts(struct gameState state, int deckCount, int discardCount, int player) {
	if (state.deckCount[player] != deckCount || state.discardCount[player] != discardCount) {
		return 1;
	}

	return 0;
}

/************************************************
* processResult()
*
* Checks the results of minionAction() function call.
*
* Two possible unique scenarios:
*	1: Player chooses to gain 2 coins
*		> Player should gain 2 coins
*		> no players should discard their cards or draw cards from the deck
*	2: Player chooses to discard hand and draw 4 cards
*		> Player should not gain 2 coins
*		> Player should discard their hand and draw 4 cards from their decks
*		> Any other players with over 4 cards should discard their hands and darw 4 cards from their decks
*
* @param:	Pointer to errorState struct.
* @param:	gameState struct.
*
* @return:	None - any errors from baronAction() is recorded
*			to the errorState struct.
*************************************************/
void processResult(struct gameState state, struct errorState * error) {
	//Analyze gameState struct based on contents of errorState struct
	//Print current test stats to console
	printf("\nChosen Player: %d", error->chosenPlayer);

	printf("\nChose to gain coins: ");
	if (error->choice1 == 1) {
		printf("true");
	}
	else if (error->choice1 == 0) {
		printf("false");
	}
	else {
		printf("error");
	}

	printf("\nChose to discard hand: ");
	if (error->choice2 == 1) {
		printf("true");
	}
	else if (error->choice2 == 0) {
		printf("false");
	}
	else {
		printf("error");
	}

	printf("\nPlayers that should discard: ");
	for (int i = 0; i < state.numPlayers; i++) {
		printf("\n\tPlayer %d: ", i);
		printf("%d cards in hand - ", error->startingHandSize[i]);
		if (error->startingHandSize[i] > 4) {
			printf("true");
		}
		else if (error->startingHandSize[i] <= 4) {
			printf("false");
		}
		else {
			printf("error");
		}
	}

	printf("\n");

	//Player chooses to gain 2 coins -> check that no cards were discarded or drawn
	if (error->choice1 == 1) {
		//Check that coins increased by 2 and no discard actions were performed
		int discardDetected = checkAllDeckAndDiscardCounts(state, 4, 0);
		if (state.coins != 2 || discardDetected != 0) {
			if (state.coins != 2) {
				printf("\nError: Incorrect number of coins.\n\tExpected: 2\n\tActual: %d\n", state.coins);
			}
			if (discardDetected != 0) {
				printf("\nError: Changes to players' deck counts and/or discard pile counts detected");
			}
			error->gainCoins_error++;
		}
		else {
			printf("Test Passed.");
			error->gainCoins_success++;
		}
	}

	//Player chooses to discard hand and draw 4 cards
	//Check that this action was performed for the player,
	//and any other player with 5 or more cards in their hand.
	else if (error->choice2 == 1) {
		//Check that player discarded hand and drew 4 cards from deck

		//Discard should contain an amount of cards equal to the player's original hand size,
		//and because deck size is 4, after drawing from deck 4 times deck size should be 0.
		//discardSuccessful should equal 0 after this function call if the discard happens as planned.
		int invokerPlayerDiscardSuccessful = checkPlayerDeckAndDiscardCounts(state, 0, error->startingHandSize[error->chosenPlayer], error->chosenPlayer);
		if (invokerPlayerDiscardSuccessful != 0) {
			printf("\nError: Unexpected discardCount and deckCount values detected for invoking player.");
			printf("\n\tExpected:\n\t\tdeckCount: 0\n\t\tdiscardCount: %d\n\tActual:\n\t\tdeckCount: %d\n\t\tdiscardCount: %d",
				error->startingHandSize[error->chosenPlayer], state.deckCount[error->chosenPlayer], state.discardCount[error->chosenPlayer]);
			error->discard_error++;
		}

		//Check all other players -> did they discard and draw 4 cards if starting hand is larger than 4?
		else {
			int otherPlayerDiscardError = 0;
			for (int i = 0; i < state.numPlayers; i++) {
				if (i != error->chosenPlayer) {
					//_yes = 1 if player's deck and discards match expected of a player who discarded their hand and drew 4 cards
					int otherPlayerDiscard_yes = checkPlayerDeckAndDiscardCounts(state, 0, error->startingHandSize[i], i);

					//_no = 1 if player's deck and discards match expected of a player who did not discard and draw -> had less than 5 starting cards
					int otherPlayerDiscard_no = checkPlayerDeckAndDiscardCounts(state, 4, 0, i);

					if ((otherPlayerDiscard_yes == 1 && error->startingHandSize[i] > 4) || (otherPlayerDiscard_no == 1 && error->startingHandSize[i] <= 4)) {
						otherPlayerDiscardError = 1;
					}
				}
			}

			if (otherPlayerDiscardError == 1) {
				printf("\nError: Unexpected discardCount and deckCount values detected for a player other than the card invoker.");
				error->discard_error++;
			}

			else if (otherPlayerDiscardError == 0) {
				printf("\nTest Passed.");
				error->discard_success++;
			}
		}
	}

	//Player doesn't choose either choice 1 or 2 -> then nothing happens?
	else {
		printf("\nPlayer did not choose either choice1 or choice2. Nothing happens.");
		error->nothing_happens++;
	}
}

/************************************************
* analyzeErrors()
*
* Prints analysis of aggregate test variables
*
* @param:	Pointer to errorState struct.
*
* @return:	None - analysis printed to console.
*************************************************/
void analyzeErrors(struct errorState error) {
	printf("\n\nAnalyzing Results...\n");

	printf("\nScenario 1: Player chooses to gain 2 coins.\n");
	printf("\tTotal Tests Run: %d\n", error.gainCoins_error + error.gainCoins_success);
	printf("\tSuccessful Tests: %d\n", error.gainCoins_success);
	printf("\tFailed Tests: %d\n", error.gainCoins_error);

	printf("\nScenario 2: Player chooses to discard hand and draw 4 cards.\n");
	printf("\tTotal Tests Run: %d\n", error.discard_error + error.discard_success);
	printf("\tSuccessful Tests: %d\n", error.discard_success);
	printf("\tFailed Tests: %d\n", error.discard_error);

	printf("\nScenario 3: Player chooses neither choice1 or choice2 - nothing happens.\n");
	printf("\tTotal Tests Run: %d\n", error.nothing_happens);
}

int main() {
	srand(time(NULL));
	struct gameState game;
	int k[10];

	struct errorState error;
	memset(&error, 23, sizeof(struct errorState));

	//Initialize test tracking variables
	error.gainCoins_error = 0;
	error.gainCoins_success = 0;

	error.discard_error = 0;
	error.discard_success = 0;

	error.nothing_happens = 0;

	//Main testing loop
	for (int i = 0; i < 200; i++) {
		//Reset contents of errorState struct
		resetError(&error);

		printf("\nTest %d:\n", i + 1);

		//generate randomized supply
		setSupply(k);

		//pick random number of players - 2 to 4
		int numPlayers = (rand() % 3) + 2;

		//Initialize randomized gameState
		memset(&game, 23, sizeof(struct gameState));
		int r = initializeGame(numPlayers, k, 13, &game);
		game.coins = 0;
		game.numActions = 0;
		clearDiscard(&game);
		clearDeck(&game);
		randomize(&game, &error, k);

		//Choose random player to use in minionAction() call
		int randomPlayer = rand() % game.numPlayers;	//Returns 0 to numPlayers - 1
		error.chosenPlayer = randomPlayer;

		//Choose random choice values to use in minionAction() call
		int randomChoice1 = rand() % 2;
		error.choice1 = randomChoice1;

		int randomChoice2 = rand() % 2;
		error.choice2 = randomChoice2;

		//Call function and process results
		minionAction(randomPlayer, randomChoice1, randomChoice2, &game);
		processResult(game, &error);
	}

	//Analyze aggregate test results
	analyzeErrors(error);

	return 0;
}