#include <bits/stdc++.h>
#define POPULATION_SIZE 20 
#define TIMELIMIT 10
#define AWARD 250

using namespace std;

typedef struct Solution {
	vector<int> seq_a, seq_b;
	int best_value = INT_MIN, best_start, best_end;
} solution;

int size_a, size_b;
vector<vector<int>> BackwardEdges; // ai <- bj
vector<vector<int>> FowardEdges; // ai -> bj
vector<solution> population(POPULATION_SIZE);

int timediff_in_sec(clock_t start, clock_t end);
void solution_build(solution &sol);
bool solution_comp(solution &a, solution &b);
void solution_objective_function(solution &sol);
void solution_print(solution &sol);
void read_input(void);

int main(){
	read_input();
	solution sol, best;
	clock_t start = clock();
	while(timediff_in_sec(start, clock()) < TIMELIMIT){
		solution_build(sol);
		solution_objective_function(sol);
		if(solution_comp(sol, best))
			best = sol;
	}
	solution_print(best);
	return 0;
}


int timediff_in_sec(clock_t start, clock_t end){
	return (end - start)/ CLOCKS_PER_SEC;
}

void solution_build(solution &sol){
	sol.seq_a.resize(size_a);
	for(int i = 0; i < size_a; i++) sol.seq_a[i] = i;
	shuffle(sol.seq_a.begin(), sol.seq_a.end(), default_random_engine(clock()));
	sol.seq_b.resize(size_b);
	for(int i = 0; i < size_b; i++) sol.seq_b[i] = i;
	shuffle(sol.seq_b.begin(), sol.seq_b.end(), default_random_engine(clock()));
}

bool solution_comp(solution &a, solution &b){
	if(a.best_value > b.best_value)
		return true;
	if(a.best_value < b.best_value)
		return false;
	return (a.best_end - a.best_start) < (b.best_end - b.best_start);
}

void solution_objective_function(solution &sol){
	sol.best_value = 0;
	sol.best_start = 0;
	sol.best_end = 0;
	int curr_value = 0;
	int curr_start = 0;
	for(int i = 0; i < min(size_a, size_b); i++){
		if(i){
			curr_value -= BackwardEdges[sol.seq_b[i-1]][sol.seq_a[i]];
			if(curr_value <= 0){
				curr_value = 0;
				curr_start = i;
			}
		}
		curr_value += 2*AWARD - FowardEdges[sol.seq_a[i]][sol.seq_b[i]];
		if(curr_value >= sol.best_value){
			if(curr_value == sol.best_value  and sol.best_end - sol.best_start >= i + 1 - curr_start)
				continue;
			sol.best_value = curr_value;
			sol.best_start = curr_start;
			sol.best_end = i+1;
		}		
	}
}

void  solution_print(solution &sol){
	printf("OF: %d - Path: ", sol.best_value);
	for(int i = 0; i < min(size_a, size_b); i++){
		printf("(%d -> %d [%d])", sol.seq_a[i], sol.seq_b[i], FowardEdges[sol.seq_a[i]][sol.seq_b[i]]);
		if(i < min(size_a, size_b) - 1)
			printf(" -> ");
		else
			printf("\n");
	}
//	printf("Range: [%d,%d]\n", sol.best_start, sol.best_end);
}

void read_input(void){
	cin >> size_a >> size_b;
	FowardEdges.assign(size_a, vector<int>(size_b));
	for(auto &row: FowardEdges) for(auto &edge: row) cin >> edge;
	BackwardEdges.assign(size_b, vector<int>(size_a));
	for(auto &row: BackwardEdges) for(auto &edge: row) cin >> edge;
}
