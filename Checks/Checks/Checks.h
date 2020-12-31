#pragma once

using std::vector;
using std::pair;
using std::set;
using std::string;
using std::cin;
using std::cout;
using std::endl;

class Deck {

	// White square, black square, white check, black check, white queen, black queen
	enum square { w_s, b_s, w_c, b_c, w_q, b_q };
	enum direction { up_right, up_left, down_right, down_left };
	const int row_num = 8, col_num = 8, checks_num = 12;

	bool turn;

	set<pair<int, int>> white;
	set<pair<int, int>> black;
	set<pair<int, int>> q_white;
	set<pair<int, int>> q_black;
	vector<vector<square>> desk;
	vector<pair<int, int>> n_move;
	vector<vector<pair<int, int>>> must_eat, eatens, moves;

public:
	Deck();
	void printDeck();
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
/*
	vector<pair<int, int>> canQueenEatThisDirection(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		direction dir
	);
*/
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
	void isValidMove();

	void prototype() {
		turn = true;
		string s_move;
		mustEat();
		if (must_eat.empty()) {
			possibleMoves();
		}
		while (true) {
			cout << "Enter yours move:" << endl;
			cin >> s_move;
			n_move = isValidRecord(s_move);
			if (n_move.empty()) {
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
		int i = 3;
		while (--i) {
			string s_move;
			cin >> s_move;

			vector<pair<int, int>> n_move = isValidRecord(s_move);
			if (n_move.empty()) {
				cout << "Invalid move notation!" << endl;
				continue;
			}
			mustEat();
			if (!must_eat.empty()) {
				if (!isValidEat()) {
					cout << "Invalid move! You must eat this turn!" << endl;
					continue;
				}
			}
			else {
				possibleMoves();
				isValidMove();
			}
			printDeck();

			turn = !turn;
		}
		if (turn) cout << "BLACK WINS!!!";
		else cout << "WHITE WINS!!!";
	}
};
