/*
 * stack.c
 *
 * Created: 17.02.2020 07:33:04
 *  Author: mschr
 */ 

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "map.h"

// Utility function to initialize stack
struct Stack* newStack(int capacity)
{
	struct Stack *pt = (struct Stack*)malloc(sizeof(struct Stack));

	pt->maxsize = capacity;
	pt->top = -1;
	pt->items = (position_t*)malloc(sizeof(position_t) * capacity);

	return pt; 
}

// Utility function to return the size of the stack
int size(struct Stack *pt)
{
	return pt->top + 1;
}

// Utility function to check if the stack is empty or not
int isEmpty(struct Stack *pt)
{
	return pt->top == -1;	// or return size(pt) == 0;
}

// Utility function to check if the stack is full or not
int isFull(struct Stack *pt)
{
	return pt->top == pt->maxsize - 1;	// or return size(pt) == pt->maxsize;
}

// Utility function to add an element x in the stack
void push(struct Stack *pt, position_t x)
{
	// check if stack is already full. Then inserting an element would
	// lead to stack overflow
	if (isFull(pt))
	{
		myprintf_P("OverFlow\nProgram Terminated\n");
		exit(EXIT_FAILURE);
	}

	//myprintf_P("Inserting %d\n", x);
	pt->items[++pt->top] = x;
}

// Utility function to return top element in a stack
position_t peek(struct Stack *pt)
{
	// check for empty stack
	if (!isEmpty(pt))
	return pt->items[pt->top];
	else
	exit(EXIT_FAILURE);
}

// Utility function to pop top element from the stack
position_t pop(struct Stack *pt)
{
	// check for stack underflow
	if (isEmpty(pt))
	{
		myprintf_P("UnderFlow\nProgram Terminated\n");
		exit(EXIT_FAILURE);
	}

	//myprintf_P("Removing %d\n", peek(pt));

	// decrease stack size by 1 and (optionally) return the popped element
	return pt->items[pt->top--];
}


