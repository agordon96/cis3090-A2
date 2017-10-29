//Aaron Gordon 0884023
#include <ctype.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void populate_dicts(char *inputStr, char *inputDict, char *encDict) {
  int i;
  int j = 0;
  int randNum;
  
  inputDict[0] = '\0';
  encDict[0] = '\0';

  for(i = 0; i < strlen(inputStr); i++) {
    if(!isspace(inputStr[i]) && !strchr(inputDict, inputStr[i])) {
      inputDict[j] = inputStr[i];
      inputDict[j + 1] = '\0';
      encDict[j] = ']';
      j++;
    }
  }
  
  inputDict[j] = '\0';
  encDict[j] = '\0';
  
  srand(time(NULL));
  for(i = 0; i < j; i++) {
    do {
      randNum = rand() % j;
    } while(encDict[randNum] != ']' || (i == randNum && i != (j - 1)));
    encDict[randNum] = inputDict[i];
  }
  
  for(i = 0; i < strlen(inputStr); i++) {
    for(j = 0; j < strlen(inputDict); j++) {
      if(inputStr[i] == inputDict[j]) {
        inputStr[i] = encDict[j];
        break;
      }
    }
  }
}

int search_in_file(char *filename, char *str) {
	char line[512];
  FILE *file;

  file = fopen(filename, "r");
	if(!file) {
    return 0;
  }

	while(fgets(line, 512, file)) {
    line[strlen(line) - 1] = '\0';   
		if(strcmp(line, str) == 0) {
      fclose(file);
			return 1;
		}
	}

  fclose(file);
  return 0;
}

void permutate_string(char *str, char *sentDecDict, const char *encDict, int charsToSwap, char *filename) {
  char *token;
  char checkWords[256];
  char temp[256];
  char tempDecDict[256];
  char tempChar;
  int i, j, k;
  int index;
  int isWord;
  int len;
  
  if(strlen(str) < 3) {
    if(search_in_file(filename, str)) {
      printf("\"%s\" contains only words.\n", str);
    }
  }

  strcpy(tempDecDict, sentDecDict);
  index = strlen(encDict) - charsToSwap;
  
  for(i = index; i < strlen(encDict); i++) {
    isWord = 1;
    strcpy(checkWords, "");
    strcpy(tempDecDict, sentDecDict);
    
    tempChar = tempDecDict[index];
    tempDecDict[index] = tempDecDict[i];
    tempDecDict[i] = tempChar;
    
    if(charsToSwap > 2) {
      permutate_string(str, tempDecDict, encDict, charsToSwap - 1, filename);
    } else {
      for(j = 0; j < strlen(str); j++) {
        for(k = 0; k < strlen(tempDecDict); k++) {
          len = strlen(checkWords);
          if(str[j] == tempDecDict[k]) {
            checkWords[len] = encDict[k];
            checkWords[len + 1] = '\0';
            break;
          } else if(isspace(str[j])) {
            checkWords[len] = str[j];
            checkWords[len + 1] = '\0';
            break;
          }
        }
      }
      
      strcpy(temp, checkWords);
      token = strtok(temp, " ");

      while(token) {
        if(!search_in_file(filename, token)) {
          isWord = 0;
        }

        token = strtok(NULL, " ");
      }

      if(!isWord) {
        continue;
      }
      
      printf("\"%s\" contains only words.\n", checkWords);
    }
  }
}

int main() {
  char inputStr[256];
  char inputDict[256];
  char encDict[256];
  char sendDicts[3][256];
  char *filename = "/usr/share/dict/words";
  char *token;
  char tempChar;
  int numProcs;
  int currProc;
  int i;
  
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &currProc);
  
  while(1) {
    if(currProc == 0) {
      fgets(inputStr, 256, stdin);
      if(strcmp(inputStr, "\n") == 0) {
        continue;
      }
      
      token = strtok(inputStr, "\n");  
      strcpy(inputStr, token);
      
      if(strcmp(inputStr, "q") == 0) {
        for(i = 1; i < numProcs; i++) {
          strcpy(sendDicts[0], "exit mpi");
          MPI_Send(sendDicts, 768, MPI_CHAR, i, 0, MPI_COMM_WORLD); 
        }
        
        break;
      }

      populate_dicts(inputStr, inputDict, encDict);
      if(strlen(inputDict) > numProcs) {
        printf("\nString length is beyond number of processes specified. Please input a smaller string.\n");
        printf("You can restart the program and put in a larger number for processes if you wish to use the string.\n\n");
        continue;
      }
      
      strcpy(sendDicts[0], inputStr);
      strcpy(sendDicts[1], inputDict);
      strcpy(sendDicts[2], encDict);
      
      for(i = 1; i < strlen(inputDict); i++) {
        MPI_Send(sendDicts, 768, MPI_CHAR, i, 0, MPI_COMM_WORLD); 
      }
      
      strcpy(sendDicts[1], sendDicts[2]);
      permutate_string(sendDicts[0], sendDicts[1], sendDicts[2], strlen(sendDicts[1]) - 1, filename);
      printf("Process %d of %d completed.\n", currProc, (int)(strlen(sendDicts[1]) - 1));
    } else {
      MPI_Recv(sendDicts, 768, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if(strcmp(sendDicts[0], "exit mpi") == 0) {
        break;
      }
      
      strcpy(sendDicts[1], sendDicts[2]);
      tempChar = sendDicts[1][0];
      sendDicts[1][0] = sendDicts[1][currProc];
      sendDicts[1][currProc] = tempChar;
      
      permutate_string(sendDicts[0], sendDicts[1], sendDicts[2], strlen(sendDicts[1]) - 1, filename);
      
      printf("Process %d of %d completed.\n", currProc, (int)(strlen(sendDicts[1]) - 1));
    }
  }
  
  MPI_Finalize(); 
  return 0;
}