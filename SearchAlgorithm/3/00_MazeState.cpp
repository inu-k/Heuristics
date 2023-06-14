#include <string>
#include <sstream>
#include <random>
#include <iostream>
using namespace std;

// 座標
struct Coord
{
    int y_;
    int x_;
    Coord(const int y = 0, const int x = 0) : y_(y), x_(x) {}
};


constexpr const int H = 3;
constexpr const int W = 4;
constexpr int END_TURN = 4;

// 迷路探索クラス
class MazeState
{
private:
    static constexpr const int dx[4] = {1, -1, 0, 0};
    static constexpr const int dy[4] = {0, 0, 1, -1};

    int points_[H][W] = {};
    int turn_ = 0;

public:
    Coord character_ = Coord();
    int game_score_ = 0;
    MazeState() {}

    // H*Wの迷路を生成
    MazeState(const int seed)
    {
        auto mt_for_construct = mt19937(seed);
        this->character_.y_ = mt_for_construct() % H;
        this->character_.x_ = mt_for_construct() % W;

        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                if (y == character_.y_ && x == character_.x_) continue;
                this->points_[y][x] = mt_for_construct() % 10;   
            }
        }
    }

    // ゲームの終了判定
    bool isDone() const 
    {
        return this->turn_ == END_TURN;
    }

    // 指定したactionでゲームを1ターン進める
    void advance(const int action)
    {
        this->character_.x_ += this->dx[action];
        this->character_.y_ += this->dy[action];
        auto &point = this->points_[this->character_.y_][this->character_.x_];
        if (point > 0)
        {
            this->game_score_ += point;
            point = 0;
        }
        this->turn_++;
    }

    // 現在の状況でプレイヤーが可能な行動を全て取得する
    vector<int> legalActions() const
    {
        vector<int> actions;
        for (int action = 0; action < 4; action++)
        {
            int ty = this->character_.y_ + this->dy[action];
            int tx = this->character_.x_ + this->dx[action];
            if (ty >= 0 && ty < H && tx >= 0 && tx < W) actions.emplace_back(action);
        }
        return actions;
    }

    // 現在のゲーム状況を文字列にする
    string toSting() const
    {
        stringstream ss;
        ss << "turn:\t" << this->turn_ << "\n";
        ss << "score:\t" << this->game_score_ << "\n";
        for (int h = 0; h < H; h++)
        {
            for (int w = 0; w < W; w++)
            {
                if (this->character_.y_ == h && this->character_.x_ == w) ss << '@';
                else if (this->points_[h][w] > 0) ss << this->points_[h][w];
                else ss << '.';
            }
            ss << "\n";
        }
        
        return ss.str();
    }
};


using State = MazeState;
mt19937 mt_for_action(0);

// ランダムに行動を決定する
int randomAction(const State & state)
{
    auto legal_actions = state.legalActions();
    return legal_actions[mt_for_action() % (legal_actions.size())];
}

// シードを指定してゲーム状況を表示しながらAIにプレイさせる
void playGame(const int seed)
{
    auto state = State(seed);
    cout << state.toSting() << endl;
    while(!state.isDone())
    {
        state.advance(randomAction(state));
        cout << state.toSting() << endl;
    }
}

int main()
{
    playGame(111111);
    return 0;
}
