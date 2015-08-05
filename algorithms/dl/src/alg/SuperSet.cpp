#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef unsigned int set2;
#define first(n) ((set2) ((1U << (n)) - 1U))


/*binary search*/
int bsrch( vector< int > &x, vector< vector<int> > &a, int left, int right){
	int mid;

	while (left < right) {
		min_check++;
		mid = (left + right) / 2;
		if (a[mid] < x) left = mid + 1;  else right = mid;
	}
	if (a[left] == x) return left;
	return -1;
}

set2 nextset(set2 x){
	set2 smallest, ripple, new_smallest, ones;

	smallest = x & -x;
	ripple = x + smallest;
	new_smallest = ripple & -ripple;
	ones = ((new_smallest / smallest) >> 1) - 1;
	return ripple | ones;
}

int printset(set2 s, int N, int K,  vector< vector<int> > &Set0, vector<int> &Set1 ){
	int i;
	vector<int> subset;
	for (i = 1; i <= N; i++) {
		if (s & 1)  subset.push_back( Set1[i-1] );
		s >>= 1;
	}
	return bsrch( subset, Set0, 0, (int)Set0.size() - 1 );
}

int SuperSet( int N, int K, vector< vector<int> > &Set0, vector<int> &Set1 ){
	int i;
	set2 x;

	i = 1;  x = first(K);
	while (! (x & ~first(N))) {
		if( printset(x, N, K, Set0, Set1 ) != -1 ) return 1;
		x = nextset(x);  i++;
	}
	return 0;
}
