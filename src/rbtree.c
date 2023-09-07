#include "rbtree.h"

#include <stdlib.h>

/////////////////////////
// 새 tree를 생성하는 함수 //
/////////////////////////
rbtree *new_rbtree(void)
{
  // rbtree의 인자를 1개 메모리 할당(할당된 공간의 값을 모두 0으로 초기화)
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // nil노드 생성 및 초기화
  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));
  // nil노드는 항상 BLACK
  nilNode->color = RBTREE_BLACK;
  // tree의 nil과 root를 nilNode로 설정(tree가 빈 경우 root는 nil노드여야 함)
  p->nil = nilNode;
  p->root = nilNode;
  return p;
}

// 각 노드와 그 자식 노드들의 메모리를 반환하는 함수
void traverse_delete_node(rbtree *t, node_t *node)
{
  if (node->left != t->nil)
    traverse_delete_node(t, node->left);
  if (node->right != t->nil)
    traverse_delete_node(t, node->right);

  // 현재 노드의 메모리를 반환
  free(node);
}

// 트리를 순회하면서 각 노드의 메모리를 반환하는 함수
void delete_rbtree(rbtree *t)
{
  node_t *node = t->root;
  if (node != t->nil)
    traverse_delete_node(t, node);

  // nil 노드오 rbtree 구조체의 메모리를 반환
  free(t->nil);
  free(t);
  // TODO: reclaim the tree nodes's memory
}

// 오른쪽으로 회전하는 함수
void right_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_right = node->right;

  // 부모가 루트인 경우 : 현재 노드를 루트로 지정(노드를 삭제한 경우만 해당)
  if (parent == t->root)
    t->root = node;
  else
  {
    // 1-1) 노드의 부모를 grand_parent로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }

  node->parent = grand_parent; // 1-2) 노드를 grand_parent의 자식으로 변경(양방향 연결)
  parent->parent = node;       // 2-1) parent의 부모를 노드로 변경
  node->right = parent;        // 2-2) parent를 노드의 자식으로 변경(양방향 연결)
  node_right->parent = parent; // 3-1) 노드의 자식의 부모를 parent로 변경
  parent->left = node_right;   // 3-2) 노드의 자식을 부모의 자식으로 변경(양방향 연결)
}

// 왼쪽으로 회전하는 함수
void left_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_left = node->left;

  // 부모가 루트인 경우 : 현재 노드를 루트로 지정(노드를 삭제한 경우만 해당)
  if (parent == t->root)
    t->root = node;
  else
  {
    // 1-1) 노드의 부모를 grand_parent로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }
  node->parent = grand_parent; // 1-2) 노드를 grand_parent의 자식으로 변경(양방향 연결)
  parent->parent = node;       // 2-1) parent의 부모를 노드로 변경
  node->left = parent;         // 2-2) parent를 노드의 자식으로 변경(양방향 연결)
  parent->right = node_left;   // 3-1) 노드의 자식의 부모를 parent로 변경
  node_left->parent = parent;  // 3-2) 노드의 자식을 부모의 자식으로 변경(양방향 연결)
}

/////////////////////////////////
// 노드 삽입 후 불균형을 복구하는 함수 //
////////////////////////////////
void rbtree_insert_fixup(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle;
  int is_left = node == parent->left; // 현재 노드가 왼쪽자식인지 여부
  int is_parent_is_left;              // 부모가 왼쪽 자식인지 여부

  // 추가된 노드가 root노드인 경우 : 색만 변경
  if (node == t->root)
  {
    node->color = RBTREE_BLACK;
    return;
  }

  // 부모가 BLACK인 경우 : 변경 없음
  if (parent->color == RBTREE_BLACK)
    return;

  is_parent_is_left = grand_parent->left == parent;
  uncle = (is_parent_is_left) ? grand_parent->right : grand_parent->left;

  // [case 1] : 부모와 부모의 형제가 모두 RED 인 경우 // Recoloring
  if (uncle->color == RBTREE_RED)
  {
    parent->color = RBTREE_BLACK;
    uncle->color = RBTREE_BLACK;
    grand_parent->color = RBTREE_RED;

    rbtree_insert_fixup(t, grand_parent);
    return;
  }
  if (is_parent_is_left)
  {
    if (is_left)
    {
      // [case 2] : 부모의 형제가 BLACK & 부모가 [왼쪽 자식] & 현재 노드가 [왼쪽 자식] 인 경우
      right_rotate(t, parent);
      exchange_color(parent, parent->right);
      return;
    }
    // [case 3] : 부모의 형제가 BLACK & 부모가 [왼쪽 자식] & 현재 노드가 [오른쪽 자식] 인 경우
    left_rotate(t, node);
    right_rotate(t, node);
    exchange_color(node, node->right);
    return;
  }
  if (is_left)
  {
    // [case 3] : 부모의 형제가 BLACK & 부모가 [오른쪽 자식] & 현재 노드가 [왼쪽 자식] 인 경우
    right_rotate(t, node);
    left_rotate(t, node);
    exchange_color(node, node->left);
    return;
  }

  // [case 2] : 부모의 형제가 BLACK & 부모가 [오른쪽 자식] & 현재 노드가 [오른쪽 자식] 인 경우
  left_rotate(t, parent);
  exchange_color(parent, parent->left);
}

//////////////////////////////////////////////////
// 노드를 삽입하는 함수 / 안에 노드의 불균형을 복구하는 함수 //
/////////////////////////////////////////////////
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // 새 노드 생성
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED; // 항상 red로 추가
  // 추가한 노드의 자식들을 nil 노드로 설정
  new_node->left = t->nil;
  new_node->right = t->nil;

  // 새 노드를 삽입할 위치 탐색
  node_t *current = t->root;

  while (current != t->nil)
  {
    if (key < current->key)
    {
      if (current->left == t->nil)
      {
        current->left = new_node; // 새 노드를 왼쪽 자식으로 추가
        break;
      }
      current = current->left;
    }
    else
    {
      if (current->right == t->nil)
      {
        current->right = new_node; // 새 노드를 오른쪽 자식으로 추가
        break;
      }
      current = current->right;
    }
  }

  new_node->parent = current; // 새 노드의 부모 지정

  // root 가 nil 이면(트리가 비어있으면) 새 노드를 트리의 루트로 지정
  if (current == t->nil)
  {
    t->root = new_node;
  }
  // 불균형 복구
  rbtree_insert_fixup(t, new_node);
  return t->root;
}

// key에 해당하는 노드를 반환하는 함수
// tree내에 해당 key가 있는지 탐색하여 해당 노드의 포인터를 반환한다.
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (key == current->key)
      return current;
    else
      current = (key < current->key) ? current->left : current->right;
  }
  return NULL; // 해당 key값을 가진 노드가 없을 경우 NULL 반환
}

// key가 최소값에 해당하는 노드를 반환하는 함수
node_t *rbtree_min(const rbtree *t)
{
  node_t *current = t->root;
  while (current->left != t->nil)
  {
    current = current->left;
  }
  return current;
}

node_t *find_successor(const rbtree *t, const node_t *z)
{
  node_t *current = z->right;
  while (current->left != t->nil)
  {
    current = current->left;
  }
  return current;
}

// key가 최대값에 해당하는 노드를 반환하는 함수
node_t *rbtree_max(const rbtree *t)
{
  node_t *current = t->root;
  while (current->right != t->nil)
  {
    current = current->right;
  }
  return current;
}

/////////////////////////////////
// 노드 삭제 후 불균형을 복구하는 함수 //
////////////////////////////////
// parent : extra_black 이 부여된 노드의 부모
// is_left : extra_black 이 부여된 노드가 왼쪽 자식인지 여부
void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left)
{
  // 삭제 후 대체한 노드가 RED(red & black) : BLACK으로 변경
  node_t *extra_black = is_left ? parent->left : parent->right;
  if (extra_black->color == RBTREE_RED)
  {
    extra_black->color = RBTREE_BLACK;
    return;
  }

  node_t *sibling = is_left ? parent->right : parent->left; // sibling이 왼쪽 자식인지 / 오른쪽 자식인지
  node_t *sibling_left = sibling->left;
  node_t *sibling_right = sibling->right;

  // [CASE 3] 형제가 RED인 경우
  if (sibling->color == RBTREE_RED)
  {
    if (is_left)
      left_rotate(t, sibling);
    else
      right_rotate(t, sibling);

    exchange_color(sibling, parent);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  node_t *near = is_left ? sibling_left : sibling_right;    // 형제의 자식 중 extra_black으로부터 가까운 노드
  node_t *distant = is_left ? sibling_right : sibling_left; // 형제의 자식 중 extra_black으로부터 먼 노드

  // is_left : extra_black 이 부여된 노드가 왼쪽 자식일 때
  // [CASE 4] 형제가 BLACK, 형제의 가까운 자식이 RED, 형제의 더 먼 자식이 BLACK
  if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  {
    right_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  // is_left : extra_black 이 부여된 노드가 왼쪽 자식일 때
  // [CASE 5] 형제가 BLACK, 형제의 더 먼 자식이 RED
  if (is_left && distant->color == RBTREE_RED)
  {
    left_rotate(t, sibling);
    exchange_color(sibling, parent);
    distant->color = RBTREE_BLACK;
    return;
  }

  // extra_black 이 부여된 노드가 오른쪽 자식일 때
  // [CASE 4] 형제가 BLACK, 형제의 가까운 자식이 RED, 형제의 더 먼 자식이 BLACK
  if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  {
    left_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  // extra_black 이 부여된 노드가 오른쪽 자식일 때
  // [CASE 5] 형제가 BLACK, 형제의 더 먼 자식이 RED
  if (distant->color == RBTREE_RED)
  {
    right_rotate(t, sibling);
    exchange_color(sibling, parent);
    distant->color = RBTREE_BLACK;
    return;
  }

  // [CASE 2] 형제가 BLACK, 형제의 자식이 둘 다 BLACK
  sibling->color = RBTREE_RED;

  if (parent != t->root)
    rbtree_erase_fixup(t, parent->parent, parent->parent->left == parent);
}

////////////////////////////////////////////
// 노드를 삭제하는 함수 / 안에 불균형을 복구하는 함수 //
////////////////////////////////////////////
int rbtree_erase(rbtree *t, node_t *delete)
{
  node_t *remove; // 트리에서 없어질 노드
  node_t *remove_parent;
  node_t *replace_node;
  int is_remove_black;
  int is_remove_left;

  // step 1) delete 삭제 후 remove 대체할 replace-node 찾기
  // step 1-1) delete 노드의 자식이 둘인 경우 : delete의 키를 후계자 노드의 키값으로 대체, 노드의 색은 delete의 색 유지
  if (delete->left != t->nil && delete->right != t->nil)
  {
    remove = find_successor(t, delete); // 후계자 노드 (오른쪽 서브트리에서 가장 작은 노드)
    replace_node = remove->right;       // 오른쪽 서브트리에서 가장 작은 값이므로 자식이 있다면 오른쪽만 존재
    delete->key = remove->key;          // delete의 키를 후계자 노드의 키값으로 대체(색은 변경X)
  }
  else
  {
    // step 1-2) delete 노드의 자식이 없거나 하나인 경우 : delete 노드의 자식으로 대체, 노드의 색도 대체되는 노드의 색으로 변경
    remove = delete;
    // 대체할 노드 : 자식이 있으면 자식노드로, 없으면 nil노드로 대체
    replace_node = (remove->right != t->nil) ? remove->right : remove->left;
  }
  remove_parent = remove->parent;

  // step 2) remove 노드 제거
  // [CASE 1] : remove = root
  if (remove == t->root)
  {
    t->root = replace_node;        // 대체할 노드를 트리의 루트로 지정
    t->root->color = RBTREE_BLACK; // 루트 노드는 항상 BLACK
    free(remove);
    return 0; // 불균형 복구 함수 호출 불필요 (제거 전 트리에 노드가 하나 혹은 두개 -> 불균형이 발생하지 않음)
  }

  // step 2-1) remove의 부모 / remove의 자식 이어주기
  is_remove_black = remove->color; // remove 노드 제거 전에 지워진 노드의 색 저장
  is_remove_left = remove_parent->left == remove;

  // step 2-1-1) 자식 연결
  if (is_remove_left)
    remove_parent->left = replace_node; // remove가 왼쪽 자식 이었을 경우 : remove 부모의 왼쪽에 이어주기
  else
    remove_parent->right = replace_node; // remove가 오른쪽 자식 이었을 경우 : remove 부모의 오른쪽에 이어주기

  // step 2-1-2) 부모도 연결(양방향 연결)
  replace_node->parent = remove_parent;
  free(remove);

  // [CASE 2~6] : remove 노드가 검정 노드인 경우
  // step 3) 불균형 복구 함수 호출
  if (is_remove_black)
    rbtree_erase_fixup(t, remove_parent, is_remove_left);
  return 0;
}

// 정렬하는 함수
// 중위순회 결과를 arr에 담음
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  rb_inorder(t->root, arr, t, 0);
  return 0;
}

int rb_inorder(node_t *root, key_t *res, const rbtree *t, int i)
{
  if (root == t->nil)
  {
    return i;
  }
  i = rb_inorder(root->left, res, t, i);
  res[i] = root->key;
  i = rb_inorder(root->right, res, t, i + 1);
}

// Recoloring
void exchange_color(node_t *a, node_t *b)
{
  int temp = a->color;
  a->color = b->color;
  // b->color = (temp == RBTREE_BLACK) ? RBTREE_BLACK : RBTREE_RED;
  b->color = temp;
}
