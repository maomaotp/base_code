#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node
{
    char buf[20];
    int a;
    struct _node *next;
}linknode, *linklist;

linklist CreateLinklist()
{
    linklist h;
    int a;
    h = (linklist)malloc(sizeof(linknode));
    printf("ADDRH = %p, sizeof(linknode)==%d\n", h, sizeof(linknode));
    h->next = NULL;
}

void insert_linklist(linklist h, const char* name, int num) 
{
    linklist node;

    node = (linklist)malloc(sizeof(linknode));
    strcpy(node->buf, name);
    node->a = num;
    node->next = h->next;
    h->next = node;
}

//获取链表倒数第N个值
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

//反转链表
void rever_linklist(linklist h)
{
    linklist tmp, cur;
    int len = 0;
    
    cur = h->next;
    h->next = NULL;
    while(cur) {
        tmp = cur;
        cur = cur->next;
        tmp->next = h->next;
        h->next = tmp;
    }
        
}

int main()
{
    linklist h;
    int i;
    char buf[20] = "hello linkllist";

    h = CreateLinklist();

    for(i=0; i<10; i++) {
        insert_linklist(h, buf, i);
    }
	linklist tmp = h;

    rever_linklist(h);

	printf("rever:");
    while(h->next) {
        printf("node->i=%d ", h->next->a);
        h=h->next;
    }
	printf("\n");

    return 0;
}
