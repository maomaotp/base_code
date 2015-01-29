#include <stdio.h>
#include <stdlib.h>

typedef struct _node
{
	int num;
	struct _node *next;
}linknode;

linknode *create_link()
{
	linknode *head;
	head = (linknode *)malloc(sizeof(linknode));
	if (!head) {
		return NULL;
	}
	head->next = NULL;
	return head;
}

void insert_link(linknode *head, int member)
{
	if (!head) {
		return;
	}
	linknode *tmp;
	tmp = (linknode *)malloc(sizeof(linknode));
	if (!tmp) {
		return;
	}
	tmp->next = head->next;
	head->next = tmp;
	tmp->num = member;
}

void print_link(linknode *head)
{
	if (!head) {
		return;
	}
	linknode *tmp = head->next;
	while(tmp){
		printf("%d\n", tmp->num);	
		tmp = tmp->next;				
	}
}

linknode *find_node(const linknode *head, const int number)
{
	if (!head){
		return NULL;
	}
	linknode *tmp_first;
	linknode *tmp;
	tmp_first = head->next;
	tmp = head->next;


	int i=0;
	while(i++ <= number){
		tmp_first = tmp_first->next;
		if (tmp_first == NULL) {
			printf("test:%d\n", i);
			return NULL;
		}
	}
	while(tmp_first->next){
		tmp = tmp->next;	
		tmp_first = tmp_first->next;
	}
	return tmp;
}

int main()
{
	linknode *head;
	head = create_link();

	int i;
	for(i=0; i<30; i++){
		insert_link(head, i);
	}
	print_link(head);
	linknode *tmp = NULL;
	tmp = find_node(head, 8);
	printf("the top:%d\n", tmp->num);

	return 0;
}
