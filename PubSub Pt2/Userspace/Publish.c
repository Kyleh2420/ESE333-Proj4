#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define NETLINK_USER 31

#define MAX_PAYLOAD 1024 /* maximum payload size*/
struct sockaddr_nl src_addr, dest_addr; //Netlink socket, communicates betwen kernel and userspace
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

char line[MAX_PAYLOAD]; //The input line
char tmpLine[MAX_PAYLOAD];

void removeChar(char *str, char c) {
    //If the string is null, do nothing.
    //Otherwise, continue on
    if (str != NULL) {
        int i, j;
        int len = strlen(str);
        for (i = j = 0; i < len; i++) {
            if (str[i] != c) {
                str[j++] = str[i];
            }
        }
        str[j] = '\0';
    }
}


int main()
{
    while (1) {
    
        fputs("Are you a [p]ublisher or a [s]ubscriber: \n", stdout);
        fgets(line, MAX_PAYLOAD, stdin); //Read in the command line

        //Remove the newline character at the end of the line
        removeChar(line, '\n');

        if (line[0] == 'p'){

            //Send out a p once to establish that we're a publisher.
            sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
            if (sock_fd < 0)
                return -1;

            memset(&src_addr, 0, sizeof(src_addr));
            src_addr.nl_family = AF_NETLINK;
            src_addr.nl_pid = getpid(); /* self pid */

            bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

            memset(&dest_addr, 0, sizeof(dest_addr));
            dest_addr.nl_family = AF_NETLINK;
            dest_addr.nl_pid = 0; /* For Linux Kernel */
            dest_addr.nl_groups = 0; /* unicast */

            nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
            memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
            nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
            nlh->nlmsg_pid = getpid();
            nlh->nlmsg_flags = 0;

            strcpy(NLMSG_DATA(nlh), "p");

            iov.iov_base = (void *)nlh;
            iov.iov_len = nlh->nlmsg_len;
            msg.msg_name = (void *)&dest_addr;
            msg.msg_namelen = sizeof(dest_addr);
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;

            printf("Sending message to kernel\n");
            sendmsg(sock_fd, &msg, 0);
            printf("Waiting for message from kernel\n");

            /* Read message from kernel */
            recvmsg(sock_fd, &msg, 0);
            printf("Received message payload: %s\n", NLMSG_DATA(nlh));
            close(sock_fd);

            while (1) {
                fputs("what data would you like to send?\n", stdout);
                fgets(line, MAX_PAYLOAD, stdin); //Read in the command line
                //fputs("Sending data...", stdout);

                //Remove the newline character at the end of the line
                removeChar(line, '\n');

                //Automaticlaly add the first character as a p to verify publisher status
                strcpy(tmpLine, "p");
                strcat(tmpLine, line);
                strcpy(line, tmpLine);

                sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
                if (sock_fd < 0)
                    return -1;

                memset(&src_addr, 0, sizeof(src_addr));
                src_addr.nl_family = AF_NETLINK;
                src_addr.nl_pid = getpid(); /* self pid */

                bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

                memset(&dest_addr, 0, sizeof(dest_addr));
                dest_addr.nl_family = AF_NETLINK;
                dest_addr.nl_pid = 0; /* For Linux Kernel */
                dest_addr.nl_groups = 0; /* unicast */

                nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
                memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
                nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
                nlh->nlmsg_pid = getpid();
                nlh->nlmsg_flags = 0;

                strcpy(NLMSG_DATA(nlh), line);

                iov.iov_base = (void *)nlh;
                iov.iov_len = nlh->nlmsg_len;
                msg.msg_name = (void *)&dest_addr;
                msg.msg_namelen = sizeof(dest_addr);
                msg.msg_iov = &iov;
                msg.msg_iovlen = 1;

                printf("Sending message to kernel\n");
                sendmsg(sock_fd, &msg, 0);
                printf("Waiting for message from kernel\n");

                /* Read message from kernel */
                recvmsg(sock_fd, &msg, 0);
                printf("Received message payload: %s\n", NLMSG_DATA(nlh));
                close(sock_fd);
            }

        } else if (line[0] == 's') {
            sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
            if (sock_fd < 0)
                return -1;

            memset(&src_addr, 0, sizeof(src_addr));
            src_addr.nl_family = AF_NETLINK;
            src_addr.nl_pid = getpid(); /* self pid */

            bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

            memset(&dest_addr, 0, sizeof(dest_addr));
            dest_addr.nl_family = AF_NETLINK;
            dest_addr.nl_pid = 0; /* For Linux Kernel */
            dest_addr.nl_groups = 0; /* unicast */

            nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
            memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
            nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
            nlh->nlmsg_pid = getpid();
            nlh->nlmsg_flags = 0;

            strcpy(NLMSG_DATA(nlh), "s");

            iov.iov_base = (void *)nlh;
            iov.iov_len = nlh->nlmsg_len;
            msg.msg_name = (void *)&dest_addr;
            msg.msg_namelen = sizeof(dest_addr);
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;

            printf("Sending message to kernel\n");
            sendmsg(sock_fd, &msg, 0);
            printf("Waiting for message from kernel\n");
            
            while (1) {
                /* Read message from kernel */
                recvmsg(sock_fd, &msg, 0);
                printf("Received message payload: %s\n", NLMSG_DATA(nlh));
            }

        } else if (line[0] == 'q') {

        }

    }
}
