#include <bits/stdc++.h>
#define rep(i,n) for(int i = 0; i < int(n); i++)
using namespace std;
typedef long long ll;

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

class Boxes
{
public:
    int n, m;
    int next_box;
    vector<vector<int>> b;
    vector<int> b_sum;
    Boxes() {}
    Boxes(int n, int m, vector<vector<int>> b) : n(n), m(m), b(b)
    {
        next_box = 0;
        b_sum.resize(m, 0);
        rep(i, m)
        {
            for (auto e : b[i]) b_sum[i] += e;
        }
    }
    
    // k番目の箱がどこにあるかを返す
    // res = (i, j) とすると、i番目の山の下からj番目(どちらも0-indexed)にある
    pair<int, int> search_box(int k)
    {
        rep(i, n)
        {
            rep(j, b[i].size())
            {
                if (b[i][j] == k)
                {
                    return make_pair(i, j);
                }
            }
        }
        return make_pair(-1, -1);
    }

    // (i1, j)の箱以上の箱を山i2に移動させる
    void move(int i1, int j, int i2)
    {
        cout << b[i1][j]+1 << " " << i2+1 << endl;
        for (int k = j; k < int(b[i1].size()); k++)
        {
            b_sum[i2] += b[i1][k];
            b[i2].push_back(b[i1][k]);
        }
        for (int k = b[i1].size()-1; k >= j; k--)
        {
            b_sum[i1] -= b[i1][k];
            b[i1].pop_back();
        }
        return;
    }

    // 次に出すべき箱を取り出す
    void pop()
    {
        cout << next_box+1 << " 0" << endl;
        auto [i, j] = search_box(next_box);
        assert(j == int(b[i].size())-1);
        b_sum[i] -= b[i][j];
        b[i].pop_back();
        next_box++;
        return;
    }

    void solve()
    {
        rep(i, n)
        {
            // cerr << "i = " << i << endl;
            auto [i1, j] = search_box(i);
            // cerr << "search done:" << i1 << " " << j << " " << b[i1][j] << endl;
            if (j != int(b[i1].size())-1)
            {
                vector<int> cnt(m, 0);
                cnt[i1] = 1;
                int tmp = m-1;
                for (int k = i+1; k < n; k++)
                {
                    auto [ti, tj] = search_box(k);
                    if (cnt[ti] == 0) tmp--;
                    cnt[ti]++;

                    if (tmp == 1) break;
                }

                // search 0
                int i2 = -1;
                rep(k, m)
                {
                    if (cnt[k] == 0)
                    {
                        i2 = k;
                        break;
                    }
                }

                vector<int> increasing_indices;
                int start = j;
                while (true)
                {
                    int min_idx = -1;
                    int min_val = 1e9;
                    for (int k = start+1; k < int(b[i1].size()); k++)
                    {
                        if (b[i1][k] < min_val)
                        {
                            min_val = b[i1][k];
                            min_idx = k;
                        }
                    }
                    if (min_idx == -1) break;
                    increasing_indices.push_back(min_idx);
                    start = min_idx;
                }

                for (int k = increasing_indices.size()-2; k >= 0; k--)
                {
                    move(i1, increasing_indices[k]+1, i2);
                }
                move(i1, j+1, i2);
            }
            // cerr << "move done" << endl;
            pop();
        }
        return;
    }
};

int main()
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> b(m);
    rep(i, m)
    {
        rep(j, n/m)
        {
            int tmp;
            cin >> tmp;
            tmp--;
            b[i].push_back(tmp);
        }
    }

    Boxes boxes(n, m, b);
    boxes.solve();
    return 0;
}
