// Z'vonty Flugence (zaf17)
#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
  int value;
  struct Node* next;
} Node;

Node* create_node(int value) {
  Node* node = malloc(sizeof(Node));
  node->value = value;
  node->next = NULL;
  return node;
}

void list_print(Node* head) {
  int i = 0;
  for(Node* currNode = head; currNode != NULL; currNode = currNode->next, i++) {
    if(i != 0) {
      printf(" -> ");
    }
    printf("%d", currNode->value);
  }
  printf("\n");
}

Node* list_append(Node* head, int value) {
  Node* currNode = head;
  // Loop to the last node
  while(currNode->next != NULL) {
    currNode = currNode->next;
  }
  currNode->next = create_node(value);
  return currNode->next;
}

Node* list_prepend(Node* head, int value) {
  Node* node = create_node(value);
  node->next = head;
  return node;
}

void list_free(Node* head) {
  if(head != NULL) {
    Node* node = head;
    while(node != NULL) {
      Node* currNode = node;
      node = node->next;
      free(currNode);
    }
  }
}

// Harder than it looks
Node* list_remove(Node* head, int value) {
  // Check head case
  if(head->value == value) {
    Node* node = head->next;
    free(head);
    return node;
  }

  for(Node* currNode = head; currNode->next != NULL; currNode = currNode->next) {
    if(currNode->next->value == value) {
      Node* node_to_remove = currNode->next;
      currNode->next = currNode->next->next;
      free(node_to_remove);
      return head;
    }
  }
  return head;
}

int main() {
  Node* head = create_node(1); 
  list_print(head); // 1
  Node* end = list_append(head, 2); 
  list_print(head); // 1 -> 2
  end->next = create_node(3);
  list_print(head); // 1 -> 2 -> 3
  head = list_prepend(head, 0);
  list_print(head); // 0 -> 1 -> 2 -> 3
  list_append(head, 4);
  list_print(head); // 0 -> 1 -> 2 -> 3 -> 4
  list_append(head, 5);
  list_print(head); // 0 -> 1 -> 2 -> 3 -> 4 -> 5
  
  head = list_remove(head, 5);
  list_print(head); // 0 -> 1 -> 2 -> 3 -> 4
  head = list_remove(head, 3);
  list_print(head); // 0 -> 1 -> 2 -> 4
  head = list_remove(head, 0);
  list_print(head); // 1 -> 2 -> 4

  list_free(head);

  return 0;
}