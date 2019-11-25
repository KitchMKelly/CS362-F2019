/************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/10/2019
* Description:	unittest1.c tests the implementation
*				of the dominion.c function baronAction().
*				Constructs the game state, randomizes 
*				the player's hand and choice 1 input,
*				and calls baronAction(). The 
*				success/failure of each test is printed to the console.
************************************************/

#include "dominion.h"
#include <stdio.h> 
#include <stdlib.h> 

/************************************************
* setState()
* 
* Sets the state of the gameState struct to
* the test start state.
*
* @param:	Pointer to gameState struct
* @param:	Pointer to integer array

* @return:	None - gameState set to starting state
************************************************/
void setState(struct gameState *state, int * k) {
	memset(state, 23, sizeof(struct gameState));
	int r = initializeGame(2, k, 13, state);
	memset(state->deck[0], 0, sizeof(int) * MAX_DECK);
	state->deckCount[0] = 0;
	state->numBuys = 0;
	state->coins = 0;
}

/************************************************
* setHand()
*
* Sets the player's hand to a state where
* baronAction() can be successfully called.
*
* @param:	Pointer to gameState struct
* @param:	Pointer to integer array
* @param:	Integer representing if estate card 
*			should be added to hand.
*
* @return:	None - player hand set to ready state
************************************************/
void setHand(struct gameState * state, int * k, int estateFlag) {
	int j;
	//Set player's current hand -> hand size restricted to 7 cards max
	//First test includes estate card in player's hand
	//Set all cards to blank value (-1)
	for (j = 0; j < 7; j++) {
		state->hand[0][j] = -1;
	}

	//Determine if estate card should be included
	if (estateFlag) {
		//Choose random position for estate card
		int estatePos = rand() % 7;
		state->hand[0][estatePos] = estate;
	}
	
	//Fill in other hand position values -> all kingdom cards from k array
	for (j = 0; j < 7; j++) {
		if (state->hand[0][j] == -1) {
			int randCard = rand() % 10;
			state->hand[0][j] = k[randCard];
		}
	}
	state->handCount[0] = 7;
}

int main() {
	srand(time(0));
	//Declare gameState struct and choose kingdomcards -> must include baron card
	struct gameState state;
	int k[10] = { adventurer, council_room, feast, gardens, mine, remodel, smithy, village, baron, great_hall };

	//player does not need to be random -> this function will function the same
	//for all players->just ensure that player number is valid
	//Since player doesn't matter, all tests will be done using player 0

	int j = 0;
	printf("Beginning Testing of baronAction() function...\n\n");

	/*********************************************************************************
	* Test 1: Estate card in player's hand and player chooses to discard estate card *
	*********************************************************************************/

	printf("Test 1: Estate card in player's hand and player chooses to discard estate card\n");
	setState(&state, k);
	setHand(&state, k, 1);				//Sets hand with estate card included
	
	//Call baronAction function
	//choice set to 1 -> player chooses to discard estate card
	printf("Calling baronAction()...\n");
	baronAction(1, &state, 0);

	//Check that estate card was removed from hand,
	//check that estate card is in player's discard,
	//check that player's hand size decremented,
	//check that numBuys = 1 and coins = 4

	//Because test will fail, use if statements to show full branch coverage
	//as opposed to assert calls, which will terminate the test
	printf("Checking results...\n\n");
	if (state.numBuys != 1) {
		printf("Error: numBuys is incorrect - should be 1\n");
	}
	else {
		printf("Passed: Correct value of numBuys - should be 1\n");
	}
	if (state.coins != 4) {
		printf("Error: coins is incorrect - should be 4\n");
	}
	else {
		printf("Passed: Correct value of coins - should be 4\n");
	}
	int handSize = 6;
	if (state.handCount[0] != 6) {
		printf("Error: Player's hand size did not decrement - should be 6\n");
		 handSize = 7;
	}
	else {
		printf("Passed: Correct player hand size - should be 6\n");
	}
	int discarded = 0;
	for (j = 0; j < state.discardCount[0]; j++) {
		if (state.discard[0][j] == estate) {
			printf("Passed: Estate card found in player's discard pile\n");
			discarded = 1;
		}
	}
	if (discarded == 0) {
		printf("Error: Estate card not found in player's discarded pile\n");
	}

	int estatePresent = 0;
	for (j = 0; j < handSize; j++) {
		if (state.hand[0][j] == estate) {
			printf("Error: Estate card found in player hand\n\n");
			estatePresent = 1;
		}
	}
	if (estatePresent == 0) {
		printf("Passed: Estate card discarded\n\n");
	}
	
	/*************************************************************************************
	* Test 2: Estate card in player's hand and player chooses not to discard estate card *
	*************************************************************************************/
	printf("Test 2: Estate card in player's hand and player chooses not to discard estate card\n");
	setState(&state, k);
	setHand(&state, k, 1);				//Sets hand with estate card included


	//Call baronAction function
	//choice set to 0 -> player chooses not to discard estate card
	printf("Calling baronAction()...\n");
	baronAction(0, &state, 0);

	//Check that estate card was not removed from hand,
	//check that estate card is in player's discard,
	//check that player's hand size decremented,
	//check that numBuys = 1 and coins = 4
	printf("Checking results...\n\n");
	if (state.numBuys != 1) {
		printf("Error: numBuys is incorrect - should be 1\n");
	}
	else {
		printf("Passed: Correct value of numBuys - should be 1\n");
	}
	if (state.coins == 4) {
		printf("Error: coins is incorrect - should be 0\n");
	}
	else {
		printf("Passed: Correct value of coins - should be 0\n");
	}
	if (state.deckCount[0] != 1) {
		printf("Error: Player's deck size did not increase - should be 1\n");
	}
	else {
		printf("Passed: Correct player deck size - should be 1\n");
	}
	discarded = 0;
	for (j = 0; j < state.discardCount[0]; j++) {
		if (state.discard[0][j] == estate) {
			printf("Error: Estate card found in player's discard pile\n");
			discarded = 1;
		}
	}
	if (discarded == 0) {
		printf("Passed: Estate card not found in player's discarded pile\n");
	}

	estatePresent = 0;
	for (j = 0; j < state.deckCount[0]; j++) {
		if (state.deck[0][j] == estate) {
			estatePresent++;
		}
	}
	if (estatePresent == 1) {
		
		printf("Passed: One estate card found in player deck\n\n");
	}
	else {
		printf("Error: Incorrect number of estate cards found in player deck\n\n");
	}

	/*************************************************************************************
	* Test 3: Estate card not in player's hand and player chooses to discard estate card *
	*************************************************************************************/
	printf("Test 3: Estate card not in player's hand.\n");
	printf("Choice does not affect function outcome.\n");
	setState(&state, k);
	setHand(&state, k, 0);				//Sets hand without estate card included

	//Call baronAction function
	//choice set to 1 -> player chooses to discard estate card
	printf("Calling baronAction()...\n");
	baronAction(1, &state, 0);

	//Check that estate card was not removed from hand,
	//check that estate card is in player's discard,
	//check that player's hand size decremented,
	//check that numBuys = 1 and coins = 4
	printf("Checking results...\n\n");
	if (state.numBuys != 1) {
		printf("Error: numBuys is incorrect - should be 1\n");
	}
	else {
		printf("Passed: Correct value of numBuys - should be 1\n");
	}
	if (state.coins == 4) {
		printf("Error: coins is incorrect - should be 0\n");
	}
	else {
		printf("Passed: Correct value of coins - should be 0\n");
	}
	if (state.deckCount[0] != 1) {
		printf("Error: Player's deck size did not increase - should be 1\n");
	}
	else {
		printf("Passed: Correct player deck size - should be 1\n");
	}
	discarded = 0;
	for (j = 0; j < state.discardCount[0]; j++) {
		if (state.discard[0][j] == estate) {
			printf("Error: Estate card found in player's discard pile\n");
			discarded = 1;
		}
	}
	if (discarded == 0) {
		printf("Passed: Estate card not found in player's discarded pile\n");
	}

	estatePresent = 0;
	for (j = 0; j < state.deckCount[0]; j++) {
		if (state.deck[0][j] == estate) {
			estatePresent++;
		}
	}
	if (estatePresent == 1) {

		printf("Passed: One estate card found in player deck\n");
	}
	else {
		printf("Error: Incorrect number of estate cards found in player deck\n");
	}

	return 0;
}