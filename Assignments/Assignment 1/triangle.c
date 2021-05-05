#include <stdlib.h>
#include <stdio.h>

void triangle(unsigned width)
{
    unsigned i, j;
    i = 1;
    for(i = 1; i <= width; i++) {
	    j = 0;
	    for (j = 0; j < i; j++) {
		   putchar('*');
	    }
	   putchar('\n');
    }
}

void v_triangle(unsigned width) {
	unsigned i, j;
	i = width;
	for (i = width; i >= 1; i--) {
		j = 0;
		for (j = 0; j < i; j++) {
			putchar('*');
		}
		putchar('\n');
	}
}

void h_triangle(unsigned width) {
	unsigned i, j, k;
	i = width;
	for (i = width; i >= 1; i--) {
		j = width;
	       	for (j = width; j > i; j--) {
			putchar(' ');
		}
		k = 1;
		for (k = 1; k <= i; k++) {
			putchar('*');
		}
		putchar('\n');
	}
}


int main(int argc, char *argv[]) {
	if (argc == 2) {
			int w = atoi(argv[1]);
			if (w <= 0) {
				printf("You need to input an int greater than 0 \n");
				return EXIT_FAILURE;
			} else {
			printf("You entered %d, here is triangle function: \n", w);
			triangle(w);
		}
	}

	if (argc == 3) {
		int w = atoi(argv[1]);
			if (w <= 0) {
				printf("You need to input an int greater than 0 \n");
				return EXIT_FAILURE;
			} else {
				if (*argv[2] == 'v') {
					printf("You entered %d v, here is v_triangle: \n", w);
					v_triangle(w);
			}
				if (*argv[2] == 'h') {
					printf("You entered %d h, here is h_triangle: \n", w);
					h_triangle(w);
			}	if (*argv[2] != 'v' || *argv[2] != 'h') {
			       		printf("You need to enter 'v' or 'h' for the second argument \n");
					return EXIT_FAILURE;
				}	
			
			}
		}		
	else {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;	

}
