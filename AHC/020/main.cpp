#include <bits/stdc++.h>
#define rep(i,n) for(int i = 0; i < n; i++)
using namespace std;
typedef long long ll;

class TimeKeeper
{
private:
    chrono::high_resolution_clock::time_point start_time_;
    int64_t time_threshold_;

public:
    // 時間制限をミリ秒単位で指定してインスタンスをつくる
    TimeKeeper(const int64_t &time_threshold)
        : start_time_(chrono::high_resolution_clock::now()),
          time_threshold_(time_threshold)
    {
    }

    // インスタンスを生成したときから指定した時間制限を超過したか判定する
    bool isTimeOver() const
    {
        using chrono::duration_cast;
        using chrono::milliseconds;
        auto diff = chrono::high_resolution_clock::now() - this->start_time_;

        return duration_cast<milliseconds>(diff).count() >= time_threshold_;
    }
};

class TelevisionGraph
{
public:
    int N, M, K;
    vector<ll> x, y, w, a, b;
    vector<int> u, v;
    vector< vector<pair<pair<int, ll>, int>> > G;
    vector<bool> B;
    vector<ll> P;

    TelevisionGraph(int N, int M, int K, vector<ll> x, vector<ll> y, vector<int> u, vector<int> v, vector<ll> w, vector<ll> a, vector<ll> b)
        : N(N), M(M), K(K), x(x), y(y), u(u), v(v), w(w), a(a), b(b)
    {
        G.resize(N);
        B.resize(M);
        P.resize(N);
        rep(i, M)
        {
            G[u[i]].emplace_back(make_pair(v[i], w[i]), i);
            G[v[i]].emplace_back(make_pair(u[i], w[i]), i);
        }
    }

    void print_state()
    {
        for (auto p : this->P)
        {
            cout << p << " ";
        }
        cout << endl;
        for (auto b : this->B)
        {
            cout << b << " ";
        }
        cout << endl;
        return;
    }

    ll calc_score()
    {
        int cnt = count_available();
        if (cnt != K) return ll(1000000.0 * double(cnt+1)/double(K));
        ll cost = 0;
        rep(i, M)
        {
            if (this->B[i])
            {
                cost += this->w[i];
            }
        }
        rep(i, N)
        {
            cost += this->P[i] * this->P[i];
        }
        return ll(1000000.0 * (1.0 + 100000000.0 / (double(cost) + 10000000.0)));
    }

    // MSTを構成する辺がtrueであるような配列を返す
    // prim
    vector<bool> MST()
    {
        vector<bool> ans(M, false);
        vector<ll> V;
        V.push_back(0);
        while(V.size() != N)
        {
            int edge_idx = -1;
            int min_vertex = -1;
            ll min_weight = 1e15;
            for (auto v : V)
            {
                for (auto e : G[v])
                {
                    auto it1 = find(V.begin(), V.end(), v), it2 = find(V.begin(), V.end(), e.first.first);
                    if (it1 == V.end() || it2 != V.end()) continue;
                    if (e.first.second < min_weight)
                    {
                        min_weight = e.first.second;
                        min_vertex = e.first.first;
                        edge_idx = e.second;
                    }
                }
            }
            V.push_back(min_vertex);
            ans[edge_idx] = true;
        }
        return ans;
    }

    bool isin(ll a, ll b, ll x, ll y, ll p)
    {
        if (p < 0) return false;
        return (a-x)*(a-x) + (b-y)*(b-y) <= p*p;
    }

    void dfs(int v, vector<bool>& vis)
    {
        vis[v] = true;
        for (auto e : this->G[v])
        {
            if (!this->B[e.second]) continue;
            int nv = e.first.first;
            if (!vis[nv]) dfs(nv, vis);
        }
        return;
    }

    // 全てのP > 0である点が1と繋がっているかどうか
    bool is_connected_to_root()
    {
        vector<bool> vis(N, false);
        dfs(0, vis);
        rep(i, N)
        {
            if (this->P[i] > 0 && !vis[i])
            {
                return false;
            }
        }
        return true;
    }

    // 視聴可能住民の数を返す
    int count_available()
    {
        int ans = 0;
        vector<bool> vis(N, false);
        dfs(0, vis);
        rep(i, K)
        {
            rep(j, N)
            {
                if (!vis[j]) continue;
                if (isin(this->a[i], this-> b[i], this->x[j], this->y[j], this->P[j]))
                {
                    ans++;
                    break;
                }
            }
        }
        return ans;
    }

    // 指定された放送局について、範囲外となる住民がでない半径で最小のものを返す
    ll calc_min_raidus(int broadcaster_id, int l, int r)
    {
        ll now_p = this->P[broadcaster_id];
        while(l+1 < r)
        {
            ll m = (l+r) / 2;
            this->P[broadcaster_id] = m;
            if (count_available() == K) r = m;
            else l = m;
        }
        this->P[broadcaster_id] = now_p;
        return r;
    }
};

int main()
{
    auto start = chrono::high_resolution_clock::now();
    int N, M, K;
    cin >> N >> M >> K;
    vector<ll> x(N), y(N), w(M), a(K), b(K);
    vector<int> u(M), v(M);
    rep(i, N)
    {
        cin >> x[i] >> y[i];
    }
    rep(i, M)
    {
        cin >> u[i] >> v[i] >> w[i];
        u[i]--;
        v[i]--;
    }
    rep(i, K)
    {
        cin >> a[i] >> b[i];
    }

    TelevisionGraph tele_graph(N, M, K, x, y, u, v, w, a, b);

    rep(i, M)
    {
        tele_graph.B[i] = true;
    }
    rep(i, N)
    {
        tele_graph.P[i] = 5000;
    }
    tele_graph.print_state();
    tele_graph.B = tele_graph.MST();
    tele_graph.print_state();

    vector<pair<ll, int>> dist_from_root;
    rep(i, N)
    {
        dist_from_root.emplace_back(-1*(tele_graph.x[i]*tele_graph.x[i] + tele_graph.y[i]*tele_graph.y[i]), i);
    }
    sort(dist_from_root.begin(), dist_from_root.end());

    rep(idx, N)
    {
        int i = dist_from_root[idx].second;
        tele_graph.P[i] = tele_graph.calc_min_raidus(i, 1000, 5000);
        if (tele_graph.P[i] > 5000) tele_graph.P[i] = 5000;
        tele_graph.print_state();
    }
    rep(idx, N)
    {
        int i = dist_from_root[idx].second;
        ll tmp = tele_graph.calc_min_raidus(i, -1, 1000);
        if (tmp < 1000) tele_graph.P[i] = tmp;
        tele_graph.print_state();
    }

    cerr << "time[ms]: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
    tele_graph.print_state();
    rep(i, M)
    {
        if (tele_graph.B[i])
        {
            tele_graph.B[i] = false;
            if (!tele_graph.is_connected_to_root()) tele_graph.B[i] = true;
            else tele_graph.print_state();
        }
    }
    tele_graph.print_state();
    cerr << tele_graph.calc_score() << endl;
    cerr << "time[ms]: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
    return 0;
}