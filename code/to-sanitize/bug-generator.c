#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void use_after_free() {
    int *p = malloc(sizeof(int));
    *p = 42;
    free(p);
    printf("Pointer was freed, now it contains: %d\n", *p);
}

void heap_buffer_overflow() {
    char *buffer = (char *)malloc(10);
    strcpy(buffer, "Too long for the heap buffer, will overflow!");
    free(buffer);
}

void stack_buffer_overflow() {
    char buffer[10];
    strcpy(buffer, "Too long for the stack buffer, will overflow!");
}

char global_buffer[10];
void global_buffer_overflow() {
    strcpy(global_buffer, "Too long for the global buffer, will overflow!");
}

int *use_after_return_helper() {
    int local_var = 42;
    return &local_var; // Returning address of local variable
}

void use_after_return() {
    int *p = use_after_return_helper();
    printf("Use after return: %d\n", *p); // Using the returned pointer
}

void use_after_scope() {
    int *p;
    {
        int local_var = 42;
        p = &local_var; // Pointer to local variable
    }
    printf("Use after scope: %d\n", *p);
}

int uninitialized_global;
void use_uninitialized() {
    printf("Uninitialized value: %d\n", uninitialized_global);
}

void memory_leak() {
    int *p = malloc(sizeof(int));
    *p = 42;
    // No free(p)??
}

int main() {
    char opt;

    while (1) {
        printf("\nSelect a bug to generate:\n");
        printf("1. Use after free\n");
        printf("2. Heap buffer overflow\n");
        printf("3. Stack buffer overflow\n");
        printf("4. Global buffer overflow\n");
        printf("5. Use after return\n");
        printf("6. Use after scope\n");
        printf("7. Use uninitialized\n");
        printf("8. 1000 memory leaks\n");
        printf("X. Exit\n");
        printf("Enter your choice: ");
        scanf(" %c", &opt);

        switch (opt) {
            case '1':
                use_after_free();
                break;
            case '2':
                heap_buffer_overflow();
                break;
            case '3':
                stack_buffer_overflow();
                break;
            case '4':
                global_buffer_overflow();
                break;
            case '5':
                use_after_return();
                break;
            case '6':
                use_after_scope();
                break;
            case '7':
                use_uninitialized();
                break;
            case '8':
                for (int i = 0; i < 1000; i++)
                    memory_leak();
                break;
            case 'X':
            case 'x':
                return 0;
            default:
                printf("Unknown option, do not trick me!\n");
        }
        printf("Done\n");
    }

    return 0;
}

