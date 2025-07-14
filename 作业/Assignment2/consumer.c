// consumer.c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int num, sum_odd = 0;  // Variables to hold the number and sum of odd numbers
    printf("Even numbers: ");  // Print the header for even numbers

    // Read numbers from stdin until EOF
    while (scanf("%d", &num) != EOF) {
        if (num % 2 == 0) {  // Check if the number is even
            printf("%d ", num);  // Print the even number
        } else {
            sum_odd += num;  // Add the odd number to the sum
        }
    }

    // Print the sum of odd numbers
    printf("\nSum of odd numbers: %d\n", sum_odd);
    return 0;
}