#include "semaphores.h"

union semun{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int semCreate(key_t key, int val){
    int semid;
    union semun arg;

    semid = semget(key,1,0666|IPC_CREAT);
    arg.val = val;
    semctl(semid,0,SETVAL,arg);
    return semid;
}

int semDown(int semid){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    return semop(semid,&sb,1);
}

int semUp(int semid){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    return semop(semid,&sb,1);
}

int semValue(int semid){
    semctl(semid, 0, GETVAL, 0);
}

int semDelete(int semid){
    return semctl(semid,0,IPC_RMID);
}
