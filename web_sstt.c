#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>
#include <time.h>

#define VERSION		24
#define BUFSIZE		8096
#define ERROR		42
#define LOG			44
#define PROHIBIDO	403
#define NOENCONTRADO	404


struct {
	char *ext;
	char *filetype;
} extensions [] = {
	{"gif", "image/gif" },
	{"jpg", "image/jpg" },
	{"jpeg","image/jpeg"},
	{"png", "image/png" },
	{"ico", "image/ico" },
	{"zip", "image/zip" },
	{"gz",  "image/gz"  },
	{"tar", "image/tar" },
	{"htm", "text/html" },
	{"html","text/html" },
	{0,0} };

void debug(int log_message_type, char *message, char *additional_info, int socket_fd)
{
	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (log_message_type) {
		case ERROR: (void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",message, additional_info, errno,getpid());
			break;
		case FORBIDDEN:
			// Enviar como respuesta 403 Forbidden
			(void)sprintf(logbuffer,"FORBIDDEN: %s:%s",message, additional_info);
			break;
		case NOT FOUND:
			// Enviar como respuesta 404 Not Found
			(void)sprintf(logbuffer,"NOT FOUND: %s:%s",message, additional_info);
			break;
		case LOG: (void)sprintf(logbuffer," INFO: %s:%s:%d",message, additional_info, socket_fd); break;
	}

	if((fd = open("webserver.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
		(void)write(fd,logbuffer,strlen(logbuffer));
		(void)write(fd,"\n",1);
		(void)close(fd);
	}
	if(log_message_type == ERROR || log_message_type == NOENCONTRADO || log_message_type == PROHIBIDO) exit(3);
}

int checkfileExiste(const char * file) //if it finds the file, it returns 0, if it doesn't exist, it returns 1
{
    FILE *file;
    if (file = fopen(file, "r"))
    {
        fclose(file);
        return 0;
    }
    return 1;
}


void procesar_respuesta200OK(int descriptorfile){
	FILE *file;
	size_t len=0; //
	char * line=NULL; //
	ssize_t bytes_leidos;

	char * primeraLineaRespuesta="HTTP/1.1 200 OK\r\n";
	char * lineaServer="Server: laBestia696\r\n";
	char * lineaContentType="Content-Type: text/html\r\n";
	char * lineaContentLength="Content-Length: 781\r\n"; //TODO Caution: THE NUMBER must be calculated
	char * lineaKeepAlive="Connection: keep-alive\r\n";
	char * lineaParametrosKeepAlive="Keep-Alive: 20\r\n"; //Caution
	char * endPackageLine="\r\n";

	int bytes_sent=write(descriptorfile,primeraLineaRespuesta,strlen(primeraLineaRespuesta));
	bytes_sent=write(descriptorfile,lineaServer,strlen(lineaServer));
  
//calculate the date
	time_t t = time(0);
	struct tm *tlocal=localtime(&t);
	char DepartureTime[128];
	strftime(DepartureTime,128,"Date: %a, %d %b %Y %H:%M:%S GMT+1\r\n",tlocal);
	bytes_sent=write(descriptorfile,&DepartureTime,strlen(DepartureTime));
/////////////////////

	bytes_sent=write(descriptorfile,lineaContentType,strlen(lineaContentType));
	bytes_sent=write(descriptorfile,lineaContentLength,strlen(lineaContentLength));
	bytes_sent=write(descriptorfile,lineaKeepAlive,strlen(lineaKeepAlive));
	bytes_sent=write(descriptorfile,lineaParametrosKeepAlive,strlen(lineaParametrosKeepAlive));
	bytes_sent=write(descriptorfile,endPackageLine,strlen(endPackageLine));

	//At the end of the header, we send the html file:
	file=fopen("index.html","r");
	while((bytes_leidos=getline(&line, &len, file))!=-1){
		int bytes_sent=write(descriptorfile, line,strlen(line));
	}
	fclose(file);
	close(descriptorfile);

}

void procesarErrorBadRequest(int descriptorfile){
	char * primeraLineaRespuesta="HTTP/1.1 400 Bad Request\r\n";
	char * lineaServer="Server: laBestia696\r\n";
	char * lineaContentType="Content-Type: text/html\r\n";
	char * lineaContentLength="Content-Length: 781\r\n"; //TODO Caution: THE NUMBER must be calculated
	char * lineaKeepAlive="Connection: keep-alive\r\n";
	char * lineaParametrosKeepAlive="Keep-Alive: 20\r\n"; //Caution
	char * endPackageLine="\r\n";

	int bytes_sent=write(descriptorfile,primeraLineaRespuesta,strlen(primeraLineaRespuesta));
	bytes_sent=write(descriptorfile,lineaServer,strlen(lineaServer));
//calculate the date
	time_t t = time(0);
	struct tm *tlocal=localtime(&t);
	char DepartureTime[128];
	strftime(DepartureTime,128,"Date: %a, %d %b %Y %H:%M:%S GMT+1\r\n",tlocal);
	bytes_sent=write(descriptorfile,&DepartureTime,strlen(DepartureTime));
/////////////////////
	bytes_sent=write(descriptorfile,lineaContentType,strlen(lineaContentType));
	bytes_sent=write(descriptorfile,lineaContentLength,strlen(lineaContentLength));
	bytes_sent=write(descriptorfile,lineaKeepAlive,strlen(lineaKeepAlive));
	bytes_sent=write(descriptorfile,lineaParametrosKeepAlive,strlen(lineaParametrosKeepAlive));
	bytes_sent=write(descriptorfile,endPackageLine,strlen(endPackageLine));
	//close(descriptorfile);
}

void processLogo(int descriptorfile){
	FILE *file;
	size_t len=0; //
	char * line=NULL; //
	ssize_t bytes_leidos;

	char * primeraLineaRespuesta="HTTP/1.1 200 OK\r\n";
	char * lineaServer="Server: laBestia696\r\n";
	char * lineaContentType="Content-Type: image/jpg\r\n";
	char * lineaContentLength="Content-Length: 9781\r\n"; //TODO Caution: THE NUMBER must be calculated
	char * lineaKeepAlive="Connection: keep-alive\r\n";
	char * lineaParametrosKeepAlive="Keep-Alive: 20\r\n"; //Caution
	char * endPackageLine="\r\n";

	int bytes_sent=write(descriptorfile,primeraLineaRespuesta,strlen(primeraLineaRespuesta));
	bytes_sent=write(descriptorfile,lineaServer,strlen(lineaServer));
//calculate the date
	time_t t = time(0);
	struct tm *tlocal=localtime(&t);
	char DepartureTime[128];
	strftime(DepartureTime,128,"Date: %a, %d %b %Y %H:%M:%S GMT+1\r\n",tlocal);
	bytes_sent=write(descriptorfile,&DepartureTime,strlen(DepartureTime));
/////////////////////

	bytes_sent=write(descriptorfile,lineaContentType,strlen(lineaContentType));
	bytes_sent=write(descriptorfile,lineaContentLength,strlen(lineaContentLength));
	bytes_sent=write(descriptorfile,lineaKeepAlive,strlen(lineaKeepAlive));
	bytes_sent=write(descriptorfile,lineaParametrosKeepAlive,strlen(lineaParametrosKeepAlive));
	bytes_sent=write(descriptorfile,endPackageLine,strlen(endPackageLine));

	//At the end of the header, we send the html file:
	char buffer[BUFSIZE*2];
	file=fopen("logo-um.jpg","r");


	fscanf(file,"%s",buffer);
	strcat(buffer,"\0");

	bytes_sent=write(descriptorfile, buffer,strlen(buffer));


	// while((bytes_read = getline(&line, &len, file))!=-1){
	// 	int bytes_sent = write(descriptorfile, line,strlen(line));
	// }


	fclose(file);
	close(descriptorfile);

}

void checkExpression(int descriptorfile, char * line){
	regex_t regex; //variable donde crearemos regex
	int checkValue=1;
	int checkValue2=1;
	checkValue=regcomp(&regex,"GET \/index.html HTTP\/1.1\r\n",0);
	checkValue2=regcomp(&regex,"GET \/ HTTP\/1.1\r\n",0);
	//
	// if(checkValue == 0){
	// 	printf("funciona la ER\n");
	// }

	checkValue=regexec(&regex,"GET \/index.html HTTP\/1.1\r\n",0,NULL,0);
	checkValue2=regexec(&regex,"GET \/ HTTP/1.1\r\n",0,NULL,0);


	char * getIndex="GET /index.html HTTP/1.1";
	char * getSinIndex="GET / HTTP/1.1";
	char * getLogo="GET /logo-um.jpg HTTP/1.1";
	//if(checkValue==0 || checkValue2==0){
	//(PRIMER GET)
	if(strcmp(line,getIndex)==0 || strcmp(line,getSinIndex)==0){
//		printf("ha hecho match\n");//----------------------------------------------------
//		line=line+'\0';
		procesar_respuesta200OK(descriptorfile);
		return;
	}
	//LOGO REQUEST
	if(strcmp(line,getLogo)==0){
		line=line+'\0';
		printf("It asks for the logo without problem and the method will have to be done\n");
		//ALL process the logo and send it
		processLogo(descriptorfile);

		return;
	}
	//CUANDO NO EMPIEZA POR GET
	checkValue=regexec(&regex,"^GET",0,NULL,0);
	if(checkValue==1){
		printf("no hace match\n");
		procesarErrorBadRequest(descriptorfile);
		return;
	}
	//
	// else{
	// 	printf("no hace match\n");
	// 	checkValue=regexec(&regex,"^GET",0,NULL,0);
	// 	if (checkValue==1 || checkValue2==1){
	// 		procesarErrorBadRequest(descriptorfile);
	// 	}
	//}
}

void process_web_request(int descriptorfile)
{
	debug(LOG,"request","Ha llegado una peticion",descriptorfile);
	//
	// Definir buffer y variables necesarias para leer las peticiones
	//
	char buffer[BUFSIZE];
	//uint_8
	//
	// Leer la petición HTTP
	//
	ssize_t bytes_read;
	size_t len=0;
	char * line=NULL;
	int contador=0;
	FILE *fd;
	char aux[BUFSIZE];
	char delimitador[]="\r\n";


	fd = fdopen(descriptorfile,"r");
	// while ((bytes_read=getline(&line, &len, fd))!=-1){ //&& contador!=1) {
	// 	printf("%s", line);
	// 	//checkExpression(descriptorfile, line);
	// 	//contador++;

	contador = read(descriptorfile,buffer,BUFSIZE);
	//printf("%s",buffer);
	char *token=strtok(buffer,delimitador);
	while(token!=NULL){
		checkExpression(descriptorfile,token); //se pasan todas las lineas;
		printf("%s\n", token);
		token = strtok(NULL,delimitador); //solo se pasa una vez el buffer

	}
	// }

	//
	// Checking for read errors
	//


	//
	// If the read has valid data, terminate the buffer with a \ 0
	//


	//
	// Carriage return and newline characters are removed
	//


	//
	//	HANDLE THE CASES OF THE DIFFERENT METHODS USED
	//	(Only GET is supported)
	//


	//
	//	How the case of illegal access to higher directories of the system directory hierarchy is treated
	//


	//
	//	How to treat the exceptional case of the url not pointing to any html file
	//


	//
	//	Evaluate the type of file that is being requested, and act accordingly by returning it if it is supported or by returning the corresponding error 
	//  in another case
	//


	//
	//	In case the file is supported, exists, etc. the file is sent with the corresponding header, 
	//	and the file is sent in blocks of a maximum of 8kB
	//

	close(descriptorfile);
	exit(1);
}

int main(int argc, char **argv)
{
	int i, port, pid, listenfd, socketfd;
	socklen_t length;
	static struct sockaddr_in cli_addr;		// static = Initialized with 0
	static struct sockaddr_in serv_addr;	// static = Initialized with 0
	fd_set rfds;
	struct timeval tv;
	int retval;


	FD_ZERO(&rfds);
	FD_SET(0,&rfds);

	tv.tv_sec=5;
	tv.tv_usec=0;
	//  Expected arguments:
	//
	//	argv[1]
	//	The first argument of the program expects the port on which the server will listen
	//
	//  argv[2]
	//  In the second argument of the program, the directory in which the server files are located is expected
	//
	//  Verify that the arguments passed when starting the program are as expected
	//

	//
	//  Verify that the chosen directory is suitable. That it is not a system directory and that you have permissions to be used
	//

	if(chdir(argv[2]) == -1){
		(void)printf("ERROR: Can't change directory %s\n",argv[2]);
		exit(4);
	}
	// We make the process a zombie childless demon
	if(fork() != 0)
		return 0; // The parent process returns an OK to the shell

	(void)signal(SIGCHLD, SIG_IGN); // Ignore the children process
	(void)signal(SIGHUP, SIG_IGN); //  Ignore crashes

	debug(LOG,"web server starting...", argv[1] ,getpid());

	/* setup the network socket */
	if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
		debug(ERROR, "system call","socket",0);

	port = atoi(argv[1]);

	if(port < 0 || port >60000)
		debug(ERROR,"Invalid port, try a port from 1 to 60000",argv[1],0);

	/*A structure is created for the IP and port information where the server listens*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /*Listen on any available IP*/
	serv_addr.sin_port = htons(port); /*... on the port specified as a parameter*/

	if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
		debug(ERROR,"system call","bind",0);

	if( listen(listenfd,64) <0)
		debug(ERROR,"system call","listen",0);

	while(1){
		length = sizeof(cli_addr);
		if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
			debug(ERROR,"system call","accept",0);
		if((pid = fork()) < 0) {
			debug(ERROR,"system call","fork",0);
		}
		else {
			if(pid == 0) { 	// Child process
				(void)close(listenfd);
				process_web_request(socketfd); // The child terminates after calling this function

				retval=select(socketfd,&rfds,NULL,NULL,&tv);

				if(!retval){
					close(socketfd);
				}
			} else { 	// Parent process
				(void)close(socketfd);
			}
		}
	}
}
