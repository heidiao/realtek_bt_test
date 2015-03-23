#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* bd addr length and type */
#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN     6
typedef unsigned char BD_ADDR[BD_ADDR_LEN];
#endif

struct option mainopts[] = {
    {"hogp", 1, NULL, 'h'},
    {"pan", 1, NULL, 'p'},
    {0,0,0,0}
};

static void usage(void)
{
    printf("Realtek Android 4.4 Bluedroid BQB Test");
    printf("Usage:\n"
       "\trtkbqbtest ");
    
    printf("Options:\n"
            "\t--hogp(-h)   hogp tester\n"
            "\t--pan(-p)    pan tester\n"
            );
 
}

#define FIFO_SERVER "/data/data/com.android.bluetooth/fifoserver"


enum {
    HOGP_IF = 1,    
    PAN_IF
};

char *proto_if_str[] = {
    " ",
    "HOGP_IF",
    "PAN_IF"
};


typedef struct _proto_cmd_data{
    unsigned char proto_if;
    char bEnable;
}PROTO_CMD_DATA;



void baswap(BD_ADDR *dst, const BD_ADDR *src) {
  register unsigned char *d = (unsigned char *) dst;
  register const unsigned char *s = (const unsigned char *) src;
  register int i;

  for (i = 0; i < 6; i++) d[i] = s[5-i];
}

int str2ba(const char *str, BD_ADDR *ba) {
  uint8_t b[6];
  const char *ptr = str;
  int i;

  for (i = 0; i < 6; i++) {
    b[i] = (uint8_t) strtol(ptr, NULL, 16);
    if (i != 5 && !(ptr = strchr(ptr, ':')))
      ptr = ":00:00:00:00:00";
    ptr++;
  }
  baswap(ba, (const)(BD_ADDR*)b);
  return 0;
}




int main(int argc, char* argv[])
{
    int opt = 0;
    int pid_bluedroid = 0;
    unsigned char ipc_method = 0;
    PROTO_CMD_DATA proto_cmd_data;
    unsigned char proto_if = 0;



    int fd =0;
    int real_wnum = 0;

    while((opt = getopt_long(argc, argv, "h:p:", mainopts, NULL))!=EOF){
        switch(opt)
        {

            case 'h':
                proto_cmd_data.proto_if = HOGP_IF;
                proto_cmd_data.bEnable = atoi(optarg);

            break;
            case 'p':
                proto_cmd_data.proto_if = PAN_IF;
                proto_cmd_data.bEnable = atoi(optarg);
            break;        

            default:
                usage();
                exit(0);
            break;
        }
    }

 

    printf("proto(%s), bEnable(%d)", proto_if_str[proto_cmd_data.proto_if], proto_cmd_data.bEnable);


    fd=open(FIFO_SERVER,O_WRONLY,0);
    //设置非阻塞标志
    //fd=open(FIFO_SERVER,O_WRONLY,0);
    //设置阻塞标志

    if(fd<0)
    {
        printf("open fifo fail errno(%d)\n", errno);
    }

    real_wnum=write(fd, &proto_cmd_data, sizeof(proto_cmd_data));
    printf("write to fifo real_wnum(%d)\n", real_wnum);

    if(real_wnum==-1)
    {
        if(errno==EAGAIN)
            printf("write to fifo error; try later\n");
    }
    else
    {

    }


    return 0;
}
