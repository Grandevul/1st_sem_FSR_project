#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED

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

void push_back(int n, deque *a);
void push_front(int n, deque *a);
void pop_back(deque *a);
void pop_front(deque *a);
lst* back(deque *a);
lst* front(deque *a);
int size(deque *a);
int empty(deque *a);
void clear(deque *a);

#endif // DEQUE_H_INCLUDED
