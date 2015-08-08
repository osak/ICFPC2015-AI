struct TestEval{
	int holeScore(const vector <vector <int> > &ff, const int num) {
		auto f = ff;
		int h = f.size(), w = f.back().size();
		Point st;
		st.x = 0, st.y = w / 2; // 雑

		queue<Point> q;
		q.push(st);
		int distSum = 0;
		if (!f[st.x][st.y]) {
			f[st.x][st.y] = 1;
			while (!q.empty()){
				auto &p = q.front();
				q.pop();
				for (int k = 0; k < 4; ++k) {
					auto nextPoint = p;
					nextPoint.x += dx[k];
					nextPoint.y += dy[p.x % 2][k];
					if (nextPoint.x < 0 || nextPoint.x >= h || nextPoint.y < 0 || nextPoint.y >= w || f[nextPoint.x][nextPoint.y]) continue;
					f[nextPoint.x][nextPoint.y] = f[st.x][st.y] + 1;
					distSum += f[st.x][st.y];
				}
			}
		}
		int holeCnt = 0;
		for (auto &v : f) for (auto & e : v) if (!e) ++holeCnt;
		return holeCnt * -5 + distSum * 0.3;
	}

	int heightScore(const vector <vector <int> > &f) {
		int h = f.size(), w = f.back().size();
		int sum = 0;
		for (int x = 0; x < h; ++x) {
			for (int y = 0; y < w; ++y) {
				if (f[x][y]) sum -= h - x;
			}
		}
		return sum;
	}

	int distScore(const vector <vector <int> > &ff) {
		auto f = ff;

	}


	int calc(vector <vector <int> > &field, int num, vector<Unit> &units, vector<int> &source){
		if (num == source.size()) return 0;

		if (!check(field, units[source[num]].pivot, 0, num)) return -1e9;
		return holeScore(field, num) + heightScore(field);
	}
};

int calc(vector <vector <int> > &field, int num) {
	TestEval e;
	return e.calc(field, num, units, source);

}