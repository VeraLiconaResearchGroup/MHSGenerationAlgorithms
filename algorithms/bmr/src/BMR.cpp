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
long long iteration;

#include "SuperSet.cpp"
#include "kihon.cpp"

using namespace std;

#define MAX_LINE_SIZE	100000


/*partitioned hypergraph*/
typedef struct data{
	vector< vector<int> > EDGE;		// edge
	int ENUM;						// the number of edges
	int ParVer;						// partition point(node)
	int MaxVer;						// max node number in partitioned hypergraph
	int MinMaxVert;					
	int SolNum;						// the number of minimal hitting set
	int SolNum0;					// the number of minimal hitting set in the adjacent partitioned hypergraph
	vector< vector<int> > TRA;		// minimal hitting set
	vector< vector<int> > TRA0;		// minimal hitting set in the adjacent partitioned hypergraph
	vector<int> VN;				
}PROBLEM;

/*order nodes by increasing number of occurrences*/
void SortVerNum( int ENum, vector< vector<int> > &edge, vector<int> &VerN ){
		
		int i;
		vector<int>::iterator edgkm;
		int MAXVERT = 0;
		for( i = 0; i < ENum; i++ ){
			edgkm =  edge[i].end();
			--edgkm;
			if( *edgkm >  MAXVERT ){
				MAXVERT = *edgkm;
			}
		}
		
		int *VerNum = new int[MAXVERT+1];
		int *VerO = new int[MAXVERT+1];
		VerNum[0] = -1;
		VerO[0] = 0;
		for( i = 1; i < MAXVERT+1; i++){
			VerNum[i] = 0;
			VerO[i] = i;
		}
		
		for( i = 0; i < ENum; i++){		
			edgkm =  edge[i].begin();
			while( edgkm != edge[i].end() ){
				 VerNum[*edgkm]++;
			++edgkm;
			}
		}
		
		QSort( VerNum, VerO, 0, MAXVERT );
				
		int *VerNo = new int[MAXVERT+1];
		for( i = 0; i < MAXVERT+1; i++){
			VerNo[VerO[i]] = i;
			VerN.push_back( VerO[i] );
		}
	
		vector<int> tempedge;
		for(int i = 0; i < ENum; i++){		
			edgkm =  edge[i].begin();
			while( edgkm != edge[i].end() ){
				tempedge.push_back(VerNo[*edgkm]);
			++edgkm;
			}
			edge[i].clear();
			edge[i] = tempedge;
			tempedge.clear();
			sort( edge[i].begin(), edge[i].end() );
		}

	delete[] VerNo;
	delete[] VerO;
}

/*restore node number*/
void ReturnVer( vector< vector<int> > &traver,  vector<int> VN ){

	vector< vector<int> > traver2;
	vector<int> dummy;
	traver2.swap( traver );
	traver.clear();	

	vector<int>::iterator trit;
	int i;
	for( i = 0; i < (int)traver2.size(); i++ ){
		trit = traver2[i].begin();
		traver.push_back(dummy);
		while( trit != traver2[i].end() ){
			traver[i].push_back( VN[*trit] );
		++trit;
		}
		sort( traver[i].begin(), traver[i].end() );
	}

	vector< vector<int> > ().swap( traver2 );
}

/*DL algorithm*/
void DL_algorithm( vector< vector<int> > &edge, int ENum, vector< vector<int> > &Trav, int* Solnum0 ){

	int i, h, j;
		
	vector< vector<int> > Tr;			// minimal hitting sets
	vector<int> dummy;
	vector<int>::iterator edgit;
	
	/*initial minimum hitting set*/
	int Solnum = (int)edge[0].size();
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
	
	for(i = 0; i < ENum; i++ ){					// addition of hyperedge loop
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

		vector< vector<int> > ().swap( Tr );		// memory release
		
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
		iteration = iteration + t1;						
		Solnum = t1;
		Tr.swap(Trg);		// update of minmal hitting set
		vector< vector<int> > ().swap( Trg );		// memory release
	}
	Trav.swap( Tr );
	*Solnum0 = Solnum;
	vector< vector<int> > ().swap( Tr );		// memory release
}
	
/*BMR algorithm*/	
void BMR_algorithm( vector< PROBLEM > &Prob, int &le, set<int> *AllVert ){

	PROBLEM dummy4;
	Prob.push_back( dummy4 );

	set<int>::iterator AllVertk;
	vector<int>::iterator edgkm;
	vector<int> dummy;
	vector<int> tempEDGE;			// temp用のハイパーエッジ
	int i, g, superset;
	int l = le;
	
	/*detection of max node number*/
	Prob[l].MaxVer = 0;
	for( i = 0; i < Prob[l].ENUM; i++ ){
		edgkm =  Prob[l].EDGE[i].end();
		--edgkm;
		if( *edgkm >  Prob[l].MaxVer ){
			Prob[l].MaxVer = *edgkm;
		}
	}

	SortVerNum( Prob[l].ENUM, Prob[l].EDGE, Prob[l+1].VN);			// order node
		
	Prob[l].MinMaxVert = 0;
	for( i = 0; i < Prob[l].ENUM; i++ ){
		edgkm =  Prob[l].EDGE[i].begin();
		if( *edgkm >  Prob[l].MinMaxVert ){
			Prob[l].MinMaxVert = *edgkm;
		}
	}
					
	l = l + 1;
	Prob[l].ParVer = Prob[l-1].MinMaxVert;
	Prob[l].SolNum = 0;
		
	/*main loop*/	
	while( Prob[l].ParVer <=  Prob[l-1].MaxVer ){
		vector<int>::iterator itver;
		set< vector<int> > tEdge;
		int tedsize;
		int j = 0;
		vector<int>::iterator EDGEke;
		Prob[l].EDGE.clear();
		vector<int>::iterator eend;

		for( i = 0; i < Prob[l-1].ENUM; i++ ){			// partition of hypergraph
			eend =  Prob[l-1].EDGE[i].end();
			--eend;
			itver =  Prob[l-1].EDGE[i].begin();
			if( *itver <= Prob[l].ParVer ){
				while( *itver <= Prob[l].ParVer){
					tempEDGE.push_back( *itver);
					if( itver == eend ) break;
				++itver;
				}
				EDGEke = tempEDGE.end();
				--EDGEke;
				if( *EDGEke != Prob[l].ParVer){
					tedsize = (int)tEdge.size();
					tEdge.insert( tempEDGE );
					if((int)tEdge.size() > tedsize ){
						 Prob[l].EDGE.push_back(tempEDGE);
						j++;
					}
				}
				
			}
			
		tempEDGE.clear();
		}
						
		Prob[l].ENUM = j;
		vector<int> ().swap( tempEDGE );		// memory release
		
		/*BMR or DL */
		double a = 0;
		Prob[l].TRA0.clear();				// clear
		for(i = 0; i < Prob[l].SolNum; i++ ) Prob[l].TRA0.push_back( Prob[l].TRA[i] );		// copy
		Prob[l].TRA.clear();
		Prob[l].SolNum0 = Prob[l].SolNum;
					
		for(i = 0; i < Prob[l].ENUM; i++ ) a = a + Prob[l].EDGE[i].size();
		if( a > 600 && Prob[l].ENUM >= 2 ){
			BMR_algorithm( Prob, l, AllVert );					// BMR algorithm
			Prob[l].TRA.clear();
			for( i = 0; i < Prob[l+1].SolNum; i++ ){
				Prob[l].TRA.push_back( Prob[l+1].TRA[i]);
			}
			Prob[l].SolNum = Prob[l+1].SolNum;
			Prob.pop_back();
		}
		else{
			if( Prob[l].ENUM == 0 ){
				Prob[l].TRA.push_back(dummy);
				Prob[l].SolNum = 1;
			}
			else DL_algorithm( Prob[l].EDGE, Prob[l].ENUM, Prob[l].TRA, &Prob[l].SolNum );	// DL-algorithm
		}

		/*minmal hitting set \cup partition node*/
		vector< vector<int> > TRA_z, TRA_zb;
		TRA_z.reserve( Prob[l].SolNum0 );
		TRA_zb.reserve( Prob[l].SolNum );
		
		for( i = 0; i < Prob[l].SolNum0; i++ ) TRA_z.push_back( Prob[l].TRA0[i] );
					
		for( i = 0; i <  Prob[l].SolNum; i++  ){
			TRA_zb.push_back( Prob[l].TRA[i] );
			TRA_zb[i].push_back( Prob[l].ParVer );
		}
		Prob[l].TRA.clear();
			
		double sumTrg = 0;
		double aveTrg = 0;
		int t1 = (int)TRA_z.size();
		int t2 = Prob[l].SolNum0;
		int h;
		
		if( t1 >= 30000 && t2 >= 10 ){			// computation to judge which minimality check version is employed
			if( t1 >10 ){
				for( h = 0; h < 10; h++ ) sumTrg =  sumTrg + TRA_z[ (int)(t1-1)/10 * h ].size();
			}
			else for( h = 0; h < t1; h++ ) sumTrg =  sumTrg + TRA_z[ h ].size();
			
			aveTrg = sumTrg / 10;
		}
		
		if( t1 < 30000 || t2 < 10 || aveTrg > 8 || aveTrg < 3){			// minimality check (size sort version)
			SizeQSort( TRA_z, 0, Prob[l].SolNum0 - 1);
			int s0 =  Prob[l].SolNum0;
			for( i = 0; i < Prob[l].SolNum; i++ ){
				superset = 0;
				for( g = 0; g < s0 ; g++ ){
					if( (int)TRA_z[g].size() < (int)TRA_zb[i].size()){
						min_check++;
						if( MinSet2( TRA_z[g], TRA_zb[i]) == 1 ){
							superset = 1;
							break;
						}
					}
					else{
						break;
					}
				}
				if(superset == 0){
					TRA_z.push_back(TRA_zb[i]);
					Prob[l].SolNum0++;
				}
			}
		}
		else{														// minimality check (binary search version)
			vector< vector<int> > TRA_z1;
			sort( TRA_z.begin(), TRA_z.end());
			int sset, K;
			for( i = 0; i < Prob[l].SolNum; i++ ){
				sset = 0;
				for( K = (int)TRA_zb[i].size(); K > 0; K-- ){
					if( SuperSet( (int)TRA_zb[i].size(), K, TRA_z, TRA_zb[i] ) == 1 ){ 
						sset = 1;
						break;
					}
				}
				if( sset == 0 ){
					TRA_z1.push_back(TRA_zb[i]);
					Prob[l].SolNum0++;
				}
			}
			for( j = 0; j < (int)TRA_z1.size(); j++ ) TRA_z.push_back( TRA_z1[j]);
			vector< vector<int> > ().swap(TRA_z1 );
		}
			
		/*update of minmal hitting set*/
		Prob[l].SolNum = Prob[l].SolNum0;
		for (g = 0; g < Prob[l].SolNum; g++ ){		
			Prob[l].TRA.push_back( TRA_z[g] );
		}

		Prob[l].ParVer = Prob[l].ParVer + 1;
		
		vector< vector<int> > ().swap(TRA_z );		// memory release
		vector< vector<int> > ().swap(TRA_zb );		// memory release
		
	}

	 ReturnVer(  Prob[l].TRA, Prob[l].VN );			// restore node number
	 Prob[l].VN.clear();
}


int main(int argc, char *argv[]){
	
	int i;
	min_check = 0;
	iteration = 0;
	
	/*----input of instance----*/
	vector< vector<int> > edge;		// hyperedges
	vector<int> dummy;
	set<int> AllVert;
	set<int>::iterator alit;
	int ENum = 0;			// the number of hyperedges
	
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
         
	 while ( fgets( buff, sizeof(buff), fp ) != NULL ){ //←sizeof(buff)
		if ( ( seek = strtok( buff, tokn ) ) != NULL ){
			 do {
				l = strtol( seek, &ends, 0 );
				if( ends != seek ){
					value = atoi( seek ); 
					edge.push_back(dummy);
					edge[ENum].push_back(value);
					AllVert.insert(value);
				}
				/*else{
					 cout<<"\ninput_data:" << ENum+1 << " :error: "<< ends<<"\n";
					 cout<< "(It's probably because the tail of the line is blank, isn't it ?)";
					 exit(0);
				}*/
			 } while ( ( seek = strtok( NULL, tokn)) != NULL );
		 }
		sort( edge[ENum].begin(), edge[ENum].end() );
		multicheck( edge[ENum] );
	ENum++;
	}
	fclose(fp);

	/*preparation*/
	vector<int>::iterator egit; 
	int MaxVer = 0;
	for( i = 0; i < ENum; i++ ){
		egit =  edge[i].end();
		--egit;
		if( *egit >  MaxVer ){
			MaxVer = *egit;
		}
	}


	int *V = new int[ AllVert.size() + 1 ];
	int *re_V = new int[ MaxVer + 1 ];
	
	alit = AllVert.begin();
	for( i = 1; i < (int)AllVert.size() + 1; i++ ){
		V[i] = *alit;
	++alit;
	}

	set<int> tAV;
	for( i = 1; i < (int)AllVert.size() + 1; i++ ){
		re_V[ V[i] ] = i;
		tAV.insert( i );
	}

	vector< PROBLEM > Prob;
	PROBLEM dummy4;
	Prob.push_back( dummy4 );
	
	for(i = 0; i < ENum; i++ ){	
		Prob[0].EDGE.push_back( dummy );
		egit =  edge[i].begin();
		while( egit != edge[i].end() ){
			Prob[0].EDGE[i].push_back( re_V[ *egit ] );
		++egit;
		}
	}


	clock_t time1, time2;
	time1 = clock();
	
	
	Prob[0].SolNum = 0;
	Prob[0].SolNum0 = 0;
	Prob[0].ENUM =  ENum;
	int l2 = 0;
	
	/*BMR algorithm*/
	BMR_algorithm( Prob, l2, &tAV );

	time2 = clock();

	double TIME;
	TIME = ( time2 - time1 ) / CLOCKS_PER_SEC;


	/*----output----*/
	vector<int>::iterator Trk;
	for(i = 0; i <  Prob[1].SolNum; i++){
		Trk =  Prob[1].TRA[i].begin();
		while( Trk !=Prob[1].TRA[i].end() ){
			fprintf( gp, "%d ", V[ *Trk ] );
		++Trk;
		}
		fprintf( gp, "\n" );
	}
	fclose(gp);
	
	cout<< "#min_tr = " <<  Prob[1].SolNum <<"\n"; 
	printf("#min_check = %lld, ", min_check);
	printf("#iteration = %lld\n", iteration);
	

	delete[] V;
	delete[] re_V;
		

 return 0;
}
