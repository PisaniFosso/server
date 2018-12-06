// client_exe.c 
// Mode d'emploi : 
// c:\....\client_exe (addresse IP du serveur) (fichier executable) (fichier resulat)
 
/*----- Include files */
#include <stdio.h>          
#include <string.h>        
#include <sys/types.h>    
#include <netinet/in.h>   
#include <sys/socket.h>   
#include <arpa/inet.h>    
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

using namespace std;
#define  PORT_NUM         1567    
#define  IP_ADDR "139.103.64.13"  
#define  MAX_LISTEN       5    
#define  SIZE                256     

// Programme principal 
int main(int argc, char *argv[])
{             
  unsigned int         client_s;        
  struct sockaddr_in   client_addr;     
  struct sockaddr_in   server_addr;     
  unsigned char        bin_buf[SIZE];   
  //unsigned   int       fh;              
  FILE              *fh;
  unsigned int         length;          
  struct hostent       *host;           
  struct in_addr       address;         
  char                 host_name[256];  
  unsigned int         addr_len;        
  unsigned int         local_s;         
  struct sockaddr_in   local_addr;      
  struct in_addr       client_ip_addr;  

  if (argc !=4)
  {
     printf("  ERREUR     - il faut 4 argument : '(client_exe) (hostname) (fichier_executable) (non_fchier_resultats)'  \n");
     exit(1);
  }

  // Copie host name dans host_name
  strcpy(host_name, argv[1]);

  // Appel de la fonction gethostbyname()
  host = gethostbyname(argv[1]);
  if (host == NULL)
  {
    printf("  *** ERREUR - l'adresse IP '%s' non trouvee \n", host_name);
    exit(1);
  }
  
  memcpy(&address, host->h_addr, 4);
  client_s = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family      = AF_INET;            // 
  server_addr.sin_port        = htons(PORT_NUM);    // 
  server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(address)); // 
  connect(client_s, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Ouverture et lecture du fichier *.exe
  //if((fh = open(argv[2], O_RDONLY | L_INCR, S_ISGID | S_IWOTH)) == -1)
    if ((fh = fopen (argv[2], "rb"))==NULL)
  {
     printf("  ERREUR d'ouverture du fichier '%s'\n", argv[2]);
     exit(1);
  }

  // 
  printf("Envoie '%s' au serveur '%s' \n", argv[2], argv[1]);

  // Envoie le fichier *.exe au serveur 
  while(!feof(fh))
  {
     //length = read(fh, bin_buf, SIZE);
     length = fread(bin_buf, sizeof(char) ,SIZE,fh);
     cout <<"length = " << length<<endl; 
     send(client_s, bin_buf, length, 0);
  }

  // fermeture du fichier *.exe qui a ete envyer au serveur
  fclose(fh);

  close(client_s); 
  
  printf("  '%s' est execute dans le serveur \n", argv[2]);

  // Creer une nouvelle socket pour pour recevoir le fichier resultat du serveur
  local_s = socket(AF_INET, SOCK_STREAM, 0);
  
  local_addr.sin_family      = AF_INET;            
  local_addr.sin_port        = htons(PORT_NUM);    
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
  bind(local_s, (struct sockaddr *)&local_addr, sizeof(local_addr));

  listen(local_s, MAX_LISTEN);

  addr_len = sizeof(client_addr);
  client_s = accept(local_s, (struct sockaddr*)&client_addr, &addr_len);
  memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

  // Creation et ouverture su fichier resultat pour ecriture 
  //if ((fh=open(argv[3], O_WRONLY | O_CREAT | O_TRUNC | L_INCR,S_ISGID | S_IWOTH)) == -1)
    if ((fh = fopen (argv[3], "w+"))==NULL)

  {
     printf("  *** ERREUR d'ouverture du fichier '%s'\n", argv[3]);
     exit(1);
  }

  // Reception du fichier resultat du serveur 
  length = SIZE;
  while(length > 0)
  {
     length = recv(client_s, bin_buf, SIZE, 0);
     fwrite(bin_buf , sizeof(char) , length , fh );
     //write(fh, bin_buf, length);
  }

  // Fermeture du fichier recu du serveur 
  fclose(fh);

  close(local_s);
  close(client_s);

  printf("Execution du fichier '%s' et transfere du fichier resultat '%s' resussit \n", argv[2], argv[3]);

  return 0;
}
