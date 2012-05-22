#include <stdio.h>
#include <stdlib.h>

typedef struct NODE{
    int num;
    struct NODE * next;
}Node;

/////////////// main ///////////////////
int main(int argc, char *argv[])
{ 
    Node *end, *begin, *node; // указатели на начало, конец, и добавляемый элемент
    end = begin = (Node*) malloc( sizeof(Node)); // выделяем память для первого элемента
    begin->next = begin; // замыкаем на себя
    int num, // здесь хранится число
        sum=0; // здесь сумма чисел
    while( scanf("%d", &num ) == 1 ){
        // создаём кольцо...
        end->num = num; // запись числа в последний элемент
        node = (Node*) malloc(sizeof(Node)); // новый узел
        node->next = begin; // указатель нового узла на начало
        end->next = node; // указатель последнего на новый элемент
        end = node; // указатель конца на новый эл.
    }
    puts("\n\n");
    // выод содержимого списка и подсчёт суммы чисел
    for( node = begin; node->next != begin; node = node->next ){
        printf("%d\n", node->num );
        sum += node->num;
    }
    printf("\n\nSumma = %d", sum );

    fflush(stdin);
    getchar(); // пауза
    return 0;
}