#pragma once

using std::vector;
using std::pair;
using std::set;
using std::string;
using std::cin;
using std::cout;
using std::endl;

struct Position {
	bool turn;
	vector<string> white_checks;
	vector<string> black_checks;
	vector<string> white_queens;
	vector<string> black_queens;
	Position() {}
	Position(bool p_turn, vector<string> p_wc, vector<string> p_bc, vector<string> p_wq, vector<string> p_bq) :
		turn(p_turn), white_checks(p_wc), black_checks(p_bc), white_queens(p_wq), black_queens(p_bq) {

	}
};

// white square, black square, white check, black check, white queen, black queen
enum square { w_s, b_s, w_c, b_c, w_q, b_q };
enum direction { up_left, up_right, down_left, down_right };
struct Eaten {
	square figure;
	pair<int, int> coords;
};
struct Move {
	bool checkToQueen;
	vector<pair<int, int>> notation;
	vector<Eaten> eaten;
};

class Deck {

	const int row_num = 8, col_num = 8;
	const int AI_level = 3;

	bool turn;

	vector<vector<square>> desk;
	Move notation_move;
	vector<Move> move_options;

public:
	Deck();
	Deck(Position position);
	// Testing
	vector<Move> GetMoveOptions() {
		return move_options;
	}
	vector<vector<square>> GetDesk() {
		return desk;
	}

	void printDeck();
	string moveToString(const Move &move);
	string moveOptionsToString(const vector<Move> &options);
	void directionOffsets(direction dir, int &x_off, int &y_off);
	Move isValidRecord(const string &s);

	bool isFieldWithinDeck(pair<int, int> field);
	bool isPreyOnField(
		pair<int, int> field,
		pair<square, square> prey,
		Move &eat_step_queue
	);
	bool isNextFieldFree(pair<int, int> field);
	void moveFigure(pair<int, int> killer, pair<int, int> field, square figure);

	bool canEatThisDirection(
		const pair<square, square> &hunter,
		const pair<square, square> &prey,
		pair<int, int> &killer,
		Move &eat_step_queue,
		direction dir
	);
	void canCheckEatFurther(
		const pair<square, square> &hunter,
		const pair<square, square> &prey,
		pair<int, int> &killer,
		Move &eat_step_queue
	);
	void canQueenEatFurther(
		const pair<square, square> &hunter,
		const pair<square, square> &prey,
		pair<int, int> &killer,
		Move &eat_step_queue
	);
	bool isValidMove(Move move, int &ind);
	void makeMove(int ind, bool reverseMove = false);
	void findEatOptions();
	void canMoveThisDirection(pair<int, int> figure, direction dir);
	void findMoveOptions();

	void playTheGame();

	int evaluatePosition();
	//int runMiniMax(int recursive_level, int alpha, int beta);
};
