#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct lst
{
    int x;
    struct lst* next;
    struct lst* prev;

}lst;

typedef struct deque
{
    int length;
    lst* first;
    lst* last;
}deque;

void push_back(int n, deque *a)
{
    lst *el = malloc(sizeof(lst));
    el->x = n;
    el->next = NULL;
    el->prev = NULL;

    if (a->length == 0)
    {
        a->last = el;
        a->first = el;
    }
    else
    {
        el->prev = a->last;
        a->last->next = el;
        a->last = el;
    }

    a->length++;
    //printf("ok\n");
}

void push_front(int n, deque *a)
{
    lst *el = malloc(sizeof(lst));
    el->x = n;
    el->next = NULL;
    el->prev = NULL;

    if (a->length == 0)
    {
        a->last = el;
        a->first = el;
    }
    else
    {
        el->next = a->first;
        a->first->prev = el;
        a->first = el;
    }

    a->length++;
    //printf("ok\n");
}

void pop_back(deque *a)
{
    if (a->length <= 0)
    {
        return;
    }
    if (a->length == 1)
    {
        free(a->last);
        a->first = NULL;
        a->last = NULL;
    }
    else
    {
        lst *p = a->last->prev;
        free(a->last);
        p->next = NULL;
        a->last = p;
    }
    a->length--;
}

void pop_front(deque *a)
{
    if (a->length <= 0)
    {
        return;
    }
    if (a->length == 1)
    {
        free(a->first);
        a->first = NULL;
        a->last = NULL;
    }
    else
    {
        lst *p = a->first->next;
        free(a->first);
        p->prev = NULL;
        a->first = p;
    }
    a->length--;
}

lst* back(deque *a)
{
    if (a->length == 0)
    {
        //printf("error\n");
        return NULL;
    }
    //printf("%d\n", a->last->x);
    return a->last;
}

lst* front(deque *a)
{
    if (a->length == 0)
    {
        //printf("error\n");
        return NULL;
    }
    //printf("%d\n", a->first->x);
    return a->first;
}

int size(deque *a)
{
    int n = a->length;
    //printf("%d\n", n);
    return n;
}

int empty(deque *a)
{
    int n = a->length;
    //printf("%d\n", n);
    if (n > 0) return 0;
    return 1;
}

void clear(deque *a)
{
    while (a->length > 0)
    {
        pop_back(a);
    }
    //printf("ok\n");
}
