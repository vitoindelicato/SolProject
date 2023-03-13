
typedef struct node{
    char *filename;
    long int result;
    struct node *next;
} _node;


_node *create_node(long int result, char *filename);
void insert_node (_node **head, _node *new_node);
void print_list(_node *head);
void free_list(_node **head);
