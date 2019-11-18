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
# define TEST_DECK_SIZE 2

struct errorState {

	//Current test variables

	//Invoking player and player to their left
	int chosenPlayer;
	int playerLeftOfChosenPlayer;

	//Track left player's deck and discard cards
	int leftPlayerDeck[TEST_DECK_SIZE];
	int leftPlayerDeckCount;
	int leftPlayerDiscard[TEST_DECK_SIZE];
	int leftPlayerDiscardCount;

	//Track all player initial deck count and discard count
	int startingDeckSize[MAX_PLAYERS];
	int startingDiscardSize[MAX_PLAYERS];

	//Cumulative test variables

	//Success, failure, or nothing happens
	int tributeSuccess;
	int tributeError;
	int nothing_happens;

	int leftPlayerDiscardCountError;
	int leftPlayerDeckCountError;
};

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
	error->chosenPlayer = -1;
	error->playerLeftOfChosenPlayer = -1;
	error->leftPlayerDeckCount = -1;
	error->leftPlayerDiscardCount = -1;

	for (int i = 0; i < TEST_DECK_SIZE; i++) {
		error->leftPlayerDeck[i] = -1;
		error->leftPlayerDiscard[i] = -1;
	}

	for (int i = 0; i < MAX_PLAYERS; i++) {
		error->startingDeckSize[i] = -1;
		error->startingDiscardSize[i] = -1;
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
* randomize()
*
* Randomly assigns the cards in each player's deck and discard pile.
* Player hands not used in target function - does not need to be randomized. 
* 0 to 2 cards in player decks except chosen player, whose deck size is set to 4
* in case two action cards are drawn, and 0 to 2 cards in player discard.
* Function implements shuffling discard pile back into deck,
* and is thus within the scope of this randomized test.
*
* @param:	Pointer to gameState struct
* @param:	Pointer to errorState struct
* @param:	Pointer to kingdomCards array
*
* @return:	None - each player's deck will contain
*			0 to 2 cards from the supply,
*			and each player's discard pile will contain
*			0 to 2 cards from the supply.
*************************************************/
void randomize(struct gameState * state, struct errorState * error, int * k, int chosenPlayer, int leftPlayer) {
	int OTHER_PLAYER_DECKSIZE = 2;
	int CHOSEN_PLAYER_DECKSIZE = 4;

	//Iterate through each player, setting their deck, deckCount, discard, and discardCount
	for (int i = 0; i < state->numPlayers; i++) {
		int deckNum;
		if (i == chosenPlayer) {
			//Assign 4 as player's deck size
			deckNum = CHOSEN_PLAYER_DECKSIZE;
			state->deckCount[i] = deckNum;
			error->startingDeckSize[i] = deckNum;
		}
		else {
			//Assign random number for player's deck size from 0 to 2
			deckNum = rand() % OTHER_PLAYER_DECKSIZE + 1;	//Returns 0, 1, or 2
			state->deckCount[i] = deckNum;
			error->startingDeckSize[i] = deckNum;
		}


		//Assign random number for player's discard pile size
		int discardNum = rand() % 3;
		state->discardCount[i] = discardNum;
		error->startingDiscardSize[i] = discardNum;

		//Set all cards in player's deck and discard pile to -1
		//this prevents a random positon from accidentally containing a valid card

		for (int j = 0; j < deckNum; j++) {
			state->deck[i][j] = -1;
		}

		for (int j = 0; j < discardNum; j++) {
			state->discard[i][j] = -1;
		}

		//Assign random cards to player's deck and discard pile

		for (int j = 0; j < deckNum; j++) {
			//pick a random card from card array k
			int card = rand() % 10;		//Returns 0 to 9
			state->deck[i][j] = k[card];
		}

		for (int j = 0; j < discardNum; j++) {
			//pick a random card from card array k
			int card = rand() % 10;
			state->discard[i][j] = k[card];
		}

		//If current iteration is leftPlayer, update leftPlayer values in errorState struct
		if (i == leftPlayer) {
			error->leftPlayerDeckCount = deckNum;
			error->leftPlayerDiscardCount = discardNum;

			for (int j = 0; j < deckNum; j++) {
				error->leftPlayerDeck[j] = state->deck[i][j];
			}

			for (int j = 0; j < discardNum; j++) {
				error->leftPlayerDiscard[j] = state->deck[i][j];
			}
		}
	}
}

/************************************************
* processResult()
*
* Checks the results of minionAction() function call.
*
* Three possible unique scenarios:
*	1: 0 cards revealed by left player
*		> Occurs when left player has no cards in their deck and discard piles
*		> game state doesn't change
*	2: 1 unique card revealded by left player
*		> Occurs when left player only has one card in deck and discard piles
*		  or player reveals to identical cards
*		> one change to game state
*	3: 2 unique cards revealed by left player
*		> Occurs when left player reveals two different cards
*		> two changes to game state or one change applied twice
*
* @param:	Pointer to errorState struct.
* @param:	gameState struct.
*
* @return:	None - any errors from baronAction() is recorded
*			to the errorState struct.
*************************************************/
void processResult(struct gameState state, struct errorState * error) {
	
	//Track results
	int possibleTributeCardsNum = 0;
	int possibleTributeCards[] = { -1, -1, -1 };
	int numCoinsExpected = 0;
	int numDrawnCardsExpected = 0;
	int numActionsExpected = 0;
	int discardCountExpected = 0;

	//Flag to signal initial deck/discard count errors
	int countError = 0;

	//Tracking results for scenario when more than two cards possible for tribute - three possible combinations
	int numCoinsPossibilities[] = { 0, 0, 0 };
	int numDrawnCardsPossibilities[] = { 0, 0, 0 };
	int numActionsPossibilities[] = { 0, 0, 0 };
	
	//Analyze gameState struct based on contents of errorState struct
	//Print current test stats to console
	printf("\nChosen Player: %d", error->chosenPlayer);
	printf("\nChosen Player Deck Size: %d", error->startingDeckSize[error->chosenPlayer]);
	printf("\nLeft Player: %d", error->playerLeftOfChosenPlayer);

	printf("\nLeft Player Deck Size: %d", error->leftPlayerDeckCount);
	printf("\nLeft Player Discard Size: %d", error->leftPlayerDiscardCount);

	//Iterate through leftPlayer's deck and discard
	//Figure out possible tribute cards, and expected discard count
	//What cards are possible tribute cards depends on how many cards are in left player's deck and discard

	//2 cards in player's initial deck -> both should be used in tribute and discarded
	if (error->leftPlayerDeckCount == 2) {
		printf("\n2 cards in Left Player's Deck - All tributed cards come from initial deck.");
		possibleTributeCardsNum = 2;
		for (int i = 0; i < error->leftPlayerDeckCount; i++) {
			possibleTributeCards[i] = error->leftPlayerDeck[i];
		}

		if (error->leftPlayerDiscardCount == 2) {
			discardCountExpected = 4;
		}
		else if (error->leftPlayerDiscardCount == 1) {
			discardCountExpected = 3;
		}
		else if (error->leftPlayerDiscardCount == 0) {
			discardCountExpected = 2;
		}

		//default error handler
		else {
			printf("\nError: Unexpected Left Player Discard Count");
			error->leftPlayerDiscardCountError++;
			countError = 1;
		}
	}

	//1 card in player's initial deck -> action depends on discard pile
	else if (error->leftPlayerDeckCount == 1) {
		printf("\n1 card in Left Player Deck and ");

		//2 cards in discard -> shuffled back into deck and two cards drawn/discarded
		if (error->leftPlayerDiscardCount == 2) {
			printf("2 cards in Left Player Discard - Tributed cards from deck and discard or just discard.");
			possibleTributeCardsNum = 3;
			possibleTributeCards[0] = error->leftPlayerDeck[0];
			for (int i = 1; i < error->leftPlayerDiscardCount; i++) {
				possibleTributeCards[i] = error->leftPlayerDiscard[i - 1];
			}
			discardCountExpected = 3;
		}

		//1 card in discard -> shuffle both into deck and discard both
		else if (error->leftPlayerDiscardCount == 1) {
			printf("1 card in Left Player Discard - Tributed cards from deck and discard.");
			possibleTributeCardsNum = 2;
			possibleTributeCards[0] = error->leftPlayerDeck[0];
			possibleTributeCards[1] = error->leftPlayerDiscard[0];
			discardCountExpected = 2;
		}

		//0 chards in discard -> only tribute one card
		else if (error->leftPlayerDiscardCount == 0) {
			printf("0 cards in Left Player Discard - Tributed card from deck only.");
			possibleTributeCardsNum = 1;
			possibleTributeCards[0] = error->leftPlayerDeck[0];
			discardCountExpected = 1;
		}

		//default error handler
		else {
			printf("\nError: Unexpected Left Player Discard Count");
			error->leftPlayerDiscardCountError++;
			countError = 1;
		}
	}

	//0 cards in player's initial deck -> action depends on discard pile
	else if (error->leftPlayerDeckCount == 0) {
		printf("\n0 cards in Left Player Deck and ");

		//2 cards in discard -> shuffled back into deck and both drawn/discarded
		if (error->leftPlayerDiscardCount == 2) {
			printf("2 cards in Left Player Discard - Tributed cards from Left Player Discard.");
			possibleTributeCardsNum = 2;
			possibleTributeCards[0] = error->leftPlayerDiscard[0];
			possibleTributeCards[1] = error->leftPlayerDiscard[1];
			discardCountExpected = 2;
		}

		//1 card in discard -> used directly and discarded again
		else if (error->leftPlayerDiscardCount == 1) {
			printf("1 card in Left Player Discard - Tributed card from Left Player Discard.");
			possibleTributeCardsNum = 1;
			possibleTributeCards[0] = error->leftPlayerDiscard[0];
			discardCountExpected = 1;
		}

		//0 cards in discard -> nothing happens
		else if (error->leftPlayerDiscardCount == 0) {
			printf("0 cards in Left Player Discard - No cards tributed and no effect activated.");
			possibleTributeCardsNum = 0;
			discardCountExpected = 0;
			error->nothing_happens++;
		}

		//default error handler
		else {
			printf("\nError: Unexpected Left Player Discard Count");
			error->leftPlayerDiscardCountError++;
			countError = 1;
		}
	}

	//default error handler
	else {
		printf("\nError: Unexpected Left Player Deck Count");
		error->leftPlayerDeckCountError++;
		countError = 1;
	}

	//determine expected outcomes based on possible tribute cards
	//skip if initial countError encountered;
	if (countError == 0) {
		
		//predict actions in scenarios where we are certain what the actions should be
		if (possibleTributeCardsNum < 3) {
			for (int i = 0; i < possibleTributeCardsNum; i++) {
				if (possibleTributeCards[i] == copper || possibleTributeCards[i] == silver || possibleTributeCards[i] == gold) {
					numCoinsExpected += 2;
				}
				else if (possibleTributeCards[i] == estate || possibleTributeCards[i] == duchy || possibleTributeCards[i] == province || possibleTributeCards[i] == gardens || great_hall) {
					numDrawnCardsExpected += 2;
				}
				else {
					numActionsExpected += 2;
				}
			}
			
			printf("\nExpected Effects:");
			printf("\n\tGain %d coins", numCoinsExpected);
			printf("\n\tDraw %d cards", numDrawnCardsExpected);
			printf("\n\tGain %d actions", numActionsExpected);
			
		}

		//gather all possible action predictions if multiple scenarios possible
		else if (possibleTributeCardsNum == 3) {
			
			//need to determine all possible effect combinations - should be three
			//3 possible combinations of 3 cards paired in groups of 2 
			//scenario 0 = 0 & 1, scenario 1 = 0 & 2, and scenario 2 = 1 & 2 
			
			//effects of possibleTributeCards[0] added to scenarios 0 and 1
			if (possibleTributeCards[0] == copper || possibleTributeCards[0] == silver || possibleTributeCards[0] == gold) {
				numCoinsPossibilities[0] += 2;
				numCoinsPossibilities[1] += 2;
			}
			else if (possibleTributeCards[0] == estate || possibleTributeCards[0] == duchy || possibleTributeCards[0] == province || possibleTributeCards[0] == gardens || great_hall) {
				numDrawnCardsPossibilities[0] += 2;
				numDrawnCardsPossibilities[1] += 2;
			}
			else {
				numActionsPossibilities[0] += 2;
				numActionsPossibilities[1] += 2;
			}

			//effects of possibileTributeCards[1] added to scenarios 0 and 2
			if (possibleTributeCards[1] == copper || possibleTributeCards[1] == silver || possibleTributeCards[1] == gold) {
				numCoinsPossibilities[0] += 2;
				numCoinsPossibilities[2] += 2;
			}
			else if (possibleTributeCards[1] == estate || possibleTributeCards[1] == duchy || possibleTributeCards[1] == province || possibleTributeCards[1] == gardens || great_hall) {
				numDrawnCardsPossibilities[0] += 2;
				numDrawnCardsPossibilities[2] += 2;
			}
			else {
				numActionsPossibilities[0] += 2;
				numActionsPossibilities[2] += 2;
			}

			//effects of possibleTributeCards[2] added to scenarios 1 and 2
			if (possibleTributeCards[2] == copper || possibleTributeCards[2] == silver || possibleTributeCards[2] == gold) {
				numCoinsPossibilities[1] += 2;
				numCoinsPossibilities[2] += 2;
			}
			else if (possibleTributeCards[2] == estate || possibleTributeCards[2] == duchy || possibleTributeCards[2] == province || possibleTributeCards[2] == gardens || great_hall) {
				numDrawnCardsPossibilities[1] += 2;
				numDrawnCardsPossibilities[2] += 2;
			}
			else {
				numActionsPossibilities[1] += 2;
				numActionsPossibilities[2] += 2;
			}

			printf("\nThree possible effect combinations.");

			
			for (int i = 0; i < 3; i++) {
				printf("\nPossible Effects %d:", i);
				printf("\n\tGain %d coins", numCoinsPossibilities[i]);
				printf("\n\tDraw %d cards", numDrawnCardsPossibilities[i]);
				printf("\n\tGain %d actions", numActionsPossibilities[i]);
			}
			
		}
		else {
			printf("\nTributed Cards Analysis Error");
		}

		//check action predictions against function results
		if (possibleTributeCardsNum <= 3) {
			//check the known ones first
			if (possibleTributeCardsNum < 3) {
				int tributeError = 0;
				if (numCoinsExpected != state.coins) {
					printf("\nError: Unexpected number of coins.");
					printf("\n\tExpected: %d", numCoinsExpected);
					printf("\n\tActual: %d", state.coins);
					tributeError = 1;
				}
				else {
					printf("\nExpected coins match actual coins.");
				}
				if (error->startingDeckSize[error->chosenPlayer] - numDrawnCardsExpected != state.deckCount[error->chosenPlayer]) {
					printf("\nError: Unexpected number of cards in target player's deck.");
					printf("\n\tExpected: %d", error->startingDeckSize[error->chosenPlayer] - numDrawnCardsExpected);
					printf("\n\tActual: %d", state.deckCount[error->chosenPlayer]);
					tributeError = 1;
				}
				else {
					printf("\nExpected drawn cards matches actual drawn cards.");
				}
				if (numActionsExpected != state.numActions) {
					printf("\nError: Unexpected number of actions.");
					printf("\n\tExpected: %d", numActionsExpected);
					printf("\n\tActual: %d", state.numActions);
					tributeError = 1;
				}
				else {
					printf("\nExpected actions matches actual actions.");
				}
				if (tributeError == 0) {
					printf("\nTest passed.");
					error->tributeSuccess++;
				}
				else {
					error->tributeError++;
				}
			}

			//check all scenarios if multiple possible
			else {
				int tributeError = 1;
				for (int i = 0; i < 2; i++) {
					if (numCoinsPossibilities[i] == state.coins
						&& error->startingDeckSize[error->chosenPlayer] - numDrawnCardsPossibilities[i] == state.deckCount[error->chosenPlayer]
						&& numActionsPossibilities[i] == state.numActions) {
						printf("\nMatch Found");
						tributeError = 0;
						break;
					}
				}
				if (tributeError == 0) {
					printf("\nTest Passed.");
					error->tributeSuccess++;
				}
				else {
					printf("\nError: No possible scenarios match actual outcome.");
					for (int i = 0; i < 2; i++) {
						printf("\nScenario %d:", i);
						printf("\n\tGain %d coins", numCoinsPossibilities[i]);
						printf("\n\tDraw %d cards", numDrawnCardsPossibilities[i]);
						printf("\n\tGain %d actions", numActionsPossibilities[i]);
					}
					printf("\nActual:");
					printf("\n\tGain %d coins", state.coins);
					printf("\n\tDraw %d cards", state.deck[error->chosenPlayer] - error->startingDeckSize[error->chosenPlayer]);
					printf("\n\tGain %d actions", state.numActions);

					error->tributeError++;
				}
			}
		}
	}
	else {
		printf("\nTest analysis skipped for this test - unreliable initial data.");
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

	printf("\n\tTests Passed: %d", error.tributeSuccess);
	printf("\n\tTests Failed: %d", error.tributeError);
}

int main() {
	srand(time(NULL));
	struct gameState game;
	int k[10];

	struct errorState error;
	memset(&error, 23, sizeof(struct errorState));

	//Initialize test tracking variables
	error.tributeSuccess = 0;
	error.tributeError = 0;
	error.nothing_happens = 0;
	error.leftPlayerDeckCountError = 0;
	error.leftPlayerDiscardCountError = 0;

	//Main testing loop
	for (int i = 0; i < 200; i++) {
		//Reset contents of errorState struct
		resetError(&error);

		printf("\n\nTest %d:\n", i + 1);

		//generate randomized supply
		setSupply(k);

		//pick random number of players - 2 to 4
		int numPlayers = (rand() % 3) + 2;

		//Initialize gameState
		memset(&game, 23, sizeof(struct gameState));
		int r = initializeGame(numPlayers, k, 13, &game);
		game.coins = 0;
		game.numActions = 0;
		clearDiscard(&game);
		clearDeck(&game);

		//Choose random invoker player and left player to use in tributeAction() call,
		//and randomize gameState
		int randomPlayer = rand() % game.numPlayers;	//Returns 0 to numPlayers - 1
		error.chosenPlayer = randomPlayer;

		int randomLeftPlayer = rand() % game.numPlayers;
		while (randomLeftPlayer == randomPlayer) {
			randomLeftPlayer = rand() % game.numPlayers;
		}
		error.playerLeftOfChosenPlayer = randomLeftPlayer;

		randomize(&game, &error, k, randomPlayer, randomLeftPlayer);
		//Call function and process results
		tributeAction(randomPlayer, randomLeftPlayer, &game);
		processResult(game, &error);
	}

	//Analyze aggregate test results
	analyzeErrors(error);

	return 0;
}