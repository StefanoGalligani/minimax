#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<random>

#define MIN_BRANCHES 2
#define MAX_BRANCHES 6
#define MIN_DEPTH 4
#define MAX_DEPTH 8
#define MIN_VALUE -10
#define MAX_VALUE 10
#define BRANCH_PROB 70

#define FIRST_PLAYER 0 // 0 : HUMAN ; 1 : AI
#define AI_OBJECTIVE 0 // 0 : MIN ; 1 : MAX

class tree {
public:
	int expected_value;

	virtual void print(int depth) {
		for (int i = 0; i < depth; i++) {
			printf("  ");
		};
	}

	void print() {
		print(0);
	}

	virtual bool is_leaf() {
		return false;
	}
};

class node : public tree {
public:
	std::vector<tree*> children;

	void add_child(tree* child) {
		children.push_back(child);
	}

	void print(int depth) override {
		tree::print(depth);
		printf("--\n");
		for (int i = 0; i < children.size(); i++) {
			children[i]->print(depth + 1);
		}
	}
};

class leaf : public tree {
public:
	int value;

	leaf(int v) {
		value = v;
	}

	void print(int depth) override{
		tree::print(depth);
		printf("%d\n", value);
	}

	bool is_leaf() override {
		return true;
	}
};

int random_between(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

tree* random_tree(int depth) {
	if (depth == MAX_DEPTH || (depth >= MIN_DEPTH && (rand() % 100) >= BRANCH_PROB)) {
		return new leaf(random_between(MIN_VALUE, MAX_VALUE));
	}
	node* t = new node();
	int n_branches = random_between(MIN_BRANCHES, MAX_BRANCHES);
	for (int i = 0; i < n_branches; i++) {
		t->add_child(random_tree(depth + 1));
	}
	return t;
}

tree* random_tree() { return random_tree(0); }

int minimax(tree* t, int simulated_turn, int best_in_above_level) {
	if (t->is_leaf()) {
		t->expected_value = dynamic_cast<leaf*>(t)->value;
		return -1;
	}
	node* n = dynamic_cast<node*>(t);
	bool looking_for_max = (simulated_turn == AI_OBJECTIVE);
	int best_value = looking_for_max ? MIN_VALUE - 1 : MAX_VALUE + 1;
	int best_branch = -1;
	for (int i = 0; i < n->children.size(); i++) {
		minimax(n->children[i], 1 - simulated_turn, best_value);
		if (looking_for_max && n->children[i]->expected_value > best_value ||
			!looking_for_max && n->children[i]->expected_value < best_value) {
			best_value = n->children[i]->expected_value;
			best_branch = i;
			if (looking_for_max && best_value >= best_in_above_level ||
				!looking_for_max && best_value <= best_in_above_level) {
				break;
			}
		}
	}
	t->expected_value = best_value;
	return best_branch;
}

int acquire_selection(node* n, int turn) {
	if (turn == 0) {
		int selection = -1;
		printf("\n");
		while (selection < 1 || selection > n->children.size()) {
			if (AI_OBJECTIVE == 0) {
				printf("HUMAN pick a branch between %d and %d (get the highest value) : ", 1, (int)n->children.size());
			}
			else {
				printf("HUMAN pick a branch between %d and %d (get the lowest value) : ", 1, (int)n->children.size());
			}
			scanf_s("%d", &selection);
		}
		return selection - 1;
	}

	bool looking_for_max = (1 == AI_OBJECTIVE);
	int ai_selection = minimax(n, turn, looking_for_max ? MAX_VALUE + 1 : MIN_VALUE - 1);
	if (AI_OBJECTIVE == 0) {
		printf("\nAI pick a branch between %d and %d (get to the lowest value) : %d\n", 1, (int)n->children.size(), (ai_selection + 1));
	}
	else {
		printf("\nAI pick a branch between %d and %d (get to the highest value) : %d\n", 1, (int)n->children.size(), (ai_selection + 1));
	}
	printf("Expected value is %d\n", n->expected_value);
	return ai_selection;
}

int main() {
	srand(0);
	tree* t = random_tree();
	int turn = FIRST_PLAYER;
	while (!t->is_leaf()) {
		t->print();
		node* n = dynamic_cast<node*>(t);
		int next_branch = acquire_selection(n, turn);
		t = n->children[next_branch];
		turn = 1 - turn;
	}
	printf("\nValue reached: %d\n", dynamic_cast<leaf*>(t)->value);
	return 0;
}