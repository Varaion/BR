#include "DLLTest.h"

struct node* new_list() {
    
    // Allocate memory
    struct node* head = malloc(sizeof(struct node));
    
    // Initialise everything
    head->prev = NULL;
    head->val = 0;
    head->next = NULL;
    head->nodeAdded = 0;
    
    return head;
}

struct node* addNode(struct node* front, int value){
    
    // If first element to add, overwrite the initial element
    if(front->nodeAdded == 0){
        front->val = value;
        front->nodeAdded = 1;
        return front;
    }
    // Otherwise allocate memory and add node to list
    struct node* new = malloc(sizeof(struct node));
    new->prev = front;
    new->val = value;
    new->next = NULL;
    new->nodeAdded = 1;
    front->next = new;
    return new;
}

void printList(struct node* begin, int lineBreak){
    
    struct node* end = getEnd(begin);
    struct node* current = begin;
    printf("[");
    while ((current->next!=NULL) && (current != end)){
        printf("%d", current->val);
        current = current->next;
    }
    printf("%d]", current->val);
    // Do linebreak if wanted.
    if(1 == lineBreak){
        printf("\n");
    }
}

void clearList(struct node* begin){
    
    // Go through list and free all the allocated memory
    struct node* current = begin;
    while(current->next != NULL){
        struct node* newCurrent = current->next;
        free(current);
        current = newCurrent;
    }
    free(current);
}

struct node* getEnd(struct node* begin){
    
    // Go through list and return last element
	struct node* end = begin;
	while(end->next!=NULL){
		end = end->next;
	}
	return end;
}

int countElements (struct node* begin){
    
    if(begin->nodeAdded == 0){
        // If list is empty return 0
        return 0;
    }
    
    // Otherwise go through list and count elements
    struct node* tempBegin = begin;
    int elemCount = 1;
    
    while(tempBegin->next!=NULL){
        tempBegin = tempBegin->next;
        elemCount++;
    }
    
    return elemCount;
}

void split(struct node* begin, struct node* firstHalf, struct node* secondHalf){
    
    // Fancy prints
    printf("split: ");
    printList(begin, 0);
    printf(" into: ");
    
    // Save pointers and begins
    struct node* tempBegin = begin;
    struct node* tempFirstHalf = firstHalf;
    struct node* tempSecondHalf = secondHalf;
    int elemCount = countElements(begin);
    
    int i = 0;
    tempBegin = begin;
    
    // Add first half to first list
    for(i = 0; i < elemCount/2; i++){
        tempFirstHalf = addNode(tempFirstHalf, tempBegin->val);
        tempBegin = tempBegin->next;
    }
    // Add second half to second list
    for(i = elemCount/2; i < elemCount; i++){
        tempSecondHalf = addNode(tempSecondHalf, tempBegin->val);
        if(i != elemCount-1){
            tempBegin = tempBegin->next;
        }
    }
    
    // free memory of former list
    clearList(begin);
    
    // More fancy prints
    printList(firstHalf, 0);
    printf(" and ");
    printList(secondHalf, 1);
}

struct node* merge(struct node* beginFirstHalf, struct node* beginSecondHalf){
    
    // Fancy prints
    printf("merge: ");
    printList(beginFirstHalf, 0);
    printf(" and ");
    printList(beginSecondHalf, 1);
    
    // Get number of Elements per list
    int elemCountFirstHalf = countElements(beginFirstHalf);
    int elemCountSecondHalf = countElements(beginSecondHalf);
    
    /* DO MERGE ACCORDING TO INFOA-ALGORITHM */
    
    struct node* mergedListBegin = new_list();
    struct node* mergedList = mergedListBegin;
    
    if(beginFirstHalf->val<beginSecondHalf->val){

        mergedList = addNode(mergedList, beginFirstHalf->val);
        elemCountFirstHalf--;
        if(elemCountFirstHalf != 0){
            beginFirstHalf = beginFirstHalf->next;
        }
        
    }else{

        mergedList = addNode(mergedList, beginSecondHalf->val);
        elemCountSecondHalf--;
        if(elemCountSecondHalf != 0){
            beginSecondHalf = beginSecondHalf->next;
        }
        
    }
       
    while((elemCountFirstHalf != 0) && (elemCountSecondHalf != 0)){
        if(beginFirstHalf->val<beginSecondHalf->val){
            mergedList = addNode(mergedList, beginFirstHalf->val);
            elemCountFirstHalf--;
            if(elemCountFirstHalf != 0){
                beginFirstHalf = beginFirstHalf->next;
            }
        }else{
            mergedList = addNode(mergedList, beginSecondHalf->val);
            elemCountSecondHalf--;
            if(elemCountSecondHalf != 0){
                beginSecondHalf = beginSecondHalf->next;
            }
        }
    }
    
    while(elemCountFirstHalf != 0){
        mergedList = addNode(mergedList, beginFirstHalf->val);
        elemCountFirstHalf--;
        if(elemCountFirstHalf != 0){
            beginFirstHalf = beginFirstHalf->next;
        }
    }
    while(elemCountSecondHalf != 0){
        mergedList = addNode(mergedList, beginSecondHalf->val);
        elemCountSecondHalf--;
        if(elemCountSecondHalf != 0){
            beginSecondHalf = beginSecondHalf->next;
        }
    }
    
    // Return begin of merged list
    return mergedListBegin;
    
}

struct node* mergeSort(struct node* begin){
    
    // Fancy prints
    printf("mergeSort: ");
    printList(begin, 1);
    
    if(begin != getEnd(begin)){
        
        // If list contains more than one element split it
        struct node* firstHalf = new_list();
        struct node* secondHalf = new_list();
        
        split(begin, firstHalf, secondHalf);
        
        // Get sorted halves
        struct node* sortedFirstHalf = mergeSort(firstHalf);
        struct node* sortedSecondHalf = mergeSort(secondHalf);
        
        // Merge sorted halves together
        struct node* mergedList = merge(sortedFirstHalf, sortedSecondHalf);
        
        // Free memory of now unnecessary lists
        clearList(sortedFirstHalf);
        clearList(sortedSecondHalf);
        
        // Return merged list
        return mergedList;
        
    }else{
        // If list contains one or less element return it. 
        
        return begin;
        
    }
    
}

int main(int argc, char **argv){
    
    // read arguments and create list to sort
    struct node* head = new_list();
	struct node* current = head;
	int i;
	for(i = 1; i < argc; i++){
		current = addNode(current, atoi(argv[i]));
	}
	struct node* end = getEnd(head);
    
    // Do fancy prints and start sort
    printf("Liste vor Sortieren: ");
    printList(head, 1);
	head = mergeSort(head);
    
    // Print result
    printf("Liste nach Sortieren: ");
    printList(head, 1);

    // free memory of list
	clearList(head);
    
	return 0;
}