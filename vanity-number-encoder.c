#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MENU_ITEMS 2
#define NUMBER_LENGTH 10 //no null
#define MAX_LENGTH 15 //null included

const int keypad_mapping[26] = {
    2,2,2,
    3,3,3,
    4,4,4,
    5,5,5,
    6,6,6,
    7,7,7,7,
    8,8,8,
    9,9,9,9
};

void build_reverse_keypad();
void generate_combinations(const char *number, int number_index, int current_index, char *current, FILE *file_ptr);
int get_option(char* options, char selection);
void decode(const char* number);
void encode(const char* number);
typedef void (*option_func)(const char* number);
option_func menu[] = {encode,decode};

int main(int argc, char** args){
    char options[MENU_ITEMS] = {'1','2'};
    char* option_names[MENU_ITEMS] = {"Encode","Decode"};

    printf("=========================================================================\n");
    printf(" / .  .        ,     \\   .  .        .             .___           .      \n");
    printf("(  \\  / _.._ *-+-  .  )  |\\ |. .._ _ |_  _ ._. ___ [__ ._  _. _  _| _ ._.\n");
    printf(" \\  \\/ (_][ )| | \\_| /   | \\|(_|[ | )[_)(/,[       [___[ )(_.(_)(_](/,[  \n");
    printf("                 ._|                                                   \n");
    printf("=========================================================================\n");
 
    for(int i=0; i<MENU_ITEMS; i++)
        printf("[%c] %s\n", options[i], option_names[i]);//scalable for more features

    int selection_ID;
    char selection = '\0';
    do{
        if(selection=='\n') printf("\033[F\033[K");
        scanf("%c",&selection);
        if(selection=='\r') printf("\033[F");
    }while((selection_ID=get_option(options, selection))==-1);

    int current_digit = 0;
    char number[NUMBER_LENGTH];
    printf("Enter Phone#: \n\n"); //double newline to avoid deletion

    char buffer[MAX_LENGTH]="";
    while(strlen(buffer)<NUMBER_LENGTH){//ensuring number length
        printf("\033[F\033[K");
        scanf("%s",buffer);
    }

    for(int i=0; i<MAX_LENGTH-1; i++){
        if(buffer[i]>='a' && buffer[i]<='z') //lower to upper case
            buffer[i]-=32;
        if(buffer[i]>='0' && buffer[i]<='9' || buffer[i]>='A' && buffer[i]<='Z')
            number[current_digit++] = buffer[i];
    }

    menu[selection_ID](number);

    return 0;
}

int get_option(char* options, char selection){
    for(int i=0; i<MENU_ITEMS; i++)
        if(options[i]==selection)
            return i;
    return -1;
}

void decode(const char* number){
    for(int i=0; i<NUMBER_LENGTH; i++){
        if(i==3||i==6) printf("-");
        if(number[i]>'9'){ //if NaN
            printf("%d",keypad_mapping[number[i]-'A']); //minus uppercase offset
        } else{
            printf("%c",number[i]);
        }
    }
    return;
}

char reverse_keypad[10][5];

void build_reverse_keypad(){// reusing the keypad array
    for (int i = 0; i < 26; i++) {
        int digit = keypad_mapping[i];
        int len = strlen(reverse_keypad[digit]);
        reverse_keypad[digit][len] = 'A' + i;
        reverse_keypad[digit][len + 1] = '\0';
    }
}

int file_write = 0;
int add_dashes = 0;

void generate_combinations(const char *number, int number_index, int current_index, char *current, FILE *file_ptr){
    if (number[number_index] == '\0') {
        current[current_index] = '\0';

        if (file_write && file_ptr) {
            if (add_dashes)
                fprintf(file_ptr, "%.3s-%.3s-%.4s\n", current, current + 3, current + 6);
            else
                fprintf(file_ptr, "%s\n", current);
        } else {
            if (add_dashes)
                printf("%.3s-%.3s-%.4s\n", current, current + 3, current + 6);
            else
                printf("%s\n", current);
        }

        return;
    }

    int digit = number[number_index] - '0';

    if (digit < 2 || digit > 9 || number_index < 3) {//skipping alteration of 1, 0, and anything else
        current[current_index] = number[number_index];
        generate_combinations(number, number_index + 1, current_index + 1, current, file_ptr);
        return;
    }

    const char *letters = reverse_keypad[digit];

    for (int i = 0; letters[i] != '\0'; i++) {
        current[current_index] = letters[i];
        generate_combinations(number, number_index + 1, current_index + 1, current, file_ptr);
    }
}

void encode(const char *number){
    build_reverse_keypad();

    char current[NUMBER_LENGTH+1] = {0};

    char number_string[NUMBER_LENGTH+1];
    number_string[NUMBER_LENGTH]='\0';
    for(int i=0; i<NUMBER_LENGTH; i++)//converting char array to string
        number_string[i]=number[i];

    char response;
    FILE *file_ptr = NULL;

    printf("Export to file? [y] or [n]\n");
    scanf(" %c",&response);
    if(response=='y'||response=='Y'){
        file_ptr = fopen("phone-numbers.txt","w");
        file_write = 1;
    }

    printf("Add dashes? [y] or [n]\n");
    scanf(" %c",&response);
    if(response=='y'||response=='Y') add_dashes = 1;

    if(!file_ptr && file_write){
        printf("File couldn't be opened!\n");
        exit(EXIT_FAILURE);
    }
    
    generate_combinations(number_string, 0, 0, current, file_ptr);

    if(file_write) fclose(file_ptr);
    return;
}
