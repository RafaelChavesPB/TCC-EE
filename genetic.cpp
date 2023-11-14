#include <bits/stdc++.h>
#define MUTATION_RATE 0.03
#define NEW_POPULATION_RATE 0.90
#define POPULATION_SIZE 100 
#define TIMELIMIT 10 
#define AWARD 250
#define TIME_NOW timediff_in_sec(start, clock())

using namespace std;

int size_a, size_b, min_size, generation;
auto random_generator = default_random_engine(clock());
clock_t start; 

typedef struct Solution {
	vector<int> seq_a, seq_b;
	int best_value, best_start, best_end, generation;
	double created_at;
	Solution(){
		this->created_at = 0;
		this->best_value = INT_MIN;
		this->seq_a.resize(size_a);
		this->seq_b.resize(size_b);
	}
} solution;


vector<vector<int>> BackwardEdges; // ai <- bj
vector<vector<int>> FowardEdges; // ai -> bj
vector<solution> population(POPULATION_SIZE);

void read_input(void);
void remove_duplicates(vector<int> &seq);
void grasp_build(solution &sol);
int grasp_build_operation(int idx_a, vector<bool> &visited_b);
bool solution_comp(solution &a, solution &b);
void solution_crossover(solution &a, solution &b, solution &children0, solution &children1);
void solution_crossover_operation(vector<int> &father, vector<int> &mother, vector<int> &children, int cut_start, int cut_end);
void solution_mutation(solution &sol);
void solution_objective_function(solution &sol);
void solution_print(solution &sol);
double timediff_in_sec(clock_t start, clock_t end);


int max_build = INT_MIN;
int max_mutation = INT_MIN, max_mutation_diff = INT_MIN;
int max_crossover = INT_MIN, max_crossover_diff = INT_MIN;

int main(){
	read_input();
	solution best;
	vector<int> reproduction_order(POPULATION_SIZE);
	for(int i = 0; i < POPULATION_SIZE; i++) reproduction_order[i] = i;
	for(auto &sol: population) grasp_build(sol);
	start = clock();
	int time_next_step = 1;
	while(TIME_NOW - best.created_at < TIMELIMIT){
		if(int(TIME_NOW) == time_next_step){
			printf("Melhor: %d, ",best.best_value);
			printf("Primeiro: %d, ",population.front().best_value);
			printf("Ultimo: %d\n",population.back().best_value);
			time_next_step++;
		}
		for(int i = 0; i < POPULATION_SIZE*MUTATION_RATE; i++){
			int idx = random_generator() % POPULATION_SIZE;
			solution_mutation(population[idx]);
		}
		shuffle(reproduction_order.begin(), reproduction_order.end(), random_generator);
		for(int i = 0; i < POPULATION_SIZE; i+=2) {
			int a = reproduction_order[i];
			int b = reproduction_order[i+1];
			solution children0, children1;
			solution_crossover(population[a], population[b], children0, children1);
			population.push_back(children0);
			population.push_back(children1);
		}
		sort(population.begin(), population.end(), solution_comp);
		while(population.size() > POPULATION_SIZE*NEW_POPULATION_RATE) population.pop_back();
		while(population.size() < POPULATION_SIZE){
			solution sol;
			grasp_build(sol);
			population.push_back(sol);
		}
		if(solution_comp(population.front(), best))
			best = population.front();
		generation++;
	}
	printf("generations: %d, total_time: %.3lf\n", generation, TIME_NOW);
	printf("best: %d, generations: %d, created_at: %.3lf\n", best.best_value, best.generation, best.created_at);
	printf("Build: %d\n", max_build);
	printf("Crossover: %d - %d\n", max_crossover, max_crossover_diff);
	printf("Mutations: %d - %d\n", max_mutation, max_mutation_diff);

	return 0;
}



void grasp_build(solution &sol){
	vector<bool> visited_b(size_b);
	sol.seq_a.resize(size_a);
	sol.seq_b.resize(size_b);
	for(int i = 0; i < size_a; i++) sol.seq_a[i] = i;
	shuffle(sol.seq_a.begin(), sol.seq_a.end(), random_generator);
	for(int i = 0; i < min_size; i++) sol.seq_b[i] = grasp_build_operation(sol.seq_a[i], visited_b);
	vector<int> leftovers;
	if(size_b > size_a){
		for(int i = 0; i < size_b; i++)
			if(!visited_b[i])
				leftovers.push_back(i);
		shuffle(leftovers.begin(), leftovers.end(), random_generator);
		for(int i = size_a, j = 0; i < size_b; i++, j++)
			sol.seq_b[i] = leftovers[j];
	}
	sol.created_at = TIME_NOW;
	sol.generation = generation;
	solution_objective_function(sol);
	max_build = max(max_build, sol.best_value);
}



int grasp_build_operation(int idx_a, vector<bool> &visited_b){
	vector<pair<int,int>> possibles;
	for(int idx_b = 0; idx_b < size_b; idx_b++){
		if(visited_b[idx_b])
			continue;
		possibles.push_back(make_pair(FowardEdges[idx_a][idx_b], idx_b));
	}
	sort(possibles.begin(), possibles.end());
	int random_option = random_generator()%min(3, (int) possibles.size());
	int idx_choosed = possibles[random_option].second;
	visited_b[idx_choosed] = true;
	return idx_choosed;
}

bool solution_comp(solution &a, solution &b){
	if(a.best_value > b.best_value)
		return true;
	if(a.best_value < b.best_value)
		return false;
	return (a.best_end - a.best_start) < (b.best_end - b.best_start);
}

void remove_duplicates(vector<int> &seq){
	vector<bool> visited(seq.size());
	vector<int> leftovers;
	for(int i = 0; i < seq.size(); i++){
		if(visited[seq[i]])
			seq[i] = -1;
		else
			visited[seq[i]] = true;
	}
	for(int i = 0; i < seq.size(); i++) if(!visited[i]) leftovers.push_back(i);
	shuffle(leftovers.begin(), leftovers.end(), random_generator);
	for(int i = 0, j = 0; i < seq.size(); i++) if(seq[i] == -1) seq[i] = leftovers[j++];
}

void solution_crossover_operation(solution &father, solution &mother, solution &children, int cut_start, int cut_end){
	children.seq_a.assign(size_a, 0);
	children.seq_b.resize(size_b, 0);
	for(int i = 0; i < min_size; i++){
		if(cut_start <= i and i <= cut_end){
			children.seq_a[i] = father.seq_a[i];
			children.seq_b[i] = father.seq_b[i];
		}else{
			children.seq_a[i] = mother.seq_a[i];
			children.seq_b[i] = mother.seq_b[i];
		}
	}
	remove_duplicates(children.seq_a);
	remove_duplicates(children.seq_b);
	children.created_at = TIME_NOW;
	children.generation = generation;
	solution_objective_function(children);
	max_crossover = max(max_crossover, children.best_value);
	max_crossover_diff = max(children.best_value - max(father.best_value, mother.best_value), max_crossover_diff);
}

void solution_crossover(solution &a, solution &b, solution &children0, solution &children1){
	int cut_start = random_generator() % min_size;
	int cut_end = random_generator() % min_size;
	if(cut_end < cut_start) swap(cut_end, cut_start);
	solution_crossover_operation(a, b, children0, cut_start, cut_end);
	solution_crossover_operation(b, a, children1, cut_start, cut_end);
}

void solution_mutation(solution &sol){
	int idx0, idx1, idx2,  edge = min(size_a,  size_b);
	idx0 = random_generator() % edge;
	idx1 = random_generator() % edge;
	idx2 = random_generator() % edge;
	swap(sol.seq_a[idx0], sol.seq_a[idx1]);
	swap(sol.seq_b[idx1], sol.seq_b[idx2]);
	sol.created_at = TIME_NOW;
	sol.generation = generation;
	int old = sol.best_value;
	solution_objective_function(sol);
	max_mutation_diff = max(sol.best_value - old, max_mutation_diff);
	if(old < sol.best_value)
		max_mutation = max(max_mutation, sol.best_value);
}

void solution_objective_function(solution &sol){
	int curr = 0;
	sol.best_value = 0;
	for(int i = 0; i < min_size; i++){
		if(i)
			curr += -BackwardEdges[sol.seq_a[i]][sol.seq_b[i-1]];
		curr += -FowardEdges[sol.seq_a[i]][sol.seq_b[i]] + 2*AWARD;
		sol.best_value = max(sol.best_value, curr);
	}
}

void  solution_print(solution &sol){
	printf("OF: %d - Path: ", sol.best_value);
	for(int i = 0; i < min_size; i++){
		printf("(%d -> %d [%d])", sol.seq_a[i], sol.seq_b[i], FowardEdges[sol.seq_a[i]][sol.seq_b[i]]);
		if(i < min_size - 1)
			printf(" -> ");
		else
			printf("\n");
	}
	printf("[%d - %d]\n", sol.best_start, sol.best_end);
}

void read_input(void){
	cin >> size_a >> size_b;
	min_size = min(size_a,size_b);
	FowardEdges.assign(size_a, vector<int>(size_b));
	for(auto &row: FowardEdges) for(auto &edge: row) cin >> edge;
	BackwardEdges.assign(size_b, vector<int>(size_a));
	for(auto &row: BackwardEdges) for(auto &edge: row) cin >> edge;
}

double timediff_in_sec(clock_t start, clock_t end){
	return (1.0*end - start)/ CLOCKS_PER_SEC;
}
