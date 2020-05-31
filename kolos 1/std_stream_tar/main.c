#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>


int main(int argc, char* argv[]) {
	int pdesk[2];
	if (pipe(pdesk) == -1){
		perror("PIPE");
		return 1;
	}

	switch(fork()){
	  case -1:
		perror("FORK");
		return 2;
	  case 0:
		dup2(pdesk[1], STDOUT_FILENO);
		execvp("ls", argv);
		perror("EXECVP ls");
		exit(1);
	  default: {
   /* Zamknij deskryptor do zapisu,
    * skopiuj deskyptor końca do odczytu tego potoku na deskryptor wejścia standardowego,
    * wykonaj  tr "a-z" "A-Z", w przypadku błędu  obsłuż go i wyjdź, zwracając 3.
   */
  		close(pdesk[1]);
		dup2(pdesk[0], STDIN_FILENO);
		int error = execlp("tr", "tr", "a-z", "A-Z", NULL);
		if (error == -1){
			printf("error: %s\n", strerror(errno));
		}

   /* koniec */
       	 }
	}
   return 0;
   }
