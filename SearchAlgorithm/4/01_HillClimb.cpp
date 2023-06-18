#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <utility>
#include <random>
#include <assert.h>
#include <math.h>
#include <chrono>
#include <queue>
#include <algorithm>
#include <iostream>
#include <functional>
using namespace std;

mt19937 mt_for_action(0);

constexpr const int H = 5;
constexpr const int W = 5;
constexpr int END_TURN = 5;
constexpr int CHARACTER_N = 3;

using ScoreType = int64_t;
constexpr const ScoreType INF = 1000000000LL;

// 座標
struct Coord
{
    int y;
    int x;
    Coord(const int y = 0, const int x = 0) : y(y), x(x) {}
};

// 自動一人ゲームの例
// キャラクターは1マス先の最もポイントが高い床に自動で移動する。
// 合法手の中でスコアが同値のものがある場合、右、左、下、上の順で行動が優先される。
// 1ターンに上下左右四方向のいずれかに壁のない場所に1マスずつ進む。
// 床にあるポイントを踏むと自身のスコアとなり、床のポイントが消える。
// END_TURNの時点のスコアを高くすることを目的とし、
// ゲームに介入できる要素として、初期状態でのキャラクターをどこに配置するかを選択できる。
// どのようにキャラクターを配置すると最終スコアが高くなるかを考えるゲーム。

class AutoMoveState
{
private:
    static constexpr const int dx[4] = {1, -1, 0, 0};
    static constexpr const int dy[4] = {0, 0, 1, -1};

    int points[H][W] = {}; // 床のポイント 1-9
    int turn;
    Coord characters[CHARACTER_N] = {};

    // 指定キャラクターを移動させる
    void movePlayer(const int chara_id)
    {
        Coord &character = characters[chara_id];
        int best_point = -INF;
        int best_action_index = 0;
        for (int action = 0; action < 4; action++)
        {
            int ty = character.y + dy[action];
            int tx = character.x + dx[action];
            if (ty >= 0 && ty < H && tx >= 0 && tx < W)
            {
                auto point = points[ty][tx];
                if (point > best_point)
                {
                    best_point = point;
                    best_action_index = action;
                }
            }
        }

        character.y += dy[best_action_index];
        character.x += dx[best_action_index];
    }

    void advance()
    {
        for (int chara_id = 0; chara_id < CHARACTER_N; chara_id++)
        {
            movePlayer(chara_id);
        }
        for (auto &character : characters)
        {
            auto &point = points[character.y][character.x];
            game_score += point;
            point = 0;
        }
        turn++;
    }

public:
    int game_score;
    ScoreType evaluated_score;

    AutoMoveState(const int seed) : turn(0),
                                    game_score(0),
                                    evaluated_score(0)
    {
        auto mt_for_construct = mt19937(seed);
        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                points[y][x] = mt_for_construct() % 9 + 1;
            }
        }
    }

    // 初期化
    void init()
    {
        for (auto &character : this->characters)
        {
            character.y = mt_for_action() % H;
            character.x = mt_for_action() % W;
        }
    }

    // 状態遷移
    void transition()
    {
        auto &character = this->characters[mt_for_action() % CHARACTER_N];
        character.y = mt_for_action() % H;
        character.x = mt_for_action() % W;
    }

    void setCharacter(const int chara_id, const int y, const int x)
    {
        characters[chara_id].y = y;
        characters[chara_id].x = x;
    }

    bool isDone() const
    {
        return turn == END_TURN;
    }

    string toString() const
    {
        stringstream ss;
        ss << "turn:\t" << turn << "\n";
        ss << "score:\t" << game_score << "\n";
        auto board_chars = vector<vector<char>>(H, vector<char>(W, '.'));
        for (int h = 0; h < H; h++)
        {
            for (int w = 0; w < W; w++)
            {
                bool is_written = false; // この座標に書く文字が決定したか
                
                for (const auto &character : characters)
                {
                    if (character.y == h && character.x == w)
                    {
                        ss << "@";
                        is_written = true;
                        break;
                    }
                    board_chars[character.y][character.x] = '@';
                }

                if (!is_written)
                {
                    if (points[h][w] > 0)
                    {
                        ss << points[h][w];
                    }
                    else
                    {
                        ss << '.';
                    }
                }
            }
            ss << "\n";
        }

        return ss.str();
    }

    // スコア計算
    ScoreType getScore(bool is_print = false) const
    {
        auto tmp_state = *this;
        // キャラクターの位置にあるポイントを消す
        for (auto &character : characters)
        {
            auto &point = tmp_state.points[character.y][character.x];
            point = 0;
        }
        // 終了するまでキャラクターの移動を繰り返す
        while (!tmp_state.isDone())
        {
            tmp_state.advance();
            if (is_print)
            {
                cout << tmp_state.toString() << endl;
            }
        }
        return tmp_state.game_score;
    }
};

using State = AutoMoveState;

State hillClinb(const State &state, int number)
{
    State now_state = state;
    now_state.init();
    ScoreType best_score = now_state.getScore();
    for (int i = 0; i < number; i++)
    {
        auto next_state = now_state;
        next_state.transition();
        ScoreType next_score = next_state.getScore();
        if (next_score > best_score)
        {
            now_state = next_state;
            best_score = next_score;
        }
    }
    return now_state;
}

using AIFunction = std::function<State(const State &)>;

using StringAIPair = std::pair<std::string, AIFunction>;

void playGame(const StringAIPair &ai, const int seed)
{
    auto state = State(seed);
    state = ai.second(state);
    cout << state.toString() << endl;
    auto score = state.getScore(true);
    cout << "Score of " << ai.first << ": " << score << endl;
}

int main()
{
    const auto &ai = StringAIPair("hillClimb", [&](const State &state)
                                    {return hillClinb(state, 10000); });
    playGame(ai, 0);
    return 0;
}
