#include <bits/stdc++.h>
#define POPULATION_SIZE 1000 
#define MAXIT 100000
#define TIMELIMIT 10
#define AWARD 250

using namespace std;

typedef struct Solution {
	vector<int> seq_a, seq_b;
	int best_value = INT_MIN, best_start, best_end;
} solution;

int size_a, size_b;
auto random_generator = default_random_engine(clock());
vector<vector<int>> BackwardEdges; // ai <- bj
vector<vector<int>> FowardEdges; // ai -> bj
vector<solution> population(POPULATION_SIZE);

void crossover_operation(vector<int> &a, vector<int> &b, vector<int> &merged, int cut_start, int cut_len);
void solution_build(solution &sol);
bool solution_comp(solution &a, solution &b);
solution solution_crossover(solution &a, solution &b);
void solution_objective_function(solution &sol);
void solution_print(solution &sol);
void read_input(void);
int timediff_in_sec(clock_t start, clock_t end);

int main(){
	read_input();
	solution best;
	int it = 0;
	vector<int> reproduction_order(POPULATION_SIZE);
	for(int i = 0; i < POPULATION_SIZE; i++) reproduction_order[i] = i;
	for(auto &sol: population){
		solution_build(sol);
		solution_objective_function(sol);
	}
	clock_t start = clock();
	while(it++ <= MAXIT){
		shuffle(reproduction_order.begin(), reproduction_order.end(), random_generator);
		for(int i = 0; i < POPULATION_SIZE; i+=2) {
			int a = reproduction_order[i];
			int b = reproduction_order[i+1];
			population.push_back(solution_crossover(population[a], population[b]));
		}
		sort(population.begin(), population.end(), solution_comp);
		while(population.size() > POPULATION_SIZE) population.pop_back();
		//cout << population.front().best_value <<  " " << population.back().best_value << endl;
		if(solution_comp(population.front(), best)){
			it = 0;
			best = population.front();
		}
		
	}
	solution_print(best);
	return 0;
}

void crossover_operation(vector<int> &a, vector<int> &b, vector<int> &merged, int cut_start, int cut_len){
	vector<int> used(a.size(), 0);
	merged.resize(a.size());
	for(int i = 0, j = cut_start; i < cut_len; i++, j++){
		merged[i] = a[j];
		used[a[j]] = 1; 
	}
	for(int i = cut_len, j = 0; i < a.size(); i++){
		while(used[b[j]]) j++;
		merged[i] = b[j];
		used[b[j]] = 1;
	}
}


void solution_build(solution &sol){
	sol.seq_a.resize(size_a);
	for(int i = 0; i < size_a; i++) sol.seq_a[i] = i;
	shuffle(sol.seq_a.begin(), sol.seq_a.end(), random_generator);
	sol.seq_b.resize(size_b);
	for(int i = 0; i < size_b; i++) sol.seq_b[i] = i;
	shuffle(sol.seq_b.begin(), sol.seq_b.end(), random_generator); 
}

bool solution_comp(solution &a, solution &b){
	if(a.best_value > b.best_value)
		return true;
	if(a.best_value < b.best_value)
		return false;
	return (a.best_end - a.best_start) < (b.best_end - b.best_start);
}

solution solution_crossover(solution &a, solution &b){
	solution sol;
	int cut_start_a = random_generator() % (size_a/2);
	int cut_len_a = max((int) (random_generator() % (size_a - cut_start_a)), 2*size_a/5);
	crossover_operation(a.seq_a, b.seq_a, sol.seq_a, cut_start_a, cut_len_a);

	int cut_start_b = random_generator() % (size_b/2);
	int cut_len_b = max((int) (random_generator() % (size_b - cut_start_b)), 2*size_b/5);
	crossover_operation(a.seq_b, b.seq_b, sol.seq_b, cut_start_b, cut_len_b);

	solution_objective_function(sol);
	return sol;
}

void solution_objective_function(solution &sol){
	sol.best_value = 0;
	sol.best_start = 0;
	sol.best_end = 0;
	int curr_value = 0;
	int curr_start = 0;
	for(int i = 0; i < min(size_a, size_b); i++){
		if(i){
			curr_value -= BackwardEdges[sol.seq_a[i]][sol.seq_b[i-1]];
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
	printf("[%d - %d]\n", sol.best_start, sol.best_end);
}

void read_input(void){
	cin >> size_a >> size_b;
	FowardEdges.assign(size_a, vector<int>(size_b));
	for(auto &row: FowardEdges) for(auto &edge: row) cin >> edge;
	BackwardEdges.assign(size_b, vector<int>(size_a));
	for(auto &row: BackwardEdges) for(auto &edge: row) cin >> edge;
}

int timediff_in_sec(clock_t start, clock_t end){
	return (end - start)/ CLOCKS_PER_SEC;
}
