#include <bits/stdc++.h>

using namespace std;
using ll = long long;

#ifdef _DEBUG
#include "utils/debug.h"
#else
#define debug(...)
#endif

// Source: atcoder library
// Documentation:
// https://atcoder.github.io/ac-library/production/document_en/segtree.html
// https://github.com/atcoder/ac-library/blob/master/atcoder/segtree.hpp
namespace atcoder {

namespace internal {

// @param n `0 <= n`
// @return minimum non-negative `x` s.t. `n <= 2**x`
int ceil_pow2(int n) {
	int x = 0;
	while ((1U << x) < (unsigned int)(n)) x++;
	return x;
}

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
constexpr int bsf_constexpr(unsigned int n) {
	int x = 0;
	while (!(n & (1 << x))) x++;
	return x;
}

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned int n) {
#ifdef _MSC_VER
	unsigned long index;
	_BitScanForward(&index, n);
	return index;
#else
	return __builtin_ctz(n);
#endif
}

}	 // namespace internal

template <class S, S (*op)(S, S), S (*e)()>
struct segtree {
 public:
	segtree() : segtree(0) {}
	explicit segtree(int n) : segtree(std::vector<S>(n, e())) {}
	explicit segtree(const std::vector<S>& v) : _n(int(v.size())) {
		log = internal::ceil_pow2(_n);
		size = 1 << log;
		d = std::vector<S>(2 * size, e());
		for (int i = 0; i < _n; i++) d[size + i] = v[i];
		for (int i = size - 1; i >= 1; i--) {
			update(i);
		}
	}

	void set(int p, S x) {
		assert(0 <= p && p < _n);
		p += size;
		d[p] = x;
		for (int i = 1; i <= log; i++) update(p >> i);
	}

	S get(int p) const {
		assert(0 <= p && p < _n);
		return d[p + size];
	}

	S prod(int l, int r) const {
		assert(0 <= l && l <= r && r <= _n);
		S sml = e(), smr = e();
		l += size;
		r += size;

		while (l < r) {
			if (l & 1) sml = op(sml, d[l++]);
			if (r & 1) smr = op(d[--r], smr);
			l >>= 1;
			r >>= 1;
		}
		return op(sml, smr);
	}

	S all_prod() const { return d[1]; }

	template <bool (*f)(S)>
	int max_right(int l) const {
		return max_right(l, [](S x) { return f(x); });
	}
	template <class F>
	int max_right(int l, F f) const {
		assert(0 <= l && l <= _n);
		assert(f(e()));
		if (l == _n) return _n;
		l += size;
		S sm = e();
		do {
			while (l % 2 == 0) l >>= 1;
			if (!f(op(sm, d[l]))) {
				while (l < size) {
					l = (2 * l);
					if (f(op(sm, d[l]))) {
						sm = op(sm, d[l]);
						l++;
					}
				}
				return l - size;
			}
			sm = op(sm, d[l]);
			l++;
		} while ((l & -l) != l);
		return _n;
	}

	template <bool (*f)(S)>
	int min_left(int r) const {
		return min_left(r, [](S x) { return f(x); });
	}
	template <class F>
	int min_left(int r, F f) const {
		assert(0 <= r && r <= _n);
		assert(f(e()));
		if (r == 0) return 0;
		r += size;
		S sm = e();
		do {
			r--;
			while (r > 1 && (r % 2)) r >>= 1;
			if (!f(op(d[r], sm))) {
				while (r < size) {
					r = (2 * r + 1);
					if (f(op(d[r], sm))) {
						sm = op(d[r], sm);
						r--;
					}
				}
				return r + 1 - size;
			}
			sm = op(d[r], sm);
		} while ((r & -r) != r);
		return 0;
	}

 private:
	int _n, size, log;
	std::vector<S> d;

	void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
};

}	 // namespace atcoder
struct S { int val, cnt; };
S op(S a, S b) { return {a.val + b.val, a.cnt + b.cnt}; }
S e() { return {0, 0}; }
int need;
bool f(S x) { return x.val < need; }

// Builds the centroid tree of a tree (0-indexed)
// Returns {adjacency list, root (first centroid)}
// Runtime: O(NlogN)
// Tested:
// https://cses.fi/problemset/task/2079/
// https://cses.fi/problemset/task/2080/
pair<vector<vector<int>>, int> CentroidTree(vector<vector<int>> adj) {
	int N = adj.size();
	vector<int> sz(N), vis(N);

	auto dfs = [&](auto self, int v, int p) -> int {
		sz[v] = 1;
		for (int to : adj[v]) {
			if (to == p || vis[to]) continue;
			sz[v] += self(self, to, v);
		}
		return sz[v];
	};

	auto locate = [&](auto self, int v, int p, int src) -> int {
		for (int to : adj[v]) {
			if (to == p || vis[to]) continue;
			if (2 * sz[to] > sz[src]) return self(self, to, v, src);
		}
		return v;
	};

	vector<vector<int>> ret(N);
	auto go = [&](auto self, int v) -> int {
		dfs(dfs, v, v);
		v = locate(locate, v, v, v);
		vis[v] = 1;
		for (int to : adj[v]) {
			if (vis[to]) continue;
			ret[v].push_back(self(self, to));
		}
		return v;
	};
	int rt = go(go, 0);
	return { ret, rt };
}

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int N; cin >> N;
	vector<vector<int>> adj(N);
	for (int i = 0; i < N-1; ++i) {
		int a, b; cin >> a >> b;
		a--, b--;
		adj[a].push_back(b);
		adj[b].push_back(a);
	}
	auto [_, rt] = CentroidTree(adj);
	vector<int> sz(N);
	vector<int> src(N);
	auto dfs = [&](auto self, int v, int p) -> int {
		sz[v] = 1;
		for (int to : adj[v]) {
			if (to == p) continue;
			src[to] = src[v];
			sz[v] += self(self, to, v);
		}
		return sz[v];
	};
	for (int to : adj[rt]) {
		src[to] = to;
		dfs(dfs, to, rt);
	}

	vector<int> ans(N);
	auto answer = [&]() -> void {
		for (int i = 0; i < N; ++i) {
			cout << ans[i] << '\n';
		}
		exit(0);
	};

	// check if 2 centroids
	if (N % 2 == 0) {
		for (int i = 0; i < N; ++i) {
			if (sz[i] == N / 2) {
				fill(ans.begin(), ans.end(), 1);
				ans[rt] = ans[i] = 0;
				answer();
			}
		}
	}

	// only 1 centroid
	vector<int> st_roots = adj[rt];
	int K = st_roots.size();
	sort(st_roots.begin(), st_roots.end(), [&](int a, int b) {
		return sz[a] > sz[b];
	});
	map<int, int> idx;
	vector<S> arr(K);
	for (int i = 0; i < K; ++i) {
		arr[i] = {sz[st_roots[i]], 1};
		idx[st_roots[i]] = i;
	}
	atcoder::segtree<S, op, e> st(arr);

	for (int i = 0; i < N; ++i) {
		if (i == rt) {
			ans[i] = 0;
			continue;
		}
		st.set(idx[src[i]], {0, 0});
		// for (int j = 0; j < K; ++j) {
		// 	cout << st.get(j).val << " ";
		// }
		// cout << '\n';
		{
			int bad = N - sz[i];
			need = bad - N/2;
			int r = st.max_right<f>(0)+1;
			ans[i] = st.prod(0, r).cnt;
		}
		{
			int bad = N - sz[src[i]];
			need = bad - N/2;
			int r = st.max_right<f>(0)+1;
			ans[i] = min(ans[i], st.prod(0, r).cnt + 1);
		}
		st.set(idx[src[i]], {sz[src[i]], 1});
	}
	answer();
}
