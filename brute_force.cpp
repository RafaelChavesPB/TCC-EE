#include <bits/stdc++.h>
#define limit 12 
#define AWARD 250
using namespace std;

typedef struct solution {
	int path[2*limit+1];
	int pos = 0, value;
} solution;

short forwardEdges[limit][limit];
short backwardEdges[limit][limit];
short pd[2 << limit][2 << limit][limit];
int n, m;

void fillPD(int value){
	for(int i = 0; i < (1 << limit); i++)
		for(int j = 0; j < (1 << limit); j++)
			for(int k = 0; k < limit; k++)
				pd[i][j][k] = value;
}

int rec(int bm_a, int bm_b, int last_b){
	int best = 0, sol;
	if(__builtin_popcount(bm_a) == n or __builtin_popcount(bm_b) == m)
		return 0;

	if(last_b >= 0 and pd[bm_a][bm_b][last_b] >= 0)
		return pd[bm_a][bm_b][last_b];

	for(int i = 0; i < n; i++){
		if(bm_a & (1 << i))
			continue;
		for(int j = 0; j < m; j++){
			if(bm_b & (1 << j))
				continue;
			sol = rec(bm_a | (1 << i), bm_b | (1 << j),  j);
			sol += AWARD*2 - forwardEdges[i][j];
			if(last_b >= 0)
				sol -= backwardEdges[i][last_b];
			best = max(sol, best);
		}
	}

	if(last_b >= 0)
		pd[bm_a][bm_b][last_b] = best;
	return best; 
}


// solution* findPath(unordered_set<int> &a, unordered_set<int> &b, int last_b, int curr){
// 	solution *best = (solution*) malloc(sizeof(solution));
// 	best->value = curr;
// 	best->pos = 0;
// 	if(a.empty() or b.empty())
// 		return best;
// 	const unordered_set<int> a_cp = a;
// 	const unordered_set<int> b_cp = b;
// 	int aux = curr + 2*AWARD;
// 	for(auto i: a_cp){
// 		if(last_b != -1){
// 			if(aux < backwardEdges[i][last_b])
// 				continue;
// 			aux -= backwardEdges[i][last_b]; 
// 		}
// 		a.erase(i);
// 		for(auto j: b_cp){
// 			if(aux < forwardEdges[i][j])
// 				continue;
// 			b.erase(j);
// 			solution *sol = findPath(a, b, j, aux - forwardEdges[i][j]);
// 			if(sol->value > best->value or ( sol -> value == best -> value and sol->pos <= best->pos - 2)){
				
// 				free(best);
// 				best = sol;
// 				sol = NULL;
// 				best->path[best->pos++] = j;
// 				best->path[best->pos++] = i;
// 			}else{
// 				free(sol);
// 				sol = NULL;
// 			}
// 			b.insert(j);
// 		}
// 		if(last_b != -1)
// 			aux += backwardEdges[i][last_b]; 
// 		a.insert(i);
// 	}
// 	return best;
// }

int main(){
	cin >> n >> m;
	for(int i = 0; i < n; i++)
		for(int j = 0;  j < m; j++)
			cin >> forwardEdges[i][j];
	for(int i = 0; i < n; i++)
		for(int j = 0;  j < m; j++)
			cin >> backwardEdges[i][j];
	fillPD(-(n+m+1)*AWARD);
	cout << rec(0, 0, -1) << endl;

/*
	solution* ans = findPath(a, b, -1, 0);
	printf("OF %d - ", ans-> value);
	for(int i = ans -> pos - 1; i >= 0; i -= 2)
	{
		int a = ans->path[i];
		int b = ans->path[i-1];
		int edge = forwardEdges[a][b];
		printf("(%d -> %d [%d])", a, b, edge);
		if(i > 1)
			printf(" -> ");
	}
	printf("\n");
*/

}
