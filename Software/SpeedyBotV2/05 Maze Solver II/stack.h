/*
 * stack.h
 *
 * Created: 17.02.2020 09:02:43
 *  Author: schreiberm
 */ 
/****************************************************************************
// main function
int main()
{
	// create a stack of capacity 5
	struct stack *pt = newStack(5);

	push(pt, 1);
	push(pt, 2);
	push(pt, 3);

	printf("Top element is %d\n", peek(pt));
	printf("Stack size is %d\n", size(pt));

	pop(pt);
	pop(pt);
	pop(pt);

	if (isEmpty(pt))
	printf("Stack is empty");
	else
	printf("Stack is not empty");

	return 0;
}
*********************************************************************************/

#ifndef STACK_H_
#define STACK_H_

#include "map.h"

// Data structure for stack
struct Stack
{
	int maxsize;	// define max capacity of stack
	int top;
	position_t *items;
};

// Utility function to initialize stack
struct Stack* newStack(int capacity);

// Utility function to return the size of the stack
int size(struct Stack *pt);

// Utility function to check if the stack is empty or not
int isEmpty(struct Stack *pt);

// Utility function to check if the stack is full or not
int isFull(struct Stack *pt);

// Utility function to add an element x in the stack
void push(struct Stack *pt, position_t x);

// Utility function to return top element in a stack, without removing it
position_t peek(struct Stack *pt);

// Utility function to pop top element from the stack
position_t pop(struct Stack *pt);

#endif /* STACK_H_ */