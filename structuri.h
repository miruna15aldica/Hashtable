// Copyright Aldica Miruna 2022
#ifndef STRUCTURI_H_
#define STRUCTURI_H_
typedef struct ll_node_t {
	void *data;
	struct ll_node_t *next;
} ll_node_t;

typedef struct linked_list_t {
	ll_node_t *head;
	unsigned int data_size;
	unsigned int size;
} linked_list_t;

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
	/* Nr. total de noduri existente curent in toate bucket-urile. */
	unsigned int size;
	int no_elems;
	unsigned int hmax; /* Nr. de bucket-uri. */
	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor.
	 */
	unsigned int (*hash_function)(void *);
	/* (Pointer la) Functie pentru a compara doua chei. */
	int (*compare_function)(void *, void *);
};

typedef struct bookInfo {
	char *name;
	double rating;
	int purchases;
	int reachable;
	hashtable_t *definitions;
} bookInfo;

typedef struct userInfo {
	char *name;
	int hasBook;
	int banned;
	char *book;
	int days;
	int points;
} userInfo;

linked_list_t *ll_create(unsigned int data_size);
void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data);
ll_node_t *ll_remove_nth_node(linked_list_t *list, unsigned int n);
unsigned int ll_get_size(linked_list_t *list);
void ll_free(linked_list_t **pp_list);
void ll_print_int(linked_list_t *list);
void ll_print_string(linked_list_t *list);
void ht_put(hashtable_t **ht, void *key, unsigned int key_size, void *value,
	    unsigned int value_size);
int compare_function_ints(void *a, void *b);
int compare_function_strings(void *a, void *b);
unsigned int hash_function_string(void *a);
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
		       int (*compare_function)(void *, void *));
int ht_has_key(hashtable_t *ht, void *key);
void *ht_get(hashtable_t *ht, void *key);
void ht_transfer(hashtable_t **ht, info *info);
hashtable_t *ht_resize(hashtable_t *ht);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);
#endif  // STRUCTURI_H_
