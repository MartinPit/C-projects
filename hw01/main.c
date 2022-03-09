#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_out();
int octal(int *, uint64_t *, bool);
uint64_t convert(uint64_t, int);
int hexa(int *, uint64_t *, bool);
int binary(int *, uint64_t *, bool);

uint64_t accumulator = 0;
uint64_t memory = 0;

void print_error_message(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void rotate(int *input, bool mode)
{
    *input = getchar();
    bool got_num = false;
    int move = 0;

    while (*input != EOF) {
        if (*input >= '0' && *input <= '9') {
            if (!got_num) {
                got_num = true;
            }
            move *= 10;
            move += *input - 48;
            *input = getchar();

        } else if (isspace(*input)) {
            *input = getchar();
        } else if (!got_num) {
            print_error_message("Syntax error");
        } else {
            break;
        }
    }
    if (mode) {
        accumulator = (accumulator << move) | (accumulator >> (64 - move));
    } else {
        accumulator = (accumulator >> move) | (accumulator << (64 - move));
    }

    print_out();
}

int assign()
{
    bool got_number = false;

    int input = getchar();

    while (input != EOF) {
        if (input >= '0' && input <= '9') {
            if (!got_number) {
                accumulator = 0;
                got_number = true;
            }
            accumulator = accumulator * 10 + input - 48;
            input = getchar();

        } else if (!got_number && input == 'O') {
            int success = octal(&input, &accumulator, true);
            if (success == 0) {
                got_number = true;
            }
            break;

        } else if (!got_number && input == 'T') {
            int success = binary(&input, &accumulator, true);
            if (success == 0) {
                got_number = true;
            }
            break;

        } else if (!got_number && input == 'X') {
            int success = hexa(&input, &accumulator, true);
            if (success == 0) {
                got_number = true;
            }
            break;

        } else if (!got_number && input == 'm') {
            accumulator = memory;
            got_number = true;
            input = getchar();
            break;

        } else if (isspace(input) != 0) {
            input = getchar();

        } else {
            break;
        }
    }
    if (!got_number) {
        print_error_message("Syntax error");
    }
    print_out();
    return input;
}

void print_out()
{
    printf("# %" PRIu64 "\n", accumulator);
}

int operation(int mode)
{
    uint64_t num = 0;
    bool got_number = false;

    int input = getchar();

    while (input != EOF) {
        if (input >= '0' && input <= '9') {
            if (!got_number) {
                got_number = true;
            }
            num *= 10;
            num += input - 48;
            input = getchar();

        } else if (!got_number && input == 'm') {
            num = memory;
            input = getchar();
            break;

        } else if (!got_number && input == 'O') {
            int success = octal(&input, &num, false);
            if (success == 1) {
                print_error_message("Syntax error");
            }
            break;

        } else if (!got_number && input == 'T') {
            int success = binary(&input, &num, false);
            if (success == 1) {
                print_error_message("Syntax error");
            }
            break;

        } else if (!got_number && input == 'X') {
            int success = hexa(&input, &num, false);
            if (success == 1) {
                print_error_message("Syntax error");
            }
            break;

        } else if (!got_number && input == 'l') {
            rotate(&input, true);
            break;

        } else if (!got_number && input == 'r') {
            rotate(&input, false);
            break;

        } else if (isspace(input) != 0) {
            input = getchar();
            continue;

        } else {
            if (!got_number) {
                print_error_message("Syntax error");
            }
            break;
        }
    }

    switch (mode) {
    case '+':
        if (accumulator + num < accumulator) {
            print_error_message("Out of range");
        }
        accumulator += num;
        break;

    case '-':
        if (num > accumulator) {
            print_error_message("Out of range");
        }
        accumulator -= num;
        break;

    case '*':
        if (accumulator * num < accumulator) {
            print_error_message("Out of range");
        }
        accumulator *= num;
        break;

    case '/':
        if (num == 0) {
            print_error_message("Division by zero");
        }
        accumulator /= num;
        break;

    case '%':
        if (num == 0) {
            print_error_message("Division by zero");
        }
        accumulator %= num;
        break;

    case '<':
        if (num > 63) {
            print_error_message("Out of range");
        }
        accumulator <<= num;
        break;

    case '>':
        if (num > 63) {
            print_error_message("Out of range");
        }
        accumulator >>= num;
        break;
    }
    print_out();
    return input;
}

int comment()
{
    int input = getchar();
    while (input != EOF && input != '\n') {
        input = getchar();
    }
    return input;
}

int octal(int *input, uint64_t *num, bool delete)
{
    printf("# %" PRIo64 "\n", accumulator);
    if (delete) {
        accumulator = 0;
    }
    *input = getchar();
    bool got_number = false;

    while (*input != EOF) {
        if (*input >= '0' && *input <= '7') {
            if (!got_number) {
                got_number = true;
            }
            *num = *num * 8;
            *num += *input - 48;
            *input = getchar();

        } else if (!got_number && *input == 'm') {
            *num = memory;
            *input = getchar();
            break;

        } else if (isspace(*input) != 0) {
            *input = getchar();
            continue;

        } else {
            break;
        }
    }

    if (!got_number) {
        return 1;
    }
    return 0;
}
void print_binary(uint64_t num)
{
    uint64_t temp = 0;
    bool check = false;

    printf("# ");

    for (int move = 63; move >= 0; move--) {
        temp = num >> move;

        if (temp & 1) {
            putchar('1');
            check = true;

        } else if (check) {
            putchar('0');
        }
    }
    if (!check) {
        putchar('0');
    }
    putchar('\n');
}

int binary(int *input, uint64_t *num, bool delete)
{
    print_binary(accumulator);
    if (delete) {
        accumulator = 0;
    }
    *input = getchar();
    bool got_number = false;

    while (*input != EOF) {
        if (*input == '0' || *input == '1') {
            if (!got_number) {
                got_number = true;
            }
            *num = *num * 2;
            *num += *input - 48;
            *input = getchar();

        } else if (!got_number && *input == 'm') {
            *num = memory;
            *input = getchar();
            break;

        } else if (isspace(*input) != 0) {
            *input = getchar();
            continue;

        } else {
            break;
        }
    }

    if (!got_number) {
        return 1;
    }
    return 0;
}

int hexa(int *input, uint64_t *num, bool delete)
{
    printf("# %" PRIX64 "\n", accumulator);
    if (delete) {
        accumulator = 0;
    }
    *input = getchar();
    bool got_number = false;

    while (*input != EOF) {
        if (*input >= '0' && *input <= '9') {
            if (!got_number) {
                got_number = true;
            }
            *num = *num * 16;
            *num += *input - 48;
            *input = getchar();

        } else if (toupper(*input) >= 'A' && toupper(*input) <= 'F') {
            if (!got_number) {
                got_number = true;
            }
            *num = *num * 16;
            *num += toupper(*input) - 55;
            *input = getchar();

        } else if (!got_number && *input == 'm') {
            *num = memory;
            *input = getchar();
            break;

        } else if (isspace(*input) != 0) {
            *input = getchar();
            continue;

        } else {
            break;
        }
    }

    if (!got_number) {
        return 1;
    }
    return 0;
}

bool calculate(void)
{
    int command = getchar();

    while (command != EOF) {
        if (command == 'P') {
            command = assign();
        }

        if (command == 'P') {
            continue;
        }

        if (command == EOF) {
            break;
        }

        switch (command) {
        case '=':
            print_out();
            command = getchar();
            break;

        case '\n':
            command = getchar();
            break;

        case ' ':
            command = getchar();
            break;

        case 'M':
            memory += accumulator;
            command = getchar();
            break;

        case 'R':
            memory = 0;
            command = getchar();
            break;

        case 'N':
            accumulator = 0;
            print_out();
            command = getchar();
            break;

        case ';':
            command = comment();
            break;

        case '+':
            command = operation(command);
            break;

        case '-':
            command = operation(command);
            break;

        case '*':
            command = operation(command);
            break;

        case '/':
            command = operation(command);
            break;

        case '%':
            command = operation(command);
            break;

        case '<':
            command = operation(command);
            break;

        case '>':
            command = operation(command);
            break;

        case 'O':
            printf("# %" PRIo64 "\n", accumulator);
            command = getchar();
            break;

        case 'T':
            print_binary(accumulator);
            command = getchar();
            break;

        case 'X':
            printf("# %" PRIX64 "\n", accumulator);
            command = getchar();
            break;

        case 'l':
            rotate(&command, true);
            break;

        case 'r':
            rotate(&command, false);
            break;

        default:
            print_error_message("Syntax error");
            break;
        }
    }
    return true;
}

int main(void)
{
    if (!calculate()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
