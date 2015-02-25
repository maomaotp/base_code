#include <stdio.h>
#include <stdlib.h>

typedef struct _tree_node
{
	int value;
	struct _tree_node *left;
	struct _tree_node *right;
}treenode;

treenode *create_tree()
{
	treenode *head = NULL;
	head = (treenode *)malloc(sizeof(treenode));
	if (head == NULL) {
		return NULL;
	}
	head->value = 10;
	head->left = NULL;
	head->right = NULL;

	return head;
}

//前序遍历
void *traver_tree(treenode *node)
{
	if (!node){
		printf("%d\n", node->value);
		traver_tree(node->left);
		traver_tree(node->right);
	}
}

//获取二叉树的深度

int depth(treenode *head)
{
	int a,b;
	if (!head) return 0;
	else {
		int a = depth(head->right);
		int b = depth(head->left);
	}
	return (a>b)?(a+1):(b+1);
}

int main()
{
	treenode *head;
	head = create_tree();
	int buf[10] = {0,1,2,3,4,5,6,7,8,9};

	return 0;
}
