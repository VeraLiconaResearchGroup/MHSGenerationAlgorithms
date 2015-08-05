#include <stdio.h>
#include<iostream>
#include <string.h>
#include<stdlib.h>
#include <set>
#include <vector>
#include <ctype.h>
#include <time.h>
#include <algorithm>

long long int min_check;
long long int iteration;

#include "SuperSet.cpp"
#include "kihon.cpp"

using namespace std;

#define MAX_LINE_SIZE	100000


/*DL algorithm*/
void DL_algorithm( vector< vector<int> > &edge, int ENum, vector< vector<int> > &Tr, double &TIME ){

	int i, h, j;
	clock_t time1, time2;
	time1 = clock();
	vector<int> dummy;
	vector<int>::iterator edgit;

	/*initial minimum hitting set*/
	int Solnum = (int)edge[0].size();	// the number of minimum hitting sets
	edgit = edge[0].begin();
	i = 0;
	while( edgit != edge[0].end() ){
		Tr.push_back(dummy);
		Tr[i].push_back(*edgit);
	++edgit;
	i++;
	}

	/*addition of hyperedge*/
	vector< vector<int> > Trg;		// minimal hitting set intersecting the additional hyperedge
	vector< vector<int> > Tr_d;		// minimal hitting set except Trg
	vector<int> ecov;				//  the set of cardinality 1 and in Trg
	vector<int> e_d;				// the additional hyperedge except ecov
	int t1, t2, t3;					// the number of Trg, Tr_d, ecov;
	int g, g2;
	vector<int>::iterator Trit;
	vector<int>::iterator ecovk, e_dit;
	vector<int> Tr_dt;
	vector<int>::iterator edk;
	int superset;
	int frg;
	
	for(i = 0; i < ENum; i++ ){		// addition of hyperedge loop
		 t1 = 0;
		 t2 = 0;
		 t3 = 0;
				
		 for( h = 0; h < Solnum; h++){		// detection of intersection
			 frg = 0;
			 edgit = edge[i].begin();
			 Trit = Tr[h].begin();
			 while( edgit != edge[i].end() && Trit != Tr[h].end() ){
				if( *edgit < *Trit ) ++edgit;
				else if( *edgit > *Trit ) ++Trit;
				else{
					Trg.push_back( Tr[h] );
					if( Trg[t1].size() == 1 ){
						ecov.push_back( *Trit );
						t3 = t3 + 1;
					}
				t1 = t1 + 1;
				frg = 1;
				break;
				}
			}
			if( frg == 0 ){
				 Tr_d.push_back( Tr[h] );
				 t2 = t2 + 1;
			}
		}
		vector< vector<int> > ().swap( Tr );		//  memory release
					
		if( ecov.size() != 0 ){			// creation of e_d
			e_dit = edge[i].begin();
			ecovk = ecov.begin();
			while( e_dit != edge[i].end() ){
				if( ecovk == ecov.end() ){
					e_d.push_back( *e_dit );
				++e_dit;
				}
				else{
					if( *ecovk < *e_dit ){
						e_d.push_back( *e_dit);
						++ecovk;
					}
					else if( *ecovk > *e_dit ){
						e_d.push_back( *e_dit);
						++e_dit;
					}
					else{
						++ecovk;
						++e_dit;
					}
				}
			}
		}
		else e_d = edge[i];
		
		vector<int> ().swap( ecov );		// memory release
		
		double sumTrg = 0;
		double aveTrg = 0;
		if( t1 >= 30000 && t2 >= 10 ){		// computation to judge which minimality check version is employed
			if( t1 >10 ){
				for( h = 0; h < 10; h++ ) sumTrg =  sumTrg + Trg[ (int)(t1-1)/10 * h ].size();
			}
			else for( h = 0; h < t1; h++ ) sumTrg =  sumTrg + Trg[ h ].size();
			
			aveTrg = sumTrg / 10;
		}
			
		if( t1 < 30000 || t2 < 10 || aveTrg > 8 ){		// minimality check (size sort version)
			SizeQSort( Tr_d, 0, t2 - 1);
			int t1c = t1;
			for( g2 = 0; g2 < t2; g2++ ){
				edk = e_d.begin();
				while( edk != e_d.end() ){
					superset = 0;
					Tr_dt.clear();
					Tr_dt = Tr_d[g2];
					Tr_dt.push_back( *edk );
					sort2( Tr_dt );
				
					if( t1c == 0 ){
						Trg.push_back(Tr_dt);
						t1++;
					}
					else{
						for( g = 0; g < t1c; g++ ){
							if( Trg[g].size() <= Tr_dt.size()){
								min_check++;							// min_check counter
								if( MinSet2( Trg[g], Tr_dt) == 1 ){
									superset = 1;
									break;
								}
							}
						}
						if(superset == 0){
							Trg.push_back(Tr_dt);
							t1++;
						}
					}
				++edk;
				}
			}
		}
		else{					// minimality check (binary search version)
			vector< vector<int> > Trg1;
			sort( Trg.begin(), Trg.end());
			int t1c = t1;
			int sset, K;
			for( g2 = 0; g2 < t2; g2++ ){
				edk = e_d.begin();
				while( edk != e_d.end() ){
					Tr_dt.clear();
					Tr_dt = Tr_d[g2];
					Tr_dt.push_back( *edk );
					sort2( Tr_dt );
					sset = 0;

					if( t1c == 0 ){
							Trg.push_back(Tr_dt);
							t1++;
					}
					else{
						for( K = (int)Tr_dt.size(); K > 1; K-- ){
							if( SuperSet( (int)Tr_dt.size(), K, Trg, Tr_dt ) == 1 ){
								sset = 1;
								break;
							}
						}
						if( sset == 0 ){
							Trg1.push_back(Tr_dt);
							t1++;
						}
					}
					
				++edk;
				}
			}
			for( j = 0; j < (int)Trg1.size(); j++ ) Trg.push_back( Trg1[j]);
		vector< vector<int> > ().swap( Trg1 );		//  memory release
		}
			
		vector< vector<int> > ().swap( Tr_d );		// memory release
		vector<int> ().swap( e_d );					// memory release
								
		Solnum = t1;
		Tr.swap(Trg);		// update of minmal hitting set
		
		vector< vector<int> > ().swap( Trg );		// memory release
		
		iteration = iteration + t1;					// iteration counter
	}
	
	time2 = clock();
	TIME = ( time2 - time1 ) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]){
	
	vector< vector<int> > edge;		// hyperedge
	edge.reserve(1000000);
	vector<int> dummy;
	int ENum = 0;			// the number of hyperedges
	int i;
	
	/*----input of instance----*/
	FILE	*fp, *gp;
    char    buff[100000];
	const char *tokn = ",\t \n"; 
	char *seek;
	char *ends;
	long l;
	int value;

	if(argc != 3){
            printf("wrong command.\n");
            return 0;
	}
    fp = fopen(argv[1], "r");
    if(fp == NULL){
            printf("inputfile error %s.\n", argv[1]);
            return 0;
    }
    gp = fopen(argv[2], "w"); 
    if(gp == NULL){
            printf("outputfile error %s.\n", argv[2]);
            return 0;
    }
    while ( fgets( buff, sizeof(buff), fp ) != NULL ){
		if ( ( seek = strtok( buff, tokn ) ) != NULL ){
			do {
				l = strtol( seek, &ends, 0 );
				if( ends != seek ){
				value = atoi( seek ); 
				edge.push_back(dummy);
				edge[ENum].push_back(value);
				}
				/*else{
					 cout<<"\ninput_data:" << ENum+1 << " :error: "<< ends<<"\n";
					 cout<< "(It's probably because the tail of the line is blank, isn't it ?)";
					 exit(0);
				}*/
			 } while ( ( seek = strtok( NULL, tokn)) != NULL );
		}
		sort( edge[ENum].begin(), edge[ENum].end() );		// sort of nodes
		multicheck( edge[ENum] );					// elimination of same nodes
	ENum++;
	}
	fclose(fp);
        
	/*main*/
	vector< vector<int> > Tr;	// minimal hitting sets
	double TIME;
	min_check = 0;
	iteration = 0;
	DL_algorithm( edge, ENum, Tr, TIME );	// DL algorithm
	
	/*----output----*/
	vector<int>::iterator Trk;
	int Solnum = (int)Tr.size();
	for(i = 0; i < Solnum; i++){
		Trk =  Tr[i].begin();
		while( Trk != Tr[i].end() ){
		fprintf( gp, "%d ", *Trk );
		++Trk;
		}
		fprintf( gp, "\n" );
	}
		
	fclose(gp);
	cout<< "#min_tr = " << Solnum <<"\n"; 
	printf("#min_check = %lld, ", min_check);
	printf("#iteration = %lld\n", iteration);

return 0;
}
