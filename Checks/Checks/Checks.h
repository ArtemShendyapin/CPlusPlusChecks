#pragma once

using std::vector;
using std::pair;
using std::set;
using std::string;
using std::cin;
using std::cout;
using std::endl;

struct Position {
	set<pair<int, int>> white_checksCheck, black_checksCheck, white_checksQueen, black_checksQueen;
};

class Deck {

	// white_checks square, black_checks square, white_checks check, black_checks check, white_checks queen, black_checks queen
	enum square { w_s, b_s, w_c, b_c, w_q, b_q };
	enum direction { up_right, up_left, down_right, down_left };
	const int row_num = 8, col_num = 8;

	bool turn;

	set<pair<int, int>> white_checks;
	set<pair<int, int>> black_checks;
	set<pair<int, int>> white_queens;
	set<pair<int, int>> black_queens;
	vector<vector<square>> desk;
	vector<pair<int, int>> notation_move;
	vector<vector<pair<int, int>>> must_eat, eatens, moves;

public:
	Deck();
	void printDeck();
	void printMove(const vector<pair<int, int>>& move);
	void printMoveOptions(const vector<vector<pair<int, int>>>& options);
	void directionOffsets(direction dir, int& x_off, int& y_off);
	vector<pair<int, int>> isValidRecord(const string& s);

	bool isFieldWithinDeck(pair<int, int> field);
	bool isPreyOnField(
		pair<int, int> field,
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		vector<pair<int, int>>& eat_queue
	);
	bool isNextFieldFree(
		pair<int, int> field,
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		vector<pair<int, int>>& eat_queue
	);

	vector<pair<int, int>> canEatThisDirection(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		direction dir
	);
	void canCheckEatFurther(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		vector<pair<int, int>>& eat_step_queue
	);
	void canQueenEatFurther(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		vector<pair<int, int>>& eat_step_queue
	);
	void mustEat();
	bool isValidEat();
	void canMoveThisDirection(pair<int, int> figure, direction dir);
	void possibleMoves();
	bool isValidMove();

	void prototype() {
		turn = true;
		string string_move;
		mustEat();
		if (must_eat.empty()) {
			possibleMoves();
		}
		while (true) {
			cout << "Enter your move:" << endl;
			cin >> string_move;
			notation_move = isValidRecord(string_move);
			if (notation_move.empty()) {
				cout << "Invalid move notation!" << endl;
				continue;
			}
			if (!must_eat.empty()) {
				if (!isValidEat()) {
					cout << "Invalid move! You must eat this turn!" << endl;
					continue;
				}
			}
			else {
				isValidMove();
			}
			printDeck();
		}
	}

	void playTheGame() {

		turn = true;
		string string_move;
		mustEat();
		if (must_eat.empty()) {
			possibleMoves();
			printMoveOptions(moves);
		}
		else {
			printMoveOptions(must_eat);
		}

		while (true) {
			cout << "Enter your move:" << endl;
			cin >> string_move;
			notation_move = isValidRecord(string_move);
			if (notation_move.empty()) {
				cout << "Invalid move notation!" << endl;
				continue;
			}
			if (!must_eat.empty()) {
				if (!isValidEat()) {
					cout << "Invalid move! You must eat this turn!" << endl;
					continue;
				}
			}
			else {
				if (!isValidMove()) {
					cout << "Invalid move!" << endl;
					continue;
				}
			}
			printDeck();

			turn = !turn;
			mustEat();
			if (must_eat.empty()) {
				possibleMoves();
				printMoveOptions(moves);
				if (moves.empty()) {
					break;
				}
			}
			else {
				printMoveOptions(must_eat);
				printMoveOptions(eatens);
			}
		}

		if (turn) cout << "BLACK WINS!!!";
		else cout << "WHITE WINS!!!";
	}
};
