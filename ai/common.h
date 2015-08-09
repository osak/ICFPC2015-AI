#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <functional>
#endif
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

#ifdef _MSC_VER
int __builtin_popcountll(unsigned long long b) {
	int cnt = 0;
	while (b) ++cnt, b &= b - 1;
	return cnt;
}
#endif