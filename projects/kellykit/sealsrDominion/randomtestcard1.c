/****************************************************************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/16/2019
* Description:	randomtestcard1.c tests the implementation of the dominion.c function baronAction().
*				Constructs the game state, randomizes the player's hand and choice 1 input,
*				and calls baronAction(). The success/failure of each test is printed to the console.
*				//TODO: switch to printing to randomTestRestults.out
****************************************************************************************************/

#include "dominion.h"
#include <stdio.h> 
#include <stdlib.h> 

# define HANDSIZE 7

struct errorState {
	
	//Current test variables

	int playerHand[HANDSIZE];
	int estateFlag[MAX_PLAYERS];
	int choice1;
	int chosenPlayer;

	//Total test variables

	//Player chooses to discard estate,
	//and has estate success/error trackers
	int yesDiscard_hasEstate_error;
	int yesDiscard_hasEstate_success;

	//Player chooses to discard estate,
	//and doesn't have estate success/error trackers
	int yesDiscard_noEstate_error;
	int yesDiscard_noEstate_success;

	//Player chooses not to discard estate success/error trackers
	int noDiscard_error;
	int noDiscard_success;

	//How many possible error messages can there be?
	char * errorMessage;
};

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
* Randomly assigns the cards in each player's hand.
* 50% chance to contain estate card - the card in
* question for this test.
*
* @param:	Pointer to gameState struct
* @param:	Pointer to kingdomCards array
*
* @return:	None - each player's hand will contain
*			a random amount of cards from the supply.
*************************************************/
void randomize(struct gameState * state, int * k) {

	//Iterate through each player, setting their hand and handCount
	for (int i = 0; i < state->numPlayers; i++) {

		//Assign random number for player's hand size
		int handNum = rand() % (HANDSIZE + 1);		//Returns 0 to 7
		state->handCount[i] = handNum;

		//Set all cards in player's hand to -1
		//this prevents a random positon from accidentally containing estate (AKA 1)
		for (int j = 0; j < handNum; j++) {
			state->hand[i][j] = -1;
		}

		//Determine if player's hand will contain estate card or not
		//Skip if player has no cards in their hand
		if (handNum > 0) {
			int estateFlag = rand() % 2;				//Returns 0 or 1
			if (estateFlag == 1) {

				//Determine random position of estate card
				int estatePos = rand() % handNum;	//Returns 0 to handNum - 1
				state->hand[i][estatePos] = estate;
			}
		}

		//Assign random cards to player's hand
		for (int j = 0; j < handNum; j++) {

			//skip position if it has already been assigned to estate
			if (state->hand[i][j] == estate) {
				continue;
			}

			//pick a random card from card array k
			int card = rand() % 10;		//Returns 0 to 9
			state->hand[i][j] = k[card];
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
* resetError()
*
* Resets all values of errorState struct.
*
* @param:	Pointer to errorState struct.
*
* @return:	None - errorState values reset
*************************************************/
void resetError(struct errorState * error) {
	error->choice1 = -1;
	error->errorMessage = NULL;
	error->errorMessage = NULL;
	error->chosenPlayer = -1;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		error->estateFlag[i] = -1;
	}
	for (int i = 0; i < HANDSIZE; i++) {
		error->playerHand[i] = -1;
	}
}

/************************************************
* setErrorPlayerHand()
*
* Initalizes errorState struct's variable playerHand
* with the values from selected player's hand.
*
* @param:	Pointer to errorState struct.
* @param:	gameState struct.
* @param:	Integer representing chosen player.
*
* @return:	None - errorState's playerHand array
*			set to values from chosen player's hand.
*************************************************/
void setErrorPlayerHand(struct errorState * error, struct gameState state, int chosenPlayer) {
	for (int i = 0; i < state.handCount[chosenPlayer]; i++) {
		error->playerHand[i] = state.hand[chosenPlayer][i];
	}
}

/************************************************
* checkDiscard()
*
* Checks if a given card is in a player's discard pile.
*
* @param:	Pointer to gameState struct.
* @param:	Integer representing the selected player.
* @param:	Integer representing target card to search for.
*
* @return:	Integer representing presence/absence
*			of target card. Returns 1 if target card
*			is found in given player's discard pile,
*			and returns 0 otherwise.
*************************************************/
int checkDiscard(struct gameState state, int player, int card) {
	for (int i = 0; i < state.discardCount[player]; i++) {
		if (state.discard[player][i] == card) {
			return 1;
		}
	}

	return 0;
}

/************************************************
* estateCardPresent()
*
* Checks if the chosen player's hand contains the target card.
*
* @param:	errorState struct
*
* @return:	Integer representing presence/absence
*			of target card. Returns 1 if target card
*			is found in given player's hand,
*			and returns 0 otherwise.
*************************************************/
int checkHand(struct errorState error, int card) {
	for (int i = 0; i < HANDSIZE; i++) {
		if (error.playerHand[i] == card) {
			return 1;
		}
	}
	return 0;
}

/************************************************
* processResult()
*
* Checks the results of baronAction() function call.
*
* Three possible unique scenarios:
*	1: Player chooses to discard estate card and has an estate card in their hand
*		> Estate card should move from hand to discard
*		> gameState struct's numCoins should increase by 4 -> from 0 to 4
*	2: Player chooses to discard estate card and does not have an estate card in their hand
*		> Function should determine that the player doesn't have an estate card to discard
*		> Function should tell the player as much -> doesn't discard estate card or incrase numCoins
*		> Estate card should be added to player's discard pile (bug makes it go to deck)
*	3: Player chooses not to discard estate card
*		> Function should jump straight to adding estate card to player's deck
*		> Estate card should be added to player's discard pile (bug makes it go to deck)
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
	printf("\n\tPlayer Hand: ");
	for (int j = 0; j < HANDSIZE; j++) {
		printf("%d ", error->playerHand[j]);
	}

	printf("\n\tEstate Card Present: ");
	int present = checkHand(*error, estate);
	if (present == 1) {
		printf("true");
	}
	else if (present == 0) {
		printf("false");
	}
	else {
		printf("error");
	}

	printf("\n\tChose to discard estate card: ");
	if (error->choice1 == 1) {
		printf("true");
	}
	else if (error->choice1 == 0) {
		printf("false");
	}
	else {
		printf("error");
	}
	printf("\n");

	//Player chooses to discard estate card
	if (error->choice1 == 1) {
		if (present == 1) {
			//Player chooses to discard estate card and has an estate card to discard in their hand
			//Estate card should move from hand to discard
			//gameState struct's numCoins should increase by 4 -> from 0 to 4
			if (state.coins != 4 || checkDiscard(state, error->chosenPlayer, estate) != 1) {
				if (state.coins != 4) {
					printf("\nError: Incorrect number of coins.\n\tExpected: 4\n\tActual: %d\n", state.coins);
				}
				if (checkDiscard(state, error->chosenPlayer, estate) != 1) {
					printf("\nError: Estate card not discarded to player's discard pile.\n");
				}
				error->yesDiscard_hasEstate_error++;
			}
			else {
				printf("\nTest Pass.\n");
				error->yesDiscard_hasEstate_success++;
			}
		}
		else {
			//Player chooses to discard estate card and does not have an estate card to discard in their hand
			//Function should determine that the player doesn't have an estate card to discard
			//Function should tell the player as much -> doesn't discard estate card or incrase numCoins
			//Estate card should be added to player's discard pile (bug makes it go to deck)
			if (checkDiscard(state, error->chosenPlayer, estate) != 1) {
				printf("\nError: Estate card not added to player's discard pile.\n");
				error->yesDiscard_noEstate_error++;
			}
			else {
				printf("\nTest Pass.\n");
				error->yesDiscard_noEstate_success++;
			}
		}
	}
	else {
		//Player chooses not to discard estate card -> presence/absence of estate card should not matter
		//Function should jump straight to adding estate card to player's deck
		//Estate card should be added to player's discard pile (bug makes it go to deck)
		if (checkDiscard(state, error->chosenPlayer, estate) != 1) {
			printf("\nError: Estate card not added to player's discard pile.\n");
			error->noDiscard_error++;
		}
		else {
			printf("\nTest Pass.\n");
			error->noDiscard_success++;
		}
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

	printf("\nScenario 1: Player chooses to discard estate card and has an estate card in their hand.\n");
	printf("\tTotal Tests Run: %d\n", error.yesDiscard_hasEstate_error + error.yesDiscard_hasEstate_success);
	printf("\tSuccessful Tests: %d\n", error.yesDiscard_hasEstate_success);
	printf("\tFailed Tests: %d\n", error.yesDiscard_hasEstate_error);

	printf("\nScenario 2: Player chooses to discard estate card and does not have an estate card in their hand.\n");
	printf("\tTotal Tests Run: %d\n", error.yesDiscard_noEstate_error + error.yesDiscard_noEstate_success);
	printf("\tSuccessful Tests: %d\n", error.yesDiscard_noEstate_success);
	printf("\tFailed Tests: %d\n", error.yesDiscard_noEstate_error);

	printf("\nScenario 3: Player chooses not to discard estate card.\n");
	printf("\tTotal Tests Run: %d\n", error.noDiscard_error + error.noDiscard_success);
	printf("\tSuccessful Tests: %d\n", error.noDiscard_success);
	printf("\tFailed Tests: %d\n", error.noDiscard_error);
}

int main() {
	srand(time(NULL));
	struct gameState game;
	int k[10];

	struct errorState error;
	memset(&error, 23, sizeof(struct errorState));

	//Initialize test tracking variables
	error.yesDiscard_hasEstate_error = 0;
	error.yesDiscard_hasEstate_success = 0;

	error.yesDiscard_noEstate_error = 0;
	error.yesDiscard_noEstate_success = 0;

	error.noDiscard_error = 0;
	error.noDiscard_success = 0;

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
		randomize(&game, k);
		clearDiscard(&game);
		
		//Choose random player to use in baronAction() call
		int randomPlayer = rand() % game.numPlayers;	//Returns 0 to numPlayers - 1
		error.chosenPlayer = randomPlayer;

		//Add randomPlayer's hand to errorState struct
		setErrorPlayerHand(&error, game, randomPlayer);

		//Choose random choice to use in baronAction() call
		int randomChoice = rand() % 2;
		error.choice1 = randomChoice;

		//Call function and process results
		baronAction(randomChoice, &game, randomPlayer);
		processResult(game, &error);
	}

	//Analyze aggregate test results
	analyzeErrors(error);

	return 0;
}