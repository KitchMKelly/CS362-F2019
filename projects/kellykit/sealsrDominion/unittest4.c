/************************************************
* Author:		Kitch Kelly (kellykit)
* Date:			11/10/2019
* Description:	unittest4.c tests the implementation
*				of the dominion.c function tributeAction().
*				Constructs the game state and
*				calls tributeAction(). The success/failure
*				of each test is printed to the console.
************************************************/

#include "dominion.h"
#include <stdio.h> 
#include <stdlib.h>

//global constants for testing cards
# define action1 adventurer
# define action2 council_room
# define treasure1 silver
# define treasure2 gold
# define victory1 estate
# define victory2 duchy
# define nothing -1

/************************************************
* setState()
*
* Sets the state of the gameState struct to
* the ready state for testing tributeAction().
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
void setState(struct gameState *state, int * k, int tributeCard1, int tributeCard2, int placementFlag) {
	int j;
	memset(state, 23, sizeof(struct gameState));
	int r = initializeGame(2, k, 13, state);

	switch (placementFlag) {
	case -1: {
		//Both cards go in discard pile
		//set player1 deck
		memset(state->deck[1], -1, sizeof(int) * MAX_DECK);
		state->deckCount[1] = 0;

		//set player1 discard pile
		//player1's discard pile contains tributeCard1 and tributeCard2
		memset(state->discard[1], -1, sizeof(int) * MAX_DECK);
		state->discardCount[1] = 0;
		if (tributeCard1 != nothing) {
			state->discard[1][0] = tributeCard1;
			state->discardCount[1]++;
		}
		if (tributeCard2 != nothing) {
			state->discard[1][1] = tributeCard2;
			state->discardCount[1]++;
		}
		break;
	}
	case 0: {
		//tributeCard1 goes in deck and tributeCard2 goes in discard
		//set player1 deck
		//player1's deck contains tributeCard1
		memset(state->deck[1], -1, sizeof(int) * MAX_DECK);
		state->deckCount[1] = 0;
		if (tributeCard1 != nothing) {
			state->deck[1][0] = tributeCard1;
			state->deckCount[1]++;
		}
		
		//set player1 discard pile
		//player1's discard pile contains tributeCard2
		memset(state->discard[1], -1, sizeof(int) * MAX_DECK);
		if (tributeCard2 != nothing) {
			state->discard[1][0] = tributeCard2;
			state->discardCount[1]++;
		}
		break;
	}
	case 1: {
		//Both cards go in deck
		//set player1 deck
		//player1's deck contains tributeCard1 and tributeCard2
		memset(state->deck[1], -1, sizeof(int) * MAX_DECK);
		state->deckCount[1] = 0;
		if (tributeCard1 != nothing) {
			state->deck[1][0] = tributeCard1;
			state->deckCount[1]++;
		}
		if (tributeCard2 != nothing) {
			state->deck[1][1] = tributeCard2;
			state->deckCount[1] = 2;
		}
		
		//set player1 discard pile
		memset(state->discard[1], -1, sizeof(int) * MAX_DECK);
		state->discardCount[1] = 0;
		break;
	}
	}
	//set player0 deck and discard pile - requires 4 cards in deck(in case 2 victory cards drawn)
	//the particular cards in the deck don't matter - just need cards
	memset(state->deck[0], -1, sizeof(int) * MAX_DECK);
	memset(state->discard[0], -1, sizeof(int) * MAX_DECK);
	for (j = 0; j < 4; j++) {
		state->deck[0][j] = estate;
	}
	state->deckCount[0] = 4;
	state->discardCount[0] = 0;

	//set both player hands
	memset(state->hand[0], -1, sizeof(int) * MAX_DECK);
	memset(state->hand[1], -1, sizeof(int) * MAX_DECK);
	state->handCount[0] = 0;
	state->handCount[1] = 0;

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

/************************************************
* checkTribute()
*
* Checks the result of the tributeAction() function call.
*
* @param:	Pointer to gameState struct
* @param:	Integer representing tributeCard1 (if present, otherwise nothing)
* @param:	Integer representing tributeCard2 (if present, otherwise nothing)
*
* @return:	None - results printed to console
************************************************/
void checkTribute(struct gameState state, int tributeCard1, int tributeCard2) {
	int expectedNumActions = 0;
	int expectedNumCoins = 0;
	int expectedNumCards = 0;
	int numTributedCards = 2;

	//check action of tributeCard1
	switch (tributeCard1) {
	case victory1: {	//estate
		expectedNumCards += 2;
		break;
	}
	case victory2: {	//duchy
		expectedNumCards += 2;
		break;
	}
	case treasure1: {	//silver
		expectedNumCoins++;
		break;
	}
	case treasure2: {	//gold
		expectedNumCoins++;
		break;
	}
	case action1: {	//adventurer
		expectedNumActions++;
		break;
	}
	case action2: {	//council_room
		expectedNumActions++;
		break;
	}
	case nothing: {
		numTributedCards--;
		break;
	}
	default: { //unexpected card value
		printf("\nError: Unexpected card for tribute card 1");
		break;
	}
	}

	//check action of tributeCard2 if different from tributedCard1
	if (tributeCard1 != tributeCard2) {
		switch (tributeCard2) {
		case victory1: {	//estate
			expectedNumCards += 2;
			break;
		}
		case victory2: {	//duchy
			expectedNumCards += 2;
			break;
		}
		case treasure1: {	//silver
			expectedNumCoins++;
			break;
		}
		case treasure2: {	//gold
			expectedNumCoins++;
			break;
		}
		case action1: {	//adventurer
			expectedNumActions++;
			break;
		}
		case action2: {	//council_room
			expectedNumActions++;
			break;
		}
		case nothing: {
			numTributedCards--;
			break;
		}
		default: {	//nothing or unexpected card value
			//nothing happens in this case
			break;
		}
		}
	}
	else {
		numTributedCards--;
	}

	//check numActions
	if (state.numActions != expectedNumActions) {
		printf("\nError: Unexpected number of actions");
		printf("\n\tExpected: %d; Actual: %d", expectedNumActions, state.numActions);
	}
	else {
		printf("\nPassed: Observed number of actions is correct - should be %d", state.numActions);
	}

	//check coins
	if (state.coins != expectedNumCoins) {
		printf("\nError: Unexpected number coins");
		printf("\n\tExpected: %d; Actual %d", expectedNumCoins, state.coins);
	}
	else {
		printf("\nPassed: Observed number of coins is correct - should be %d", state.coins);
	}

	//check cards - correct # of cards in player 2's deck and discard pile
	//check player1 deck - should be 0
	int result = checkDeck(state, 0, 1, 2);
	switch (result){
	case -3: {
		printf("\nError in checkDeck function");
		break;
	}
	case -2: {
		printf("\nError invalid card value found in player 1's deck");
		break;
	}
	case -1: {
		printf("\nError: Unexpected number of cards in player 1's deck\n\tExpected: 0; Actual: %d", state.handCount[1]);
		break;
	}
	case 0: {
		printf("\nPassed: Number of cards in player 1's deck equals expected value - should be 0");
		break;
	}
	default: {
		printf("\nError: Player 1's deck count does not match actual number of cards in deck");
		printf("\n\tDeck Count: %d; Number of Cards Found: %d", state.handCount[1], result);
		break;
	}
	}
	
	//check player 1 discard pile - should be numTributedCards
	result = checkDeck(state, numTributedCards, 1, 3);
	switch (result) {
	case -3: {
		printf("\nError in checkDeck function");
		break;
	}
	case -2: {
		printf("\nError invalid card value found in player 1's discard pile");
		break;
	}
	case -1: {
		printf("\nError: Unexpected number of cards in player 1's discard\n\tExpected: 0; Actual: %d", state.discardCount[1]);
		break;
	}
	case 0: {
		printf("\nPassed: Number of cards in player 1's discard equals expected value - should be %d", numTributedCards);
		break;
	}
	default: {
		printf("\nError: Player 1's discard count does not match actual number of cards in discard pile");
		printf("\n\tDiscard Count: %d; Number of Cards Found: %d", state.discardCount[1], result);
		break;
	}
	}

	//check player 0 hand and deck
	//check player 0 hand - should be expectedNumCards
	result = checkDeck(state, expectedNumCards, 0, 1);
	switch (result) {
	case -3: {
		printf("\nError in checkDeck function");
		break;
	}
	case -2: {
		printf("\nError invalid card value found in player 0's hand");
		break;
	}
	case -1: {
		printf("\nError: Unexpected number of cards in player 0's hand\n\tExpected: 0; Actual: %d", state.handCount[0]);
		break;
	}
	case 0: {
		printf("\nPassed: Number of cards in player 0's hand equals expected value - should be %d", expectedNumCards);
		break;
	}
	default: {
		printf("\nError: Player 0's hand count does not match actual number of cards in hand");
		printf("\n\tHand Count: %d; Number of Cards Found: %d", state.handCount[0], result);
		break;
	}
	}

	//check player 0 deck - should be 4 - expectedNumCards
	result = checkDeck(state, 4 - expectedNumCards, 0, 2);
	switch (result) {
	case -3: {
		printf("\nError in checkDeck function");
		break;
	}
	case -2: {
		printf("\nError invalid card value found in player 0's deck");
		break;
	}
	case -1: {
		printf("\nError: Unexpected number of cards in player 0's deck\n\tExpected: 0; Actual: %d", state.deckCount[0]);
		break;
	}
	case 0: {
		printf("\nPassed: Number of cards in player 0's deck equals expected value - should be %d", 4 - expectedNumCards);
		break;
	}
	default: {
		printf("\nError: Player 0's deck count does not match actual number of cards in deck");
		printf("\n\tDeck Count: %d; Number of Cards Found: %d", state.deckCount[0], result);
		break;
	}
	}
}

int main() {
	srand(time(0));
	//Declare gameState struct and choose kingdomcards -> must include baron card
	struct gameState state;
	int k[10] = { adventurer, council_room, feast, gardens, mine, remodel, smithy, village, baron, great_hall };

	//where cards go
	int bothInDiscard = -1;
	int oneDiscardOneDeck = 0;
	int bothInDeck = 1;

	/*Possible outcomes:
		Player tributes only one card - only one card in discard
		Player tributes only one card - only one card in deck
		Player tributes only one card - both cards are the same
		For each one-card tribute:
			Tributed card is action
			Tributed card is treasure
			Tributed card is victory

		Player tributes two cards - both in discard
		Player tributes two cards - both in deck
		Player tributes two cards - one card in discard, one card in deck
		For each two-card tribute:
			Action + Action
			Treasure + Treasure
			Victory + Victory

			Action + Treasure
			Action + Victory
			Treasure + Victory

		Player tributes no cards - nothing happens
	*/
	int j;
	printf("Beginning Testing of tributeAction() function...\n");
	/**************************************************************************
	* Test Series 1: Player tributes only one card - only one card in discard
	**************************************************************************/
	//Only card is action
	printf("\nTest Series 1: Player tributes only one card - card in discard\n");
	printf("\nTest 1.1: Tributed card is an action card\n");
	setState(&state, k, action1, nothing, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, nothing);

	//Only card is treasure
	printf("\n\nTest 1.2: Tributed card is a treasure card\n");
	setState(&state, k, treasure1, nothing, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, treasure1, nothing);

	//Only card is victory
	printf("\n\nTest 1.3: Tributed card is a victory card\n");
	setState(&state, k, victory1, nothing, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, victory1, nothing);

	/**************************************************************************
	* Test Series 2: Player tributes only one card - only one card in deck
	**************************************************************************/
	//Only card is action
	printf("\n\nTest Series 2: Player tributes only one card - card in deck\n");
	printf("\nTest 2.1: Tributed card is an action card\n");
	setState(&state, k, action1, nothing, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, nothing);

	//Only card is treasure
	printf("\n\nTest 2.2: Tributed card is a treasure card\n");
	setState(&state, k, treasure1, nothing, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, treasure1, nothing);

	//Only card is victory
	printf("\n\nTest 2.3: Tributed card is a victory card\n");
	setState(&state, k, victory1, nothing, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, victory1, nothing);

	/**************************************************************************
	* Test Series 3: Player tributes only one card - both cards are the same
	**************************************************************************/
	//Both cards are action
	printf("\n\nTest Series 3: Player tributes only one card - both cards are the same\n");
	printf("\nTest 3.1: Tributed card is an action card\n");
	setState(&state, k, action1, action1, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, action1);

	//Only card is treasure
	printf("\n\nTest 3.2: Tributed card is a treasure card\n");
	setState(&state, k, treasure1, treasure1, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, treasure1, treasure1);

	//Only card is victory
	printf("\n\nTest 3.3: Tributed card is a victory card\n");
	setState(&state, k, victory1, victory1, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, victory1, victory1);

	/**************************************************************************
	* Test Series 4: Player tributes two cards - both cards in discard
	**************************************************************************/
	printf("\n\nTest Series 4: Player tributes two cards - both cards in discard");

	//Both cards are action cards
	printf("\n\nTest 4.1: Both cards are action cards\n");
	setState(&state, k, action1, action2, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, action2);

	//Both cards are treasure cards
	printf("\n\nTest 4.2: Both cards are treasure cards\n");
	setState(&state, k, treasure1, treasure2, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, treasure1, treasure2);

	//Both cards are victory cards
	printf("\n\nTest 4.3: Both cards are victory cards\n");
	setState(&state, k, victory1, victory2, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, victory1, victory2);

	//One card action, one card treasure
	printf("\n\nTest 4.4: One card is action, one card is treasure\n");
	setState(&state, k, action1, treasure1, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, treasure1);

	//One card action, one card victory
	printf("\n\nTest 4.5: One card is action, one card is victory\n");
	setState(&state, k, action1, victory1, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, victory1);

	//One card treasure, one card victory
	printf("\n\nTest 4.6: One card is action, one card is victory\n");
	setState(&state, k, action1, victory1, bothInDiscard);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, victory1);

	/**************************************************************************
	* Test Series 5: Player tributes two cards - both cards in deck
	**************************************************************************/
	printf("\n\nTest Series 5: Player tributes two cards - both cards in deck");

	//Both cards are action cards
	printf("\n\nTest 5.1: Both cards are action cards\n");
	setState(&state, k, action1, action2, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, action2);

	//Both cards are treasure cards
	printf("\n\nTest 5.2: Both cards are treasure cards\n");
	setState(&state, k, treasure1, treasure2, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, treasure1, treasure2);

	//Both cards are victory cards
	printf("\n\nTest 5.3: Both cards are victory cards\n");
	setState(&state, k, victory1, victory2, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, victory1, victory2);

	//One card action, one card treasure
	printf("\n\nTest 5.4: One card is action, one card is treasure\n");
	setState(&state, k, action1, treasure1, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, treasure1);

	//One card action, one card victory
	printf("\n\nTest 5.5: One card is action, one card is victory\n");
	setState(&state, k, action1, victory1, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, victory1);

	//One card treasure, one card victory
	printf("\n\nTest 5.6: One card is action, one card is victory\n");
	setState(&state, k, action1, victory1, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, victory1);

	/**************************************************************************
	* Test Series 6: Player tributes two cards - one card in discard, one card in deck
	**************************************************************************/
	printf("\n\nTest Series 6: Player tributes two cards - one card in discard, one card in deck");

	//Both cards are action cards
	printf("\n\nTest 6.1: Both cards are action cards\n");
	setState(&state, k, action1, action2, oneDiscardOneDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, action2);

	//Both cards are treasure cards
	printf("\n\nTest 6.2: Both cards are treasure cards\n");
	setState(&state, k, treasure1, treasure2, oneDiscardOneDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, treasure1, treasure2);

	//Both cards are victory cards
	printf("\n\nTest 6.3: Both cards are victory cards\n");
	setState(&state, k, victory1, victory2, oneDiscardOneDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, victory1, victory2);

	//One card action, one card treasure
	printf("\n\nTest 6.4: One card is action, one card is treasure\n");
	setState(&state, k, action1, treasure1, oneDiscardOneDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, treasure1);

	//One card action, one card victory
	printf("\n\nTest 6.5: One card is action, one card is victory\n");
	setState(&state, k, action1, victory1, oneDiscardOneDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, victory1);

	//One card treasure, one card victory
	printf("\n\nTest 6.6: One card is action, one card is victory\n");
	setState(&state, k, action1, victory1, oneDiscardOneDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, action1, victory1);


	/**************************************************************************
	* Test Series 7: Player tributes no cards
	**************************************************************************/
	printf("\n\nTest Series 7: Player tributes no cards\n");
	setState(&state, k, nothing, nothing, bothInDeck);
	printf("Calling tributeAction()...\n");
	tributeAction(0, 1, &state);
	checkTribute(state, nothing, nothing);
	return 0;
}