#ifndef _DLLTEST_H
#define _DLLTEST_H

/**
 * @file DLLTest.h
 *
 * @brief Representation of a double linked list.
 *
 * @author Dennis Altenhoff (daltenhoff@uni-osnabrueck.de)
 * @author Till Grenzdörffer(tgrenzdoerff@uni-osnabrueck.de)
 *
 */


#include <stdlib.h>
#include <stdio.h>

/**
 * Node-struct to represent a node in a list.
 * Also contains a variable whether or not the list already holds an element or not.
 */
struct node{
    struct node* prev;
    int val;
    struct node* next;
    int nodeAdded;
};

/**
 * @brief Constructs an empty list.
 *
 * @return node* begin of new list
 * 
 */
struct node* new_list();

/**
 * @brief Adds a note with value value to list after front
 *
 * @param front element of list after which the value will be added
 * @param value value to add
 *
 * @return node* begin of new list
 *
 */
struct node* addNode(struct node* front, int value);

/**
 * @brief Prints a list
 *
 * @param begin begin of the list to print
 * @param lineBreak 1 if a lineBreak is to be printed
 *
 */
void printList(struct node* begin, int lineBreak);

/**
 * @brief Frees for a list allocated space
 *
 * @param begin begin of the list to clear
 *
 */
void clearList(struct node* begin);

/**
 * @brief Returns last node of a list.
 *
 * @param begin begin of the list to get end from
 *
 * @return node* last node of the list
 *
 */
struct node* getEnd(struct node* begin);

/**
 * @brief Calculates number of Elements in a list.
 *
 * @param begin begin of the list to count elements of
 *
 * @return int number of Elements in the list
 *
 */
int countElements (struct node* begin);

/**
 * @brief Splits a list into two halves.
 *
 * @param begin begin of the list to split
 * @param firstHalf begin of the list representing the first half
 * @param secondHalf begin of the list representing the first half
 *
 */
void split(struct node* begin, struct node* firstHalf, struct node* secondHalf);

/**
 * @brief Merges two lists into one, sorting them in the process.
 *
 * @param beginFirstHalf begin of the first list
 * @param beginSecondHalf begin of the second list
 *
 * @return node* begin of merged list
 *
 */
struct node* merge(struct node* beginFirstHalf, struct node* beginSecondHalf);

/**
 * @brief Method to sort a given list. 
 *
 * @param begin begin of the list to sort
 *
 * @return node* begin of merged list.
 *
 */
struct node* mergeSort(struct node* begin);

/**
 * main-method
 */
int main(int argc, char **argv);

#endif