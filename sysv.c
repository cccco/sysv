#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(void)
{
    int fd = open("/tmp/msg.temp", O_CREAT | O_WRONLY, 0666);
    if (fd == -1) {
        perror("open msg.temp");
        return 1;
    }
    close(fd);

    key_t key = ftok("/tmp/msg.temp", 1);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    int msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    struct msqid_ds ds;
    ds.msg_qbytes = BUFFER_SIZE;

    int res = msgctl(msgid, IPC_SET , &ds);
    if (res == -1) {
        perror("msgctl");
        return 1;
    }

    struct msgbuf msgbuff;
    memset(msgbuff.mtext, 0, sizeof(char*) * BUFFER_SIZE);


    res= msgrcv(msgid, &msgbuff, BUFFER_SIZE, 0, NULL);
    if (res == -1) {
        perror("msgrcv");
        return 1;
    }
    msgbuff.mtext[res] = 0;

    fd = open("/home/box/message.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open message.txt");
        return 1;
    }

    write(fd, msgbuff.mtext, res);
    if (fd == -1) {
        perror("write");
        close(fd);
        return 1;
    }
    close(fd);

    return 0;
}

