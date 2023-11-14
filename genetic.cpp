#include <bits/stdc++.h>
#define APPLY_OF of_from_start_idx
#define APPLY_BUILD build_grasp
#define APPLY_CROSSOVER crossover_random_choice
#define APPLY_MUTATION mutation_rotate
#define GRASP_ALPHA 3
#define AWARD 250
#define MUTATION_RATE 100
#define NEW_POPULATION_RATE 0.90
#define TIMELIMIT 10
#define CURR_TIME timediff_in_sec(start, clock())

using namespace std;

clock_t start;
int size_a;
int size_b;
int POPULATION_SIZE = 100;
int min_size;
int generation;
int max_build = INT_MIN;
int max_mutation = INT_MIN;
int max_crossover = INT_MIN;
int max_crossover_diff = INT_MIN;
int time_next_step = 1;

auto random_generator = default_random_engine(clock());

typedef struct Solution
{
	vector<int> seq_a, seq_b;
	int best_value, best_start, best_end, generation;
	int fw_punition, bk_punition;
	double created_at;
} solution;

vector<vector<int>> BackwardEdges; 
vector<vector<int>> FowardEdges;
vector<solution> population;

bool solution_comp(solution &a, solution &b);
void build_random(solution &sol);
void build_grasp(solution &sol);
void build_grasp_operation(int idx_a, int &idx_b, vector<bool> &visited_b);
void crossover_slice(solution &a, solution &b, solution &child0, solution &child1);
void crossover_slice_operation(vector<int> &father, vector<int> &mother, vector<int> &child, int cut_start, int cut_end);
void crossover_random_choice(solution &a, solution &b, solution &child0, solution &child1);
void mutation_rotate(solution &sol);
void mutation_swap(solution &sol);
void of_from_any_idx(solution &sol);
void of_from_start_idx(solution &sol);
void print_genetic_results(solution &best);
void print_solution_details(solution &sol);
void print_step_results(solution &best);
void read_input();
void remove_duplicates_and_fill(vector<int> &seq);
double timediff_in_sec(clock_t start, clock_t end);

int main()
{
	read_input();
	POPULATION_SIZE = 10*(size_a + size_b);
	population.resize(10*(size_a+size_b));
	solution best, child0, child1;
	APPLY_BUILD(best);
	for (auto &sol : population) APPLY_BUILD(sol);
	start = clock();
	
	while (CURR_TIME - best.created_at < TIMELIMIT)
	{
	
		print_step_results(best);
			
		for (int i = 0; i < POPULATION_SIZE; i++)
			if(random_generator()%100  < MUTATION_RATE)
				APPLY_MUTATION(population[i]);
		
		
		shuffle(population.begin(), population.end(), random_generator);
		for (int i = 0; i < POPULATION_SIZE; i += 2)
		{
			crossover_slice(population[i], population[i+1], child0, child1);
			population.push_back(child0);
			population.push_back(child1);
		}
		for (int i = 0; i < 2; i++)
		{
			solution newsol;
			build_random(newsol);
			crossover_slice(best, newsol, child0, child1);
			population.push_back(child0);
			population.push_back(child1);
		}
		sort(population.begin(), population.end(), solution_comp);
		while (population.size() > POPULATION_SIZE * NEW_POPULATION_RATE) population.pop_back();
		while (population.size() < POPULATION_SIZE)
		{
			solution sol;
			APPLY_BUILD(sol);
			population.push_back(sol);
		}
		if (solution_comp(population.front(), best))
			best = population.front();
		generation++;
	}
	print_genetic_results(best);
	print_solution_details(best);
	return 0;
}

void mutation_swap(solution &sol)
{
				
	int idx0, idx1, idx2, edge = min(size_a, size_b);
	idx0 = random_generator() % edge;
	idx1 = random_generator() % edge;
	swap(sol.seq_a[idx0], sol.seq_a[idx1]);
	swap(sol.seq_b[idx0], sol.seq_b[idx1]);
	sol.created_at = CURR_TIME;
	sol.generation = generation;
	APPLY_OF(sol);
	max_mutation = max(max_mutation, sol.best_value);
}

void mutation_rotate(solution &sol)
{

	int idx[3];
	for(auto &it: idx) it = random_generator() % min_size;
	swap(sol.seq_a[idx[0]], sol.seq_a[idx[2]]);
	swap(sol.seq_b[idx[0]], sol.seq_b[idx[2]]);
	swap(sol.seq_b[idx[0]], sol.seq_b[idx[1]]);
	swap(sol.seq_b[idx[0]], sol.seq_b[idx[1]]);
	sol.created_at = CURR_TIME;
	sol.generation = generation;
	APPLY_OF(sol);
	max_mutation = max(max_mutation, sol.best_value);
}

void build_random(solution &sol)
{
	sol.seq_a.resize(size_a);
	sol.seq_b.resize(size_b);
	for (int i = 0; i < size_a; i++)
		sol.seq_a[i] = i;
	shuffle(sol.seq_a.begin(), sol.seq_a.end(), random_generator);
	for (int i = 0; i < size_b; i++)
		sol.seq_b[i] = i;
	shuffle(sol.seq_b.begin(), sol.seq_b.end(), random_generator);
	sol.created_at = CURR_TIME;
	sol.generation = generation;
	APPLY_OF(sol);
	max_build = max(max_build, sol.best_value);
}

void build_grasp(solution &sol)
{
	sol.seq_a.resize(size_a);
	sol.seq_b.resize(size_b);
	vector<bool> visited_b(size_b);
	vector<int> leftovers;
	for (int i = 0; i < size_a; i++)
		sol.seq_a[i] = i;
	shuffle(sol.seq_a.begin(), sol.seq_a.end(), random_generator);
	for (int i = 0; i < min_size; i++)
		build_grasp_operation(sol.seq_a[i], sol.seq_b[i], visited_b);

	if (size_b > size_a)
	{
		for (int i = 0; i < size_b; i++)
			if (!visited_b[i])
				leftovers.push_back(i);
		shuffle(leftovers.begin(), leftovers.end(), random_generator);
		for (int i = size_a, j = 0; i < size_b; i++, j++)
			sol.seq_b[i] = leftovers[j];
	}
	sol.created_at = CURR_TIME;
	sol.generation = generation;
	APPLY_OF(sol);
	max_build = max(max_build, sol.best_value);
}

void build_grasp_operation(int idx_a, int &idx_b, vector<bool> &visited_b)
{
	int choosed;
	vector<pair<int, int>> possibles;
	for (int idx_b = 0; idx_b < size_b; idx_b++)
	{
		if (visited_b[idx_b])
			continue;
		possibles.push_back(make_pair(FowardEdges[idx_a][idx_b], idx_b));
	}
	sort(possibles.begin(), possibles.end());
	choosed = random_generator() % min(GRASP_ALPHA, (int)possibles.size());
	idx_b = possibles[choosed].second;
	visited_b[idx_b] = true;
}

void crossover_slice_operation(solution &father, solution &mother, solution &child, int cut_start, int cut_end)
{
	child.seq_a.assign(size_a, 0);
	child.seq_b.assign(size_b, 0);
	for (int i = 0; i < min_size; i++)
	{
		if (cut_start <= i and i <= cut_end)
		{
			child.seq_a[i] = father.seq_a[i];
			child.seq_b[i] = father.seq_b[i];
		}
		else
		{
			child.seq_a[i] = mother.seq_a[i];
			child.seq_b[i] = mother.seq_b[i];
		}
	}
	remove_duplicates_and_fill(child.seq_a);
	remove_duplicates_and_fill(child.seq_b);
	child.created_at = CURR_TIME;
	child.generation = generation;
	APPLY_OF(child);
	max_crossover = max(max_crossover, child.best_value);
	max_crossover_diff = max(child.best_value - max(father.best_value, mother.best_value), max_crossover_diff);
}

void crossover_slice(solution &a, solution &b, solution &child0, solution &child1)
{
	int cut_start = random_generator() % min_size;
	int cut_end = random_generator() % min_size;

	if (cut_end < cut_start)
		swap(cut_end, cut_start);
	crossover_slice_operation(a, b, child0, cut_start, cut_end);
	crossover_slice_operation(b, a, child1, cut_start, cut_end);
}

void crossover_random_choice(solution &a, solution &b, solution &child0, solution &child1){
	child0.seq_a.resize(size_a);
	child0.seq_b.resize(size_b);
	child1.seq_a.resize(size_a);
	child1.seq_b.resize(size_b);
	for(int i = 0; i < min_size; i++){
		if(random_generator() % 2){
			child0.seq_a[i] = a.seq_a[i];
			child0.seq_b[i] = a.seq_b[i];
			child1.seq_a[i] = b.seq_a[i];
			child1.seq_b[i] = b.seq_b[i];
		}else{
			child0.seq_a[i] = b.seq_a[i];
			child0.seq_b[i] = b.seq_b[i];
			child1.seq_a[i] = a.seq_a[i];
			child1.seq_b[i] = a.seq_b[i];
		}
	}
	remove_duplicates_and_fill(child0.seq_a);
	remove_duplicates_and_fill(child0.seq_b);
	remove_duplicates_and_fill(child1.seq_a);
	remove_duplicates_and_fill(child1.seq_b);
}

bool solution_comp(solution &a, solution &b)
{
	if (a.best_value > b.best_value)
		return true;
	if (a.best_value < b.best_value)
		return false;
	return (a.best_end - a.best_start) < (b.best_end - b.best_start);
}

void of_from_any_idx(solution &sol)
{
	sol.best_value = 0;
	sol.best_start = 0;
	sol.best_end = 0;
	int curr_value = 0;
	int curr_start = 0;
	for (int i = 0; i < min_size; i++)
	{
		if (i)
		{
			curr_value -= BackwardEdges[sol.seq_a[i]][sol.seq_b[i - 1]];
			if (curr_value <= 0)
			{
				curr_value = 0;
				curr_start = i;
			}
		}
		curr_value += 2 * AWARD - FowardEdges[sol.seq_a[i]][sol.seq_b[i]];
		if (curr_value >= sol.best_value)
		{
			if (curr_value == sol.best_value and sol.best_end - sol.best_start >= i + 1 - curr_start)
				continue;
			sol.best_value = curr_value;
			sol.best_start = curr_start;
			sol.best_end = i + 1;
		}
	}
	sol.fw_punition = 0;
	sol.bk_punition = 0;
	for (int i = sol.best_start; i < sol.best_end; i++)
	{
		if (i != sol.best_start)
			sol.bk_punition += BackwardEdges[sol.seq_a[i]][sol.seq_b[i - 1]];
		sol.fw_punition += FowardEdges[sol.seq_a[i]][sol.seq_b[i]];
	}
}

void of_from_start_idx(solution &sol)
{
	int curr = 0;
	sol.best_start = 0;
	sol.best_value = 0;
	for (int i = 0; i < min_size; i++)
	{
		if (i)
			curr += -BackwardEdges[sol.seq_a[i]][sol.seq_b[i - 1]];
		curr += -FowardEdges[sol.seq_a[i]][sol.seq_b[i]] + 2 * AWARD;
		if (sol.best_value < curr)
		{
			sol.best_value = curr;
			sol.best_end = i + 1;
		}
	}
	sol.fw_punition = 0;
	sol.bk_punition = 0;
	for (int i = sol.best_start; i < sol.best_end; i++)
	{
		if (i != sol.best_start)
			sol.bk_punition += BackwardEdges[sol.seq_a[i]][sol.seq_b[i - 1]];
		sol.fw_punition += FowardEdges[sol.seq_a[i]][sol.seq_b[i]];
	}
}

void print_genetic_results(solution &best){
	printf("generations: %d, total_time: %.3lf\n", generation, CURR_TIME);
	printf("best: %d, generations: %d, created_at: %.3lf\n", best.best_value, best.generation, best.created_at);
	printf("Build: %d\n", max_build);
	printf("Crossover: %d\n", max_crossover);
	printf("Mutations: %d\n", max_mutation);
}

void print_solution_details(solution &sol)
{
	printf("OF: %d - Path: ", sol.best_value);
	for (int i = 0; i < min_size; i++)
	{
		printf("(%d -> %d [%d])", sol.seq_a[i], sol.seq_b[i], FowardEdges[sol.seq_a[i]][sol.seq_b[i]]);
		if (i < min_size - 1)
			printf(" -> ");
		else
			printf("\n");
	}
	printf("[%d - %d]\n", sol.best_start, sol.best_end);
	printf("FWP: %d - BKP %d\n", sol.fw_punition, sol.bk_punition);
}

void print_step_results(solution &best){
	if (int(CURR_TIME) == time_next_step)
	{
		printf("Melhor: %d %d %d, ", best.best_value, best.fw_punition, best.bk_punition);
		printf("Primeiro: %d, ", population.front().best_value);
		printf("Ultimo: %d\n", population.back().best_value);
		time_next_step++;
	}
}

void read_input()
{
	cin >> size_a >> size_b;
	min_size = min(size_a, size_b);
	FowardEdges.assign(size_a, vector<int>(size_b));
	for (auto &row : FowardEdges)
		for (auto &edge : row)
			cin >> edge;
	BackwardEdges.assign(size_b, vector<int>(size_a));
	for (auto &row : BackwardEdges)
		for (auto &edge : row)
			cin >> edge;
}

void remove_duplicates_and_fill(vector<int> &seq)
{
	vector<bool> visited(seq.size());
	vector<int> leftovers;
	for (int i = 0; i < seq.size(); i++)
	{
		if (visited[seq[i]])
			seq[i] = -1;
		else
			visited[seq[i]] = true;
	}
	for (int i = 0; i < seq.size(); i++)
		if (!visited[i])
			leftovers.push_back(i);
	shuffle(leftovers.begin(), leftovers.end(), random_generator);
	for (int i = 0, j = 0; i < seq.size(); i++)
		if (seq[i] == -1)
			seq[i] = leftovers[j++];
}

void fill_randomly(vector<int> &seq, int len){
	seq.resize(len);
	for(int i = 0; i < len; i++) seq[i] = i;
	shuffle(seq.begin(), seq.end(), random_generator);
}

double timediff_in_sec(clock_t start, clock_t end)
{
	return (1.0 * end - start) / CLOCKS_PER_SEC;
}
