#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <string>
#include <deque>
#include <map>
#include <vector>
#include <utility>
#include <iomanip>

using namespace std;

// tree node schema
struct node
{
    // for each node n
    pair<int, int> coord; // n.state
    // int := {x coordinate}, int := {y coordinate}
    
    pair<char, int> action; // n.action, the action applied to its parent
    // char := {x, y, s(stop)}, int := {# of moving step}
    
    struct node* parent; // n.parent
    //struct node* child; // n.child
    
    int next_action; // next action's index in the vector<int> move
    int cost; // n.path_cost
};

// function pre-declarations
size_t split( const string &input, vector<string> &output, char delimiter );
struct node* newNode( pair<int, int> coord, pair<char, int> action );
void printSolution( struct node* root_node );
int pathCost( int next_action, vector<int> move );
int heuriFunc( pair<int, int> current, pair<int, int> goal );
void BFS( pair<int, int> goal, vector<int> move );
void IDS( pair<int, int> goal, vector<int> move );
void A_Star( pair<int, int> goal, vector<int> move );

int main()
{
    FILE *input_file;
    char getLine_char[151] = {0};
    string getLine_str;
    vector<string> split_str;
    
    pair<int, int> goal_point;
    vector<int> move;
    int error_method;

    input_file = fopen("IntroAI_PR1_test.txt", "r");
    
    while( fgets(getLine_char, 150, input_file) != NULL )
    {
        // clean up variables
        goal_point.first = 0;
        goal_point.second = 0;
        error_method = 0;
        move.clear();

        // split row string
        getLine_str.assign(getLine_char);
        split(getLine_str, split_str, ' ');
        
        goal_point.first = stoi( split_str[4] );
        goal_point.second = stoi( split_str[5] );
        for(int i=9; i<split_str.size(); i++)
            move.push_back( stoi( split_str[i] ) );

        if(split_str[0] == "BFS")
        {
            cout<<getLine_str<<endl;
            cout<<"--------------------"<<endl;
            BFS( goal_point, move );
        }
        else if(split_str[0] == "IDS")
        {
            cout<<getLine_str<<endl;
            cout<<"--------------------"<<endl;
            IDS( goal_point, move );
        }
        else if(split_str[0] == "A*")
        {
            cout<<getLine_str<<endl;
            cout<<"--------------------"<<endl;
            A_Star( goal_point, move );
        }
        else error_method = -1;

        if( error_method == -1 )
        {
            cout<<"Unknown strategie!"<<endl;
            continue;
        }

    }
    
    return 0;
}

size_t split( const string &input, vector<string> &output, char delimiter )
{
    size_t pos = input.find( delimiter );
    size_t initialPos = 0;
    output.clear();

    // Decompose statement
    while( pos != string::npos )
    {
        output.push_back( input.substr( initialPos, (pos-initialPos) ) );
        initialPos = pos + 1;

        pos = input.find( delimiter, initialPos );
    }

    // Add the last one
    output.push_back( input.substr( initialPos, (min(pos, input.size())-initialPos+1) ) );

    return output.size();
}


struct node* newNode( struct node* parent, pair<int, int> coord, pair<char, int>action, int next_action )
{
    struct node* new_node = (struct node*) malloc( sizeof( struct node ) );

    // initialize all parameters
    new_node->coord.first = coord.first;
    new_node->coord.second = coord.second;
    new_node->action.first = action.first;
    new_node->action.second = action.second;
    new_node->next_action = next_action;
    new_node->parent = parent;
    //new_node->child = new_node;
    new_node->cost = 0;

    //if(parent != NULL)parent->child = new_node;

    return new_node;
}

void printSolution( struct node* goal_node, struct node* root_node)
{
    struct node* itr = goal_node;
    while(true)
    {
        cout<<"Through:("<<itr->action.first<<", "<<itr->action.second<<")";
        cout<<", To:("<<itr->coord.first<<", "<<itr->coord.second<<")"<<endl;
        itr = itr->parent;
        if(itr == root_node)break;
    }

    cout<<"Through:("<<itr->action.first<<", "<<itr->action.second<<")";
    cout<<", To:("<<itr->coord.first<<", "<<itr->coord.second<<")"<<endl;
    cout<<"===================="<<endl;
    cout<<endl;
    
    if(root_node->parent != root_node)cout<<"Error!";
}

// print node with path cost
void printSolWithEvaFunc( struct node* goal_node, struct node* root_node, vector<int> move )
{
    struct node* itr = goal_node;
    while(true)
    {
        cout<<"Through:("<<itr->action.first<<", "<<itr->action.second<<")";
        cout<<", To:("<<itr->coord.first<<", "<<itr->coord.second<<")";
        cout<<", Evaluation:";
        cout<<heuriFunc(itr->coord, goal_node->coord)<<" + ";
        cout<<pathCost(itr->next_action, move)<<endl;
        itr = itr->parent;
        if(itr == root_node)break;
    }

    cout<<"Through:("<<itr->action.first<<", "<<itr->action.second<<")";
    cout<<", To:("<<itr->coord.first<<", "<<itr->coord.second<<")";
    cout<<", Evaluation:";
    cout<<heuriFunc(itr->coord, goal_node->coord)<<" + ";
    cout<<pathCost(itr->next_action, move)<<endl;
    cout<<endl;
    
    if(root_node->parent != root_node)cout<<"Error!";
}

int heuriFunc( pair<int, int> current, pair<int, int> goal )
{
    int first_comp = 0;
    int second_comp = 0;

    first_comp = (int) floor(abs(current.first - goal.first)/9);
    second_comp = (int) floor(abs(current.second - goal.second)/9);

    return (first_comp + second_comp);
}

int pathCost( int next_action, vector<int> move )
{
    int result = 0;
    for(int i=0; i<next_action; i++)
    {
        result = result + move[i];
    }
    return result;
}

void BFS( pair<int, int> goal, vector<int> move )
{
    deque<struct node*>frontier;
    struct node* root_node = newNode( NULL, make_pair(0, 0), make_pair('s', 0), 0 );
    struct node* explored_node;

    root_node->parent = root_node;
    frontier.push_back( root_node );

    struct node* temp_node = NULL;
    while(frontier.empty() != true)
    {
        explored_node = frontier.front();
        frontier.pop_front();

        int change = move[explored_node->next_action];

        if( explored_node->coord == goal )break;
        // find the solution and stop BFS tree search

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first+change, explored_node->coord.second), 
            make_pair('x', +change), 
            explored_node->next_action+1
        ); // x+
        frontier.push_back(temp_node);

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first-change, explored_node->coord.second), 
            make_pair('x', -change), 
            explored_node->next_action+1
        ); // x-
        frontier.push_back(temp_node);

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first, explored_node->coord.second+change), 
            make_pair('y', +change), 
            explored_node->next_action+1
        ); // y+
        frontier.push_back(temp_node);

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first, explored_node->coord.second-change), 
            make_pair('y', -change), 
            explored_node->next_action+1
        ); // y-
        frontier.push_back(temp_node);

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first, explored_node->coord.second), 
            make_pair('s', 0), 
            explored_node->next_action+1
        ); // stop
        frontier.push_back(temp_node);
    }

    printSolution(explored_node, root_node);
}

void IDS( pair<int, int> goal, vector<int> move )
{
    for(int i=0; i<=move.size(); i++)
    {
        deque<struct node*>frontier;
        struct node* root_node = newNode( NULL, make_pair(0, 0), make_pair('s', 0), 0 );
        struct node* explored_node = NULL;
        int solution_flag = 0;
        int limit = i;

        root_node->parent = root_node;
        frontier.push_front(root_node);

        while( frontier.empty() != true )
        {
            explored_node = frontier.front();
            frontier.pop_front();

            if(explored_node->coord == goal)
            {
                solution_flag = 1;
                break;
            }
            else if(explored_node->next_action == limit)continue;// cutoff
            else
            {
                int change = move[explored_node->next_action];
                struct node* temp_node = NULL;

                temp_node = newNode(
                    explored_node, 
                    make_pair(explored_node->coord.first+change, explored_node->coord.second), 
                    make_pair('x', +change), 
                    explored_node->next_action+1
                ); // x+
                frontier.push_front(temp_node);

                temp_node = newNode(
                    explored_node, 
                    make_pair(explored_node->coord.first-change, explored_node->coord.second), 
                    make_pair('x', -change), 
                    explored_node->next_action+1
                ); // x-
                frontier.push_front(temp_node);

                temp_node = newNode(
                    explored_node, 
                    make_pair(explored_node->coord.first, explored_node->coord.second+change), 
                    make_pair('y', +change), 
                    explored_node->next_action+1
                ); // y+
                frontier.push_front(temp_node);

                temp_node = newNode(
                    explored_node, 
                    make_pair(explored_node->coord.first, explored_node->coord.second-change), 
                    make_pair('y', -change), 
                    explored_node->next_action+1
                ); // y-
                frontier.push_front(temp_node);

                temp_node = newNode(
                    explored_node, 
                    make_pair(explored_node->coord.first, explored_node->coord.second), 
                    make_pair('s', 0), 
                    explored_node->next_action+1
                ); // stop
                frontier.push_front(temp_node);
            }
        }
        if( solution_flag == 1 )
        {
            printSolution(explored_node, root_node);
            break;
        }
    }
}

void A_Star( pair<int, int> goal, vector<int> move )
{
    multimap<int, struct node*>frontier;
    struct node* root_node = newNode( NULL, make_pair(0, 0), make_pair('s', 0), 0 );
    struct node* explored_node;
    multimap<int, struct node*>::iterator itr;

    root_node->parent = root_node;
    frontier.insert( make_pair( ( heuriFunc(root_node->coord, goal) + pathCost(root_node->next_action, move) ), root_node ) );

    struct node* temp_node = NULL;
    while( frontier.empty() != true )
    {
        itr = frontier.begin();
        explored_node = itr->second;
        frontier.erase(itr);

        int change = move[explored_node->next_action];

        if( explored_node->coord == goal )break;
        // find the solution and stop BFS tree search

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first+change, explored_node->coord.second), 
            make_pair('x', +change), 
            explored_node->next_action+1
        ); // x+
        frontier.insert( make_pair( ( heuriFunc(temp_node->coord, goal) + pathCost(temp_node->next_action, move) ), temp_node ) );

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first-change, explored_node->coord.second), 
            make_pair('x', -change), 
            explored_node->next_action+1
        ); // x-
        frontier.insert( make_pair( ( heuriFunc(temp_node->coord, goal) + pathCost(temp_node->next_action, move) ), temp_node ) );

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first, explored_node->coord.second+change), 
            make_pair('y', +change), 
            explored_node->next_action+1
        ); // y+
        frontier.insert( make_pair( ( heuriFunc(temp_node->coord, goal) + pathCost(temp_node->next_action, move) ), temp_node ) );

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first, explored_node->coord.second-change), 
            make_pair('y', -change), 
            explored_node->next_action+1
        ); // y-
        frontier.insert( make_pair( ( heuriFunc(temp_node->coord, goal) + pathCost(temp_node->next_action, move) ), temp_node ) );

        temp_node = newNode(
            explored_node, 
            make_pair(explored_node->coord.first, explored_node->coord.second), 
            make_pair('s', 0), 
            explored_node->next_action+1
        ); // stop
        frontier.insert( make_pair( ( heuriFunc(temp_node->coord, goal) + pathCost(temp_node->next_action, move) ), temp_node ) );
    }

    if( frontier.empty() != true)printSolWithEvaFunc(explored_node, root_node, move);
}