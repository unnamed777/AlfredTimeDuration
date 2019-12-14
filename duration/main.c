//
//  main.c
//  Time Calculator
//
//  Created by Tyler on 6/26/14.
//  Copyright (c) 2014 Tyler Eich. All rights reserved.
//
//  Modified by unnamed777 <andrey@neyman.tel>
//

#include <CoreFoundation/CoreFoundation.h>
#include <regex.h>

long timeStringToSeconds(char *time);
char *secondsToTimeString(long seconds);
void removeSpaces(char *source);
char *substring(char *string, int position, int length);

int main(int argc, const char *argv[])
{
    if (argc < 1) return 0;

    char *arg = strdup(argv[1]);
    int regexCode;
    removeSpaces(arg);

    regex_t validator;

    // Check that all input characters are valid
    regexCode = regcomp(&validator, "^[0-9:+-]*$", 0);

    if (regexCode) // Non-zero return value
     {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
     }

    regexCode = regexec(&validator, arg, 0, NULL, 0);
    bool noColon = (strchr(arg, ':') == NULL);

    if (regexCode || noColon) // No match
     {
        // Return empty results
        printf("<items></items>");
        return 0;
     }

    regfree(&validator);

    // Check that input is usable
    regexCode = regcomp(&validator, "^[-+]\\?[0-9]\\+\\(:[0-5][0-9]\\)\\{1,2\\}\\([-+][0-9]\\+\\(:[0-5][0-9]\\)\\{1,2\\}\\)*$", REG_ENHANCED | REG_NOSUB);

    if (regexCode) // Non-zero return value
     {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
     }

    regexCode = regexec(&validator, arg, 0, NULL, 0);

    if (regexCode) // No match
     {
        // Return 'Please enter a valid expression'
        printf("<items><item uid=\"tylereich.duration-calculator invalid\" valid=\"no\"><title>…</title><subtitle>Please enter a valid expression</subtitle></item></items>");
        return 0;
     }

    int arglen = (int)strlen(argv[1]);
    int pos = 0;

    long value = 0;
    char operand = '+';

    for (int i = 0; i <= arglen; ++i)
     {
        char character = arg[i];
        if (character == '+' || character == '-' || i == arglen)
         {
            char *substr = substring(arg, pos, i - pos);

            if (operand == '+')
             {
                value += timeStringToSeconds(substr);
             }
            else if (operand == '-')
             {
                value -= timeStringToSeconds(substr);
             }

            operand = arg[i];
            pos = i + 1;
         }
     }

    char *timeString = secondsToTimeString(value);
    printf("<items><item uid=\"tylereich.duration-calculator result\" arg=\"%s\"><title>%s</title><subtitle>Action this item to copy this duration to the clipboard</subtitle></item></items>", timeString, timeString);
}

long timeStringToSeconds(char *time)
{
    long seconds = 0;
    const char sep = ':';
    char *token;

    token = strtok(time, &sep);
    int i = 0;

    while (token != NULL) {
        long value = strtol(token, (char**)NULL, 10);

        if (i == 0)
         {
            seconds += value * 3600;
         }
        else if (i == 1)
         {
            seconds += value * 60;
         }
        else if (i == 2)
         {
            seconds += value;
         }

        i++;
        token = strtok(NULL, &sep);
    }

    return seconds;
}

char *secondsToTimeString(long seconds)
{
    char neg = false;
    if (seconds < 0)
     {
        seconds = -seconds;
        neg = true;
     }

    char *timeString = malloc(20);
    int h, m, s;

    h = floor(seconds / 3600);
    seconds -= h * 3600;
    m = floor(seconds / 60);
    seconds -= m * 60;
    s = (int)seconds;

    snprintf(timeString, 21,
             "%s%i:%.2i:%.2i", (neg ? "-" : ""), h, m, s);
    return timeString;
}

void removeSpaces(char *source)
{
    char* i = source;
    char* j = source;
    while(*j != 0)
     {
        *i = *j++;
        if(*i != ' ')
            i++;
     }
    *i = 0;
}

char *substring(char *string, int position, int length)
{
    char *pointer;
    int c;

    pointer = malloc(length+1);

    if (pointer == NULL)
     {
        printf("Unable to allocate memory.\n");
        exit(EXIT_FAILURE);
     }

    for (c = 0; c < position; c++)
        string++;

    for (c = 0; c < length; c++)
     {
        *(pointer+c) = *string;
        string++;
     }

    *(pointer+c) = '\0';
    
    return pointer;
}
