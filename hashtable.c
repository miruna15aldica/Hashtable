// Copyright Aldica Miruna 2022
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structuri.h"

#define MAX_STRING_SIZE 41
#define HMAX 10

linked_list_t *ll_create(unsigned int data_size)
{
	linked_list_t *ll;

	ll = malloc(sizeof(*ll));

	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t *new_node;

	if (!list) {
		return;
	}

	/* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
	if (n > list->size) {
		n = list->size;
	}

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	if (prev == NULL) {
		/* Adica n == 0. */
		list->head = new_node;
	} else {
		prev->next = new_node;
	}

	list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head) {
		return NULL;
	}

	/* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei.
	 */
	if (n > list->size - 1) {
		n = list->size - 1;
	}

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (prev == NULL) {
		/* Adica n == 0. */
		list->head = curr->next;
	} else {
		prev->next = curr->next;
	}

	list->size--;

	return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int ll_get_size(linked_list_t *list)
{
	if (!list) {
		return -1;
	}

	return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t **pp_list)
{
	ll_node_t *currNode;

	if (!pp_list || !*pp_list) {
		return;
	}

	while (ll_get_size(*pp_list) > 0) {
		currNode = ll_remove_nth_node(*pp_list, 0);
		free(currNode->data);
		currNode->data = NULL;
		free(currNode);
		currNode = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void ll_print_int(linked_list_t *list)
{
	ll_node_t *curr;

	if (!list) {
		return;
	}

	curr = list->head;
	while (curr != NULL) {
		printf("%d ", *((int *)curr->data));
		curr = curr->next;
	}

	printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void ll_print_string(linked_list_t *list)
{
	ll_node_t *curr;

	if (!list) {
		return;
	}

	curr = list->head;
	while (curr != NULL) {
		printf("%s ", (char *)curr->data);
		curr = curr->next;
	}

	printf("\n");
}

/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char *)a;
	uint64_t hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
		       int (*compare_function)(void *, void *))
{
	unsigned int i;
	hashtable_t *new = malloc(sizeof(*new));
	new->buckets = malloc(hmax * sizeof(*new->buckets));
	for (i = 0; i < hmax; i++) {
		new->buckets[i] = ll_create(sizeof(info));
	}
	new->hmax = hmax;
	new->no_elems = 0;
	new->hash_function = hash_function;
	new->compare_function = compare_function;
	return new;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable
 * folosind functia put 0, altfel.
 */
int ht_has_key(hashtable_t *ht, void *key)
{
	if (!ht)
		return 0;
	if (!ht->buckets)
		return 0;
	unsigned int index = ht->hash_function(key) % ht->hmax;
	if (!ht->buckets[index])
		return 0;
	ll_node_t *actual = ht->buckets[index]->head;
	while (actual) {
		if (ht->compare_function(((info *)actual->data)->key, key) == 0)
			return 1;
		actual = actual->next;
	}
	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	if (!ht)
		return NULL;
	if (!ht->buckets)
		return NULL;
	unsigned int index = ht->hash_function(key) % ht->hmax;
	if (!ht->buckets[index])
		return NULL;
	ll_node_t *actual = ht->buckets[index]->head;
	while (actual) {
		if (ht->compare_function(((info *)actual->data)->key, key) == 0)
			return ((info *)actual->data)->value;
		actual = actual->next;
	}
	return NULL;
}

/*
 * Functie care imi ia un hashtable curent si un element dintr un hash anterior
 * info si mi-l transfera in noul hashtable
 */

void ht_transfer(hashtable_t **ht, info *info)
{
	if (!ht)
		return;
	if (!(*ht)->buckets)
		return;
	unsigned int index = (*ht)->hash_function(info->key) % (*ht)->hmax;
	if (!(*ht)->buckets[index])
		return;
	ll_add_nth_node((*ht)->buckets[index], (*ht)->buckets[index]->size,
			info);
	(*ht)->no_elems++;
	(*ht)->size++;
}
/*
 * Functie care imi ia un ht si imi pune toate infos
 * din acel hashtable intr-un nou hashtable 
 * si imi transfera toate infos in acela
 */
hashtable_t *ht_resize(hashtable_t *ht)
{
	unsigned int i, j;
	hashtable_t *new = ht_create(ht->hmax + 10, hash_function_string,
				     compare_function_strings);

	for (i = 0; i < ht->hmax; i++) {
		ll_node_t *actual = ht->buckets[i]->head;
		for (j = 0; j < ht->buckets[i]->size; j++) {
			info *in = ((info *)actual->data);

			ht_transfer(&new, in);
			actual = actual->next;
		}
		ll_free(&ht->buckets[i]);
	}
	return new;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune
 * tipul ei), in momentul in care se creeaza o noua intrare in hashtable (in
 * cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie a
 * valorii la care pointeaza key si adresa acestei copii trebuie salvata in
 * structura info asociata intrarii din ht. Pentru a sti cati octeti trebuie
 * alocati si copiati, folositi parametrul key_size.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel
 * put(ht, key_actual, value_actual), valoarea la care pointeaza key_actual
 * poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia
 * unei intrari din hashtable. Nu ne dorim acest lucru, fiindca exista riscul sa
 * ajungem in situatia in care nu mai stim la ce cheie este inregistrata o
 * anumita valoare.
 */
void ht_put(hashtable_t **ht, void *key, unsigned int key_size, void *value,
	    unsigned int value_size)
{
	if (!ht)
		return;
	if (!(*ht)->buckets)
		return;
	unsigned int index = (*ht)->hash_function(key) % (*ht)->hmax;
	if (!(*ht)->buckets[index])
		return;
	ll_node_t *actual = (*ht)->buckets[index]->head;
	while (actual) {
		if ((*ht)->compare_function(((info *)actual->data)->key, key) ==
		    0) {
			info new_infor;
			new_infor.key = malloc(key_size);
			new_infor.value = malloc(value_size);
			memcpy(new_infor.key, key, key_size);
			memcpy(new_infor.value, value, value_size);
			free(((info *)actual->data)->value);
			free(((info *)actual->data)->key);
			memcpy(actual->data, &new_infor, sizeof(info));
			return;
		}

		actual = actual->next;
	}
	info new_infor;
	new_infor.key = malloc(key_size);
	new_infor.value = malloc(value_size);
	memcpy(new_infor.key, key, key_size);
	memcpy(new_infor.value, value, value_size);
	ll_add_nth_node((*ht)->buckets[index], (*ht)->buckets[index]->size,
			&new_infor);
	(*ht)->no_elems++;
	(*ht)->size++;
	if ((*ht)->no_elems / (*ht)->hmax > 1) {
		hashtable_t *old = (*ht);
		hashtable_t *nht = ht_resize(*ht);
		// schimb referinta hashtableului
		*ht = nht;
		free(old->buckets);
		free(old);
	}
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o
 * intrare din hashtable (adica memoria pentru copia lui key --vezi observatia
 * de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void ht_remove_entry(hashtable_t *ht, void *key)
{
	if (!ht)
		return;
	if (!ht->buckets)
		return;
	unsigned int index = ht->hash_function(key) % ht->hmax;
	if (!ht->buckets[index])
		return;
	unsigned int ptr = 0;
	ll_node_t *actual = ht->buckets[index]->head;
	while (actual) {
		if (ht->compare_function(((info *)actual->data)->key, key) ==
		    0) {
			ll_node_t *trash =
			    ll_remove_nth_node(ht->buckets[index], ptr);

			free(((info *)trash->data)->key);
			free(((info *)trash->data)->value);
			free(trash->data);
			free(trash);
			break;
		}
		ptr++;
		actual = actual->next;
	}
	ht->no_elems--;
	ht->size--;
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable,
 * dupa care elibereaza si memoria folosita pentru a stoca structura hashtable.
 */
void ht_free(hashtable_t *ht)
{
	unsigned int i, j;
	for (i = 0; i < ht->hmax; i++) {
		ll_node_t *actual = ht->buckets[i]->head;
		for (j = 0; j < ht->buckets[i]->size; j++) {
			free(((info *)actual->data)->key);
			free(((info *)actual->data)->value);
			actual = actual->next;
		}
		ll_free(&ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
