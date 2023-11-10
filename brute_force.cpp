#include <bits/stdc++.h>
#define LIMIT 8
#define AWARD 250
using namespace std;

typedef struct Solution {
	int path[2*LIMIT+1];
	int pos = 0, value;
} solution;

int forwardEdges[LIMIT][LIMIT];
int backward[LIMIT][LIMIT];
int n, m;

solution* rec(unordered_set<int> &a, unordered_set<int> &b, int last_b, int curr){
	solution *best = (solution*) malloc(sizeof(solution));
	best->value = curr;
	best->pos = 0;
	if(a.empty() or b.empty())
		return best;
	const unordered_set<int> a_cp = a;
	const unordered_set<int> b_cp = b;
	int aux = curr + 2*AWARD;
	for(auto i: a_cp){
		if(last_b != -1){
			if(aux < backward[i][last_b])
				continue;
			aux -= backward[i][last_b]; 
		}
		a.erase(i);
		for(auto j: b_cp){
			if(aux < forwardEdges[i][j])
				continue;
			b.erase(j);
			solution *sol = rec(a, b, j, aux - forwardEdges[i][j]);
			if(sol->value > best->value or ( sol -> value == best -> value and sol->pos <= best->pos - 2)){
				
				free(best);
				best = sol;
				sol = NULL;
				best->path[best->pos++] = j;
				best->path[best->pos++] = i;
			}else{
				free(sol);
				sol = NULL;
			}
			b.insert(j);
		}
		if(last_b != -1)
			aux += backward[i][last_b]; 
		a.insert(i);
	}
	return best;
}

int main(){
	cin >> n >> m;
	for(int i = 0; i < n; i++)
		for(int j = 0;  j < m; j++)
			cin >> forwardEdges[i][j];
	for(int i = 0; i < m; i++)
		for(int j = 0;  j < n; j++)
			cin >> backward[i][j];
	unordered_set<int> a, b;
	for(int i = 0; i < n; i++)
		a.insert(i);
	for(int j = 0; j < m; j++)
		b.insert(j);
	solution* ans = rec(a, b, -1, 0);
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
}
