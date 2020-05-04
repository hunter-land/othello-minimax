/*
 * MinimaxPlayer.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: wong
 */
#include <iostream>
#include <assert.h>
#include "MinimaxPlayer.h"

#include <algorithm> //std::find
struct OthelloBoardNode { //Node used to construct the minimax tree
	OthelloBoardNode* prevNode;
	int playedCol, playedRow; //What turn did we take to get here from the previous state
	OthelloBoard b;// = nullptr; //Board state of THIS node
	char maxSymbol, minSymbol, nextTurnSymbol; //What symbol will play a turn from this one, what symbol are we, and who are we fighting
	std::vector<OthelloBoardNode*> nextNodes; //All the nodes which can be reached in one turn
	//Min and Max functions get the min/max of the children, based on their children, based on theirs, etc...
	//Once the bottom is reached, the utility of the terminal board is calcuated and returned.
	int getMax() {
		//std::cout << "Getting max of " << this << std::endl;
		int max = -__INT_MAX__;
		if(nextNodes.size() > 0) {
			for(OthelloBoardNode* nn : nextNodes) {
				max = std::max(max, nn->getMin());
			}
		} else {
			max = b.count_score(maxSymbol) - b.count_score(minSymbol); //Utility function for the terminal board state
			//std::cout << "Terminal reached in max, returning " << max << std::endl;
		}
		return max;
	}
	int getMin() {
		//std::cout << "Getting min of " << this << std::endl;
		int min = __INT_MAX__;
		if(nextNodes.size() > 0) {
			for(OthelloBoardNode* nn : nextNodes) {
				min = std::min(min, nn->getMax());
			}
		} else {
			min = b.count_score(maxSymbol) - b.count_score(minSymbol); //Utility function for the terminal board state
			//std::cout << "Terminal reached in min, returning " << min << std::endl;
		}
		return min;
	}
	OthelloBoardNode(OthelloBoard board) : b(board) {}
};
//Successor function, generates all possible turns and creates a node for each.
std::vector<OthelloBoardNode> generatePossibleStates(OthelloBoard fromBoard, char symbol) {
	std::vector<OthelloBoardNode> nextBoardStates;
	for(unsigned int col = 0; col < fromBoard.get_num_cols(); col++) {
		for(unsigned int row = 0; row < fromBoard.get_num_rows(); row++) {
			if(fromBoard.is_legal_move(col, row, symbol)) {
				//std::cout << "Legal move found..." << std::endl;
				OthelloBoardNode newBoard(fromBoard);
				newBoard.b.play_move(col, row, symbol);
				newBoard.playedCol = col;
				newBoard.playedRow = row;
				nextBoardStates.push_back(newBoard);
			}
		}
	}
	return nextBoardStates;
}

using std::vector;

MinimaxPlayer::MinimaxPlayer(char symb) :
		Player(symb) {

}

MinimaxPlayer::~MinimaxPlayer() {

}

void MinimaxPlayer::get_move(OthelloBoard* b, int& col, int& row) {
    // To be filled in by you
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	char opponentSymbol = b->get_p1_symbol() == symbol ? b->get_p2_symbol() : b->get_p1_symbol();
	//std::cout << "Playing as '" << symbol << "'s against '" << opponentSymbol << "'s" << std::endl;

	OthelloBoardNode *currentNode = new OthelloBoardNode(*b); //Generate a tree starting from the current state
	currentNode->nextTurnSymbol = symbol;
	currentNode->maxSymbol = symbol;
	currentNode->minSymbol = opponentSymbol;
	std::vector<OthelloBoardNode*> tree = {currentNode};
	std::vector<OthelloBoardNode*> unexpanded = {currentNode};

	//Generate minimax tree, place it into tree vector
	while(unexpanded.size() > 0) {
		OthelloBoardNode* thisNode = unexpanded[0];
		unexpanded.erase(std::find(unexpanded.begin(), unexpanded.end(), thisNode));

		std::vector<OthelloBoardNode> nextBoardNodes = generatePossibleStates(thisNode->b, thisNode->nextTurnSymbol); //Generate next turns
		//std::cout << "Expanding node into " << nextBoardNodes.size() << " other nodes" << std::endl;
		for(OthelloBoardNode bn : nextBoardNodes) {
			//OthelloBoardNode* nextNode = new OthelloBoardNode(bn);
			OthelloBoardNode* nextNode = new OthelloBoardNode(bn.b);
			nextNode->prevNode = thisNode;
			nextNode->maxSymbol = thisNode->maxSymbol;
			nextNode->minSymbol = thisNode->minSymbol;
			nextNode->playedCol = bn.playedCol;
			nextNode->playedRow = bn.playedRow;
			nextNode->nextTurnSymbol = thisNode->nextTurnSymbol == symbol ? opponentSymbol : symbol;
			thisNode->nextNodes.push_back(nextNode);

			tree.push_back(nextNode);
			unexpanded.push_back(nextNode);
		}
	}
	//std::cout << "Created a tree of size " << tree.size() << std::endl;

	//Then search through it and give the play to perform
	//std::cout << "Looking for good move out of " << currentNode->nextNodes.size() << " possible." << std::endl;
	bool hasPlayed = false;
	int bestColOpt, bestRowOpt, bestValueOpt = -__INT_MAX__;
	for(OthelloBoardNode* nn : currentNode->nextNodes) { //For all nodes we can get to
		int nnMin = nn->getMin(); //Find the best one we can take
		if(nnMin > bestValueOpt) { //If we can get to a better terminal with this path, take it
			bestValueOpt = nnMin;
			bestColOpt = nn->playedCol;
			bestRowOpt = nn->playedRow;
			hasPlayed = true;
		}
	}
	if(hasPlayed) {
		//std::cout << "Best option (" << bestValueOpt << "):" << std::endl;
		if(bestValueOpt == 0) {
			//std::cout << "\tTie is the best I can do now..." << std::endl; //If they always play their best move, the AI can only tie or lose
		} else if(bestValueOpt < 0) {
			//std::cout << "\tI can not win from here..." << std::endl; //If they always play their best move, the AI can no longer win
		}
		//std::cout << "Attempting to place an '" << symbol << "' at (" << bestColOpt << ", " << bestRowOpt << ")" << std::endl;
		col = bestColOpt;
		row = bestRowOpt;
	} else {
		if(currentNode->nextNodes.size() > 0) {
			OthelloBoardNode *nn = currentNode->nextNodes[0];
			//std::cout << "Move required:" << std::endl;
			//std::cout << "Attempting to place an '" << symbol << "' at (" << nn->playedCol << ", " << nn->playedRow << ")" << std::endl;
			//b->play_move(nn->playedCol, nn->playedRow, symbol);
			col = nn->playedCol;
			row = nn->playedRow;
		}
	}
}

MinimaxPlayer* MinimaxPlayer::clone() {
	MinimaxPlayer* result = new MinimaxPlayer(symbol);
	return result;
}
