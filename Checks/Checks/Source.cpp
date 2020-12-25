#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

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
	vector<vector<square>> field;
	vector<vector<pair<int, int>>> must_eat, moves;

public:
	Deck();
	void printDeck();
	vector<pair<int, int>> isValidRecord(const string& s);
	bool canEatThisDirection(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		direction dir
	);
	void canEatFurther(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue
	);
	vector<pair<int, int>> canQueenEatThisDirection(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		direction dir
	);
	void canQueenEatFurther(
		set<pair<int, int>>& hunter,
		set<pair<int, int>>& prey,
		pair<int, int>& killer,
		vector<pair<int, int>>& eat_queue,
		vector<pair<int, int>>& eat_step_queue
	);
	void mustEat();
	bool isValidEat(const vector<pair<int, int>>& n_move);
	void possibleMoves();
	void isValidMove(const vector<pair<int, int>>& n_move);

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
			vector<pair<int, int>> n_move = isValidRecord(s_move);
			if (n_move.empty()) {
				cout << "Invalid move notation!" << endl;
				continue;
			}
			if (!must_eat.empty()) {
				if (!isValidEat(n_move)) {
					cout << "Invalid move! You must eat this turn!" << endl;
					continue;
				}
			}
			else {
				isValidMove(n_move);
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
				if (!isValidEat(n_move)) {
					cout << "Invalid move! You must eat this turn!" << endl;
					continue;
				}
			}
			else {
				possibleMoves();
				isValidMove(n_move);
			}
			printDeck();

			turn = !turn;
		}
		if (turn) cout << "BLACK WINS!!!";
		else cout << "WHITE WINS!!!";
	}
};

Deck::Deck() {

	// Add checks coordinates
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 4; ++j) {
			white.insert({ i, i % 2 + j * 2 });
			black.insert({ row_num - 1 - i, (i + 1) % 2 + j * 2 });
		}
	}

	//white = {{0, 2}};
	//black = {{1, 1}, {1, 3}, {3, 1}, {3, 3}, {3, 5}};
	//black = {{1, 1}, {1, 3}};
	white = {};
	q_white = { { 3, 7 } };
	black = { { 2, 6 },{ 3, 5 },{ 1, 3 },{ 3, 3 } };
	// Fill the field
	for (int i = 0; i < row_num; ++i) {
		vector<square> row(col_num);
		for (int j = 0; j < col_num; ++j) {
			if ((i + j) % 2 == 0) row[j] = w_s;
			else row[j] = b_s;
		}
		field.push_back(row);
	}

	// Add checks on field
	for (auto check : white) {
		field[check.first][check.second] = w_c;
	}
	for (auto check : black) {
		field[check.first][check.second] = b_c;
	}
	/*
	for (int i = 0; i < checks_num; ++i) {
	pair<int, int> cur_check = white[i];
	field[cur_check.first][cur_check.second] = w_c;
	cur_check = black[i];
	field[cur_check.first][cur_check.second] = b_c;
	}
	*/
}

void Deck::printDeck() {

	field.clear();
	for (int i = 0; i < row_num; ++i) {
		vector<square> row(col_num);
		for (int j = 0; j < col_num; ++j) {
			if ((i + j) % 2 == 0) row[j] = w_s;
			else row[j] = b_s;
		}
		field.push_back(row);
	}

	// Add checks on field
	for (auto check : white) {
		field[check.first][check.second] = w_c;
	}
	for (auto check : black) {
		field[check.first][check.second] = b_c;
	}
	for (auto check : q_white) {
		field[check.first][check.second] = w_q;
	}
	for (auto check : q_black) {
		field[check.first][check.second] = b_q;
	}

	cout << "  ABCDEFGH" << endl;

	for (int i = row_num - 1; i >= 0; --i) {
		cout << i + 1 << " ";
		for (int j = 0; j < col_num; ++j) {
			switch (field[i][j]) {
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

bool Deck::canEatThisDirection(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue,
	direction dir
)
{
	int x = killer.first, y = killer.second;
	int x_off, y_off;
	switch (dir) {
	case up_right:
		x_off = 1;
		y_off = 1;
		break;
	case up_left:
		x_off = -1;
		y_off = 1;
		break;
	case down_right:
		x_off = 1;
		y_off = -1;
		break;
	case down_left:
		x_off = -1;
		y_off = -1;
		break;
	}
	if (x + x_off * 2 < row_num && y + y_off * 2 < col_num
		&& x + x_off * 2 >= 0 && y + y_off * 2 >= 0
		&& prey.find({ x + x_off, y + y_off }) != prey.end()
		&& prey.find({ x + x_off * 2, y + y_off * 2 }) == prey.end()
		&& hunter.find({ x + x_off * 2, y + y_off * 2 }) == hunter.end())
	{
		pair<int, int> new_killer = { x + x_off * 2, y + y_off * 2 };
		hunter.erase(killer);
		prey.erase({ x + x_off, y + y_off });
		hunter.insert({ x + x_off * 2, y + y_off * 2 });

		// If it is the first eating, need to add start position
		if (eat_queue.empty()) eat_queue.push_back({ x, y });
		eat_queue.push_back({ x + x_off * 2, y + y_off * 2 });
		canEatFurther(hunter, prey, new_killer, eat_queue);
		eat_queue.pop_back();
		if (eat_queue.size() == 1) eat_queue.pop_back();

		hunter.insert(killer);
		prey.insert({ x + x_off, y + y_off });
		hunter.erase({ x + x_off * 2, y + y_off * 2 });
		return true;
	}
	return false;
}

void Deck::canEatFurther(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue
)
{
	//		for (auto el1: eat_queue) cout << char('a' + el1.second) << el1.first + 1 << "-";
	//		cout << endl;
	if (prey.empty()) {
		if (!eat_queue.empty()) must_eat.push_back(eat_queue);
		return;
	}

	bool b = false;
	b = canEatThisDirection(hunter, prey, killer, eat_queue, up_right) || b;
	b = canEatThisDirection(hunter, prey, killer, eat_queue, up_left) || b;
	b = canEatThisDirection(hunter, prey, killer, eat_queue, down_right) || b;
	b = canEatThisDirection(hunter, prey, killer, eat_queue, down_left) || b;
	/*
	cout << "Hunter: ";
	for (auto el: hunter) cout << char('a' + el.second) << el.first + 1 << " ";
	cout << endl << "Prey: ";
	for (auto el: prey) cout << char('a' + el.second) << el.first + 1 << " ";
	cout << endl;
	*/
	if (!b && !eat_queue.empty()) must_eat.push_back(eat_queue);
}

vector<pair<int, int>> Deck::canQueenEatThisDirection(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue,
	direction dir
)
{
	int x = killer.first, y = killer.second;
	int x_dir, y_dir, x_off, y_off;
	switch (dir) {
	case up_right:
		x_dir = 1;
		y_dir = 1;
		break;
	case up_left:
		x_dir = -1;
		y_dir = 1;
		break;
	case down_right:
		x_dir = 1;
		y_dir = -1;
		break;
	case down_left:
		x_dir = -1;
		y_dir = -1;
		break;
	}
	x_off = x_dir;
	y_off = y_dir;

	while (true) {
		// Current field that we check to find a check that can be eaten
		pair<int, int> step = { x + x_off, y + y_off };
		// Field after that field on same direction
		pair<int, int> overstep = { x + x_off + x_dir, y + y_off + y_dir };
		// Can't eat check on "step" field (even if there is no check on it)
		if (x + x_off + 1 >= row_num || y + y_off + 1 >= col_num
			|| x + x_off - 1 < 0 || y + y_off - 1 < 0
			|| any_of(eat_queue.begin(), eat_queue.end(),
				[step](pair<int, int> el) { return el == step; })
			|| hunter.find(step) != hunter.end()
			|| prey.find(overstep) != prey.end() && prey.find(step) != prey.end()
			|| any_of(eat_queue.begin(), eat_queue.end(),
				[overstep](pair<int, int> el) { return el == overstep; })
			|| hunter.find(overstep) != hunter.end()) {
//			if (dir == down_right && x + x_off == 2 && y + y_off == 4) {
//				cout << any_of(eat_queue.begin(), eat_queue.end(), [step](pair<int, int> el) { return el == step; }) << endl;
//				cout << (hunter.find(step) != hunter.end()) << endl << (prey.find(overstep) != prey.end()) << endl;
//			}

			return vector<pair<int, int>>();
		}
		// We can eat on "step" field, now we need to find out, is there check to eat?
		if (prey.find({ x + x_off, y + y_off }) != prey.end()) {
			eat_queue.push_back({ x + x_off, y + y_off });
			break;
		}
		x_off += x_dir;
		y_off += y_dir;
	}

	x_off += x_dir;
	y_off += y_dir;
	// Find all fields where queen can be placed after eating
	vector<pair<int, int>> rez;
	while (true) {
		pair<int, int> step = { x + x_off, y + y_off };
		pair<int, int> overstep = { x + x_off + x_dir, y + y_off + y_dir };
		if (x + x_off >= row_num || y + y_off >= col_num
			|| x + x_off < 0 || y + y_off < 0
			|| any_of(eat_queue.begin(), eat_queue.end(),
				[step](pair<int, int> el) { return el == step; })
			|| hunter.find(step) != hunter.end()) {

			return rez;
		}
		else {
			rez.push_back({ x + x_off, y + y_off });
		}
		x_off += x_dir;
		y_off += y_dir;
	}
	return rez;
}

void Deck::canQueenEatFurther(
	set<pair<int, int>>& hunter,
	set<pair<int, int>>& prey,
	pair<int, int>& killer,
	vector<pair<int, int>>& eat_queue,
	vector<pair<int, int>>& eat_step_queue
)
{
	if (eat_step_queue.empty()) {
		eat_step_queue.push_back(killer);
	}
	vector<pair<int, int>> canEat;
	bool b = false;
	for (int dirInt = 0; dirInt < 4; ++dirInt) {
		direction dir = static_cast<direction>(dirInt);
		canEat = canQueenEatThisDirection(hunter, prey, killer, eat_queue, dir);
		if (!canEat.empty()) {
			b = true;
			for (auto field : canEat) {
				cout << char('a' + field.second) << field.first + 1 << endl;
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

	if (!b && !eat_queue.empty()) must_eat.push_back(eat_step_queue);
}

set<pair<int, int>> set_union(set<pair<int, int>>& a, set<pair<int, int>>& b) {
	set<pair<int, int>> rez = a;
	for (auto el : b) {
		rez.insert(el);
	}
	return rez;
}

void Deck::mustEat() {

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
//	const_hunter = hunter;

	for (auto check : const_hunter) {

		cout << "Check: " << char('a' + check.second) << check.first + 1 << endl;
		vector<pair<int, int>> eat_queue;
		must_eat.clear();
		canEatFurther(hunter, prey, check, eat_queue);
		cout << "Can eat:" << endl;
		for (auto el : must_eat) {
			for (auto el1 : el) {
				cout << char('a' + el1.second) << el1.first + 1 << "-";
				//					cout << el1.first << el1.second << "-";
			}
			cout << endl;
		}
	}
	for (auto check : const_hunter_q) {

		cout << "Queen: " << char('a' + check.second) << check.first + 1 << endl;
		vector<pair<int, int>> eat_queue, eat_step_queue;
		must_eat.clear();
		canQueenEatFurther(hunter, prey, check, eat_queue, eat_step_queue);
		cout << "Can eat:" << endl;
		for (auto el : must_eat) {
			for (auto el1 : el) {
				cout << char('a' + el1.second) << el1.first + 1 << "-";
				//					cout << el1.first << el1.second << "-";
			}
			cout << endl;
		}
	}
}

bool Deck::isValidEat(const vector<pair<int, int>>& n_move) {

	cout << "Turn: ";
	for (auto el1 : n_move) {
		cout << char('a' + el1.second) << el1.first + 1 << "-";
		//cout << el1.first << el1.second << "-";
	}
	cout << endl;
	if (any_of(must_eat.begin(), must_eat.end(),
		[n_move](vector<pair<int, int>> el) { return el == n_move; })) {

		set<pair<int, int>> hunter, prey;
		if (turn) {
			hunter = white;
			prey = black;
		}
		else {
			hunter = black;
			prey = white;
		}
		if (turn) {
			for (int i = 1; i < n_move.size(); ++i) {
				int x = (n_move[i].first - n_move[i - 1].first) / 2;
				x += n_move[i - 1].first;
				int y = (n_move[i].second - n_move[i - 1].second) / 2;
				y += n_move[i - 1].second;
				black.erase({ x, y });
			}
			white.erase(n_move.front());
			white.insert(n_move.back());
		}
		else {
			for (int i = 1; i < n_move.size(); ++i) {
				int x = (n_move[i].first - n_move[i - 1].first) / 2;
				x += n_move[i - 1].first;
				int y = (n_move[i].second - n_move[i - 1].second) / 2;
				y += n_move[i - 1].second;
				white.erase({ x, y });
			}
			black.erase(n_move.front());
			black.insert(n_move.back());
		}
		return true;
	}
	else return false;
}

void Deck::possibleMoves() {

	moves.clear();
	set<pair<int, int>> hunter = turn ? white : black;
	int x_off = turn ? 1 : -1;
	for (auto check : hunter) {
		int x = check.first, y = check.second;
		if (x + x_off >= 0 && x + x_off < row_num && y + 1 < col_num
			&& white.find({ x + x_off, y + 1 }) == white.end()
			&& black.find({ x + x_off, y + 1 }) == black.end())
		{
			vector<pair<int, int>> step;
			step.push_back({ x, y });
			step.push_back({ x + x_off, y + 1 });
			moves.push_back(step);
		}
		if (x + x_off >= 0 && x + x_off < row_num && y - 1 >= 0
			&& white.find({ x + x_off, y - 1 }) == white.end()
			&& black.find({ x + x_off, y - 1 }) == black.end())
		{
			vector<pair<int, int>> step;
			step.push_back({ x, y });
			step.push_back({ x + x_off, y - 1 });
			moves.push_back(step);
		}
	}
}

void Deck::isValidMove(const vector<pair<int, int>>& n_move) {
	if (!any_of(moves.begin(), moves.end(),
		[n_move](vector<pair<int, int>> el) { return el == n_move; })) {
		cout << "Invalid move! Right moves: ";
		for (auto step : moves) {
			cout << char(step.front().second + 'a') << step.front().first + 1
				<< "-" << char(step.back().second + 'a') << step.back().first + 1 << ", ";
		}
		cout << endl;
		return;
	}
	if (turn) {
		white.erase(n_move.front());
		white.insert(n_move.back());
	}
	else {
		black.erase(n_move.front());
		black.insert(n_move.back());
	}
}


int main() {

	Deck deck;

	deck.printDeck();
	deck.prototype();

	return 0;
}