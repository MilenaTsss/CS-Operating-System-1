#include <stdbool.h>

bool is_digit(char symbol) {
    return symbol >= '0' && symbol <= '9';
}

bool is_letter(char symbol) {
    return symbol >= 'a' && symbol <= 'z' || symbol >= 'A' && symbol <= 'Z';
}

bool is_separator(char symbol) {
    return !is_letter(symbol) && !is_digit(symbol);
}

bool is_not_separator(char symbol) {
    return is_letter(symbol) || is_digit(symbol);
}


void count(char *str, int *count_num) {
    if (*str == 0) {
        return;
    }

    if (is_separator(*str)) {
        str++;
    } else {
        if (is_letter(*str)) {
            count_num[0]++;
        }
        count_num[1]++;
        str++;
    }

    while (*str != 0) {
        if (is_not_separator(*str)) {
	    //printf("%c", *str);
            if (is_separator(*(str - 1))) {
                if (is_letter(*str)) {
                    count_num[0]++;
		    //printf("%c", *str);
                }
		//printf("%c", *str);
                count_num[1]++;
            }
        }
        str++;
    }
}
