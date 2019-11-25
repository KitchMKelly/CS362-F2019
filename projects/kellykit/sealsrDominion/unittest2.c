/************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/10/2019
* Description:	unittest2.c tests the implementation
*				of the dominion.c function minionAction().
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
* @param:	Integer representing the number of cards
*			in each player's hands.
*
* @return:	None - gameState set to starting state
************************************************/
void setState(struct gameState *state, int * k, int numCards) {
	int j;
	memset(state, 23, sizeof(struct gameState));
	int r = initializeGame(2, k, 13, state);

	//set player decks -> both should have 4 cards
	memset(state->deck[0], -1, sizeof(int) * MAX_DECK);
	memset(state->deck[1], -1, sizeof(int) * MAX_DECK);
	for (j = 0; j < 4; j++) {
		int randCard = rand() % 10;
		state->deck[0][j] = k[randCard];

		randCard = rand() % 10;
		state->deck[1][j] = k[randCard];
	}
	state->deckCount[0] = 4;
	state->deckCount[1] = 4;

	//set player hands -> both have number of cards equal to numCards
	memset(state->hand[0], -1, sizeof(int) * MAX_DECK);
	memset(state->hand[1], -1, sizeof(int) * MAX_DECK);
	for (j = 0; j < numCards; j++) {
		int randCard = rand() % 10;
		state->hand[0][j] = k[randCard];

		randCard = rand() % 10;
		state->hand[1][j] = k[randCard];
	}
	state->handCount[0] = numCards;
	state->handCount[1] = numCards;

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

	//player does not need to be random -> this function will function the same
	//for all players->just ensure that player number is valid
	//Since player doesn't matter, all tests will be done using player 0

	int j;
	printf("Beginning Testing of minionAction() function...\n\n");

	/*********************************************************************************
	* Test 1: Player chooses to gain coins *
	*********************************************************************************/
	printf("Test 1: Player chooses to gain coins\n");
	setState(&state, k, 7);		//Arbitrary number of cards in player hands -> no discard/drawing should be done

	//Call minionAction function
	//choice1 set to 1 -> player gains 2 coins
	//choice2 set to 1 -> can be either 1 or 0, it is not used for this scenario
	printf("\nCalling minionAction()...\n");
	minionAction(0, 1, 1, &state);

	//check +1 action
	if (state.numActions == 1) {
		printf("Passed: Number of actions equals expected value - should be 1\n");
	}
	else {
		printf("Error: Unexpected number of actions\n\tExpected: 1; Actual: %d\n", state.numActions);
	}

	//check +2 coins
	if (state.coins == 2) {
		printf("Passed: Number of coins equals expected value - should be 2\n");
	}
	else {
		printf("Error: Unexpected number of coins\n\tExpected: 2; Actual: %d\n", state.coins);
	}

	//check player decks
	for (j = 0; j < 2; j++) {
		int result = checkDeck(state, 4, j, 2);
		switch (result) {
		case -3: {
			printf("Error in checkDeck function\n");
			break;
		}
		case -2: {
			printf("Error invalid card value found in player %d's deck\n", j);
			break;
		}
		case -1: {
			printf("Error: Unexpected number of cards in player %d's deck\n\tExpected: 4; Actual: %d\n", j, state.deckCount[j]);
			break;
		}
		case 0: {
			printf("Passed: Number of cards in player %d's deck equals expected value - should be 4\n", j);
			break;
		}
		default: {
			printf("Error: Player %d's deck count does not match actual number of cards in deck", j);
			printf("\n\tDeck Count: %d; Number of Cards Found: %d", state.deckCount[j], result);
			break;
		}
		}
	}

	//check player discard piles
	for (j = 0; j < 2; j++) {
		int result = checkDeck(state, 0, j, 3);
		switch (result) {
		case -3: {
			printf("Error in checkDeck function\n");
			break;
		}
		case -2: {
			printf("Error invalid card value found in player %d's deck\n", j);
			break;
		}
		case -1: {
			printf("Error: Unexpected number of cards in player %d's discard pile\n\tExpected: 0; Actual: %d\n", j, state.discardCount[j]);
			break;
		}
		case 0: {
			printf("Passed: Number of cards in player %d's discard pile equals expected value - should be 0\n", j);
			break;
		}
		default: {
			printf("Error: Player %d's discard count does not match actual number of cards in discard pile", j);
			printf("\n\tDiscard Count: %d; Number of Cards Found: %d", state.discardCount[j], result);
			break;
		}
		}
	}

	/*********************************************************************************
	* Test 2: Player chooses to discard and other player has more than 4 cards in their hand
	*********************************************************************************/
	printf("\nTest 2: Player chooses to discard and other player has more than 4 cards in their hand\nBoth should discard and draw\n");
	setState(&state, k, 7);		//Arbitrary number of cards in player hands -> no discard/drawing should be done

	//Call minionAction function
	//choice1 set to 0 -> player does not gain 2 coins
	//choice2 set to 1 -> player discards
	printf("\nCalling minionAction()...\n");
	minionAction(0, 0, 1, &state);

	//check +1 action
	if (state.numActions == 1) {
		printf("Passed: Number of actions equals expected value - should be 1\n");
	}
	else {
		printf("Error: Unexpected number of actions\n\tExpected: 1; Actual: %d\n", state.numActions);
	}

	//check +2 coins
	if (state.coins == 0) {
		printf("Passed: Number of coins equals expected value - should be 0\n");
	}
	else {
		printf("Error: Unexpected number of coins\n\tExpected: 0; Actual: %d\n", state.coins);
	}

	//check player decks
	for (j = 0; j < 2; j++) {
		int result = checkDeck(state, 0, j, 2);
		switch (result) {
		case -3: {
			printf("Error in checkDeck function\n");
			break;
		}
		case -2: {
			printf("Error invalid card value found in player %d's deck\n", j);
			break;
		}
		case -1: {
			printf("Error: Unexpected number of cards in player %d's deck\n\tExpected: 0; Actual: %d\n", j, state.deckCount[j]);
			break;
		}
		case 0: {
			printf("Passed: Number of cards in player %d's deck equals expected value - should be 0\n", j);
			break;
		}
		default: {
			printf("Error: Player %d's deck count does not match actual number of cards in deck", j);
			printf("\n\tDeck Count: %d; Number of Cards Found: %d\n", state.deckCount[j], result);
			break;
		}
		}
	}

	//check player discard piles
	for (j = 0; j < 2; j++) {
		int result = checkDeck(state, 7, j, 3);
		switch (result) {
		case -3: {
			printf("Error in checkDeck function\n");
			break;
		}
		case -2: {
			printf("Error invalid card value found in player %d's deck\n", j);
			break;
		}
		case -1: {
			printf("Error: Unexpected number of cards in player %d's discard pile\n\tExpected: 7; Actual: %d\n", j, state.discardCount[j]);
			break;
		}
		case 0: {
			printf("Passed: Number of cards in player %d's discard pile equals expected value - should be 7\n", j);
			break;
		}
		default: {
			printf("Error: Player %d's discard count does not match actual number of cards in discard pile", j);
			printf("\n\tDiscard Count: %d; Number of Cards Found: %d\n", state.discardCount[j], result);
			break;
		}
		}
	}

	/*********************************************************************************
	* Test 3: Player chooses to discard and other player has less than 5 cards in their hand
	*********************************************************************************/
	printf("\nTest 3: Player chooses to discard and other player has less than 5 cards in their hand\nOnly player 0 should discard and draw\n");
	setState(&state, k, 7);		//Arbitrary number of cards in player hands -> no discard/drawing should be done

	//Call minionAction function
	//choice1 set to 0 -> player does not gain 2 coins
	//choice2 set to 1 -> player discards
	printf("\nCalling minionAction()...\n");
	minionAction(0, 0, 1, &state);

	//check +1 action
	if (state.numActions == 1) {
		printf("Passed: Number of actions equals expected value - should be 1\n");
	}
	else {
		printf("Error: Unexpected number of actions\n\tExpected: 1; Actual: %d\n", state.numActions);
	}

	//check +2 coins
	if (state.coins == 0) {
		printf("Passed: Number of coins equals expected value - should be 0\n");
	}
	else {
		printf("Error: Unexpected number of coins\n\tExpected: 0; Actual: %d\n", state.coins);
	}

	//check player decks
	//check player 0 deck -> should have 0 cards in deck after drawing all 4 cards
	j = 0;
	int result = checkDeck(state, 0, j, 2);
	switch (result) {
	case -3: {
		printf("Error in checkDeck function\n");
		break;
	}
	case -2: {
		printf("Error invalid card value found in player %d's deck\n", j);
		break;
	}
	case -1: {
		printf("Error: Unexpected number of cards in player %d's deck\n\tExpected: 0; Actual: %d\n", j, state.deckCount[j]);
		break;
	}
	case 0: {
		printf("Passed: Number of cards in player %d's deck equals expected value - should be 0\n", j);
		break;
	}
	default: {
		printf("Error: Player %d's deck count does not match actual number of cards in deck", j);
		printf("\n\tDeck Count: %d; Number of Cards Found: %d\n", state.deckCount[j], result);
		break;
	}
	}

	//check player 1 deck -> should have 4 cards (initial amount) left in deck
	j = 1;
	result = checkDeck(state, 4, j, 2);
	switch (result) {
	case -3: {
		printf("Error in checkDeck function\n");
		break;
	}
	case -2: {
		printf("Error invalid card value found in player %d's deck\n", j);
		break;
	}
	case -1: {
		printf("Error: Unexpected number of cards in player %d's deck\n\tExpected: 4; Actual: %d\n", j, state.deckCount[j]);
		break;
	}
	case 0: {
		printf("Passed: Number of cards in player %d's deck equals expected value - should be 4\n", j);
		break;
	}
	default: {
		printf("Error: Player %d's deck count does not match actual number of cards in deck", j);
		printf("\n\tDeck Count: %d; Number of Cards Found: %d\n", state.deckCount[j], result);
		break;
	}
	}

	//check player discard piles
	//check player 1 discard pile -> should be 7 after discarding initial hand
	j = 0;
	result = checkDeck(state, 7, j, 3);
	switch (result) {
	case -3: {
		printf("Error in checkDeck function\n");
		break;
	}
	case -2: {
		printf("Error invalid card value found in player %d's deck\n", j);
		break;
	}
	case -1: {
		printf("Error: Unexpected number of cards in player %d's discard pile\n\tExpected: 7; Actual: %d\n", j, state.discardCount[j]);
		break;
	}
	case 0: {
		printf("Passed: Number of cards in player %d's discard pile equals expected value - should be 7\n", j);
		break;
	}
	default: {
		printf("Error: Player %d's discard count does not match actual number of cards in discard pile", j);
		printf("\n\tDiscard Count: %d; Number of Cards Found: %d\n", state.discardCount[j], result);
		break;
	}
	}

	//check player 2 discard pile -> should be 0
	j = 1;
	result = checkDeck(state, 0, j, 3);
	switch (result) {
	case -3: {
		printf("Error in checkDeck function\n");
		break;
	}
	case -2: {
		printf("Error invalid card value found in player %d's deck\n", j);
		break;
	}
	case -1: {
		printf("Error: Unexpected number of cards in player %d's discard pile\n\tExpected: 0; Actual: %d\n", j, state.discardCount[j]);
		break;
	}
	case 0: {
		printf("Passed: Number of cards in player %d's discard pile equals expected value - should be 0\n", j);
		break;
	}
	default: {
		printf("Error: Player %d's discard count does not match actual number of cards in discard pile", j);
		printf("\n\tDiscard Count: %d; Number of Cards Found: %d\n", state.discardCount[j], result);
		break;
	}
	}

	/*********************************************************************************
	* Test 4: Player chooses neither option - increase actions and that's it
	*********************************************************************************/
	printf("\nTest 4: Player chooses neither option\nIncrease actions and that's it\n");
	setState(&state, k, 7);		//Arbitrary number of cards in player hands -> no discard/drawing should be done

	//Call minionAction function
	//choice1 set to 0 -> player does not gain 2 coins
	//choice2 set to 1 -> player discards
	printf("\nCalling minionAction()...\n");
	minionAction(0, 0, 0, &state);

	//check +1 action
	if (state.numActions == 1) {
		printf("Passed: Number of actions equals expected value - should be 1\n");
	}
	else {
		printf("Error: Unexpected number of actions\n\tExpected: 1; Actual: %d\n", state.numActions);
	}

	//check +2 coins
	if (state.coins == 0) {
		printf("Passed: Number of coins equals expected value - should be 0\n");
	}
	else {
		printf("Error: Unexpected number of coins\n\tExpected: 0; Actual: %d\n", state.coins);
	}

	//check player decks
	for (j = 0; j < 2; j++) {
		int result = checkDeck(state, 4, j, 2);
		switch (result) {
		case -3: {
			printf("Error in checkDeck function\n");
			break;
		}
		case -2: {
			printf("Error invalid card value found in player %d's deck\n", j);
			break;
		}
		case -1: {
			printf("Error: Unexpected number of cards in player %d's deck\n\tExpected: 0; Actual: %d\n", j, state.deckCount[j]);
			break;
		}
		case 0: {
			printf("Passed: Number of cards in player %d's deck equals expected value - should be 0\n", j);
			break;
		}
		default: {
			printf("Error: Player %d's deck count does not match actual number of cards in deck", j);
			printf("\n\tDeck Count: %d; Number of Cards Found: %d\n", state.deckCount[j], result);
			break;
		}
		}
	}

	//check player discard piles
	for (j = 0; j < 2; j++) {
		int result = checkDeck(state, 0, j, 3);
		switch (result) {
		case -3: {
			printf("Error in checkDeck function\n");
			break;
		}
		case -2: {
			printf("Error invalid card value found in player %d's deck\n", j);
			break;
		}
		case -1: {
			printf("Error: Unexpected number of cards in player %d's discard pile\n\tExpected: 7; Actual: %d\n", j, state.discardCount[j]);
			break;
		}
		case 0: {
			printf("Passed: Number of cards in player %d's discard pile equals expected value - should be 7\n", j);
			break;
		}
		default: {
			printf("Error: Player %d's discard count does not match actual number of cards in discard pile", j);
			printf("\n\tDiscard Count: %d; Number of Cards Found: %d\n", state.discardCount[j], result);
			break;
		}
		}
	}
	return 0;
}