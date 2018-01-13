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
	int count;
}
group_t;

typedef enum {
	STATE_START,
	STATE_WORD,
	STATE_SPACE,
	STATE_OPERATOR
}
state_t;

int add_group(int, int, int);
void set_letter(letter_t *, int, int);
void set_group(group_t *, letter_t *, int);
void set_groups_power(int);
int compare_groups(const void *, const void *);
void cryptarithm(int, int, int);
int check_remaining_terms(int, int, int);
int check_terms(int, int);
void free_groups(void);

int letters_n, groups_n1, word_len_max, terms_n, *sums, groups_n2, used_digits[LETTERS_MAX], nodes_n, solutions_n;
letter_t letters[LETTERS_MAX];
group_t *groups;

int main(void) {
	int terms_idx, word_len, symbol;
	state_t state;

	/* Reset parsing variables for first cryptarithm */
	letters_n = 0;
	groups_n1 = 0;
	word_len_max = 0;
	terms_idx = 0;
	word_len = 0;
	state = STATE_START;

	/* Parse input */
	symbol = fgetc(stdin);
	while (!feof(stdin)) {
		int groups_idx, digit;
		switch (symbol) {
		case ' ':
			if (state == STATE_START || state == STATE_SPACE) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}
			if (state == STATE_WORD) {
				set_groups_power(word_len);
			}
			state = STATE_SPACE;
			break;
		case '+':
			if (state != STATE_SPACE) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}
			state = STATE_OPERATOR;
			break;
		case '=':
			if (state != STATE_SPACE) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}
			terms_idx++;
			state = STATE_OPERATOR;
			break;
		case '\n':
			if (state != STATE_WORD) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}
			if (state == STATE_WORD) {
				set_groups_power(word_len);
			}
			terms_n = terms_idx+1;
			if (terms_n < 2) {
				fprintf(stderr, "Invalid cryptarithm\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}

			/* Parsing complete for one cryptarithm */
			sums = malloc(sizeof(int)*(size_t)(word_len_max*terms_n));
			if (!sums) {
				fprintf(stderr, "Could not allocate memory for sums\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}

			/* Sort input by ascending power/term index/symbol */
			qsort(groups, (size_t)groups_n1, sizeof(group_t), compare_groups);

			/* Group input by power/term index/symbol */
			groups_n2 = 0;
			for (groups_idx = 1; groups_idx < groups_n1; groups_idx++) {
				if (compare_groups(groups+groups_idx, groups+groups_n2)) {
					groups_n2++;
					if (groups_n2 < groups_idx) {
						groups[groups_n2] = groups[groups_idx];
					}
				}
				else {
					groups[groups_n2].count++;
				}
			}
			groups_n2++;

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
			free_groups();

			/* Reset parsing variables for next cryptarithm */
			letters_n = 0;
			groups_n1 = 0;
			word_len_max = 0;
			terms_idx = 0;
			word_len = 0;
			state = STATE_START;

			break;
		default:
			if (!isupper(symbol) || state == STATE_OPERATOR) {
				fprintf(stderr, "Invalid input\n");
				fflush(stderr);
				free_groups();
				return EXIT_FAILURE;
			}
			if (state == STATE_WORD) {
				word_len++;
			}
			else {
				word_len = 1;
				state = STATE_WORD;
			}
			if (!add_group(symbol, terms_idx, word_len)) {
				free_groups();
				return EXIT_FAILURE;
			}
		}
		symbol = fgetc(stdin);
	}
	return EXIT_SUCCESS;
}

int add_group(int symbol, int terms_idx, int word_len) {
	int letters_idx, value_min;
	for (letters_idx = 0; letters_idx < letters_n && letters[letters_idx].symbol != symbol; letters_idx++);
	if (word_len == 1) {
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
	if (groups_n1 == 0) {
		groups = malloc(sizeof(group_t));
		if (!groups) {
			fprintf(stderr, "Could not allocate memory for groups\n");
			fflush(stderr);
			return 0;
		}
	}
	else {
		group_t *groups_tmp = realloc(groups, sizeof(group_t)*((size_t)groups_n1+1));
		if (!groups_tmp) {
			fprintf(stderr, "Could not reallocate memory for groups\n");
			fflush(stderr);
			return 0;
		}
		groups = groups_tmp;
	}
	set_group(groups+groups_n1, letters+letters_idx, terms_idx);
	return 1;
}

void set_letter(letter_t *letter, int symbol, int value_min) {
	letter->symbol = symbol;
	letter->value_min = value_min;
	letter->value = LETTERS_MAX;
	letters_n++;
}

void set_group(group_t *group, letter_t *letter, int terms_idx) {
	group->letter = letter;
	group->power = 0;
	group->terms_idx = terms_idx;
	group->count = 1;
	groups_n1++;
}

void set_groups_power(int word_len) {
	int groups_idx, power;
	for (groups_idx = groups_n1-2, power = 1; power < word_len; groups_idx--, power++) {
		groups[groups_idx].power = power;
	}
	if (word_len > word_len_max) {
		word_len_max = word_len;
	}
}

int compare_groups(const void *a, const void *b) {
const group_t *group_a = (const group_t *)a, *group_b = (const group_t *)b;
	if (group_a->power != group_b->power) {
		return group_a->power-group_b->power;
	}
	if (group_a->terms_idx != group_b->terms_idx) {
		return group_a->terms_idx-group_b->terms_idx;
	}
	return group_a->letter->symbol-group_b->letter->symbol;
}

void cryptarithm(int power_last, int terms_idx_last, int groups_idx) {
	nodes_n++;
	if (groups_idx == groups_n2) {
		if (check_remaining_terms(power_last, terms_idx_last, terms_n)) {
			int letter_idx;
			printf("SOLUTION %d\n", ++solutions_n);
			for (letter_idx = 0; letter_idx < letters_n; letter_idx++) {
				printf("%c = %d\n", letters[letter_idx].symbol, letters[letter_idx].value);
			}
			fflush(stdout);
		}
	}
	else {
		if (groups[groups_idx].power > power_last) {
			int terms_idx;
			if (power_last > -1) {
				if (!check_remaining_terms(power_last, terms_idx_last, terms_n)) {
					return;
				}
				for (terms_idx = 0; terms_idx < terms_n; terms_idx++) {
					sums[groups[groups_idx].power*terms_n+terms_idx] = sums[power_last*terms_n+terms_idx]/LETTERS_MAX;
				}
			}
			else {
				for (terms_idx = 0; terms_idx < terms_n; terms_idx++) {
					sums[groups[groups_idx].power*terms_n+terms_idx] = 0;
				}
			}
		}
		else {
			if (!check_remaining_terms(groups[groups_idx].power, terms_idx_last, groups[groups_idx].terms_idx)) {
				return;
			}
		}
		if (groups[groups_idx].letter->value == LETTERS_MAX) {

			/* Letter not set - Try all possible values */
			int value;
			for (value = groups[groups_idx].letter->value_min; value < LETTERS_MAX; value++) {
				if (!used_digits[value]) {
					used_digits[value] = 1;
					groups[groups_idx].letter->value = value;
					sums[groups[groups_idx].power*terms_n+groups[groups_idx].terms_idx] += value*groups[groups_idx].count;
					cryptarithm(groups[groups_idx].power, groups[groups_idx].terms_idx, groups_idx+1);
					sums[groups[groups_idx].power*terms_n+groups[groups_idx].terms_idx] -= value*groups[groups_idx].count;
					groups[groups_idx].letter->value = LETTERS_MAX;
					used_digits[value] = 0;
				}
			}
		}
		else {

			/* Letter already set */
			sums[groups[groups_idx].power*terms_n+groups[groups_idx].terms_idx] += groups[groups_idx].letter->value*groups[groups_idx].count;
			cryptarithm(groups[groups_idx].power, groups[groups_idx].terms_idx, groups_idx+1);
			sums[groups[groups_idx].power*terms_n+groups[groups_idx].terms_idx] -= groups[groups_idx].letter->value*groups[groups_idx].count;
		}
	}
}

int check_remaining_terms(int power, int terms_idx_last, int terms_max) {
int terms_idx;
	if (terms_idx_last == 0) {
		terms_idx_last++;
	}
	for (terms_idx = terms_idx_last; terms_idx < terms_max && check_terms(power, terms_idx); terms_idx++);
	return terms_idx >= terms_max;
}

int check_terms(int power, int terms_idx) {
	if (power == word_len_max-1) {
		return sums[power*terms_n+terms_idx] == sums[power*terms_n+terms_idx-1];
	}
	else {
		return sums[power*terms_n+terms_idx]%LETTERS_MAX == sums[power*terms_n+terms_idx-1]%LETTERS_MAX;
	}
}

void free_groups(void) {
	if (groups_n1 > 0) {
		free(groups);
	}
}
