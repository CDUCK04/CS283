#include <string.h>
#include <stdio.h> 
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
 if (buff == NULL) {
        return -2; // Error: Buffer pointer is NULL
    }

    char *dest = buff;    // Pointer to the destination buffer
    char *src = user_str; // Pointer to the user-supplied string
    int char_count = 0;   // Tracks the number of characters copied
    int prev_was_space = 1; // Assume leading spaces initially

    // Process the user string
    while (*src != '\0') {
        if (char_count >= len) {
            return -1; // Error: Input string exceeds buffer size
        }

        if (*src == ' ' || *src == '\t') {
            // Skip leading or consecutive spaces
            if (!prev_was_space) { 
                *dest = ' '; // Add a single space
                dest++;
                char_count++;
                prev_was_space = 1;
            }
        } else {
            *dest = *src; // Copy non-whitespace character
            dest++;
            char_count++;
            prev_was_space = 0;
        }

        src++;
    }

    // Remove trailing space if present
    if (char_count > 0 && *(dest - 1) == ' ') {
        dest--;
        char_count--;
    }

    // Fill the remaining buffer with dots
    while (dest < buff + len) {
        *dest = '.';
        dest++;
    }

    return char_count;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    printf("]\n");
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
   int count = 0; // number of words
   int in_word = 0; // tracks if currently in a word

   if(str_len > len){
	   printf("error: Porvided string is too long");
	   exit(3);
   }

  for(int i = 0; i< str_len; i++){
	 if(*(buff + i) != ' '){
		if(!in_word) { // start a new word
		       count++;
	       		in_word=1;
	 	}
	}else {
		in_word = 0; // end of the current word
	}
	}

    return count; // return the count of words
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
void reverse_string(char * buff, int len, int str_len){
        char *start = buff;
        char *end = buff +str_len - 1;
        char temp;

      if(str_len > len){
        printf("error: Porvided string is too long");
        exit(3);
      }
	//swap characters
      while(start < end){
              temp = *start;
              *start = *end;
              *end = temp;

              start ++;
              end--;
        }
}

void print_words(char *buff, int len, int str_len) {
	if(str_len > len){
        printf("error: Porvided string is too long");
        exit(3);
        }
    printf("Word Print\n");
    printf("----------\n");

    int word_count = 0;  // Counter for word numbering
    int char_count = 0;  // Tracks characters in the current word
    int is_in_word = 0;  // Tracks whether inside a word

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ') {  // Non-space character check
            if (!is_in_word) {     // Start of a new word
                word_count++;
                printf("%d. ", word_count);
                is_in_word = 1;
            }
            putchar(*(buff + i));  // Print character
            char_count++;
        } else if (is_in_word) {  // End of a word
            printf("(%d)\n", char_count);
            char_count = 0;
            is_in_word = 0;
        }
    }

    // Handle the last word (if the string doesn't end with a space)
    if (is_in_word) {
        printf("(%d)\n", char_count);
    }

    printf("\nNumber of words returned: %d\n", word_count);
	
}
int main(int argc, char *argv[]){

    char *buff ;//placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    
    // This check ensures safety by verifying that at least one command-line argument
    // is provided beyond the program name (argc >= 2). Without this check, accessing
    // argv[1] when argc < 2 would lead to undefined behavior (e.g., segmentation fault).
    // It also validates that the first argument starts with a '-' to ensure a valid option.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    
    // This condition ensures that the user provides at least three arguments:
    // the program name (argv[0]), an option flag (argv[1]), and the user-supplied string (argv[2]).
    // If fewer arguments are provided, the program cannot proceed and exits gracefully
    // after showing the correct usage.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
	buff = (char *)malloc(BUFFER_SZ * sizeof(char));
	if(buff == NULL){
		printf("Memory allocation failed\n");
		exit(99);
	}

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
	
	case 'r':
	    reverse_string(buff,BUFFER_SZ, user_str_len);
	    break;
       
        case 'w':
	    print_words(buff,BUFFER_SZ, user_str_len);
	    break;
	   
	case 'x':
	    printf("Not Implemented!");
	    exit(1);
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?

// Providing both the pointer (buff) and the length (len) to the helper functions
// ensures safety, flexibility, and reusability. While the buffer size is fixed
// (BUFFER_SZ = 50) in this specific program, passing the length explicitly:
// 1. Prevents buffer overflows by ensuring operations stay within bounds.
// 2. Makes the functions adaptable to buffers of varying sizes, improving reusability.
// 3. Improves code readability by clarifying the range of data being processed.

//   ./stringfun -r "hello"
