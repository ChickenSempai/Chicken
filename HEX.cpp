
//

#include <iostream>
#include <vector>
#include <time.h>
#include <random>
template <class RandomAccessIterator, class URNG>
void shuffl(RandomAccessIterator first, RandomAccessIterator last, URNG&& g)
{
	for (auto i = (last - first) - 1; i > 0; --i) {
		std::uniform_int_distribution<decltype(i)> d(0, i);
		std::swap(*(first[i]), *(first[d(g)]));
	}
}
enum class tpHex : short { Vert, Horiz, Empty = SHRT_MAX };
enum {Up_Left, Up_Right, Right, Lower_Right, Lower_Left, Left};

template <typename T>
class graph  {
public:
	graph (int nodes, T val) :node_val(nodes, val) {}
	graph(const graph& g) :node_val(g.node_val) {}
	virtual ~graph() {}
	int all_nodes() const { return node_val.size(); }
	T get_node(int x) const { return node_val[x]; }
	void set_node(int x, T val) { node_val[x] = val; }
	T operator[](int x) const { return node_val[x]; }
	std::vector<T>* get_pt() { return &node_val; }
	virtual void set_edge(int x, int a) = 0;
	virtual int get_edges(int x) = 0;
private:
	std::vector <int> edges;
protected:
	std::vector <T> node_val;
};

class _hex : public graph <tpHex> {
public:
	_hex(int sideLen) :graph(sideLen* sideLen, tpHex::Empty),
								edges(sideLen* sideLen, std::vector <int>(6, -1)), dimens(sideLen) {}
	_hex(const _hex& h) :graph(h), edges(h.edges), dimens(h.side()) {}
	void disp_hex();
	int get_edges(int x);
	int side() const { return dimens; }
	bool is_right(int pos)const { return pos % dimens == dimens - 1; }
	bool is_lower(int pos)const { return pos >= dimens * (dimens - 1); }
	bool is_left(int pos)const { return pos % dimens == 0; }
	bool is_upper(int pos)const { return pos < dimens; }
private:
	int dimens;
	std::vector <std::vector<int>> edges;
protected:
	void set_edge(int x, int a) {};
};


inline void _hex::disp_hex()
{
	using namespace std;
	for (int i = 0; i < dimens; i++) {
		for (int j = 0; j < (dimens - i); j++)
			cout << "  ";
		for (int k = 0; k < dimens; k++) {
			switch (this->get_node(k + i * dimens)) {
			case tpHex::Empty :
				cout << " . ";
				break;
			case tpHex::Horiz :
				cout << " X ";
				break;
			case tpHex::Vert :
				cout << " 0 ";
			}
			if (k < (dimens - 1))
				cout << '-';
		}
		cout << i + 1;
		cout << endl;
		for (int j = 0; j < (dimens - i); j++)
			cout << "  ";
		if (i < (dimens - 1)) {
			for (int k = 0; k < dimens; k++) {
				if (k == (dimens - 1))
					cout << "/";
				else cout << "/ \\ ";
			}
			cout << endl;
		}
	}
	for (int i = 0; i < dimens; i++) {
		if (i < 10)
			cout << ' ' << i + 1 << ' ' << ' ';
		else cout << i + 1 << ' ' << ' ';
	}
	cout << endl;
}

int _hex::get_edges(int x)
{
	static bool reinitialize=true;
	static int it;
	if (reinitialize) {
		it = Up_Left;
		reinitialize = false;
	}
	while (it <= Left) {
		it++;
		if (edges[x][it - 1] != -1)
			return edges[x][it - 1];
	}
	reinitialize = true;
	return -1;
}

class actor {
public:
	actor(tpHex n) : color(n) {};
	virtual ~actor() {}
	void set_color(tpHex n) { color = n; }
	tpHex get_color() const{ return color; }
	virtual void move(_hex& board) = 0;
protected:
	tpHex color;

};

class Human : public actor {
public:
	Human(tpHex color) : actor(color) {};
	~Human() {}
	void move(_hex& board)
	{
		int x = -1, y = -1, temp;
		static bool first = true;
		if (first) {
			do {
				std::cout << "Enter row and column: ";
				std::cin >> x >> y;
			} while ((temp = (x - 1) * board.side() + (y - 1)) >= board.all_nodes() || x <= 0 || y <= 0 || x > board.side() || y > board.side());
			first = false;
		}
		else 
			do {
				std::cout << "Enter row and column: ";
				std::cin >> x >> y;
			} while ((temp = (x - 1) * board.side() + (y - 1)) >= board.all_nodes() ||
				board[temp] != tpHex::Empty || x <= 0 || y <= 0 || x > board.side() || y > board.side());

		board.set_node(temp, color);
		return;
	}
};

class AI_weak : public actor {
public:
	AI_weak(tpHex color) : actor(color) {};
	~AI_weak() {}
	void move(_hex& board)
	{
		int i;
		do
			i = rand() % board.all_nodes();
		while (board[i] != tpHex::Empty);
		board.set_node(i, color);
	}
};

inline tpHex test_win_fast(_hex& board);

class AI_MonteCarlo : public actor {
public:
	AI_MonteCarlo(tpHex color) : actor(color), testing_cases(1500) {}
	~AI_MonteCarlo() {}
	void move(_hex& board)
	{

		static bool first = true;
		if (first) {

			for (int i = board.side() / 4; i <= (board.side() * 3 / 4); i++)
				for (int j = (board.side() / 4) + i * board.side(); j <= (board.side() * 3 / 4) + i * board.side(); j++)
					if (board[j] != tpHex::Empty) {
						board.set_node(j, color);
						first = false;
						return;
					}
			for (int i = board.side() / 4; i <= (board.side() * 3 / 4); i++)
				for (int j = (board.side() / 4) + i * board.side(); j <= (board.side() * 3 / 4) + i * board.side(); j++)
					board.set_node(j, tpHex::Horiz); //temporary
		}

		std::random_device rd;
		_hex cpy_brd(board);
		std::vector <tpHex*> shuf(cpy_brd.all_nodes());
		std::vector <bool> occupied(cpy_brd.all_nodes(), false);
		int my_steps = 0, his_steps = 0;

		for (int i = 0, j = 0; i < cpy_brd.all_nodes(); i++, j++) {
			if (cpy_brd[i] != tpHex::Empty) {
				occupied[i] = true;
				shuf.erase(shuf.begin() + j);
				j--;
				if (cpy_brd[i] != color)
					my_steps++;
				else his_steps++;
			}
			else shuf[j] = &(*cpy_brd.get_pt())[i];
		}
		int best_move;
		int most_wins = -1, win_case = 0;
		int k = 0;
		if (cpy_brd.all_nodes() % 2 == 0) {
			if (his_steps > my_steps)
				*(shuf[k++]) = color;
		}

		for (; k < shuf.size() / 2; k++)
			*(shuf[k]) = tpHex::Horiz;

		if (cpy_brd.all_nodes() % 2 != 0)
			if (his_steps == my_steps)
				*(shuf[k++]) = color;

		for (; k < shuf.size(); k++)
			*(shuf[k]) = tpHex::Vert;

		tpHex* temp;
		for (int i = 0, j = 0; i < cpy_brd.all_nodes(); i++) {
			win_case = 0;
			if (occupied[i] == false) {
				if (*(shuf[j]) != color) {
					for (k = 0; *(shuf[k]) != color; k++);
					std::swap(*(shuf[j]), *(shuf[k]));
				}
				temp = shuf[j];
				shuf.erase(shuf.begin() + j);

				for (int tests = 0; tests < testing_cases; tests++) {
					shuffl(shuf.begin(), shuf.end(), std::mt19937(rd()));
					if (test_win_fast(cpy_brd) == color)
						win_case++;
				}
				if (win_case > most_wins) {
					most_wins = win_case;
					best_move = i;
				}
				shuf.insert(shuf.begin() + j, temp);
				j++;
			}
		}
		board.set_node(best_move, color);

		if (first) {
			first = false;
			for (int i = board.side() / 4; i <= (board.side() * 3 / 4); i++)
				for (int j = (board.side() / 4) + i * board.side(); j <= (board.side() * 3 / 4) + i * board.side(); j++)
					board.set_node(j, tpHex::Empty);
		}
		return;
	}
private:
	int testing_cases; //initializes at given number
};

tpHex Djikstra_path(int i, _hex& h)
{
	tpHex color = h[i];
	std::vector <bool> visited(h.all_nodes(), false);
	std::vector <int> queue = { i };
	for (; queue.size() != 0; ) {
		i = queue.back();
		queue.pop_back();
		visited[i] = true;
		if (h.is_lower(i) && color == tpHex::Vert) {
			return tpHex::Vert;
		}
		else if (h.is_right(i) && color == tpHex::Horiz) {
			return tpHex::Horiz;
		}
		for (int j = 0, k; j < 6; j++) {
			switch (j) {
			case 0:
				if (!h.is_upper(i) && !h.is_left(i)) {
					k = i - (h.side() + 1);
					break;
				}
				j++;
			case 1:
				if (!h.is_upper(i)) {
					k = i - h.side();
					break;
				}
				j++;
			case 2:
				if (!h.is_right(i)) {
					k = i + 1;
					break;
				}
				j++;
			case 3:
				if (!h.is_lower(i) && !h.is_right(i)) {
					k = i + (h.side() + 1);
					break;
				}
				j++;
			case 4:
				if (!h.is_lower(i)) {
					k = i + h.side();
					break;
				}
				j++;
			case 5:
				if (!h.is_left(i)) {
					k = i - 1;
					break;
				}
				j++;
			}
			if (j<6)
				if (!visited[k]) {
					if (h[k] == color) {
						queue.push_back(k);
					}
				}

		}
	

	}
	return tpHex::Empty;
}

inline tpHex test_win_fast(_hex& h) 
{
	for (int i = 0; i < h.side(); i++) 
		if (h[i] == tpHex::Vert)
			if (Djikstra_path(i, h) == tpHex::Vert)
				return tpHex::Vert;
	return tpHex::Horiz;
}

inline tpHex test_win_path(_hex& h)
{
	for (int i = 0, j = 0; i < h.side(); i++, j += h.side()) {
		if (h[i] == tpHex::Vert)
			if (Djikstra_path(i, h) == tpHex::Vert)
				return tpHex::Vert;
		if (h[j] == tpHex::Horiz)
			if (Djikstra_path(j, h) == tpHex::Horiz)//horiz
				return tpHex::Horiz;
	}
	return tpHex::Empty;
}

int game_hex(_hex& h, actor* rPl, actor* bPl)
{
	bool player_turn = 1;
	while (true) {
		// Choose who makes a move, give chosen player data to make a move
		if (player_turn = !player_turn)
			bPl->move(h);
		else rPl->move(h);
		std::cout << "\x1b[2J\x1b[H";
		h.disp_hex();
		// Test win condition
		switch (test_win_path(h)) {
		case tpHex::Horiz:
			std::cout << "First player won \'X\'\n";
			return 0;
		case tpHex::Vert:
			std::cout << "Second player won \'0\'\n";
			return 0;
		}
	}
}

int main()
{
	using namespace std;
	int n = -1;
//	while (n < 3 || n > 50) {
//		cout << "Enter side of the board" << endl;
//		cin >> n;
//	}
	_hex hex_brd(7);
	actor* player_first = new AI_MonteCarlo(tpHex::Horiz);
	actor* player_second = new AI_MonteCarlo(tpHex::Vert);
	cout << "First player \'" << (static_cast<int> (player_first->get_color()) ? 'X' : '0') << "\' connects horizontally" << endl;
	cout << "Second player \'" << (static_cast<int> (player_second->get_color()) ? 'X' : '0') << "\' connects vertically" << endl;

	hex_brd.disp_hex();
	game_hex(hex_brd, player_first, player_second);
	delete player_first;
	delete player_second;
	return 0;
}
