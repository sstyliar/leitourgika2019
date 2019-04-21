#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>


int sock = 0;
volatile sig_atomic_t terminate = 0; 

void recvHandler() {
    char msg[1024];
    while (1) {
        int receive = recv(sock, msg, 1024, 0);
        if (receive > 0) {
            printf("%s\n", msg);
        }
		else if (receive == 0) {
			printf("error in recvHandler....\n");
            break;
        }
		else{
			// break; //just for safe keeping
		}
    }
}

void sendHandler() {
    char msg[1024];
    while (1) {
        fgets(msg,1024,stdin);	
		msg[strlen(msg)-1] = '\0';
        send(sock, msg, strlen(msg) + 1, 0);
        if (strcmp(msg, "exit") == 0) {
			printf("Client logged out\n");
			send(sock, msg, strlen(msg) + 1, 0);
			terminate = 1;
			break;
        }
	}
}

int main(int argc,char *argv[]){
	int i;
	char msg[1024],name[50];
	struct sockaddr_in addr;
	
	addr.sin_family=AF_INET;
	inet_aton("127.0.0.1",&addr.sin_addr); //localhost
	addr.sin_port=htons(1111);
	sock=socket(AF_INET,SOCK_STREAM,0); //stream socket
	connect(sock,(struct sockaddr *)&addr,sizeof(addr));
	printf("connected, pls give nickname\n");

	fgets(name,50,stdin);
	name[strlen(name)-1]='\0';
	send(sock,name,strlen(name)+1,0);


	pthread_t sendThread;
    if (pthread_create(&sendThread, NULL, (void *) sendHandler, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t recvThread;
    if (pthread_create(&recvThread, NULL, (void *) recvHandler, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

	//force script to stay in stasis untli client says bye bye
	while(1){
		if(terminate == 1){
			break;
		}
	}
	close(sock);

return 0;
}