#include <search.h>
	#include <stdio.h>
	
	struct node {    /* В дереве будут запоминаться указатели
	                  на эти структуры */
	char *string;
	int  length;
	};
	
	char string_space [10000]; /* Пространство для хранения
	                            цепочек символов */
	struct node nodes [500];   /* Пространство для хранения
	                            структур */
	struct node *root=NULL;    /* Указатель на корень */
	
	main()
	{
	char *strptr = string_space;
	struct node *nodeptr = nodes;
	void print_node (), twalk ();
	int i = 0, node_compare ();
	
	while (gets (strptr) != NULL && i++ < 500) {
	
	  /* Инициализация структуры */
	  nodeptr -> string = strptr;
	  nodeptr -> length = strlen(strptr);
	
	  /* Поместить структуру в дерево */
	  (void) tsearch ((char *) nodeptr, (char **)&root,
	                  node_compare);
	
	  /* Скорректировать указатели */
	  strptr += nodeptr->length + 1;
	    nodeptr++;
	}

	int end=0;
	for(end=0;end<500;end++) {
	 printf("nodes[%d] = 0x%x\n",end,nodes[end]);
	}


	twalk ((char *) root, print_node);
	}
	
	/* Функция сравнивает две структуры в соответствии
	 с алфавитной упорядоченностью цепочек символов */
	int node_compare (node1, node2)
	char *node1, *node2;
	{
	return strcmp (((struct node *) node1)->string,
	               ((struct node *) node2)->string);
	}
	
	/* Функция распечатывает узел при первом заходе в него */
	void print_node (node, order, level)
	char **node;
	VISIT order;
	int level;
	{
	if (order == preorder || order == leaf) {
	  (void) printf ("string = %20s, length = %d\n",
	            (*((struct node **)node)) -> string,
	            (*((struct node **)node)) -> length);
	}
	}