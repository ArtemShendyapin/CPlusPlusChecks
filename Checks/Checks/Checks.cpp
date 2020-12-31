#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include "Checks.h"

using std::vector;
using std::pair;
using std::set;
using std::string;
using std::cin;
using std::cout;
using std::endl;

Deck::Deck() {

	// Add checks coordinates
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 4; ++j) {
			white.insert({ i, i % 2 + j * 2 });
			black.insert({ row_num - 1 - i, (i + 1) % 2 + j * 2 });
		}
	}
/*
	//white = {{0, 2}};
	//black = {{1, 1}, {1, 3}, {3, 1}, {3, 3}, {3, 5}};
	//black = {{1, 1}, {1, 3}};
	white = {};
	q_white = { { 2, 6 } };
	black = { { 1, 5 },{ 3, 5 },{ 1, 3 },{ 3, 3 } };
*/
	// Fill the field
	for (int i = 0; i < row_num; ++i) {
		vector<square> row(col_num);
		for (int j = 0; j < col_num; ++j) {
			if ((i + j) % 2 == 0) row[j] = w_s;
			else row[j] = b_s;
		}
		desk.push_back(row);
	}

	// Add checks on field
	for (auto check : white) {
		desk[check.first][check.second] = w_c;
	}
	for (auto check : black) {
		desk[check.first][check.second] = b_c;
	}
}

void Deck::printDeck() {

	desk.clear();
	for (int i = 0; i < row_num; ++i) {
		vector<square> row(col_num);
		for (int j = 0; j < col_num; ++j) {
			if ((i + j) % 2 == 0) row[j] = w_s;
			else row[j] = b_s;
		}
		desk.push_back(row);
	}

	// Add checks on field
	for (auto check : white) {
		desk[check.first][check.second] = w_c;
	}
	for (auto check : black) {
		desk[check.first][check.second] = b_c;
	}
	for (auto check : q_white) {
		desk[check.first][check.second] = w_q;
	}
	for (auto check : q_black) {
		desk[check.first][check.second] = b_q;
	}

	cout << "  ABCDEFGH" << endl;

	for (int i = row_num - 1; i >= 0; --i) {
		cout << i + 1 << " ";
		for (int j = 0; j < col_num; ++j) {
			switch (desk[i][j]) {
			case w_s:
				cout << ' ';
				break;
			case b_s:
				cout << char(219);
				break;
			case w_c:
				cout << 'W';
				break;
			case b_c:
				cout << 'B';
				break;
			case w_q:
				cout << 'Q';
				break;
			case b_q:
				cout << 'q';
				break;
			}
		}
		cout << endl;
	}
}

void Deck::printMove(const vector<pair<int, int>>& move) {
	for (int i = 0; i < move.size() - 1; ++i) {
		cout << char('a' + move[i].second) << move[i].first + 1 << "-";
	}
	cout << char('a' + move.back().second) << move.back().first + 1;
}

void Deck::printMoveOptions(const vector<vector<pair<int, int>>>& options) {
	if (options.empty()) return;
	cout << "Move options:" << endl;
	for (int i = 0; i < options.size() - 1; ++i) {
		printMove(options[i]);
		cout << ", ";
	}
	printMove(options.back());
	cout << endl;
}


vector<pair<int, int>> Deck::isValidRecord(const string& s) {

	vector<pair<int, int>> rez, no_ans;

	int row = 0, col = 0;
	if (s.size() % 3 != 2) return no_ans;
	for (int i = 0; i < s.size(); ++i) {
		if (i % 3 == 0) {
			if (s[i] < 'a' || s[i] > 'h') return no_ans;
			row = s[i] - 'a';
		}
		if (i % 3 == 1) {
			if (s[i] < '1' || s[i] > '8') return no_ans;
			col = s[i] - '1';
		}
		if (i % 3 == 2) {
			if (s[i] != '-') return no_ans;
			rez.push_back({ col, row });
		}
	}
	rez.push_back({ col, row });
	return rez;
}

void Deck::directionOffsets(direction dir, int &x_off, int &y_off) {
	switch (dir) {
	case up_right:
		x_off = 1;
		y_off = 1;
		break;
	case up_left:
		x_off = 1;
		y_off = -1;
		break;
	case down_right:
		x_off = -1;
		y_off = 1;
		break;
	case down_left:
		x_off = -1;
		y_off = -1;
		break;
	}

}

bool Deck::isFieldWithinDeck(pair<int, int> field) {
	int x = field.first, y = field.second;
	if (x < row_num && x >= 0 && y < col_num && y >= 0) return true;
	return false;
}

bool Deck::isPreyOnField(
	pair<int, int> field,
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	vector<pair<int, int>>& eat_queue
)
{
	if (prey.find(field) != prey.end() &&
		!any_of(eat_queue.begin(), eat_queue.end(),
			[field](pair<int, int> el) { return el == field; }) &&
		hunter.find(field) == hunter.end()) return true;
	return false;
}

bool Deck::isNextFieldFree(
	pair<int, int> field,
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	vector<pair<int, int>>& eat_queue
)
{
	if (prey.find(field) == prey.end() &&
		!any_of(eat_queue.begin(), eat_queue.end(),
			[field](pair<int, int> el) { return el == field; }) &&
		hunter.find(field) == hunter.end()) return true;
	return false;
}

vector<pair<int, int>> Deck::canEatThisDirection(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue,
	direction dir
)
{
	int x = killer.first, y = killer.second;
	int x_off, y_off;
	directionOffsets(dir, x_off, y_off);

	vector<pair<int, int>> rez;
	// Current field that we check to find a check that can be eaten
	pair<int, int> step = { x + x_off, y + y_off };
	// Field after that field on same direction
	pair<int, int> overstep = { x + x_off + x_off, y + y_off + y_off };
	if (isFieldWithinDeck(overstep) &&
		isPreyOnField(step, hunter, prey, eat_queue) &&
		isNextFieldFree(overstep, hunter, prey, eat_queue)) {

		rez.push_back(overstep);
		eat_queue.push_back(step);
	}
	
	return rez;
}

void Deck::canCheckEatFurther(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue,
	vector<pair<int, int>>& eat_step_queue
)
{
	// If it is the first eating, need to add start position
	if (eat_step_queue.empty()) {
		eat_step_queue.push_back(killer);
	}

	vector<pair<int, int>> canEat;
	bool isSomethingToEat = false;
	int dirColor = 0;
	if (turn == false) {
		dirColor = 2;
	}
	for (int dirInt = dirColor; dirInt < dirColor+2; ++dirInt) {
		direction dir = static_cast<direction>(dirInt);
		canEat = canEatThisDirection(hunter, prey, killer, eat_queue, dir);
		if (!canEat.empty()) {
			isSomethingToEat = true;
			for (auto field : canEat) {
				hunter.erase({ killer.first, killer.second });
				hunter.insert({ field.first, field.second });
				eat_step_queue.push_back({ field.first, field.second });
				canCheckEatFurther(hunter, prey, field, eat_queue, eat_step_queue);
				hunter.insert({ killer.first, killer.second });
				hunter.erase({ field.first, field.second });
				eat_step_queue.pop_back();
			}
			eat_queue.pop_back();
		}
	}

	if (!isSomethingToEat && !eat_queue.empty()) {
		must_eat.push_back(eat_step_queue);
		cout << "isSomethingToEat=" << isSomethingToEat << endl;
		printMove(eat_step_queue);
		cout << endl;
		eatens.push_back(eat_queue);
	}
}

void Deck::canQueenEatFurther(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue,
	vector<pair<int, int>>& eat_step_queue
)
{
	// If it is the first eating, need to add start position
	if (eat_step_queue.empty()) {
		eat_step_queue.push_back(killer);
	}
	vector<pair<int, int>> canEat;
	bool isSomethingToEat = false;
	for (int dirInt = 0; dirInt < 4; ++dirInt) {
		direction dir = static_cast<direction>(dirInt);
		canEat = canEatThisDirection(hunter, prey, killer, eat_queue, dir);
		if (!canEat.empty()) {
			isSomethingToEat = true;
			for (auto field : canEat) {
				hunter.erase({ killer.first, killer.second });
				hunter.insert({ field.first, field.second });
				eat_step_queue.push_back({ field.first, field.second });
				canQueenEatFurther(hunter, prey, field, eat_queue, eat_step_queue);
				hunter.insert({ killer.first, killer.second });
				hunter.erase({ field.first, field.second });
				eat_step_queue.pop_back();
			}
			eat_queue.pop_back();
		}
	}

	if (!isSomethingToEat && !eat_queue.empty()) {
		must_eat.push_back(eat_step_queue);
		eatens.push_back(eat_queue);
	}
}

set<pair<int, int>> set_union(set<pair<int, int>>& a, set<pair<int, int>>& b) {
	set<pair<int, int>> rez = a;
	for (auto el : b) {
		rez.insert(el);
	}
	return rez;
}

void Deck::mustEat() {

	must_eat.clear();
	eatens.clear();
	set<pair<int, int>> const_hunter, const_hunter_q, hunter, prey;
	set<pair<int, int>>::iterator it;
	if (turn) {
		hunter = set_union(white, q_white);
		prey = set_union(black, q_black);
		const_hunter = white;
		const_hunter_q = q_white;
	}
	else {
		hunter = set_union(black, q_black);
		prey = set_union(white, q_white);
		const_hunter = black;
		const_hunter_q = q_black;
	}

	for (auto check : const_hunter) {

		vector<pair<int, int>> eat_queue, eat_step_queue;
		canCheckEatFurther(hunter, prey, check, eat_queue, eat_step_queue);
	}
	for (auto check : const_hunter_q) {

		vector<pair<int, int>> eat_queue, eat_step_queue;
		canQueenEatFurther(hunter, prey, check, eat_queue, eat_step_queue);
	}
}

bool Deck::isValidEat() {

	int eatInd = -1;
	for (int i = 0; i < must_eat.size(); ++i) {
		if (must_eat[i] == n_move) {
			eatInd = i;
			break;
		}
	}
	if (eatInd == -1) return false;

	set<pair<int, int>> &hunterCheck = turn ? white : black, &hunterQueen = turn ? q_white : q_black;
	set<pair<int, int>> &preyCheck = turn ? black : white, &preyQueen = turn ? q_black : q_white;

	pair<int, int> firstField = must_eat[eatInd].front(), lastField = must_eat[eatInd].back();
	if (hunterCheck.find(firstField) != hunterCheck.end()) {
		hunterCheck.erase(firstField);
		if (lastField.first == 0 || lastField.first == row_num - 1) {
			hunterQueen.insert(lastField);
		}
		else
		{
			hunterCheck.insert(lastField);
		}
	}
	else
	{
		hunterQueen.erase(firstField);
		hunterQueen.insert(lastField);
	}

	vector<pair<int, int>> eatenFigures = eatens[eatInd];
	for (auto figure : eatenFigures) {
		if (preyCheck.find(figure) != preyCheck.end()) {
			preyCheck.erase(figure);
		}
		else 
		{
			preyQueen.erase(figure);
		}
	}
	return true;
}

void Deck::canMoveThisDirection(pair<int, int> figure, direction dir) {
	int x = figure.first, y = figure.second;
	int x_off, y_off;
	directionOffsets(dir, x_off, y_off);
	pair<int, int> step = { x + x_off, y + y_off };
	if (isFieldWithinDeck(step) &&
		white.find(step) == white.end() &&
		black.find(step) == black.end() &&
		q_white.find(step) == q_white.end() &&
		q_black.find(step) == q_black.end()) {
		
		moves.push_back({ figure, step });
	}
}

void Deck::possibleMoves() {

	moves.clear();
	set<pair<int, int>> &hunterChecks = turn ? white : black;
	set<pair<int, int>> &hunterQueens = turn ? q_white : q_black;
	for (auto figure : hunterChecks) {
		int dirColor = 0;
		if (turn == false) {
			dirColor = 2;
		}
		for (int dirInt = dirColor; dirInt < dirColor + 2; ++dirInt) {
			direction dir = static_cast<direction>(dirInt);
			canMoveThisDirection(figure, dir);
		}
	}
	for (auto figure : hunterQueens) {
		for (int dirInt = 0; dirInt < 4; ++dirInt) {
			direction dir = static_cast<direction>(dirInt);
			canMoveThisDirection(figure, dir);
		}
	}
}

bool Deck::isValidMove() {
	if (!any_of(moves.begin(), moves.end(),
		[=](vector<pair<int, int>> move) { return move == n_move; })) {

		return false;
	}

	set<pair<int, int>> &hunterCheck = turn ? white : black, &hunterQueen = turn ? q_white : q_black;
	pair<int, int> firstField = n_move.front(), lastField = n_move.back();
	if (hunterCheck.find(firstField) != hunterCheck.end()) {
		hunterCheck.erase(firstField);
		if (lastField.first == 0 || lastField.first == row_num - 1) {
			hunterQueen.insert(lastField);
		}
		else {
			hunterCheck.insert(lastField);
		}
	}
	else {
		hunterQueen.erase(firstField);
		hunterQueen.insert(lastField);
	}

	return true;
}


int main() {

	Deck deck;

	deck.printDeck();
//	deck.prototype();
	deck.playTheGame();

	return 0;
}