#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <utility>
#include <random>
#include <assert.h>
#include <math.h>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <functional>
#include <queue>
#include <set>

using namespace std;
random_device rnd;
mt19937 mt_for_action(0);

constexpr const int H = 3;  // 迷路の高さ
constexpr const int W = 3;  // 迷路の幅
constexpr int END_TURN = 4; // ゲーム終了ターン

enum WinningStatus
{
    WIN,
    LOSE,
    DRAW,
    NONE,
};

class AlternateMazeState
{
private:
    static constexpr int dx[4] = {1, -1, 0, 0};
    static constexpr int dy[4] = {0, 0, 1, -1};
    struct Character
    {
        int y;
        int x;
        int game_score;
        Character(const int y = 0, const int x = 0) : y(y), x(x), game_score(0) {};
    };
    vector<vector<int>> points; // 床のポイント
    int turn; // 現在のターン
    vector<Character> characters;

public:
    AlternateMazeState(const int seed) : points(H, vector<int>(W)),
                                         turn(0),
                                         characters({
                                            Character(H/2, (W/2)-1),
                                            Character(H/2, (W/2)+1)
                                         })
    {
        auto mt_for_construct = mt19937(seed);

        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                int point = mt_for_construct() % 10;
                if (this->characters[0].y == y && this->characters[0].x == x) continue;
                if (this->characters[1].y == y && this->characters[1].x == x) continue;
                this->points[y][x] = point;
            }
        }
    }

    bool isDone() const
    {
        return this->turn == END_TURN;
    }

    // 指定したactionでゲームを1ターン進め、次のプレイヤー視点にする
    void advance(const int action)
    {
        auto &character = this->characters[0];
        character.x += dx[action];
        character.y += dy[action];
        auto &point = this->points[character.y][character.x];
        if (point > 0)
        {
            character.game_score = point;
            point = 0;
        }
        this->turn++;
        swap(this->characters[0], this->characters[1]);
    }

    vector<int> legalActions() const
    {
        vector<int> actions;
        const auto &character = this->characters[0];
        for (int action = 0; action < 4; action++)
        {
            int ty = character.y + dy[action];
            int tx = character.x + dx[action];
            if (ty >= 0 && ty < H && tx >= 0 && tx < W)
            {
                actions.emplace_back(action);
            }
        }
        return actions;
    }

    WinningStatus getWinningState() const
    {
        if (isDone())
        {
            if (this->characters[0].game_score > this->characters[1].game_score) return WinningStatus::WIN;
            if (this->characters[0].game_score < this->characters[1].game_score) return WinningStatus::LOSE;
            return WinningStatus::DRAW;
        }
        return WinningStatus::NONE;
    }

    string toString() const
    {
        stringstream ss("");
        ss << "turn:\t" << this->turn << endl;
        for (int player_id = 0; player_id < this->characters.size(); player_id++)
        {
            int actual_player_id = player_id;
            if (this->turn % 2 == 1) actual_player_id = (actual_player_id + 1) % 2;
            const auto &chara = this->characters[actual_player_id];
            ss << "score(" << actual_player_id << "):\t" << chara.game_score;
            ss << "\ty: " << chara.y << " x: " << chara.x << "\n";
        }

        for (int h = 0; h < H; h++)
        {
            for (int w = 0; w < W; w++)
            {
                bool is_written = false; // この座標に書く文字が決定したか
                for (int player_id = 0; player_id < this->characters.size(); player_id++)
                {
                    int actual_player_id = player_id;
                    if (this->turn % 2 == 1) actual_player_id = (actual_player_id + 1) % 2;

                    const auto &character = this->characters[actual_player_id];
                    if (character.y == h && character.x == w)
                    {
                        if (actual_player_id == 0) ss << 'A';
                        else ss << 'B';
                        is_written = true;
                    }
                }
                if (!is_written)
                {
                    if (this->points[h][w] > 0) ss << points[h][w];
                    else ss << '.';
                }
            }
            ss << '\n';
        }

        return ss.str();
    }
};

using State = AlternateMazeState;

// ランダムに行動を決定する
int randomAction(const State &state)
{
    auto legal_actions = state.legalActions();
    return legal_actions[mt_for_action() % (legal_actions.size())];
}

// ゲームを一回プレイしてゲーム状況を表示する
void playGame(const int seed)
{
    auto state = State(seed);
    cout << state.toString() << endl;

    while(!state.isDone())
    {
        // 1p
        {
            cout << "1p ------------------------------------" << endl;
            int action = randomAction(state);
            cout << "action: " << action << endl;
            state.advance(action); // ここで視点が入れ替わり1p視点に
            cout << state.toString() << endl;
            if (state.isDone())
            {
                switch (state.getWinningState())
                {
                case (WinningStatus::WIN):
                    cout << "winner: 2p" << endl;
                    break;
                case (WinningStatus::LOSE):
                    cout << "winner: 1p" << endl;
                    break;
                default:
                    cout << "DRAW" << endl;
                    break;
                }
                break;
            }
        }

        // 2p
        {
            cout << "2p ------------------------------------" << endl;
            int action = randomAction(state);
            cout << "action: " << action << endl;
            state.advance(action); // ここで視点が入れ替わり2p視点に
            cout << state.toString() << endl;
            if (state.isDone())
            {
                switch (state.getWinningState())
                {
                case (WinningStatus::WIN):
                    cout << "winner: 1p" << endl;
                    break;
                case (WinningStatus::LOSE):
                    cout << "winner: 2p" << endl;
                    break;
                default:
                    cout << "DRAW" << endl;
                    break;
                }
                break;
            }
        }
    }
}

int main()
{
    playGame(4121859904);
    return 0;
}
