/*

date: 14.04.2019
author: Daulet

Keygen for crackme

*/

#include <stdio.h>
#include <string.h>

#define MAX_STR_LEN 30

//
//  structure that represents single element
//  in sequence of chars for serial
//
typedef struct _NODE
{
  struct _NODE*   Parent;         //  Ancestor of this node
  char            Letter;         //  Letter of this node
  int             Value;          //  Calculated hash till this node
  int             ChildrenAmount; //  Amount of child nodes
  struct _NODE**  Children;       //  Array of child nodes
} NODE, *PNODE;

/**
  Print serial by leaf node

  @param  Leaf  final node or final letter of the serial
**/
int PrintSerial(
  _In_ PNODE Leaf
)
{
  PNODE current = Leaf;
  int counter = 0;
  int size = 0;
  char* sequence = NULL;

  //  in order to allocate space for sequence
  //  calculate parents
  while (current->Parent != NULL)
  {
    size++;
    current = current->Parent;
  }

  
  if (size > MAX_STR_LEN)
  {
    //sequence too long
    return 1;
  }

  sequence = (char*) malloc(sizeof(char) * (size + 1));

  if (NULL == sequence)
  {
    printf("%s\n", "Memory allocation error");
    return 3;
  }

  current = Leaf;

  while (current->Parent != NULL)
  {
    sequence[counter] = current->Letter;
    current = current->Parent;
    counter++;
  }

  sequence[size] = '\0';

  printf("%s\n", sequence);

  free(sequence);

  return 0;
}

/**
  Allocate and put initial data to node

  @param  Node    node returned
  @param  Parent  parent for node allocated
  @param  Letter  letter of this node
  @param  Value   hash on this node
**/
int AllocNode(
  _Out_ PNODE* Node,
  _In_opt_ PNODE Parent,
  _In_opt_ char Letter,
  _In_opt_ int Value
)
{
  *Node = (PNODE) malloc(sizeof(NODE));

  if (*Node == NULL)
  {
    printf("%s\n", "Memory allocation error");
    return 3;
  }

  (*Node)->Parent = Parent;
  (*Node)->Letter = Letter;
  (*Node)->Value = Value;
  (*Node)->ChildrenAmount = 0;
  (*Node)->Children = NULL;

  return 0;
}

/**
  Build tree of nodes recursivly

  @param  value   hash status
  @param  Root    root node
**/
int BuildNode(
  _In_ int value, 
  _In_ PNODE Root
)
{
  const char CHAR_END = 'z' - 0x30;
  const char CHAR_START = '0' - 0x30;
  const int MULTIPLIER = 0x0a;

  char current = CHAR_END;
  int counter = 0;

  //  in order to allocate space for child
  //  calculate them
  while (current >= CHAR_START)
  {
    if (value - current >= 0 && 
      (value - current) % MULTIPLIER == 0)
    {
      counter++;
    }

    current--;
  }

  Root->ChildrenAmount = counter;
  Root->Children = (PNODE*) malloc(sizeof(PNODE) * counter);

  if (NULL == Root->Children)
  {
    printf("%s\n", "Memory allocation error");
    return 3;
  }

  current = CHAR_END;
  counter = 0;

  //  loop again to create child
  while (current >= CHAR_START)
  {
    if (value - current >= 0 && 
      (value - current) % MULTIPLIER == 0)
    {
      PNODE newNode = NULL;
      int result = AllocNode(
        &newNode, 
        Root, 
        current + 0x30, 
        (value - current) / MULTIPLIER
      );

      if (result)
      {
        //  possibly memory allocation error
        return result;
      }

      Root->Children[counter] = newNode;
      counter++;

      if (newNode->Value == 0)
      {
        //  we found sequence
        //  need to print it
        PrintSerial(newNode);
        system("pause");
      }
      else
      {
        //  going deeper
        BuildNode(newNode->Value, newNode);
      }
    }

    current--;
  }

  return 0;
}

/**
  Free memory allocated for nodes and children recursivly

  @param  Node         Root node to be freed
**/
void freeNodes(
  _In_ PNODE Node
)
{
  for (int i = 0; i < Node->ChildrenAmount; i++)
  {
    if (0 != Node->Children[i]->ChildrenAmount)
    {
      //  if child has no children, free it
      free(Node->Children[i]);
    }
    else
    {
      //  need to free 
      freeNodes(Node->Children[i]);
    }
  }

  free(Node);
}

/**
  Program takes name as input name, calculate its hash
  Finally generate from hash sequences of valid serials

  @param  argc         must be 2 (name of the program and input name)
  @param  argv         contains input name (must be no longer than MAX_STR_LEN)

  @retval 0 - in case of success, anything 
**/
int main(
  _In_ int argc, 
  _In_ char **argv
)
{
  if (argc != 2)
  {
    printf("%s\n", "Usage: CrackmeKeygen <Name>");
    return 1;
  }

  if (strlen(argv[1]) > MAX_STR_LEN)
  {
    printf("%s %d\n", "Too long name. Maximum name length: ", MAX_STR_LEN);
    return 1;
  }

  //
  //  Calculating login
  //  

  int login = 0;  //  here we store hash

  for (unsigned int i = 0; i < strlen(argv[1]); i++)
  {
    if (argv[1][i] < 'A')
    {
      printf("%S\n", L"Login have to contain characters larger that 'A' (check ASCII)");
      return 2;
    }

    if (argv[1][i] > 'Z')
    {
      argv[1][i] -= 0x20; //  accoring to crackme
    }

    login += argv[1][i];
  }

  login = login ^ 0x5678;
  login = login ^ 0x1234;

  //
  //  Calculating serial
  //

  PNODE Root = (PNODE) malloc(sizeof(NODE));
  Root->Value = login;
  Root->ChildrenAmount = 0;
  Root->Children = NULL;
  Root->Letter = 0;
  Root->Parent = NULL;

  int result = BuildNode(login, Root);

  //
  //  Free
  //
  freeNodes(Root);

  return 0;
}