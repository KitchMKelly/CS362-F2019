/************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/10/2019
* Description:	unittest3.c tests the implementation
*				of the dominion.c function ambassadorAction().
*				Constructs the game state, randomizes
*				the player's hand, choice 1, and choice2 input,
*				and calls minionAction(). The
*				success/failure of each test is printed to the console.
************************************************/

#include "dominion.h"
#include <stdio.h> 
#include <stdlib.h> 

/************************************************
* setState()
*
* Sets the state of the gameState struct to
* the ready state for testing minionAction().
*
* @param:	Pointer to gameState struct
* @param:	Pointer to integer array
* @param:	Integer representing the number of copies
*			of target card in target player's hand.
*
* @return:	None - gameState set to starting state
************************************************/
void setState(struct gameState *state, int * k, int numTargetCards) {
	int target = adventurer;
	int j;
	memset(state, 23, sizeof(struct gameState));
	int r = initializeGame(2, k, 13, state);

	//set player decks
	memset(state->deck[0], -1, sizeof(int) * MAX_DECK);
	memset(state->deck[1], -1, sizeof(int) * MAX_DECK);
	state->deckCount[0] = 0;
	state->deckCount[1] = 0;

	//set player hands
	//target player's hand will contain target cards equal to numTargetCards
	memset(state->hand[0], -1, sizeof(int) * MAX_DECK);
	memset(state->hand[1], -1, sizeof(int) * MAX_DECK);

	for (j = 0; j < numTargetCards; j++) {
		state->hand[0][j] = target;
	}
	state->handCount[0] = numTargetCards;
	state->handCount[1] = 0;

	//set player discard piles
	memset(state->discard[0], -1, sizeof(int) * MAX_DECK);
	memset(state->discard[1], -1, sizeof(int) * MAX_DECK);
	state->discardCount[0] = 0;
	state->discardCount[1] = 0;

	state->numActions = 0;
	state->coins = 0;
}

/************************************************
* checkDeck()
*
* Checks that all cards in the deck are valid,
* that the number of cards in the specified
* player's deck matches that deck's count, and that
* the deck count matches the specified expected count.
*
* @param:	Pointer to gameState struct
* @param:	Integer representing expected deck count
* @param:	Integer representing target player
* @param:	Integer representing target deck:
*			1 = hand
*			2 = deck
*			3 = discard pile
*			other = error
*
* @return:	Integer representing findings:
*		-3 = deck assignment error
*		-2 = invalid card value found
*		-1 = deck's counter == actual number of cards in the deck,
*		     but count != expectedCount
*		0 = all is good - expected matches actual # cards in deck and deck counter
*		>=1 = deck's counter != actual number of cards in the deck
*			  returns found number of cards in the deck in this case
************************************************/
int checkDeck(struct gameState state, int expectedCount, int player, int deckToCheck) {
	int j;
	int * deck;
	int  deckCount;
	//set deck and deckCounter to point to either player hand, deck, or discard
	//1 = hand
	//2 = deck
	//3 = discard
	//default = error
	switch (deckToCheck) {
	case 1: {
		deck = state.hand[player];
		deckCount = state.handCount[player];
		break;
	}
	case 2: {
		deck = state.deck[player];
		deckCount = state.deckCount[player];
		break;
	}
	case 3: {
		deck = state.discard[player];
		deckCount = state.discardCount[player];
		break;
	}
	default: {
		return -3;
	}
	}

	int counter = 0;
	for (j = 0; j < MAX_DECK; j++) {
		if (deck[j] != -1) {
			if (deck[j] < 0 || deck[j] > 26) {
				return -2;
			}
			counter++;
		}
	}
	//return actual # cards if deck's counter and actual don't match
	if (counter != deckCount) {
		return counter;
	}

	//return -1 if actual # cards matches deck counter, but not expected
	if (counter != expectedCount) {
		return -1;
	}

	//return 0 if everything matches expected
	return 0;
}

int main() {
	srand(time(0));
	//Declare gameState struct and choose kingdomcards -> must include baron card
	struct gameState state;
	int k[10] = { adventurer, council_room, feast, gardens, mine, remodel, smithy, village, baron, great_hall };
	
	//target card that will be used for these tests
	int target = adventurer;

	//player does not need to be random -> this function will function the same
	//for all players->just ensure that player number is valid
	//Since player doesn't matter, all tests will be done using player 0

	/*Possible outcomes:
		Player chooses a value for choice2 that is out of bounds
		Player chooses a value for choice2 that is greater than the number of target cards in the player's hand
		Player chooses in bounds number for choice2 and has enough copies of choice1 to return to supply
			One copy or two copies
	*/
	int j;
	printf("Beginning Testing of ambassadorAction() function...\n\n");

	/*********************************************************************************
	* Test 1: Player chooses value for choice2 that is out of bounds
	*********************************************************************************/
	printf("Test 1: Player chooses value for choice2 that is out of bounds\n");
	setState(&state, k, 2);		//Two target cards in player hand

	//Call ambassadorAction function
	//choice1 set to our unit test target card
	//choice2 set to 3 -> out of bounds value
	printf("\nCalling ambassadorAction()...\n");
	int result = ambassadorAction(target, 3, 0, &state);
	
	//check value of result
	if (result == -1) {
		printf("Passed: Out of bounds value for choice2 returns error value\n");
	}
	else {
		printf("Error: Unexpected return value\n");
		printf("\tExpected: -1; Actual: %d\n", result);
	}

	/*********************************************************************************
	* Test 2: Player chooses value for choice2 that is greater than the number of target cards in their hand
	*********************************************************************************/
	printf("Test 2: Player chooses value for choice2 that is greater than the number of target cards in their hand\n");
	setState(&state, k, 1);		//One target card in player hand

	//Call ambassadorAction function
	//choice1 set to our unit test target card
	//choice2 set to 3 -> out of bounds value
	printf("\nCalling ambassadorAction()...\n");
	result = ambassadorAction(target, 2, 0, &state);

	//check value of result
	if (result == -1) {
		printf("Passed: Value for choice2 greater than number of target cards in player hand returns error value\n");
	}
	else {
		printf("Error: Unexpected return value\n");
		printf("\tExpected: -1; Actual: %d\n", result);
	}

	/*********************************************************************************
	* Test 3: Player chooses to return max number of target cards,
	*		  and has max number of target cards in their hand
	*********************************************************************************/
	printf("Test 3: Player chooses to return max number of target cards, and has max number of target cards in their hand\n");
	setState(&state, k, 2);		//Two target cards in player hand

	//Call ambassadorAction function
	//choice1 set to our unit test target card
	//choice2 set to 3 -> out of bounds value
	printf("\nCalling ambassadorAction()...\n");
	result = ambassadorAction(target, 2, 0, &state);

	//check value of result
	if (result == 0) {
		printf("Passed: Function returned success value 0\n");
	}
	else {
		printf("Error: Unexpected return value\n");
		printf("\tExpected: 0; Actual: %d\n", result);
	}

	//check that target player hand does not contain target card -> both should be returned to the supply
	int player = 0;
	result = checkDeck(state, 0, player, 1);
	switch (result) {
	case -3: {
		printf("Error in checkDeck function\n");
		break;
	}
	case -2: {
		printf("Error invalid card value found in player %d's hand\n", player);
		break;
	}
	case -1: {
		printf("Error: Unexpected number of cards in player %d's hand\n\tExpected: 0; Actual: %d\n", player, state.handCount[player]);
		break;
	}
	case 0: {
		printf("Passed: Number of cards in player %d's hand equals expected value - should be 0\n", player);
		break;
	}
	default: {
		printf("Error: Player %d's hand count does not match actual number of cards in hand", player);
		printf("\n\tHand Count: %d; Number of Cards Found: %d\n", state.handCount[player], result);
		break;
	}
	}

	//check that other player discard contains one card -> the target card
	player = 1;
	result = checkDeck(state, 1, player, 3);
	switch (result) {
	case -3: {
		printf("Error in checkDeck function\n");
		break;
	}
	case -2: {
		printf("Error invalid card value found in player %d's discard\n", player);
		break;
	}
	case -1: {
		printf("Error: Unexpected number of cards in player %d's discard\n\tExpected: 1; Actual: %d\n", player, state.discardCount[player]);
		break;
	}
	case 0: {
		printf("Passed: Number of cards in player %d's discard equals expected value - should be 1\n", player);
		if (state.discard[player][state.discardCount[player] - 1] == target) {
			printf("Passed: Card in player %d's discard matches the target card - should be 7\n", player);
		}
		else {
			printf("Error: Card in player %d's discard does not match target card\n\tExpected: %d; Actual: %d\n", player, target, state.discard[player][state.discardCount[player] - 1]);
		}
		break;
	}
	default: {
		printf("Error: Player %d's discard count does not match actual number of cards in discard\n", player);
		printf("\n\tDiscard Count: %d; Number of Cards Found: %d\n", state.discardCount[player], result);
		break;
	}
	}

	//check supply count -> initialized to 10, 2 added back, and one taken -> should be 11
	if (state.supplyCount[target] == 11) {
		printf("Passed: Number of cards target card's supply matches expected value - should be 11\n");
	}
	else {
		printf("Error: Unexpected number of cards in target card's supply\n\tExpected: 11; Actual %d\n", state.supplyCount[target]);
	}
	return 0;
}