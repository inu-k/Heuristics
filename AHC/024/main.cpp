#include <bits/stdc++.h>
#define rep(i,n) for(int i = 0; i < n; i++)
using namespace std;
typedef long long ll;
typedef pair<ll, ll> P;
int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};

class TimeKeeper
{
private:
    chrono::high_resolution_clock::time_point start_time;
    int64_t time_threshold;
 
public:
    // 時間制限をミリ秒単位で指定してインスタンスをつくる
    TimeKeeper(const int64_t &time_threshold)
        : start_time(chrono::high_resolution_clock::now()),
          time_threshold(time_threshold)
    {}

    // インスタンスを生成したときからの経過時間をミリ秒単位で返す
    int64_t getElapsedTime() const
    {
        auto diff = chrono::high_resolution_clock::now() - this->start_time;
        return chrono::duration_cast<chrono::milliseconds>(diff).count();
    }
 
    // インスタンスを生成したときから指定した時間制限を超過したか判定する
    bool isTimeOver() const
    {
        auto diff = chrono::high_resolution_clock::now() - this->start_time;
        return chrono::duration_cast<chrono::milliseconds>(diff).count() >= time_threshold;
    }
};

class Solver
{
public:
    int n, m;
    vector<vector<int>> init_c, c;
    vector<vector<int>> adj_cnt;
    vector<vector<bool>> adj_flg;
    vector<int> color_cnt;
    random_device rng;
    int TIME_LIMIT = 1980;
    double start_temp = 1, end_temp = 1e-10;

    Solver(vector<vector<int>> c, int n, int m)
    {
        this->n = n;
        this->m = m;
        this->init_c = c;
        this->c = c;

        // 各色のペアについて隣接している数を数える
        adj_cnt.resize(m+1, vector<int>(m+1, 0));
        rep(i, this->n+1)
        {
            rep(j, this->n+1)
            {
                if (this->c[i][j] != this->c[i+1][j])
                {
                    this->adj_cnt[this->c[i][j]][this->c[i+1][j]]++;
                    this->adj_cnt[this->c[i+1][j]][this->c[i][j]]++;
                }
                if (this->c[i][j] != this->c[i][j+1])
                {
                    this->adj_cnt[this->c[i][j]][this->c[i][j+1]]++;
                    this->adj_cnt[this->c[i][j+1]][this->c[i][j]]++;
                }
            }
        }
        cerr << "adj cnt done" << endl;

        this->adj_flg.resize(m+1, vector<bool>(m+1, false));
        rep(i, this->m+1)
        {
            rep(j, this->m+1)
            {
                if (this->adj_cnt[i][j] > 0) this->adj_flg[i][j] = true;
            }
        }
        cerr << "adj flg done" << endl;

        // 各色の数を数える
        color_cnt.resize(m+1, 0);
        rep(i, this->n+2)
        {
            rep(j, this->n+2)
            {
                this->color_cnt[this->c[i][j]]++;
            }
        }
        cerr << "color cnt done" << endl;

        cerr << "init done" << endl;
    }

    // 最初の隣接状態が保たれているかどうか
    // チェックすべきは変更前の色に関する隣接性と変更後の色に関する隣接性だけ
    bool is_adj(int old_color, int new_color)
    {
        rep(i, this->m+1)
        {
            if (i != old_color)
            {
                if (this->adj_cnt[i][old_color] > 0 && !this->adj_flg[i][old_color]) return false;
                if (this->adj_cnt[i][old_color] == 0 && this->adj_flg[i][old_color]) return false;
            }
            if (i != new_color)
            {
                if (this->adj_cnt[i][new_color] > 0 && !this->adj_flg[i][new_color]) return false;
                if (this->adj_cnt[i][new_color] == 0 && this->adj_flg[i][new_color]) return false;
            }
            // if (this->adj_cnt[i][j] > 0 && !this->adj_flg[i][j])
            // {
            //     // cerr << "i: " << i << " j: " << j << " adj_cnt: " << this->adj_cnt[i][j] << " adj_flg: " << this->adj_flg[i][j] << endl;
            //     return false;
            // }
            // if (this->adj_cnt[i][j] == 0 && this->adj_flg[i][j]) return false;
            
        }
        return true;
    }

    void dfs_inner(int i, int j, int color, vector<vector<bool>> &visited, int &res)
    {
        if (visited[i][j]) return;
        visited[i][j] = true;
        res++;
        rep(k, 4)
        {
            int ni = i + dx[k], nj = j + dy[k];
            if (ni < 0 || ni >= this->n+2 || nj < 0 || nj >= this->n+2) continue;
            if (this->c[ni][nj] == color) this->dfs_inner(ni, nj, color, visited, res);
        }
        return;
    }

    // i, jを含む色colorの連結成分の大きさを返す
    int dfs(int i, int j, int color)
    {
        vector<vector<bool>> visited(this->n+2, vector<bool>(this->n+2, false));
        int res = 0;
        this->dfs_inner(i, j, color, visited, res);
        return res;
    }

    // 連結かどうか
    bool is_connected(int i, int j, int color)
    {
        int cnt = this->dfs(i, j, color);
        // cerr << "cnt: " << cnt << " color_cnt: " << this->color_cnt[color] << endl;
        if (cnt != this->color_cnt[color]) return false;
        return true;
    }

    // 変更が許されるかどうか
    bool is_ok(int i, int j, int old_color, int new_color)
    {
        // 連結性
        if (!this->is_connected(i, j, new_color)) return false;
        // cerr << "0 is still connected" << endl;

        // (i, j)の隣にold_colorがあるはずなので、それを探す
        int ni = -1, nj = -1;
        rep(k, 4)
        {
            int ti = i + dx[k], tj = j + dy[k];
            if (this->c[ti][tj] == old_color)
            {
                ni = ti, nj = tj;
                break;
            }
        }
        if (ni == -1) return false;  // 多分ここには到達しない
        if (!this->is_connected(ni, nj, old_color)) return false;
        // cerr << "old color is still connected" << endl;

        // 隣接性
        if (!this->is_adj(old_color, new_color)) return false;
        // cerr << "adj is still ok" << endl;
        return true;
    }

    // ランダムにマスを選んで、そのマスの色を0にする
    void random_zero(int& ok_cnt)
    {
        int i = rng() % this->n + 1, j = rng() % this->n + 1;
        // int i = 1, j = 1;
        int old_color = this->c[i][j];
        // cerr << i << " " << j << " " << old_color << endl;
        if (this->color_cnt[old_color] == 1) return;  // 0にはできない
        this->c[i][j] = 0;
        this->color_cnt[old_color]--;
        this->color_cnt[0]++;
        // 隣接している数を更新する
        rep(k, 4)
        {
            int ni = i + dx[k], nj = j + dy[k];
            this->adj_cnt[old_color][this->c[ni][nj]]--;
            this->adj_cnt[this->c[ni][nj]][old_color]--;
            this->adj_cnt[0][this->c[ni][nj]]++;
            this->adj_cnt[this->c[ni][nj]][0]++;
        }

        if (!this->is_ok(i, j, old_color, 0))
        {
            // 元に戻す
            this->c[i][j] = old_color;
            this->color_cnt[old_color]++;
            this->color_cnt[0]--;
            rep(k, 4)
            {
                int ni = i + dx[k], nj = j + dy[k];
                this->adj_cnt[old_color][this->c[ni][nj]]++;
                this->adj_cnt[this->c[ni][nj]][old_color]++;
                this->adj_cnt[0][this->c[ni][nj]]--;
                this->adj_cnt[this->c[ni][nj]][0]--;
            }
            return;
        }

        ok_cnt++;
        return;
    }

    // ランダムにマスを選んで、そのマスの色を外側にある色にする
    // 内側の色の領域が減って外側の領域が内側に来るので、縮むように動くはず
    void random_shrink(int& ok_cnt, int& force_cnt, TimeKeeper& time_keeper)
    {
        int i = rng() % this->n + 1, j = rng() % this->n + 1;
        int old_color = this->c[i][j];
        // if (old_color == 0) return;  // 0を別の色にするとスコアが下がるので駄目
        if (this->color_cnt[old_color] == 1) return;  // 0にはできない

        // 外側の点
        vector<pair<int, int>> outer;
        if (i < this->n/2) outer.push_back(make_pair(i-1, j));
        else outer.push_back(make_pair(i+1, j));
        if (j < this->n/2) outer.push_back(make_pair(i, j-1));
        else outer.push_back(make_pair(i, j+1));
        // if (i < this->n/2 && j < this->n/2) outer.push_back(make_pair(i-1, j-1));
        // else if (i < this->n/2 && j >= this->n/2) outer.push_back(make_pair(i-1, j+1));
        // else if (i >= this->n/2 && j < this->n/2) outer.push_back(make_pair(i+1, j-1));
        // else outer.push_back(make_pair(i+1, j+1));

        auto [oi, oj] = outer[rng() % outer.size()];

        int new_color = this->c[oi][oj];
        if (new_color == old_color) return;  // 同じ色なら変更する必要はない
        this->c[i][j] = new_color;
        this->color_cnt[old_color]--;
        this->color_cnt[new_color]++;
        // 隣接している数を更新する
        rep(k, 4)
        {
            int ni = i + dx[k], nj = j + dy[k];
            this->adj_cnt[old_color][this->c[ni][nj]]--;
            this->adj_cnt[this->c[ni][nj]][old_color]--;
            this->adj_cnt[new_color][this->c[ni][nj]]++;
            this->adj_cnt[this->c[ni][nj]][new_color]++;
        }

        if (!this->is_ok(i, j, old_color, new_color))
        {
            // 元に戻す
            this->c[i][j] = old_color;
            this->color_cnt[old_color]++;
            this->color_cnt[new_color]--;
            rep(k, 4)
            {
                int ni = i + dx[k], nj = j + dy[k];
                this->adj_cnt[old_color][this->c[ni][nj]]++;
                this->adj_cnt[this->c[ni][nj]][old_color]++;
                this->adj_cnt[new_color][this->c[ni][nj]]--;
                this->adj_cnt[this->c[ni][nj]][new_color]--;
            }
            return;
        }

        if (old_color == 0)
        {
            double temp = this->start_temp + (this->end_temp - this->start_temp) * double(time_keeper.getElapsedTime() / double(this->TIME_LIMIT));
            double prob = exp(-1 / temp);
            bool is_force = prob > double(rng() % 10000) / 10000;
            if (!is_force)
            {
                // 元に戻す
                this->c[i][j] = old_color;
                this->color_cnt[old_color]++;
                this->color_cnt[new_color]--;
                rep(k, 4)
                {
                    int ni = i + dx[k], nj = j + dy[k];
                    this->adj_cnt[old_color][this->c[ni][nj]]++;
                    this->adj_cnt[this->c[ni][nj]][old_color]++;
                    this->adj_cnt[new_color][this->c[ni][nj]]--;
                    this->adj_cnt[this->c[ni][nj]][new_color]--;
                }
                return;
            }
            force_cnt++;
        }

        ok_cnt++;
        return;
    }

    void solve()
    {
        TimeKeeper time_keeper(this->TIME_LIMIT);
        int cnt = 0, ok_cnt = 0, force_cnt = 0;
        int64_t print_time = 100;
        while (!time_keeper.isTimeOver())
        {
            this->random_shrink(ok_cnt, force_cnt, time_keeper);
            cnt++;

            if (time_keeper.getElapsedTime() > print_time)
            {
                cerr << "cnt: " << cnt << " ok_cnt: " << ok_cnt << " force_cnt: " << force_cnt << endl;
                this->print();
                print_time += 100;
            }
        }
        // this->random_zero();
        cerr << ok_cnt << " " << cnt << endl;
        return;
    }

    void print_all()
    {
        rep(i, this->n+2)
        {
            rep(j, this->n+2)
            {
                cout << this->c[i][j] << " ";
            }
            cout << endl;
        }
    }

    void print()
    {
        rep(i, this->n)
        {
            rep(j, this->n)
            {
                cout << this->c[i+1][j+1] << " ";
            }
            cout << endl;
        }
    }
};

int main()
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> c(n+2, vector<int>(n+2, 0));
    rep(i, n)
    {
        rep(j, n)
        {
            cin >> c[i+1][j+1];
        }
    }
    cerr << "input done" << endl;
    Solver solver(c, n, m);
    solver.solve();
    solver.print();
    return 0;
}
