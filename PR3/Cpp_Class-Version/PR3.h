#ifndef PR3_H
#define PR3_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

using namespace std;

// Function pre-declaration
size_t split( const string& input, vector<string>& output, char delimiter );
bool comparison(const struct sample& A, const struct sample& B, const int& selected_attr);
vector<pair<string, int>> class_counts(struct node* node);
float gini_impurity(struct node* node);
float information_gain(struct node* parent, struct node* true_child, struct node* false_child);
string classify_function(struct sample sample, struct node* node);
bool is_leaf_node(struct node* node);
void holdout_method(struct node* total, struct node* train_root, struct node* valid_root);
float validation(struct node* train_root, vector<struct sample> valid_data);
void build_tree(struct node* node, int attr_num);
void print_tree(struct node* node, string space);
pair<float, float> find_best_partition(struct node* node, int selected_attr);
pair<struct node*, struct node*> partition(struct node* node, string attr, float threshold);
pair<vector<pair<string, int>>, vector<pair<string, int>>> pseudo_partition(
	struct node* parent, 
	int selected_attr, 
	float threshold);
struct node* read_iris(fstream& input_file);
struct node* read_cross200(fstream& input_file);
struct node* read_optical(fstream& input_file);
struct node* read_wine(fstream& input_file);
struct node* read_file(fstream& input_file, int attr_num);

// Some data structure
struct sample
{
	vector<pair<string, float>>value;
	string class_attr;
};

struct node
{
	vector<struct sample>data;
	struct node* parent;
	struct node* true_branch;
	struct node* false_branch;
	float impurity;
	string part_attr;
	float part_thres;
	bool is_leaf;
};

class node
{
private:
	vector<struct sample>data;
	struct node* parent;
	struct node* true_branch;
	struct node* false_branch;
	float impurity;
	string part_attr;
	float part_thres;
	bool is_leaf;
public:
	size_t split( const string& input, vector<string>& output, char delimiter );
	bool comparison(const struct sample& A, const struct sample& B, const int& selected_attr);
	vector<pair<string, int>> class_counts(struct node* node);
	float gini_impurity(struct node* node);
	float information_gain(struct node* parent, struct node* true_child, struct node* false_child);
	string classify_function(struct sample sample, struct node* node);
	bool is_leaf_node(struct node* node);
	void holdout_method(struct node* total, struct node* train_root, struct node* valid_root);
	float validation(struct node* train_root, vector<struct sample> valid_data);
	void build_tree(struct node* node, int attr_num);
	void print_tree(struct node* node, string space);
	pair<float, float> find_best_partition(struct node* node, int selected_attr);
	pair<struct node*, struct node*> partition(struct node* node, string attr, float threshold);
	pair<vector<pair<string, int>>, vector<pair<string, int>>> pseudo_partition(
		struct node* parent, 
		int selected_attr, 
		float threshold);
	struct node* read_iris(fstream& input_file);
	struct node* read_cross200(fstream& input_file);
	struct node* read_optical(fstream& input_file);
	struct node* read_wine(fstream& input_file);
	struct node* read_file(fstream& input_file, int attr_num);
};

class sorter
{
private:
	int criteria; // The alias of the position of the attribute in 'sample.value'.
public:
	sorter( const int& selected_attr )
	{
		criteria = selected_attr;
	}
	bool operator()( const struct sample& A, const struct sample& B )const
	{
		return comparison(A, B, criteria);
	}
};

#endif // PR3_H