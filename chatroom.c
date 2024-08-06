#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include<time.h>
#define SHM_SIZE 1024
struct message {
    char username[32];
    char text[256];
};
struct chat_room {
    struct message messages[10];
    int num_messages;
    int num_users;
    char usernames[10][32];
};
int shmid;
struct chat_room* chat_ptr;

void handler(int sig) {
    shmdt(chat_ptr);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}
int main()
{
    int i,j,k;
    FILE *fptr;
    time_t t;
    time(&t);
    fptr = fopen("chat.txt","a");
    if(fptr == NULL)
    {
       printf("Error!");
       exit(1);
    }
    fprintf(fptr,"\n%s",ctime(&t));
    signal(SIGINT, handler);
    key_t key = ftok(".", 'c');
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    chat_ptr = (struct chat_room*) shmat(shmid, NULL, 0);
    printf("Welcome to the chat room !\n");
    printf("To Display the:  \n");
    printf("\tHistory\t: /history\n");
    printf("\tUsers  \t: /users\n");
    printf("To exit the chat room please enter: /exit\n");
    printf("Let's chat:  (? ??? ?)\n");
    
    while (1)
    {
        char input[256],chat[256];
        printf("> ");
        fgets(input, 256, stdin);
        if (strncmp(input, "/exit", 5) == 0)
        {
                break;
        }
        else if (strncmp(input, "/history", 8) == 0)
        {
                printf("Chat history: \n");
                for (i = 0; i < chat_ptr->num_messages; i++)
                {
                        printf("%s: %s", chat_ptr->messages[i].username, chat_ptr->messages[i].text);
                }
                printf("\n");
        }
        else if (strncmp(input, "/users", 6) == 0)
        {
                int i,j,k,size;
                printf("USERS: \n");
                size = chat_ptr ->num_users;
                for ( i = 0; i < size; i ++)
                {
                        for ( j = i + 1; j < size; j++)
                        {
                                if ( strcmp(chat_ptr->usernames[i],chat_ptr->usernames[j]) == 0)
                                {
                                        for(k=j;k<size;k++)
                                        {
                                                strcpy(chat_ptr->usernames[k],chat_ptr->usernames[k+1]);
                                        }
                                        strcpy(chat_ptr->usernames[k],"");
                                        size--;
                                        j--;
                                }
                        }
                }
                chat_ptr ->num_users = size;
                for ( i = 0; i < chat_ptr->num_users; i++)
                {
                        printf("%s ", chat_ptr->usernames[i]);
                }
              printf("\n");
        }
        else
        {
                int size;
                fprintf(fptr,"%s",input);
                printf("TEXT> ");
                fgets(chat, 256, stdin);
                fprintf(fptr,"%s",chat);
                struct message new_message;
                strcpy(new_message.username, "User ");
                strcat(new_message.username, input);
                chat_ptr->num_users++;
                strcpy(new_message.text, chat);
                chat_ptr->num_messages++;
                strcpy(chat_ptr->usernames[chat_ptr ->num_users] , input);
                size = chat_ptr ->num_users;
                if (chat_ptr->num_messages >= 10)
                {
                        for (i = 1; i < 10; i++)
                        {
                                chat_ptr->messages[i - 1] = chat_ptr->messages[i];
                        }
                        chat_ptr->num_messages = 9;
                }
                chat_ptr->messages[chat_ptr->num_messages] = new_message;
                chat_ptr->num_messages++;
                for (i = 0; i < chat_ptr->num_messages; i++)
                {
                        printf("%s: %s", chat_ptr->messages[i].username, chat_ptr->messages[i].text);
                }
        }
    }
    handler(0);
    return 0;
}

