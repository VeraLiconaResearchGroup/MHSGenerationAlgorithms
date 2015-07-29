#include <stdio.h>
#include<iostream>
#include <string.h>
#include<stdlib.h>
#include <set>
#include <vector>
#include <ctype.h>
#include <time.h>
#include <algorithm>

using namespace std;

/*quick sort by size*/
void SizeQSort( vector< vector<int> > &x, int left, int right){

	if( left < 0 || right < 0 ) return;
    int i, j;
    int pivot;
    i = left;
    j = right;
	pivot = (int)x[(left + right) / 2].size();

    while (1) {
		while ( (int)x[i].size() < pivot) i++; 
		while ( pivot < (int)x[j].size()) j--;  
        if (i >= j) break; 
		x[i].swap( x[j]); 
        i++;  
        j--;
    }
    if (left < i - 1) SizeQSort(x, left, i - 1);
    if (j + 1 <  right) SizeQSort(x, j + 1, right); 
}

/*allocation of a new element*/
void sort2( vector<int> &a ){

	int i, size;
	int temp;
	size = (int)a.size() - 1;
	for( i = size; i >0 ; i-- ){
		if( a[i] < a[i-1] ){
			temp =  a[i-1];
			a[i-1] = a[i];
			a[i] = temp;
		}
		else break;
	}
}


/*elimination of same nodes*/
void multicheck( vector<int>  &e ){
	vector<int> e1;
	e1.swap(e);
	e.clear();
	int i;
	int size = (int)e1.size();
		e.push_back(e1[0]);
		
	for( i = 0; i < size - 1; i++ ){
		if( e1[i] != e1[i+1] ){
			e.push_back( e1[i+1]);
		}
	}
	vector<int> ().swap( e1 );
}

/*check whether b \supset a or not*/
bool MinSet2( vector<int> &a, vector<int> &b ){

	vector<int>::iterator tk, tk2;
	int cnt;
	
	cnt = 0;
	tk = a.begin();
	tk2 = b.begin();

	while( tk != a.end() ){
		while( tk2 != b.end() ){
			if( *tk == *tk2 ){
				cnt++;
				if( cnt == a.size()){
					return (1);
				}
			++tk2;
			break;	
			}
			else if( *tk < *tk2 ){
				return (0);
			}
		++tk2;
		}
	++tk;
	}
return (0);
}
