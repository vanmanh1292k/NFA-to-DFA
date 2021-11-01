#include <iostream>
#include <vector>
#include <set> //lưu trữ các phần tử không bị trùng lặp 
#include <algorithm>
#include <queue>
#include <bitset>
#include <iomanip>
#include <fstream>

#define MAX_NFA_STATE 20 // trạng thái max 
#define MAX_ALPHABET_SIZE 10 // kí tự đầu vào max

using namespace std;

// tạo 1 class NFA state 
class NFAstate {
public:
	//2-мерный массив для сохранения таблицы переходов
	int transitions[MAX_ALPHABET_SIZE][MAX_NFA_STATE]; // tạo 1 mảng số nguyên 2 chiều (chuyển đổi)
	
	// tạo 1 hàm chuyển 
	//конструктор, состояние инициализации
	NFAstate() {
		for (int i = 0; i < MAX_ALPHABET_SIZE; ++i) {
			for (int j = 0; j < MAX_NFA_STATE; ++j) {
				transitions[i][j] = -1;
			}
		}
	}
} *NFAstates; // tạo 1 con trỏ trỏ tới class NFAstates 

// tạo 1 strusct (cấu trúc) trạng thái DFA 
struct DFAstate {
	bool final_state; //Проверьте, является ли конечное состояние. //khởi tạo biến (trạng thái cuối) kiểu bool 
	bitset<MAX_NFA_STATE> closureNFAstate; // Сохраняет состояние, достижимое через ϵ - closure.// phương thức khởi tạo mặc định với tất cả các bit 0 ( bao đóng trạng thái NFA)
	bitset<MAX_NFA_STATE> transitions[MAX_ALPHABET_SIZE]; //Сохраняет состояние,  достижимое через move(T,a). // mảng 1 chiều(chuyển) được khởi tạo mặc định với tất cả các bit 0
	int symbolicTransition[MAX_ALPHABET_SIZE]; // khởi tạo mảng 1 chiều (chuyển đổi tượng trưng )
};

set<int> NFA_finalStates; //tạo 1 giá trị set NFA_finalStates (vùng chứa liên kết) (trạng thái cuối NFA) kiểu int 
vector<int> DFA_finalStates; // tạo 1 vector DFA_finalStates (trạng thái cuối DFA) kiểu int
vector<DFAstate*> DFAstates; // tạo 1 vector DFAstates (trạng thái DFA) kiểu cấu trúc DFAstate
queue<int> incompleteDFAstates; // tạo 1 hàng đợi incompleteDFAstates (trạng thái DFA không hoàn chỉnh) kiểu int 
int N, M;

// For 1 state with epsilon closure // Tập hợp các trạng thái NFA có thể truy cập được từ trạng thái s cho một epsilon chuyển 
// Множество состояний НКА, достижимых из состояния s при одном ϵ-переход
// epsilonClosure(0, DFAstates[0]->closureNFAstate)   // DFAstates[0]->closureNFAstate = 1
void epsilonClosure(int state, bitset<MAX_NFA_STATE>& closure) {
	for (int i = 0; i < N && NFAstates[state].transitions[0][i] != -1; ++i) {
		if (closure[NFAstates[state].transitions[0][i]] == 0) {
			closure[NFAstates[state].transitions[0][i]] = 1;
			epsilonClosure(NFAstates[state].transitions[0][i], closure);
		}
	}
}

// For set of state with epsilon closure // Tập hợp các trạng thái NFA có thể truy cập được từ trạng thái s từ tập hợp T cho một epsilon chuyển 
// Множество состояний НКА, достижимых из состояния s из множества T  при одном ϵ-переход
void epsilonClosure(bitset<MAX_NFA_STATE> state, bitset<MAX_NFA_STATE>& closure) {
	for (int i = 0; i < N; ++i) {
		if (state[i] == 1) {
			epsilonClosure(i, closure);
		}
	}
}

// Return bitset of state NFA could be in after moving from "previous_state" with "symbol"
// Множество состояний НКА, в которые иммется переход из состояния s при входном симболе a
//Tập hợp các trạng thái NFA, trong đó có sự chuyển đổi từ trạng thái s tại ký hiệu đầu vào a
void NFAmove(int previous_state, int symbol, bitset<MAX_NFA_STATE>& y) {
	for (int i = 0; i < N && NFAstates[previous_state].transitions[symbol][i] != -1; ++i) {
		y[NFAstates[previous_state].transitions[symbol][i]] = 1;
	}
}

// Return bitset of state NFA could be in after moving from set of "previous_state" with "symbol"
// Множество состояний НКА, в которые иммется переход из некоторого состояния s Є T при входном симболе a
// Tập hợp các trạng thái NFA, trong đó có sự chuyển đổi từ một số trạng thái s tại ký hiệu đầu vào a
void NFAmove(bitset<MAX_NFA_STATE> previous_state, int symbol, bitset<MAX_NFA_STATE>& y) {
	for (int i = 0; i < N; ++i) {
		if (previous_state[i] == 1) {
			NFAmove(i, symbol, y);
		}
	}
}


//функция для распечатки НКА состоянии // truyền vào tham 1 mảng tham số NFA[] thuộc class NFAstate 
void print(NFAstate NFA[]) {
	cout << setw(20) << "STATE/INPUT  |";
	char a = 'a';
	for (int i = 0; i < M; ++i) {
		cout << setw(10) << a++ << "  |";
	}
	cout << setw(10) << "^";
	cout << "  |" << endl << endl;

	for (int i = 0; i < N; ++i) {
		cout << setw(17) << i << "  |";
		for (int j = 1; j <= M; ++j) {
			cout << setw(10);
			bool check = false;
			for (int k = 0; k < N; ++k) {
				if (NFA[i].transitions[j][k] != -1) {
					check = true;
					cout << "(" << NFA[i].transitions[j][k] << ")";
				}
			}
			if (!check) {
				cout << "-";
			}
			cout << "  |";
		}

		cout << setw(10);
		bool check2 = false;
		for (int k = 0; k < N; ++k) {
			if (NFA[i].transitions[0][k] != -1) {
				check2 = true;
				cout << "(" << NFA[i].transitions[0][k] << ")";
			}
		}
		if (!check2) {
			cout << " - ";
		}
		cout << "  |" << endl;
	}
}

//главный алгоритм программы.
int main() {
	int F, f;

	cout << "************************* NFA to DFA *************************" << endl << endl;
	cout << "Enter total number of states in NFA : ";
	cin >> N;
	cout << "Enter number of elements in alphabet : ";
	cin >> M;
	cout << "Enter number of final state in NFA: ";
	cin >> F;
	cout << "Enter " << F << " final state in NFA: ";
	for (int i = 0; i < F; ++i) {
		cin >> f;
		NFA_finalStates.insert(f); //thêm phần các trạng thái cuối vào vùng chứa liên kết NFA_finalStates
	}
	cout << "\n\n";

	NFAstates = new NFAstate[N]; // cấp phát động cho 1 mảng các trạng thái NFAstate , con trỏ trỏ đến mảng là vị trí phần tử đầu tiên của mảng 
	for (int i = 0; i < N; i++) {
		cout << "For state " << i << endl;
		char a = 'a';
		int y, yn;
		for (int j = 0; j < M; j++) {
			cout << "Enter no. of output states for input " << a << ": ";
			a++;
			cin >> yn;
			cout << "Enter output states :" << endl;
			for (int k = 0; k < yn; k++) {
				cin >> y;
				NFAstates[i].transitions[a - 'a'][k] = y; // (trạng thái i). (chuyển đổi trạng thái [ b - 'a'][y])
			}
		}
		cout << "Enter no. of output states for input ^ : ";
		cin >> yn;
		cout << "Enter output states :" << endl;
		for (int k = 0; k < yn; k++) {
			cin >> y;
			NFAstates[i].transitions[0][k] = y;
		}
		cout << endl;
	}

	// read in the underlying NFA
	/*int i, X, T, A, Y, j;
	ifstream fin("NFA.txt");
	fin >> N >> M;
	NFAstates = new NFAstate[N];
	fin >> F;
	for (i = 0; i < F; i++) {
		fin >> X;
		NFA_finalStates.insert(X);
	}
	fin >> T;
	while (T--) {
		fin >> X >> A >> Y;
		for (i = 0; i < Y; i++) {
			fin >> j;
			NFAstates[X].transitions[A][i] = j;
		}
	}
	fin.close();*/

	cout << "\n\n***** TRANSITION TABLE OF NFA *****\n";
	print(NFAstates);

	// Start building DFA table // bat dau tao DFA 
	int D = 1;
	DFAstates.push_back(new DFAstate); // day mot phan tu moi vua duoc cap phat dong vao cuoi vector
	DFAstates[0]->closureNFAstate[0] = 1; //phần tử [0] của vector trỏ đến bộ bit bằng 1 ;
	epsilonClosure(0, DFAstates[0]->closureNFAstate); // (không có giá trị trả về) Tập hợp các trạng thái NFA có thể truy cập được từ trạng thái s từ tập hợp T cho một epsilon chuyển

	// Check 0 is final state in DFA
	for (int i = 0; i < N; ++i) {
		if (DFAstates[0]->closureNFAstate[i] == 1 && NFA_finalStates.find(i) != NFA_finalStates.end()) {
			DFAstates[0]->final_state = true;
			DFA_finalStates.push_back(0);
			break;
		}
	}

	int j;
	incompleteDFAstates.push(0); // thêm ph tử 0 và queue
	while (!incompleteDFAstates.empty()) { // nếu hàng đợi ko rỗng 
		int s = incompleteDFAstates.front(); // gán s là ph tử đầu tiên của hàng đợi( s = 0) 
		incompleteDFAstates.pop(); // xóa phần tử cuối trong hàng đợi 

		for (int i = 1; i <= M; ++i) {
			// Kiểm tra xem từ state s có thể đi đến các state nào thông qua i symbol
			NFAmove(DFAstates[s]->closureNFAstate, i, DFAstates[s]->transitions[i]);
			// Và các state có thể đến từ state ở trên thông qua epsilon
			epsilonClosure(DFAstates[s]->transitions[i], DFAstates[s]->transitions[i]);
			cout << "DFAstates[s]->closureNFAstate : " << "s = " << s << "\t" << DFAstates[s]->closureNFAstate << endl;

			cout << "DFAstates[s]->transitions[i] : " << "s = " << s << "\t" << "i = " << i << "\t" << DFAstates[s]->transitions[i] << endl << endl;
			for (j = 0; j < D; ++j) {
				if (DFAstates[s]->transitions[i] == DFAstates[j]->closureNFAstate) {
					DFAstates[s]->symbolicTransition[i] = j;
					break;
				}
			}
			if (j == D) {
				DFAstates[s]->symbolicTransition[i] = D;
				DFAstates.push_back(new DFAstate);
				DFAstates[D]->closureNFAstate = DFAstates[s]->transitions[i];

				for (j = 0; j < N; ++j) {
					if (DFAstates[D]->closureNFAstate[j] == 1 && NFA_finalStates.find(j) != NFA_finalStates.end()) {
						DFAstates[D]->final_state = true;
						DFA_finalStates.push_back(D);
						break;
					}
				}
				incompleteDFAstates.push(D);
				D++;
			}
		}
	}

	cout << "\n\n***** TRANSITION TABLE OF DFA *****\n";
	char a = 'a';
	cout << setw(20) << "STATE/INPUT  |";
	for (int i = 0; i < M; ++i) {
		cout << setw(10) << a++ << "  |";
	}
	cout << setw(10) << "FINAL  |";
	cout << endl;

	char z = 'A';
	for (int i = 0; i < D; ++i) {
		cout << setw(17) << char(z + i) << "  |";
		for (int j = 1; j <= M; ++j) {
			cout << setw(10) << char(z + DFAstates[i]->symbolicTransition[j]) << "  |";
		}

		bool flag = false;
		for (auto x : DFA_finalStates) {
			if (i == x) {
				flag = true;
				cout << setw(10) << "1  |";
				break;
			}
		}
		if (!flag) {
			cout << setw(10) << "0  |";
		}
		cout << endl;
	}


	system("pause");
	return 0;
}