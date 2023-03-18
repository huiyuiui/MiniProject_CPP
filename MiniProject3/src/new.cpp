#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <cassert>
#include <set>
#include <map>
#define INF 0x7FFFFFFF
#define P std::pair<int,int>
struct Point {
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    float x, y;
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

class Board {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
    int get_next_player(int player) const {
        return 3 - player;
    }
private:
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)return false;
        for (Point dir: directions) {
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    Board(std::array<std::array<int, SIZE>, SIZE> cur_board , int player) {
      int black=0,white=0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = cur_board[i][j];
                if(board[i][j]==1)black++;
                else white++;
            }
        }
        cur_player = player;
        disc_count[EMPTY] = 64-black-white;
        disc_count[BLACK] = black;
        disc_count[WHITE] = white;
        next_valid_spots = get_valid_spots();
    }
    Board(Board const&cur_board) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = cur_board.board[i][j];
            }
        }
        cur_player = cur_board.cur_player;
        disc_count[EMPTY] = cur_board.disc_count[EMPTY];
        disc_count[BLACK] = cur_board.disc_count[BLACK];
        disc_count[WHITE] = cur_board.disc_count[WHITE];
        next_valid_spots = cur_board.next_valid_spots;
        done = cur_board.done;
        winner = cur_board.winner;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
};
int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
std::set<P> corner;
std::set<P> no_entry;
std::map<P,std::set<P>> ma;
std::vector<P>ne = {P(0,1),P(1,0),P(1,1),P(7,1),P(6,0),P(6,1),P(7,6),P(6,7),P(6,6),P(0,6),P(1,7),P(1,6)};
std::vector<P>cor = {P(0,0),P(7,0),P(7,7),P(0,7)};
void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}
int get_state_value(Board cur_board){
    int sum = 0;
    std::vector<P>our_cor;
    for(int i=0;i < 4;i++)if(board[cor[i].first][cor[i].second] == player) our_cor.push_back(P(cor[i].first , cor[i].second));
    for(int i=0;i < cur_board.SIZE;i++)for(int j=0;j<cur_board.SIZE;j++){
        if(cur_board.board[i][j] == player){
            if(corner.find(P(i,j))!=corner.end())sum += 25;
            else if(no_entry.find(P(i,j))!=no_entry.end()){
                int flag = 1;
                for(auto it=our_cor.begin() ; it!=our_cor.end() ;++it){
                    auto ptr = ma.find(P(it->first,it->second));
                    if(ptr->second.find(P(i,j))!=ptr->second.end())flag=0;
                }
                if(flag)sum -= 10;
                else flag += 15;
            }
            else if( i==0 || i==7 || j==0 || j==7)sum += 5;
            else sum += 1;
        }
        else if(cur_board.board[i][j] == (3-player)){
            if(corner.find(P(i,j))!=corner.end())sum -= 40;
            else if(no_entry.find(P(i,j))!=no_entry.end())sum += 6;
            else if(i==0 || i==7 || j==0 || j==7)sum -= 5;
            else sum -= 1;
        }
    }
    if(cur_board.cur_player==player)return sum + (int)cur_board.next_valid_spots.size();
    else return sum - (int)cur_board.next_valid_spots.size() * 1.5;
}

int MinMax(Board cur , int depth , int alpha , int beta , int cur_player){
    if(depth==0 ||(cur.next_valid_spots.empty() && cur_player==player)) return get_state_value(cur);
    else if(cur.next_valid_spots.empty()){
        Board com(cur);
        com.cur_player = 3 - cur_player;
        com.next_valid_spots = cur.get_valid_spots();
        if(com.next_valid_spots.empty()) return get_state_value(cur);
        else {
            if(cur_player==player)return MinMax(com , depth-1, alpha ,beta , 3-cur_player);
            else if(cur_player==3-player)return MinMax(com , depth-1, alpha ,beta ,3-cur_player);
        }
    }
    if(cur_player == player){
        int value = -INF;
        for(int i=0;i<(int)cur.next_valid_spots.size();i++){
            Board next(cur);
            next.put_disc(cur.next_valid_spots[i]);
            value = std::max ( value , MinMax(next , depth-1, alpha ,beta , 3-cur_player));
            alpha = std::max(value , alpha);
            if(alpha >= beta) break;
        }
        return value;
    }
    else if(cur_player == 3 - player){
        int value = INF;
        for(int i=0;i<(int)cur.next_valid_spots.size();i++){
            Board next(cur);
            next.put_disc(cur.next_valid_spots[i]);
            value = std::min(value , MinMax(next , depth-1, alpha ,beta ,3-cur_player));
            beta = std::min(value,beta);
            if(alpha >= beta) break;
        }
        return value;
    }
}
void read_valid_spots(std::ifstream& fin){
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({(float)x, (float)y});
    }
}
void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = (int)next_valid_spots.size();
    srand(time(NULL));
    int index = -1;
    int alpha = -INF , beta = INF;
    for(int i=0;i < n_valid_spots;i++){
        Board next(board , player);
        next.put_disc(next_valid_spots[i]);
        int value = MinMax(next , 5 ,alpha , beta , 3-player);
        if(value > alpha) alpha = value , index = i;
    }
    Point p = next_valid_spots[index];
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    for(int i=0;i<4;i++)corner.insert(P(cor[i].first,cor[i].second));
    int flag=0;
    std::set<P> ptr;
    for(int i=0;i<12;i++){
        no_entry.insert(P(ne[i].first,ne[i].second));
        ptr.insert(P(ne[i].first , ne[i].second));
        if( i % 3 ==2) ma.insert(std::pair<P,std::set<P>>(P(cor[flag].first ,cor[flag].second) , ptr )) ,flag++ ,ptr.clear();
    }
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
