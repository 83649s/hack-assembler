#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char* name; 
    int location;
}symbolTableEntry; 

// Takes a provided integer and returns its binary representation in the format
// of an array of the binary values
char* binaryEncode(int n, int bitLength) {
    char* binaryNum = malloc(sizeof(char) * bitLength + 1); char* flippedBinaryNum = malloc(sizeof(char) * bitLength + 1);
    
    int initialArraySize = 1;
    for (int i = 0; n > 0; ++i) {
        binaryNum[i] = (char) (n % 2) + '0';
        n = n / 2;
        initialArraySize = i;
    }
    ++initialArraySize;
    for (int i = initialArraySize; i < bitLength; ++i) {
        binaryNum[i] = '0';
    }

    for (int i = 0; i < bitLength; ++i) {
        memcpy(&flippedBinaryNum[i], &binaryNum[(bitLength - 1) - i], sizeof(char));
    }
    free(binaryNum);
    return flippedBinaryNum;
}

// Returns the file name without the .asm extension
// while appending the provided suffix appended to the end
char* formatFileName(char* fileName, char* suffix) {
    int nameSize = 0;
    for (int i = 0; fileName[i] != '\0'; ++i) {
        nameSize = i; 
    }
    ++nameSize;
    int suffixSize = sizeof(&suffix) / sizeof(char);

    char* formattedFileName = malloc(sizeof(char) * nameSize +
                                     sizeof(char) * suffixSize);

    for (int i = 0; i < nameSize - 4; ++i) {
        formattedFileName[i] = fileName[i];
    }

    for (int i = 0; i < suffixSize; ++i) {
        formattedFileName[(nameSize - 4) + i] = suffix[i];
    }
    return formattedFileName;
}


char* aInstruction(char* instruction) {
   char* aInstruction = malloc(sizeof(char) * 17);
   aInstruction[0] = '0';
   char binaryValue[16];
   for (int i = 0; i < 14; ++i) {
       binaryValue[i] = instruction[i + 1];
   }
   char* remainingInstruction = binaryEncode(atoi(binaryValue), 15);
   for (int i = 0; i < 15; ++i) {
       aInstruction[i + 1] = remainingInstruction[i];
   }
   return aInstruction;
}

char* cInstruction(char* instruction) {
    char* cInstruction = malloc(sizeof(char) * 17);
    int instructionLength = 0;
    for (int i = 0; instruction[i] != '\0'; ++i) {
        if (instruction[i] != ' ') {
            instructionLength = i; 
        }
    }
    for (int i = 0; i < 3; ++i) {
        cInstruction[i] = '1';
    }

    int hasDest = 0;
    int destEndIndex = 0;
    char* dest = malloc(sizeof(char) * 4);

    int hasJump = 0;
    int jumpStartIndex = instructionLength;
    char* jump = malloc(sizeof(char) * 4);

    char* comp = malloc(sizeof(char) * 4);

    for (int i = 0; i < instructionLength; ++i) {
        if (instruction[i] == '=') { hasDest = 1; destEndIndex = i + 1;
        }
        else if (instruction[i] == ';') {
            hasJump = 1;
            jumpStartIndex = i;
        }
    }
    int compIncludesM = 0;
    int compPos = 0;
    for (int i = destEndIndex; i < jumpStartIndex; ++i) {
        comp[compPos] = instruction[i];
        ++compPos;
        if (instruction[i] == 'M') {
            compIncludesM = 1;
        }
    }

    char* compValue = malloc(sizeof(char) * 7);
    char compBinaryTable[18][7] = {"101010", "111111", "111010", "001100", "110000",
                                   "001101", "110001", "001111", "110011", "011111",
                                   "110111", "001110", "110010", "000010", "010011",
                                   "000111", "000000", "010101"}; 
    char compTable[18][4] = {"0", "1", "-1", "D", "A", "!D", "!A", "-D", "-A", "D+1",
                             "A+1", "D-1", "A-1", "D+A", "D-A", "A-D", "D&A", "D|A"};
    char compTableA[10][4] = {"M", "!M", "-M", "M+1", "M-1", "D+M", "D-M", "M-D", "D&M",
                              "D|M"};
    int compTableAIndexes[10] = {4, 6, 8, 10, 12, 13, 14, 15, 16, 17};

    if (compIncludesM) {    
        for (int i = 0; i < 10; ++i) {
            if (strcmp(comp, compTableA[i]) == 0) {
                strcpy(compValue, compBinaryTable[compTableAIndexes[i]]);
                break;
            }
        }
    }
    else {
        for (int i = 0; i < 18; ++i) {
            if (strcmp(comp, compTable[i]) == 0) {
                strcpy(compValue, compBinaryTable[i]);
                break;
            }
        }
    }

    char* jumpValue = malloc(sizeof(char) * 4);
    if (hasJump) {
        int jumpPos = 0;
        for (int i = jumpStartIndex + 1; i < instructionLength; ++i) {
            jump[jumpPos] = instruction[i];
            ++jumpPos;
        }
        
        char jumpBinaryTable[7][4] = {"001", "010", "011", "100", "101", "110", "111"};
        char jumpTable[7][4] = {"JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};

        for (int i = 0; i < 7; ++i) {
            if (strcmp(jump, jumpTable[i]) == 0) {
                strcpy(jumpValue, jumpBinaryTable[i]);
                break;
            }
        }
    }
    else {
        for (int i = 0; i < 3; ++i) {
            jumpValue[i] = '0';
        }
    }

    char* destValue = malloc(sizeof(char) * 4);
    if (hasDest) {
        for (int i = 0; i < destEndIndex - 1; ++i) {
            dest[i] = instruction[i];
        }

        char destBinaryTable[7][4] = {"001", "010", "011", "100", "101", "110", "111"};
        char destTable[7][4] = {"M", "D", "MD", "A", "AM", "AD", "AMD"};

        for (int i = 0; i < 7; ++i) {
            if (strcmp(dest, destTable[i]) == 0) {
                strcpy(destValue, destBinaryTable[i]);
                break;
            }
        }
    }
    else {
        for (int i = 0; i < 3; ++i) {
            destValue[i] = '0';
        }
    }

    if (compIncludesM) {
        cInstruction[3] = '1';
    }
    else {
        cInstruction[3] = '0';
    }

    for (int i = 0; i < 6; ++i) {
        memcpy(&cInstruction[i + 4], &compValue[i], sizeof(char));
    }

    for (int i = 0; i < 3; ++i) {
        memcpy(&cInstruction[i + 10], &destValue[i], sizeof(char));
    }

    for (int i = 0; i < 3; ++i) {
        memcpy(&cInstruction[i + 13], &jumpValue[i], sizeof(char));
    }

    free(destValue);
    free(jumpValue);
    free(compValue);
    free(dest);
    free(jump);
    free(comp);

    return cInstruction;
}

// Parses the given input file and dumps the contents without whitespaces
// and comments into the temp file
void removeWhitespaceAndComments(FILE* inFile, FILE* tempFile) {
    char c;
    char lastC;
    int isComment = 0;
    int isFullLineComment = 0;
    c = fgetc(inFile);
    while (c != EOF) {
        if (lastC == '\n' && isFullLineComment) {
            isFullLineComment = 0;
        }
        if (c == '/' && lastC == '\n') {
            isFullLineComment = 1;
        }

        if (c == '/') {
           isComment = 1; 
        }

        if (isComment && c == '\n') {
            isComment = 0;
        }

        if (c != ' ' && !(c == '\n' && lastC == '\n') && !isComment && !isFullLineComment) {
            fputc(c, tempFile);
        }
        
        lastC = c; 
        c = fgetc(inFile);
    }
    rewind(tempFile);
}

// Adds predefined symbols to the symbol table
symbolTableEntry* assignPredefinedSymbols(symbolTableEntry *symbolTable) {
    for (int i = 0; i < 5; ++i) {
        symbolTable[i].location = i;
    }
    symbolTable[0].name = "SP";
    symbolTable[1].name = "LCL";
    symbolTable[2].name = "ARG";
    symbolTable[3].name = "THIS";
    symbolTable[4].name = "THAT";

    for (int i = 0; i < 16; ++i) {
        char* formattedRegisterName = malloc(sizeof(char) * 4);
        sprintf(formattedRegisterName, "R%i", i);
        symbolTable[5 + i].name = formattedRegisterName;
        symbolTable[5 + i].location = i;
    }

    symbolTable[21].location = 16384;
    symbolTable[22].location = 24576;

    symbolTable[21].name = "SCREEN";
    symbolTable[22].name = "KBD";

    return symbolTable;
}

// Returns 1 in the case of the string including only numerical digits 0 if not
int isDigit(char* string) {
    int isDigit;
    int allCharsAreDigits = 1;
    // Generates an array of every single digit
    char digitArray[11];
    for (int i = 0; i < 10; ++i) {
        digitArray[i] = (char) i + '0';
    }

    for (int i = 0; string[i] != '\0'; ++i) {
        isDigit = 0;
        for (int j = 0; j < 10; ++j) {
            if (digitArray[j] == string[i]) {
                isDigit = 1;
            }
        }
        if (!isDigit) {
            allCharsAreDigits = 0;
            break;
        }
    }
    return allCharsAreDigits;
}

// Adds defined variables to the symbol table and writes them to the second temp file
symbolTableEntry* parseForSymbols(FILE* tempFile, FILE* secondTempFile, int *freeMemoryLocation, int *symbolSize, symbolTableEntry *symbolTable) {
    char line[17];
    int lineNumber = 0;
    while (fgets(line, sizeof(line), tempFile)) {
        if (line[0] == '(') {
            char *name = malloc(sizeof(char) * 32);
            int nameLength = 1;
            for (int i = 0; line[i] != ')'; ++i) {
               nameLength = i; 
            }
            int namePos = 0;
            for (int i = 1; i < nameLength + 1; ++i) {
                name[namePos] = line[i];
                ++namePos;
            }
            name[namePos] = 0;

            symbolTable = realloc(symbolTable, (sizeof(symbolTableEntry) * (*symbolSize + 1)));
            symbolTable[*symbolSize].name = name;
            symbolTable[*symbolSize].location = lineNumber;
            ++*symbolSize;
        }
        else {
            ++lineNumber;
        }
    }
    
    rewind(tempFile);

    while (fgets(line, sizeof(line), tempFile)) {
        char formattedLine[31];
        for (int i = 0; line[i] != '\0'; ++i) {
            formattedLine[i] = line[i + 1];
        }

        if (line[0] == '@' && !isDigit(formattedLine)) {
            char replacementName[32];
            char *nameWithoutNewline = malloc(sizeof(char));
            for (int i = 0; formattedLine[i] != '\0'; ++i) {
                if (formattedLine[i] != '\n') {
                    nameWithoutNewline[i] = formattedLine[i];
                }
            }

            int symbolFoundInTable = 0;
            for (int i = 0; i < *symbolSize; ++i) {
                if (strcmp(symbolTable[i].name, nameWithoutNewline) == 0) {
                    sprintf(replacementName, "%i", symbolTable[i].location); 
                    symbolFoundInTable = 1;
                }
            }

            if (!symbolFoundInTable) {
                symbolTable = realloc(symbolTable, (sizeof(symbolTableEntry) * (*symbolSize + 1)));
                symbolTable[*symbolSize].name = nameWithoutNewline;
                symbolTable[*symbolSize].location = *freeMemoryLocation;
                sprintf(replacementName, "%i", *freeMemoryLocation);
                ++*symbolSize;
                ++*freeMemoryLocation;
            }
            
            fprintf(secondTempFile, "@%s\n", replacementName);
        }
        else if (line[0] != '(') {
            fprintf(secondTempFile, "%s", line);
        }
    }

    rewind(tempFile);
    rewind(secondTempFile);
    return symbolTable;
}


int main(int argc, char** argv) {
    const char* fileName = argv[1];

    FILE* inFile = fopen(fileName, "r");
    FILE* outFile = fopen(formatFileName(argv[1], ".hack"), "w");
    FILE* tempFile = fopen("temp.asm", "w+");
    FILE* secondTempFile = fopen("temp2.asm", "w+");

    symbolTableEntry *symbolTable = malloc(sizeof(*symbolTable) * 23);

    int freeMemoryLocation = 16;
    int symbolSize = 23;

    // Intializes the symbol table
    symbolTable = assignPredefinedSymbols(symbolTable);

    // Removes whitespace and comments then puts the result in tempFile
    removeWhitespaceAndComments(inFile, tempFile);
    parseForSymbols(tempFile, secondTempFile, &freeMemoryLocation, &symbolSize, symbolTable);

    char line[17];

    while (fgets(line, sizeof(line), secondTempFile)) {
        if (line[0] == '@') {
            fprintf(outFile, "%s\n", aInstruction(line));
        }
        else { 
            fprintf(outFile, "%s\n", cInstruction(line));
        }
    }

    fclose(tempFile);
    fclose(secondTempFile);
    fclose(inFile);
    fclose(outFile);
    remove("temp.asm");
    remove("temp2.asm");
    return 0;
}

