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
using std::to_string;
using std::cin;
using std::cout;
using std::endl;

Deck::Deck() {

	// Fill the field
	turn = true;
	for (int i = 0; i < row_num; ++i) {
		vector<square> row(col_num);
		for (int j = 0; j < col_num; ++j) {
			if ((i + j) % 2 == 0) row[j] = b_s;
			else row[j] = w_s;
		}
		desk.push_back(row);
	}
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 4; ++j) {
			desk[i][i % 2 + j * 2] = w_c;
			desk[row_num - 1 - i][(i + 1) % 2 + j * 2] = b_c;
		}
	}
}

Deck::Deck(Position position) {
	turn = position.turn;
	for (int i = 0; i < row_num; ++i) {
		vector<square> row(col_num);
		for (int j = 0; j < col_num; ++j) {
			if ((i + j) % 2 == 0) row[j] = b_s;
			else row[j] = w_s;
		}
		desk.push_back(row);
	}
	for (auto check : position.white_checks) desk[check[1] - '1'][check[0] - 'a'] = w_c;
	for (auto check : position.black_checks) desk[check[1] - '1'][check[0] - 'a'] = b_c;
	for (auto check : position.white_queens) desk[check[1] - '1'][check[0] - 'a'] = w_q;
	for (auto check : position.black_queens) desk[check[1] - '1'][check[0] - 'a'] = b_q;
}

void Deck::printDeck() {

	cout << "  ABCDEFGH" << endl;

	for (int row = row_num - 1; row >= 0; --row) {
		cout << row + 1 << " ";
		for (int col = 0; col < col_num; ++col) {
			switch (desk[row][col]) {
			case b_s:
				cout << ' ';
				break;
			case w_s:
				cout << char(219);
				break;
			case w_c:
				cout << 'w';
				break;
			case b_c:
				cout << 'b';
				break;
			case w_q:
				cout << 'W';
				break;
			case b_q:
				cout << 'B';
				break;
			}
		}
		cout << endl;
	}
}

string Deck::moveToString(const Move &move) {
	string rez = "";
	for (int i = 0; i < move.notation.size() - 1; ++i) {
		pair<int, int> coords = move.notation[i];
		rez += char('a' + coords.second) + to_string(coords.first + 1) + "-";
	}
	rez += char('a' + move.notation.back().second) + to_string(move.notation.back().first + 1);
	return rez;
}

string Deck::moveOptionsToString(const vector<Move> &options) {
	string rez = "";
	if (options.empty()) return rez;
	for (int i = 0; i < options.size() - 1; ++i) {
		rez += moveToString(options[i]) + ", ";
	}
	rez += moveToString(options.back());
	return rez;
}


Move Deck::isValidRecord(const string& s) {

	Move rez, no_ans;

	int row = 0, col = 0;
	if (s.size() % 3 != 2) return no_ans;
	for (int i = 0; i < s.size(); ++i) {
		if (i % 3 == 0) {
			row = s[i] - 'a';
			if (row < 0 || row > row_num - 1) return no_ans;
		}
		if (i % 3 == 1) {
			col = s[i] - '1';
			if (col < 0 || col > col_num - 1) return no_ans;
		}
		if (i % 3 == 2) {
			if (s[i] != '-') return no_ans;
			rez.notation.push_back({ col, row });
		}
	}
	rez.notation.push_back({ col, row });
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
	pair<square, square> prey,
	Move &eat_step_queue
)
{
	if ((desk[field.first][field.second] == prey.first ||
		desk[field.first][field.second] == prey.second) &&
		!any_of(eat_step_queue.eaten.begin(), eat_step_queue.eaten.end(),
			[field](Eaten el) { return el.coords == field; })) return true;
	return false;
}

bool Deck::isNextFieldFree(pair<int, int> field)
{
	if (desk[field.first][field.second] == b_s) return true;
	return false;
}

void Deck::moveFigure(pair<int, int> killer, pair<int, int> field, square figure) {
	desk[field.first][field.second] = figure;
	desk[killer.first][killer.second] = b_s;
}

bool Deck::canEatThisDirection(
	const pair<square, square> &hunter,
	const pair<square, square> &prey,
	pair<int, int> &killer,
	Move &eat_step_queue,
	direction dir
)
{
	int x = killer.first, y = killer.second;
	int x_off, y_off;
	directionOffsets(dir, x_off, y_off);

	// Current field that we check to find a check that can be eaten
	pair<int, int> step = { x + x_off, y + y_off };
	// Field after that field on same direction
	pair<int, int> overstep = { x + x_off + x_off, y + y_off + y_off };
	if (isFieldWithinDeck(overstep) &&
		isPreyOnField(step, prey, eat_step_queue) &&
		isNextFieldFree(overstep)) {

		if (desk[x][y] == w_c && overstep.first == row_num - 1 ||
			desk[x][y] == b_c && overstep.first == 0) {
			eat_step_queue.checkToQueen = true;
		}
		eat_step_queue.notation.push_back(overstep);
		Eaten eaten;
		eaten.figure = desk[step.first][step.second];
		eaten.coords = step;
		eat_step_queue.eaten.push_back(eaten);
		return true;
	}
	return false;
}

void Deck::canCheckEatFurther(
	const pair<square, square> &hunter,
	const pair<square, square> &prey,
	pair<int, int> &killer,
	Move &eat_step_queue
)
{
	bool isSomethingToEat = false;
	int dir_color = 0;
	if (turn == false) {
		dir_color = 2;
	}
	for (int dir_int = dir_color; dir_int < dir_color+2; ++dir_int) {
		direction dir = static_cast<direction>(dir_int);
		if (canEatThisDirection(hunter, prey, killer, eat_step_queue, dir)) {
			isSomethingToEat = true;
			pair<int, int> field = eat_step_queue.notation.back();
			moveFigure(killer, field, hunter.first);
			canCheckEatFurther(hunter, prey, field, eat_step_queue);
			moveFigure(field, killer, hunter.first);
			eat_step_queue.notation.pop_back();
			eat_step_queue.eaten.pop_back();
		}
	}

	if (!isSomethingToEat && !eat_step_queue.eaten.empty()) {
		move_options.push_back(eat_step_queue);
	}
}

void Deck::canQueenEatFurther(
	const pair<square, square> &hunter,
	const pair<square, square> &prey,
	pair<int, int> &killer,
	Move &eat_step_queue
)
{
	vector<pair<int, int>> canEat;
	bool isSomethingToEat = false;
	for (int dir_int = 0; dir_int < 4; ++dir_int) {
		direction dir = static_cast<direction>(dir_int);
		if (canEatThisDirection(hunter, prey, killer, eat_step_queue, dir)) {
			isSomethingToEat = true;
			pair<int, int> field = eat_step_queue.notation.back();
			moveFigure(killer, field, hunter.second);
			canQueenEatFurther(hunter, prey, field, eat_step_queue);
			moveFigure(field, killer, hunter.second);
			eat_step_queue.notation.pop_back();
			eat_step_queue.eaten.pop_back();
		}
	}

	if (!isSomethingToEat && !eat_step_queue.eaten.empty()) {
		move_options.push_back(eat_step_queue);
	}
}

void Deck::findEatOptions() {

	move_options.clear();

	pair<square, square> hunter, prey;
	if (turn) {
		hunter = { w_c, w_q };
		prey = { b_c, b_q };
	}
	else {
		hunter = { b_c, b_q };
		prey = { w_c, w_q };
	}
	for (int row = 0; row < row_num; ++row) {
		for (int col = row % 2; col < col_num; col += 2) {
			pair<int, int> field = { row, col };
			Move eat_step_queue;
			eat_step_queue.checkToQueen = false;
			eat_step_queue.notation.push_back(field);
			if (desk[row][col] == hunter.first) {
				canCheckEatFurther(hunter, prey, field, eat_step_queue);
			}
			else if (desk[row][col] == hunter.second) {
				canQueenEatFurther(hunter, prey, field, eat_step_queue);
			}
		}
	}
}

bool Deck::isValidMove(Move move, int &ind) {

	for (int i = 0; i < move_options.size(); ++i) {
		if (move_options[i].notation == move.notation) {
			ind = i;
			return true;
		}
	}
	return false;
}

void Deck::canMoveThisDirection(pair<int, int> figure, direction dir) {
	int x = figure.first, y = figure.second;
	int x_off, y_off;
	directionOffsets(dir, x_off, y_off);
	pair<int, int> step = { x + x_off, y + y_off };
	if (isFieldWithinDeck(step) && desk[step.first][step.second] == b_s) {
		
		Move move;
		if (desk[x][y] == w_c && step.first == row_num - 1 ||
			desk[x][y] == b_c && step.first == 0) {
			move.checkToQueen = true;
		}
		else {
			move.checkToQueen = false;
		}
		move.notation = { figure, step };
		move_options.push_back(move);
	}
}

void Deck::findMoveOptions() {

	move_options.clear();

	square hunter_check = turn ? w_c : b_c;
	square hunter_queen = turn ? w_q : b_q;
	int dir_color = 0;
	if (turn == false) {
		dir_color = 2;
	}
	for (int row = 0; row < row_num; ++row) {
		for (int col = row % 2; col < col_num; ++col) {
			if (desk[row][col] == hunter_check) {
				pair<int, int> figure = { row, col };
				for (int dir_int = dir_color; dir_int < dir_color + 2; ++dir_int) {
					direction dir = static_cast<direction>(dir_int);
					canMoveThisDirection(figure, dir);
				}
			}
			else if (desk[row][col] == hunter_queen) {
				pair<int, int> figure = { row, col };
				for (int dir_int = 0; dir_int < 4; ++dir_int) {
					direction dir = static_cast<direction>(dir_int);
					canMoveThisDirection(figure, dir);
				}
			}
		}
	}
}

void Deck::makeMove(int ind, bool reverseMove) {

	if (reverseMove) turn = !turn;

	square hunter_check = turn ? w_c : b_c;
	square hunter_queen = turn ? w_q : b_q;

	Move move = move_options[ind];
	pair<int, int> before = !reverseMove ? move.notation.front() : move.notation.back();
	pair<int, int> after = !reverseMove ? move.notation.back() : move.notation.front();

	if (desk[before.first][before.second] == hunter_check) {
		if (move.checkToQueen) {
			desk[after.first][after.second] = hunter_queen;
		}
		else {
			desk[after.first][after.second] = hunter_check;
		}
	}
	else {
		desk[after.first][after.second] = hunter_queen;
	}
	if (reverseMove && move.checkToQueen) desk[after.first][after.second] = hunter_check;

	if (before != after) desk[before.first][before.second] = b_s;

	if (!reverseMove) {
		for (auto eaten : move.eaten) {
			desk[eaten.coords.first][eaten.coords.second] = b_s;
		}
	}
	else {
		for (auto eaten : move.eaten) {
			desk[eaten.coords.first][eaten.coords.second] = eaten.figure;
		}
	}
	if (!reverseMove) turn = !turn;
}

void Deck::playWithPlayer() {

	string string_move;
	int ind_move = 0;
	findEatOptions();
	if (move_options.empty()) {
		findMoveOptions();
	}
	cout << moveOptionsToString(move_options) << endl;

	while (true) {
		cout << "Enter your move:" << endl;
		cin >> string_move;
		notation_move = isValidRecord(string_move);
		if (notation_move.notation.empty()) {
			cout << "Invalid move notation!" << endl;
			continue;
		}
		if (!isValidMove(notation_move, ind_move)) {
			cout << "Invalid move! Check out right moves!" << endl;
			continue;
		}
		makeMove(ind_move);
		printDeck();

		turn = !turn;
		findEatOptions();
		if (move_options.empty()) {
			findMoveOptions();
			if (move_options.empty()) {
				break;
			}
		}
		cout << moveOptionsToString(move_options) << endl;
	}

	if (turn) cout << "BLACK WINS!!!";
	else cout << "WHITE WINS!!!";
}

int Deck::evaluatePosition() {
	int res = 0;
	const int checkValue = 8;
	const int figureNotOnBorder = 2, figureNotNearBorder = 1;// checkControlCenter = 2;
	const int checkAdvancement = 1;
	const int queenValue = 16;
	
	for (int row = 0; row < row_num; ++row) {
		for (int col = row % 2; col < col_num; col += 2) {
			square field = desk[row][col];
			if (field == b_s) continue;
			int val = 0;
			if (field == w_c || field == b_c)
				val = checkValue + (turn ? row : row_num - 1 - row) * checkAdvancement;
			else
				val = queenValue;
			if (row != 0 && row != row_num - 1) {
				val += figureNotOnBorder;
				if (row != 1 && row != row_num - 2) {
					val += figureNotNearBorder;
				}
			}
			if (col != 0 && col != col_num - 1) {
				val += figureNotOnBorder;
				if (col != 1 && col != col_num - 2) {
					val += figureNotNearBorder;
				}
			}

			if (field == w_c || field == w_q) {
				res += val;
			} else if (field == b_c || field == b_q) {
				res -= val;
			}
		}
	}
	return res;
}

int Deck::runMiniMax(int recursive_level, int alpha, int beta) {
	if (recursive_level >= 2 * AI_level) {
		return evaluatePosition();
	}
	findEatOptions();
	if (move_options.empty()) {
		findMoveOptions();
		if (move_options.empty()) {
			return turn ? MIN_VALUE : MAX_VALUE;
		}
	}
	int MinMax = turn ? MIN_VALUE : MAX_VALUE;
	int bestMove = NOT_INITIALIZED;
	int test = NOT_INITIALIZED;
	vector<Move> save_move_options = move_options;
	for (int move_ind = 0; move_ind < save_move_options.size(); ++move_ind) {
		makeMove(move_ind);
//		cout << "Variant: " << moveToString(move_options[move_ind]) << endl;
//		printDeck();
		test = runMiniMax(recursive_level + 1, alpha, beta);
		move_options = save_move_options;
		makeMove(move_ind, true);
//		cout << "Variant back: " << moveToString(move_options[move_ind]) << endl;
//		cout << turn << endl;
//		printDeck();

		if ((test > MinMax && turn) ||
			(test <= MinMax && !turn))
		{
			MinMax = test;
			bestMove = move_ind;
		}

		if (turn && alpha < test)
			alpha = test;
		else if (!turn && beta > test)
			beta = test;
		if (beta < alpha)
			break;
	}

	if (bestMove == NOT_INITIALIZED) {
		return evaluatePosition();
	}

	if (recursive_level == 0 && bestMove != NOT_INITIALIZED) {
		makeMove(bestMove);
	}
	return MinMax;
}

void Deck::playWithAI() {

	printDeck();
	string string_move;
	int ind_move = 0;
	findEatOptions();
	if (move_options.empty()) {
		findMoveOptions();
	}
	cout << moveOptionsToString(move_options) << endl;

	while (true) {
		if (turn == false) {
			runMiniMax(0, MIN_VALUE, MAX_VALUE);
		}
		else {
			cout << "Enter your move:" << endl;
			cin >> string_move;
			notation_move = isValidRecord(string_move);
			if (notation_move.notation.empty()) {
				cout << "Invalid move notation!" << endl;
				continue;
			}
			if (!isValidMove(notation_move, ind_move)) {
				cout << "Invalid move! Check out right moves!" << endl;
				cout << moveOptionsToString(move_options) << endl;
				continue;
			}
			makeMove(ind_move);
		}
		printDeck();

		findEatOptions();
		if (move_options.empty()) {
			findMoveOptions();
			if (move_options.empty()) {
				break;
			}
		}
		cout << moveOptionsToString(move_options) << endl;
	}

	if (turn) cout << "BLACK WINS!!!";
	else cout << "WHITE WINS!!!";
}

int main() {

	//Position pos(true, { "c3" }, {  }, {  }, { "c7" });
	//Deck deck(pos);
	Deck deck;

	deck.playWithAI();
	string s;
	cin >> s;

	return 0;
}