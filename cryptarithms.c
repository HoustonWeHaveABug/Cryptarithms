#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define LETTERS_MAX 10

typedef struct {
	int symbol;
	int value_min;
	int value;
}
letter_t;

typedef struct {
	letter_t *letter;
	int power;
	int terms_idx;
}
word_letter_t;

typedef enum {
	STATE_START,
	STATE_WORD,
	STATE_SPACE,
	STATE_OPERATOR
}
state_t;

int add_word_letter(int, int, int);
void set_letter(letter_t *, int, int);
void set_word_letter(word_letter_t *, letter_t *, int);
void set_word_letters_power(int);
int compare_word_letters(const void *, const void *);
void cryptarithm(int, int, int);
int check_terms(int, int);
void free_word_letters(void);

int letters_n, word_letters_n, word_len_max, terms_n, *sums, used_digits[LETTERS_MAX], nodes_n, solutions_n;
letter_t letters[LETTERS_MAX];
word_letter_t *word_letters;

int main(void) {
	int terms_idx, word_len, symbol;
	state_t state;

	/* Reset parsing variables for first cryptarithm */
	letters_n = 0;
	word_letters_n = 0;
	word_len_max = 0;
	terms_idx = 0;
	word_len = 0;
	state = STATE_START;

	/* Parse input
	symbol = fgetc(stdin);
	while (!feof(stdin)) {
		int digit;
		switch (symbol) {
		case ' ':
			if (state == STATE_START || state == STATE_SPACE) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}
			if (state == STATE_WORD) {
				set_word_letters_power(word_len);
			}
			state = STATE_SPACE;
			break;
		case '+':
			if (state != STATE_SPACE) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}
			state = STATE_OPERATOR;
			break;
		case '=':
			if (state != STATE_SPACE) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}
			terms_idx++;
			state = STATE_OPERATOR;
			break;
		case '\n':
			if (state != STATE_WORD) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}
			if (state == STATE_WORD) {
				set_word_letters_power(word_len);
			}
			terms_n = terms_idx+1;
			if (terms_n < 2) {
				fprintf(stderr, "Invalid cryptarithm\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}

			/* Parsing complete for one cryptarithm */
			sums = malloc(sizeof(int)*(size_t)(word_len_max*terms_n));
			if (!sums) {
				fprintf(stderr, "Could not allocate memory for sums\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}

			/* Sort word letters by ascending power/term index/symbol */
			qsort(word_letters, (size_t)word_letters_n, sizeof(word_letter_t), compare_word_letters);

			/* Search for all solutions in cryptarithm */
			for (digit = 0; digit < LETTERS_MAX; digit++) {
				used_digits[digit] = 0;
			}
			nodes_n = 0;
			solutions_n = 0;
			cryptarithm(-1, -1, 0);
			printf("Nodes %d\n", nodes_n);

			/* Free cryptarithm data */
			free(sums);
			free_word_letters();

			/* Reset parsing variables for next cryptarithm */
			letters_n = 0;
			word_letters_n = 0;
			word_len_max = 0;
			terms_idx = 0;
			word_len = 0;
			state = STATE_START;

			break;
		default:
			if (!isupper(symbol) || state == STATE_OPERATOR) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_word_letters();
				return EXIT_FAILURE;
			}
			if (!add_word_letter(symbol, terms_idx, word_len)) {
				free_word_letters();
				return EXIT_FAILURE;
			}
			if (state == STATE_WORD) {
				word_len++;
			}
			else {
				word_len = 1;
				state = STATE_WORD;
			}
		}
		symbol = fgetc(stdin);
	}
	return EXIT_SUCCESS;
}

int add_word_letter(int symbol, int terms_idx, int word_len) {
	int letters_idx, value_min;
	for (letters_idx = 0; letters_idx < letters_n && letters[letters_idx].symbol != symbol; letters_idx++);
	if (word_len == 0) {
		value_min = 1;
	}
	else {
		value_min = 0;
	}
	if (letters_idx == letters_n) {
		if (letters_n == LETTERS_MAX) {
			fprintf(stderr, "Too many letters\n");
			fflush(stderr);
			return 0;
		}
		set_letter(letters+letters_idx, symbol, value_min);
	}
	else {
		if (letters[letters_idx].value_min < value_min) {
			letters[letters_idx].value_min = value_min;
		}
	}
	if (word_letters_n == 0) {
		word_letters = malloc(sizeof(word_letter_t));
		if (!word_letters) {
			fprintf(stderr, "Could not allocate memory for word_letters\n");
			fflush(stderr);
			return 0;
		}
	}
	else {
		word_letter_t *word_letters_tmp = realloc(word_letters, sizeof(word_letter_t)*((size_t)word_letters_n+1));
		if (!word_letters_tmp) {
			fprintf(stderr, "Could not reallocate memory for word_letters\n");
			fflush(stderr);
			return 0;
		}
		word_letters = word_letters_tmp;
	}
	set_word_letter(word_letters+word_letters_n, letters+letters_idx, terms_idx);
	return 1;
}

void set_letter(letter_t *letter, int symbol, int value_min) {
	letter->symbol = symbol;
	letter->value_min = value_min;
	letter->value = LETTERS_MAX;
	letters_n++;
}

void set_word_letter(word_letter_t *word_letter, letter_t *letter, int terms_idx) {
	word_letter->letter = letter;
	word_letter->power = 0;
	word_letter->terms_idx = terms_idx;
	word_letters_n++;
}

void set_word_letters_power(int word_len) {
	int word_letters_idx, power;
	for (word_letters_idx = word_letters_n-2, power = 1; power < word_len; word_letters_idx--, power++) {
		word_letters[word_letters_idx].power = power;
	}
	if (word_len > word_len_max) {
		word_len_max = word_len;
	}
}

int compare_word_letters(const void *a, const void *b) {
const word_letter_t *word_letter_a = (const word_letter_t *)a, *word_letter_b = (const word_letter_t *)b;
	if (word_letter_a->power != word_letter_b->power) {
		return word_letter_a->power-word_letter_b->power;
	}
	if (word_letter_a->terms_idx != word_letter_b->terms_idx) {
		return word_letter_a->terms_idx-word_letter_b->terms_idx;
	}
	return word_letter_a->letter->symbol-word_letter_b->letter->symbol;
}

void cryptarithm(int power_last, int terms_idx_last, int word_letters_idx) {
	nodes_n++;
	if (word_letters_idx == word_letters_n) {
		if (check_terms(power_last, terms_idx_last)) {
			int letter_idx;
			printf("SOLUTION %d\n", ++solutions_n);
			for (letter_idx = 0; letter_idx < letters_n; letter_idx++) {
				printf("%c = %d\n", letters[letter_idx].symbol, letters[letter_idx].value);
			}
			fflush(stdout);
		}
	}
	else {
		if (word_letters[word_letters_idx].power > power_last) {
			int terms_idx;
			if (power_last > -1) {
				if (!check_terms(power_last, terms_idx_last)) {
					return;
				}
				for (terms_idx = 0; terms_idx < terms_n; terms_idx++) {
					sums[word_letters[word_letters_idx].power*terms_n+terms_idx] = sums[power_last*terms_n+terms_idx]/LETTERS_MAX;
				}
			}
			else {
				for (terms_idx = 0; terms_idx < terms_n; terms_idx++) {
					sums[word_letters[word_letters_idx].power*terms_n+terms_idx] = 0;
				}
			}
		}
		else {
			if (terms_idx_last > 0 && word_letters[word_letters_idx].terms_idx > terms_idx_last && !check_terms(word_letters[word_letters_idx].power, terms_idx_last)) {
				return;
			}
		}
		if (word_letters[word_letters_idx].letter->value == LETTERS_MAX) {

			/* Letter not set - Try all possible values */
			int value;
			for (value = word_letters[word_letters_idx].letter->value_min; value < LETTERS_MAX; value++) {
				if (!used_digits[value]) {
					used_digits[value] = 1;
					word_letters[word_letters_idx].letter->value = value;
					sums[word_letters[word_letters_idx].power*terms_n+word_letters[word_letters_idx].terms_idx] += value;
					cryptarithm(word_letters[word_letters_idx].power, word_letters[word_letters_idx].terms_idx, word_letters_idx+1);
					sums[word_letters[word_letters_idx].power*terms_n+word_letters[word_letters_idx].terms_idx] -= value;
					word_letters[word_letters_idx].letter->value = LETTERS_MAX;
					used_digits[value] = 0;
				}
			}
		}
		else {
			/* Letter already set */
			sums[word_letters[word_letters_idx].power*terms_n+word_letters[word_letters_idx].terms_idx] += word_letters[word_letters_idx].letter->value;
			cryptarithm(word_letters[word_letters_idx].power, word_letters[word_letters_idx].terms_idx, word_letters_idx+1);
			sums[word_letters[word_letters_idx].power*terms_n+word_letters[word_letters_idx].terms_idx] -= word_letters[word_letters_idx].letter->value;
		}
	}
}

int check_terms(int power, int terms_idx) {
	return sums[power*terms_n+terms_idx]%LETTERS_MAX == sums[power*terms_n+terms_idx-1]%LETTERS_MAX;
}

void free_word_letters(void) {
	if (word_letters_n > 0) {
		free(word_letters);
	}
}
