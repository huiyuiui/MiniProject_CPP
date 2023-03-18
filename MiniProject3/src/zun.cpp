#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <string>

using namespace std;

struct Point
{
    float x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point &rhs) const
    {
        return !operator==(rhs);
    }
    Point operator+(const Point &rhs) const
    {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point &rhs) const
    {
        return Point(x - rhs.x, y - rhs.y);
    }
};

class OthelloBoard
{
public:
    enum SPOT_STATE
    {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const array<Point, 8> directions{{Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                      Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                      Point(1, -1), Point(1, 0), Point(1, 1)}};
    array<array<int, SIZE>, SIZE> board;
    vector<Point> next_valid_spots; //有效棋
    array<int, 3> disc_count;       //空白 黑棋 白旗
    int cur_player;                 //現在玩家
    bool done;
    int winner;
    int get_next_player(int player) const //下一個走棋的是誰
    {
        return 3 - player;
    }

private:
    bool is_spot_on_board(Point p) const //是否在棋盤上
    {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const //取得該格棋子
    {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) //放棋子
    {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const //判斷是否下對
    {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const //棋子是否有效
    {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir : directions)
        {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY)
            {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) //翻棋子
    {
        for (Point dir : directions)
        {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY)
            {
                if (is_disc_at(p, cur_player))
                {
                    for (Point s : discs)
                    {
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
    OthelloBoard(array<array<int, SIZE>, SIZE> nowBoard, int nowPlayer)
    {
        for (int i = 0; i < 3; i++)
            this->disc_count[i] = 0;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                this->board[i][j] = nowBoard[i][j];
                if (this->board[i][j] == 0)
                    this->disc_count[0]++;
                else if (this->board[i][j] == 1)
                    this->disc_count[1]++;
                else if (this->board[i][j] == 2)
                    this->disc_count[2]++;
            }
        }
        this->next_valid_spots = this->get_valid_spots();
        this->cur_player = nowPlayer;
        this->done = false;
        this->winner = -1;
    }
    OthelloBoard(OthelloBoard const &obj)
    { //複製棋盤
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                this->board[i][j] = obj.board[i][j];
            }
        }
        this->next_valid_spots = obj.next_valid_spots;
        for (int i = 0; i < 3; i++)
        {
            this->disc_count[i] = obj.disc_count[i];
        }
        this->cur_player = obj.cur_player;
        this->done = obj.done;
        this->winner = obj.winner;
    }
    vector<Point> get_valid_spots() const //獲得所有有效棋子
    {
        vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) //放棋子，更新棋盤
    {
        if (!is_spot_valid(p))
        {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0)
        {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0)
            {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs)
                    winner = EMPTY;
                else if (black_discs > white_discs)
                    winner = BLACK;
                else
                    winner = WHITE;
            }
        }
        return true;
    }
};

int player;
const int SIZE = 8;
int score[SIZE][SIZE];
array<array<int, SIZE>, SIZE> board;
vector<Point> next_valid_spots;

void read_board(ifstream &fin);
void read_valid_spots(ifstream &fin);
void write_valid_spot(ofstream &fout);
void set_score();
int alpha_beta_pruning(OthelloBoard &nowBoard, int depth, int alpha, int beta, int curPlayer);
int get_value(OthelloBoard &obj);

void read_board(ifstream &fin)
{
    fin >> player;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(ifstream &fin)
{
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++)
    {
        fin >> x >> y;
        next_valid_spots.push_back({(float)x, (float)y});
    }
}

void write_valid_spot(ofstream &fout)
{
    int n_valid_spots = (int)next_valid_spots.size();
    int index = 0;
    int value = -1e7;
    int alpha = -1e7;
    int beta = 1e7;
    for (int i = 0; i < n_valid_spots; i++)
    {
        OthelloBoard nextBoard(board, player);
        nextBoard.put_disc(next_valid_spots[i]);
        int rtnValue = alpha_beta_pruning(nextBoard, 5, alpha, beta, 3 - player);
        if (rtnValue > value)
        {
            value = rtnValue;
            index = i;
        }
        alpha = max(alpha, value);
    }
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << endl;
    fout.flush();
}

void set_score()
{
    score[0][0] = score[0][7] = score[7][0] = score[7][7] = 99;
    score[0][2] = score[0][5] = score[2][0] = score[2][7] = score[5][0] = score[5][7] = score[7][2] = score[7][5] = 10;
    score[2][2] = score[2][5] = score[5][2] = score[5][5] = 8;
    score[0][3] = score[0][4] = score[3][0] = score[3][7] = score[4][0] = score[4][7] = score[7][3] = score[7][4] = 6;
    score[2][3] = score[2][4] = score[3][2] = score[3][5] = score[4][2] = score[4][5] = score[5][3] = score[5][4] = 5;
    score[1][3] = score[1][4] = score[3][1] = score[3][6] = score[4][1] = score[4][6] = score[6][3] = score[6][4] = 4;
    score[1][2] = score[1][5] = score[2][1] = score[2][6] = score[5][1] = score[5][6] = score[6][2] = score[6][5] = 2;
    score[0][1] = score[0][6] = score[1][0] = score[1][7] = score[6][0] = score[6][7] = score[7][1] = score[7][6] = -10;
    score[1][1] = score[1][6] = score[6][1] = score[6][6] = -99;
    score[3][3] = score[3][4] = score[4][3] = score[4][4] = 1;
}

int alpha_beta_pruning(OthelloBoard &nowBoard, int depth, int alpha, int beta, int curPlayer)
{

    if (depth == 0 || (nowBoard.next_valid_spots.empty() && curPlayer == player)) //到底或沒棋可以下
    {
        return get_value(nowBoard);
    }
    if (curPlayer == player)
    {
        int value = -1e7;
        for (int i = 0; i < (int)nowBoard.next_valid_spots.size(); i++)
        {
            OthelloBoard nextBoard(nowBoard);
            nextBoard.put_disc(nowBoard.next_valid_spots[i]);
            value = max(value, alpha_beta_pruning(nextBoard, depth - 1, alpha, beta, 3 - curPlayer));
            alpha = max(alpha, value);
            if (alpha >= beta)
                break;
        }
        return value;
    }
    else if (curPlayer == 3 - player)
    {
        int value = 1e7;
        for (int i = 0; i < (int)nowBoard.next_valid_spots.size(); i++)
        {
            OthelloBoard nextBoard(nowBoard);
            nextBoard.put_disc(nowBoard.next_valid_spots[i]);
            value = min(value, alpha_beta_pruning(nextBoard, depth - 1, alpha, beta, 3 - curPlayer));
            beta = min(beta, value);
            if (beta <= alpha)
                break;
        }
        return value;
    }
}

int get_value(OthelloBoard &obj)
{
    set_score();
    //危險格佔領
    if (obj.board[0][0] == player)
        score[1][1] = 30;
    if (obj.board[0][7] == player)
        score[1][6] = 30;
    if (obj.board[7][0] == player)
        score[6][1] = 30;
    if (obj.board[7][7] == player)
        score[6][6] = 30;
    int value = 0;
    for (int i = 0; i < SIZE; i++) //棋盤分數
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (obj.board[i][j] == player)
            {
                value += score[i][j];
            }
            else if (obj.board[i][j] == (3 - player))
            {
                value -= score[i][j];
            }
        }
    } //行動力
    if (obj.cur_player == player)
        value += (int)obj.next_valid_spots.size() * 2;
    else if (obj.cur_player == (3 - player))
        value -= (int)obj.next_valid_spots.size() * 4;
    return value;
}

int main(int, char **argv)
{
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
