#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <iostream> 

struct ListNode {
     int val;
     ListNode *next;
     ListNode(int x) : val(x), next(NULL) {}
 };

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2);
ListNode* createList(int* numbers, int length);
void printList(ListNode* list);
void calculateCarry(int *carry, int *value);


int main(int argc, char** argv ){
    int input[argc];
    for(int i = 1; i < argc; i++){
        input[i-1] = atoi(argv[i]);
    }
    ListNode* list1 = createList(input, argc-1);
    ListNode* list2 = createList(input, argc-2);
    printList(addTwoNumbers(list1, list2));


}


ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    int carry = 0;
    int value = 0;
    ListNode* solutionList = NULL;
    value = l1->val + l2->val + carry;
    l1 = l1->next;
    l2 = l2->next;
    calculateCarry(&carry, &value);
    solutionList = new ListNode(value);
    ListNode* temp = solutionList;
    while(l1 && l2){
        value = l1->val + l2->val + carry;
        calculateCarry(&carry, &value);
        temp->next = new ListNode(value);
        l1 = l1->next;
        l2 = l2->next;
        temp = temp->next;
    }

    while(l1 && !l2){
        value = l1->val + carry;
        calculateCarry(&carry, &value);
        temp->next = new ListNode(value);
        l1 = l1->next;
        temp = temp->next;
    }

    while(!l1 && l2){
        value = l2->val + carry;
        calculateCarry(&carry, &value);
        temp->next = new ListNode(value);
        l2 = l2->next;
        temp = temp->next;
    }

    if(carry > 0){
        temp->next = new ListNode(carry);
    }
    return solutionList;
}

void calculateCarry(int *carry, int *value){
    *carry = floor(*value / 10);
    *value = *value % 10;

}

ListNode* createList(int* numbers, int length){
    ListNode* list = new ListNode(numbers[0]);
    ListNode* temp = list;
    for(int i = 1; i < length; i++){
        temp->next = new ListNode(numbers[i]);
        temp = temp->next;
    }
    return list;
}

void printList(ListNode* list){
    if(list){
        std::cout << list->val << " -> ";
        return printList(list->next);
    }
    std::cout << std::endl;
}
