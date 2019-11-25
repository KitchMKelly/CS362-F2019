/************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/10/2019
* Description:	unittest5.c tests the implementation
*				of the dominion.c function mineAction().
*				Constructs the game state and
*				calls mineAction(). The success/failure
*				of each test is printed to the console.
************************************************/

#include "dominion.h"
#include <stdio.h> 
#include <stdlib.h>

# define nothing -1

/************************************************
* setState()
*
* Sets the state of the gameState struct to
* the ready state for testing mineAction().
*
* @param:	Pointer to gameState struct
* @param:	Pointer to integer array
* @param:	Integer representing the first tributed card
* @param:	Integer representing the second tributed card
* @param:	Integer representing where tributed cards should initially be placed
*			-1 = both in discard
*			 0 = one in discard and one in deck
*			 1 = both in deck
*
* @return:	None - gameState set to starting state
************************************************/
void setState(struct gameState *state, int * k, int cardToPlace) {
	//because this function only affects player0 hand, only need to initialize player0 hand
	int j;
	memset(state, 23, sizeof(struct gameState));
	int r = initializeGame(2, k, 13, state);

	//set player 0 hand
	//the only card should be the target card to swap
	memset(state->hand[0], -1, sizeof(int) * MAX_DECK);
	state->hand[0][0] = cardToPlace;
	state->handCount[0] = 1;

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
		//return -3 if deckCheck assignment error
		return -3;
	}
	}

	int counter = 0;
	for (j = 0; j < MAX_DECK; j++) {
		if (deck[j] != -1) {
			if (deck[j] < 0 || deck[j] > 26) {
				//return -2 if invalid card value found
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

void checkMineAction(struct gameState state, int cardPlaced) {
	//outcome depends on cardPlaced
	switch (cardPlaced) {
	case copper: {
		//hand should contain only silver
		if (state.deck[0][0] != silver) {
			printf("Error: Player 0 hand does not contan silver card\n");
		}
		else {
			printf("Passed: Player 0 hand contains silver card\n");
		}

		//check trash pile for trashed card
		if (state.trash[state.trashedCardCount - 1] != copper) {
			printf("Error: Copper card not added to trash pile\n");
		}
		else {
			printf("Passed: Copper card sent to trash pile\n");
		}
		break;
	}
	case silver: {
		//hand should contain only gold
		if (state.deck[0][0] != gold) {
			printf("Error: Player 0 hand does not contan gold card\n");
		}
		else {
			printf("Passed: Player 0 hand contains gold card\n");
		}

		//check trash pile for trashed card
		if (state.trash[state.trashedCardCount - 1] != silver) {
			printf("Error: Silver card not added to trash pile\n");
		}
		else {
			printf("Passed: Silver card sent to trash pile\n");
		}
		break;
	}
	default: {
		//hand should contain only original card
		if (state.deck[0][0] != cardPlaced) {
			printf("Error: Player 0 hand does not contain original non-silver/non-copper card\n");
		}
		else {
			printf("Passed: Player 0 hand contains original non-silver/non-copper card\n");
		}
		break;
	}
	}
}

int main() {
	/*
	* Possible outcomes:
	*	No silver/copper in hand -> nothing happens
	*	Copper in hand -> becomes silver
	*	Silver in hand -> becomes gold
	*/

	srand(time(0));
	//Declare gameState struct and choose kingdomcards -> must include baron card
	struct gameState state;
	int k[10] = { adventurer, council_room, feast, gardens, mine, remodel, smithy, village, baron, great_hall };

	/*************************************************************
	* Test 1: No silver or copper card in hand - nothing happens
	*************************************************************/
	printf("\n\nmineAction() Test 1: No silver or copper card in hand - nothing happens\n");
	printf("Calling mineAction()...\n");
	setState(&state, k, nothing);
	mineAction(0, silver, 0, &state);
	checkMineAction(state, nothing);

	/*************************************************************
	* Test 2: Copper card in hand - trash for silver
	*************************************************************/
	printf("\n\nmineAction() Test 2: Copper card in hand - trash for silver\n");
	printf("Calling mineAction()...\n");
	setState(&state, k, copper);
	mineAction(0, silver, 0, &state);
	checkMineAction(state, copper);

	/*************************************************************
	* Test 3: Silver card in hand - trash for gold
	*************************************************************/
	printf("\n\nmineAction() Test 3: Silver card in hand - trash for gold\n");
	printf("Calling mineAction()...\n");
	setState(&state, k, silver);
	mineAction(0, gold, 0, &state);
	checkMineAction(state, silver);

	return 0;
}