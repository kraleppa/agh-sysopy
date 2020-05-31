#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
  DIR * katalog;
  if (argc!=2)
  {
    printf ("Wywołanie %s ścieżka",argv[0]);
    return 1;
  }
  struct dirent *pozycja;

/*Otwórz katalog, w przypadku błędu otwarcia zwróć błąd funkcji otwierającej i zwrócć 1. 
Wyświetl zawartość katalogu katalog, pomijając "." i ".."
Jeśli podczas tej operacji wartość errno zostanie ustawiona, zwróć błąd funkcji czytającej oraz wartość 2. */
  katalog = opendir(argv[1]);
  if (katalog == NULL){
    printf("error: %s\n", strerror(errno));
    return 1;
  }

  while(1){
    pozycja = readdir(katalog);
    if (pozycja == NULL){
      if (strcmp(strerror(errno), "Success") == 0){
        printf("ok");
        return 0;
      }
      printf("error: %s\n", strerror(errno));
      return 2;
    }

    if (strcmp(pozycja -> d_name, ".") == 0 || strcmp(pozycja -> d_name, "..") == 0){
      continue;
    }
    printf("%s\n", pozycja -> d_name);
  }
    
  

  









  /*koniec*/
  closedir(katalog);
  return (0);
}
