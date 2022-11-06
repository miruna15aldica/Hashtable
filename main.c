// Copyright Aldica Miruna 2022
#include "structuri.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_SIZE 41
#define HMAX 10

// se citeste o definitie si se adauga in htable
void add_def(hashtable_t **book)
{
	if (book == NULL) {
		printf("The book is not in the library.\n");
	} else {
		char name[21];
		char val[21];
		scanf("%s %s\n", name, val);

		ht_put(book, name, strlen(name) + 1, val, strlen(val) + 1);
	}
}

// se cauta o carte si se afiseaza date despre aceasta daca exista
void printBook(bookInfo *book)
{
	size_t i;
	double ranking = 0.0;
	char *name = calloc(strlen(book->name) - 1, sizeof(char));
	for (i = 1; i < strlen(book->name) - 1; i++) {
		name[i - 1] = book->name[i];
	}

	if (book->purchases != 0)
		ranking = book->rating / book->purchases;
	printf("Name:%s ", name);
	printf("Rating:%.3f ", ranking);
	printf("Purchases:%d\n", book->purchases);
	free(name);
}

// se printeaza date despre un user
void printUser(userInfo *user)
{
	printf("Name:%s Points:%d\n", user->name, user->points);
}

// functie care imi citeste numele unei carti dintre ghilimele
char *getBookName()
{
	int dim = 1;
	char *name = calloc(3, sizeof(char));
	char *newName;
	name[dim - 1] = fgetc(stdin);
	dim++;
	name[dim - 1] = fgetc(stdin);
	dim++;
	while (name[dim - 2] != '"') {
		name[dim - 1] = fgetc(stdin);
		dim++;
		name = realloc(name, dim * sizeof(char));
		name[dim - 1] = '\0';
	}
	newName = calloc(strlen(name) + 1, sizeof(char));
	strcpy(newName, name);
	free(name);
	return newName;
}

// functie care imi itereaza prin elementele htableului curent
// si imi afiseaza rankingul de carti
void getBooksRanking(hashtable_t *table)
{
	unsigned int i;
	int index = 1;
	bookInfo *book1 = NULL;

	// cat timp mai am elemente in htable
	while (table->no_elems) {
		// iterez prin buckets
		for (i = 0; i < table->hmax; i++) {
			linked_list_t *bucket = table->buckets[i];
			ll_node_t *book = bucket->head;
			// cat timp am un element
			while (book) {
				// ii preiau datele
				info *bookData = (info *)book->data;
				bookInfo *bookNow = (bookInfo *)bookData->value;
				// daca n avem mometan o prima carte o punem pe aceasta
				if (book1 == NULL) {
					book1 = bookNow;
				} else {
					double rating, rating1;
					// punem rating 0 daca n a fost cumparata
					if (bookNow->purchases == 0)
						rating = 0;
					else
						rating = bookNow->rating /
							 bookNow->purchases;
					if (book1->purchases == 0)
						rating1 = 0;
					else
						rating1 = book1->rating /
							  book1->purchases;
					// daca cartea nou gasita e mai buna decat
					// cea mai buna carte curenta cf cu cerinta
					if (rating > rating1 ||
					    (rating == rating1 &&
					     bookNow->purchases >
						 book1->purchases) ||
					    (rating == rating1 &&
					     bookNow->purchases ==
						 book1->purchases &&
					     strcmp(book1->name,
						    bookNow->name) > 0)) {
						// se ia noua cea mai buna carte
						bookInfo *aux = bookNow;
						bookNow = book1;
						book1 = aux;
					}
				}
				book = book->next;
			}
		}
		// daca avem o cea mai buna carte o afisam
		if (book1) {
			printf("%d. ", index);
			printBook(book1);
		}
		index++;
		// eliminam cartea din htable si dezalocam tot
		char *name = calloc(strlen(book1->name) + 1, sizeof(char));
		strcpy(name, book1->name);
		free(book1->name);
		ht_free(book1->definitions);
		ht_remove_entry(table, name);
		free(name);
		book1 = NULL;
	}
}

// functie care imi itereaza prin elementele htableului curent
// si imi afiseaza rankingul de useri
void getUsersRanking(hashtable_t *table)
{
	unsigned int i;
	userInfo *user1 = NULL;
	int index = 1;

	// cat timp mai am elemente in htable
	while (table->no_elems) {
		// iterez prin buckets
		for (i = 0; i < table->hmax; i++) {
			linked_list_t *bucket = table->buckets[i];
			ll_node_t *user = bucket->head;
			// cat timp am un element
			while (user) {
				// ii preiau datele
				info *userData = (info *)user->data;
				userInfo *userNow = (userInfo *)userData->value;
				// daca utilizatorul curent e banat
				// atunci il eliberez si o iau de la inceput cu iterarea
				if (userNow->banned == 1) {
					if (userNow->hasBook)
						free(userNow->book);
					char *name =
					    calloc(strlen(userNow->name) + 1,
						   sizeof(char));
					strcpy(name, userNow->name);
					free(userNow->name);
					ht_remove_entry(table, name);
					free(name);
					i = 0;
					break;
				}
				// daca n avem mometan un prim utilizator il luam pe aceasta
				if (user1 == NULL) {
					user1 = userNow;
				} else {
					// daca utilizatorul nou gasit e mai bun decat
					// cel mai bun user curenta cf cu cerinta
					if (user1->points < userNow->points ||
					    (user1->points == userNow->points &&
					     strcmp(user1->name,
						    userNow->name) > 0)) {
						// se ia noul utilizator in top
						userInfo *aux = userNow;
						userNow = user1;
						user1 = aux;
					}
				}
				user = user->next;
			}
		}
		// daca avem un cel mai bun user il afisam
		// si il scoatem si dezalocam din htable
		if (user1) {
			printf("%d. ", index);
			printUser(user1);
			index++;
			char *name =
			    calloc(strlen(user1->name) + 1, sizeof(char));
			strncpy(name, user1->name, strlen(user1->name));
			if (user1->hasBook)
				free(user1->book);
			free(user1->name);
			ht_remove_entry(table, name);
			free(name);
		}

		user1 = NULL;
	}
}

// iterez prin definitiile unui htable si le eliberez
void free_def(hashtable_t *table)
{
	unsigned int i;

	for (i = 0; i < table->hmax; i++) {
		linked_list_t *bucket = table->buckets[i];
		ll_node_t *book = bucket->head;
		while (book) {
			info *bookData = (info *)book->data;
			bookInfo *bookNow = (bookInfo *)bookData->value;
			ht_free(bookNow->definitions);
			free(bookNow->name);
			book = book->next;
		}
	}
}

int main()
{
	// declar si initializez htableurile
	hashtable_t *database = NULL;
	hashtable_t *users = NULL;
	database =
	    ht_create(HMAX, hash_function_string, compare_function_strings);
	users = ht_create(HMAX, hash_function_string, compare_function_strings);

	// cat timp citesc comenzi de la utilizator
	while (1) {
		// declar o comanda
		char command[MAX_STRING_SIZE];

		// citesc comanda
		scanf("%s ", command);
		// daca adaug o carte
		if (strncmp(command, "ADD_BOOK", 8) == 0) {
			bookInfo newBook;
			int defNumber;
			// o declar si ii pun numele
			newBook.name = getBookName();
			scanf("%d\n", &defNumber);
			// daca exista cartea atunci o updatez prin noilr
			// definitii
			if (ht_has_key(database, newBook.name)) {
				bookInfo *book =
				    (bookInfo *)ht_get(database, newBook.name);

				for (int i = 0; i < defNumber; i++) {
					add_def(&book->definitions);
				}
				free(newBook.name);

			} else {
				// daca nu atunci creez definitiile
				newBook.definitions =
				    ht_create(HMAX, hash_function_string,
					      compare_function_strings);
				// si initializez celelalte variabile si citesc
				// definitiile
				newBook.reachable = 1;
				newBook.purchases = 0;
				newBook.rating = 0;
				for (int i = 0; i < defNumber; i++) {
					add_def(&newBook.definitions);
				}
				// pun cartea in hashtable
				ht_put(&database, newBook.name,
				       strlen(newBook.name) + 1, &newBook,
				       sizeof(newBook));
			}

			// daca este de preluat o care
		} else if (strncmp(command, "GET_BOOK", 8) == 0) {
			char *name;

			// iau numele
			name = getBookName();

			// caut daca exista si daca da o afisez
			if (!ht_has_key(database, name)) {
				printf("The book is not in the library.\n");
			} else {
				bookInfo *book =
				    (bookInfo *)ht_get(database, name);
				printBook(book);
			}

			free(name);
			// daca trebuie sa scot o carte din librarie
		} else if (strncmp(command, "RMV_BOOK", 8) == 0) {
			char *name;

			name = getBookName();
			// verific dacac exista si daca da o scot
			// si eliberez structurile
			if (!ht_has_key(database, name)) {
				printf("The book is not in the library.\n");
			} else {
				bookInfo *book =
				    (bookInfo *)ht_get(database, name);
				ht_free(book->definitions);
				free(book->name);
				ht_remove_entry(database, name);
			}

			free(name);
			// daca este o functie de adaugat definitie intr-o carte
		} else if (strncmp(command, "ADD_DEF", 6) == 0) {
			char *name;

			name = getBookName();

			// preiau numele cartii si vad daca exista cartea si
			// daca da adaug definitia in cartea respectiva
			if (!ht_has_key(database, name)) {
				printf("The book is not in the library.\n");
			} else {
				bookInfo *book =
				    (bookInfo *)ht_get(database, name);
				add_def(&book->definitions);
			}

			free(name);
			// daca e o functie in care se vrea definitia dintr-o
			// carte
		} else if (strncmp(command, "GET_DEF", 6) == 0) {
			char *name;
			char def[MAX_STRING_SIZE];
			// se ia numele cartii
			name = getBookName();
			scanf("%s\n", def);
			// se verifica daca exista cartea
			if (!ht_has_key(database, name)) {
				printf("The book is not in the library.\n");
			} else {
				// daca exista se ia cartea
				bookInfo *book =
				    (bookInfo *)ht_get(database, name);
				// se verifica daca exista definitia si daca da
				// se afiseaza
				if (!ht_has_key(book->definitions, def)) {
					printf("The definition is not in the "
					       "book.\n");
				} else {
					printf("%s\n",
					       (char *)ht_get(book->definitions,
							      def));
				}
			}

			free(name);
			// daca e o functie in care se vrea definitia dintr-o
			// carte
		} else if (strncmp(command, "RMV_DEF", 6) == 0) {
			char *name;
			char def[MAX_STRING_SIZE];
			// se ia numele cartii
			name = getBookName();
			scanf("%s\n", def);

			// se verifica daca exista cartea
			if (!ht_has_key(database, name)) {
				printf("The book is not in the library.\n");
			} else {
				// daca exista se ia cartea
				bookInfo *book =
				    (bookInfo *)ht_get(database, name);
				// se verifica daca exista definitia si daca da
				// se sterge
				if (!ht_has_key(book->definitions, def)) {
					printf("The definition is not in the "
					       "book.\n");
				} else {
					ht_remove_entry(book->definitions, def);
				}
			}
			free(name);

			// daca se vrea sa se adauge un utilizator
		} else if (strncmp(command, "ADD_USER", 8) == 0) {
			char name[21];
			scanf("%s\n", name);
			// se ia numele si se verifica daca nu exista deja
			if (ht_has_key(users, name)) {
				printf("User is already registered.\n");
			} else {
				// daca nu exista se creaza si se initializeaza
				userInfo user;
				user.name =
				    calloc(strlen(name) + 1, sizeof(char));
				strcpy(user.name, name);
				user.hasBook = 0;
				user.banned = 0;
				user.points = 100;
				user.book = NULL;
				// si se adauga in htableul de utilizatori
				ht_put(&users, user.name, strlen(user.name) + 1,
				       &user, sizeof(user));
			}
			// daca se vrea sa se imprumute o carte
		} else if (strncmp(command, "BORROW", 6) == 0) {
			char name[MAX_STRING_SIZE];
			char *book;
			int days;

			// se citeste numele cartii si cate zile se va imprumuta
			scanf("%s ", name);
			book = getBookName();
			scanf("%d ", &days);
			// daca nu exista utilizatorul se afiseaza eroare
			if (!ht_has_key(users, name)) {
				printf("You are not registered yet.\n");
			} else {
				userInfo *user = ht_get(users, name);

				// daca e banat se afiseaza eroare
				if (user->banned) {
					printf("You are banned from this "
					       "library.\n");
					// daca deja a imprumutat o carte se
					// afiseaza eroare
				} else if (user->hasBook) {
					printf("You have already borrowed a "
					       "book.\n");
					// daca nu exista cartea se afiseaza
					// eroare
				} else if (!ht_has_key(database, book)) {
					printf("The book is not in the "
					       "library.\n");
				} else {
					// se preia cartea
					bookInfo *bookE =
					    ht_get(database, book);
					// daca deja e imprumutata cuiva se da
					// eroare
					if (!bookE->reachable) {
						printf(
						    "The book is borrowed.\n");
					} else {
						// altfel se marcheaza ca
						// imprumutata userului
						user->hasBook = 1;
						user->book =
						    calloc(strlen(book) + 1,
							   sizeof(char));
						strcpy(user->book, book);
						user->days = days;
						bookE->reachable = 0;
						// se aplica modificarile
						// structurilor in hashtable
						ht_put(&users, name,
						       strlen(name) + 1, user,
						       sizeof(*user));
						ht_put(&database, bookE->name,
						       strlen(bookE->name) + 1,
						       bookE, sizeof(*bookE));
					}
				}
			}
			free(book);
			// daca un user returneaza o carte
		} else if (strncmp(command, "RETURN", 12) == 0) {
			char userName[MAX_STRING_SIZE];
			char *bookName;
			int days;
			double rating;
			// se iau datele userului si ale cartii
			scanf("%s ", userName);
			bookName = getBookName();
			scanf("%d %lf\n", &days, &rating);

			userInfo *user = ht_get(users, userName);
			bookInfo *bookE = ht_get(database, bookName);
			// daca userul este banat sau nu imprumutase vreo carte
			// sau nu imprumutase cartea respectiva atunci se
			// afiseaza erori
			if (user->banned) {
				printf("You are banned from this library.\n");
			} else if (user->hasBook == 0) {
				printf("You didn't borrow this book.\n");
			} else if (strcmp(bookName, user->book) != 0) {
				printf("You didn't borrow this book.\n");
			} else {
				// altfel se returneaza cartea si se marcheaza
				// cu userul nu mai are nicio carte imprumutata
				user->hasBook = 0;
				// si se calculeaza punctajul actual
				if (user->days >= days)
					user->points += user->days - days;
				else
					user->points -= (days - user->days) * 2;
				// si se face din nou cartea vizibila in
				// librarie si se aduna ratingul
				bookE->reachable = 1;
				bookE->purchases++;
				bookE->rating += rating;
				free(user->book);
				user->book = NULL;
				user->hasBook = 0;
				// daca in urma noului punctaj e banat marcam
				// asta
				if (user->points < 0) {
					printf("The user %s has been banned "
					       "from this library.\n",
					       userName);
					user->banned = 1;
				}
				free(user->book);
				user->book = NULL;
				// updatam in htable
				ht_put(&users, userName, strlen(userName) + 1,
				       user, sizeof(*user));
				ht_put(&database, bookName,
				       strlen(bookName) + 1, bookE,
				       sizeof(*bookE));
			}

			free(bookName);

			// daca un user a pierdut cartea
		} else if (strncmp(command, "LOST", 4) == 0) {
			// se preia numele utilizatorului si a cartii
			char name[MAX_STRING_SIZE];
			char *bookName;
			scanf("%s ", name);
			bookName = getBookName();
			fgetc(stdin);
			// daca nu exista userul se afiseaza eroare
			if (!ht_has_key(users, name)) {
				printf("You are not registered yet.\n");
			} else {
				// se preiau datele
				userInfo *user = ht_get(users, name);
				bookInfo *bookE = ht_get(database, bookName);
				// se verifica daca userul nu e cumva banat
				if (user->banned) {
					printf("You are banned from this "
					       "library.\n");
				} else {
					// se sterge cartea din sistem
					ht_free(bookE->definitions);
					free(bookE->name);
					ht_remove_entry(database, bookName);
					free(user->book);
					// se elibereaza de la utilizator si se
					// scade punctajul
					user->book = NULL;
					user->hasBook = 0;
					user->points -= 50;
					// se verifica daca in urma pierderii a
					// fost banat
					if (user->points < 0) {
						printf("The user %s has been "
						       "banned from this "
						       "library.\n",
						       name);
						user->banned = 1;
					}
					// se actualizeaza userul in htable
					ht_put(&users, name, strlen(name) + 1,
					       user, sizeof(*user));
				}
			}

			free(bookName);
			// daca se vrea terminarea programului
		} else if (strncmp(command, "EXIT", 4) == 0) {
			printf("Books ranking:\n");
			// se calculeaza book rankingul
			getBooksRanking(database);
			// se calculeaza user rankingul
			printf("Users ranking:\n");
			getUsersRanking(users);
			// se elibereaza structurile
			free_def(database);
			ht_free(database);
			ht_free(users);
			// se iese din while
			break;
		}
	}

	return 0;
}
