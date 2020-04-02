#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
	struct _pulse   pulse;
	char msg[255];
} my_message_t;

int main(int argc, char *argv[]) {
	name_attach_t *attach = name_attach(NULL, "mydevice", 0);
	if (attach == NULL){
		exit(EXIT_FAILURE);
	}

	FILE *fp_1 = fopen("/dev/local/mydevice", "r");

	if(fp_1 == NULL){
		exit(EXIT_FAILURE);
	}

	char status[40];
	char value[40];

	if(fscanf(fp_1, "%s %s\n", status, value) == 2){
		if(strcmp("status", status) == 0){
			printf("Status: %s\n", value);

			if(strcmp("closed", value) == 0){
				name_detach(attach, 0);
				exit(EXIT_SUCCESS);
			}

			fclose(fp_1);
		}
	} else {
		exit(EXIT_FAILURE);
	}

	my_message_t msg;

	while(1){
		int rcvid = MsgReceivePulse(attach->chid, &msg, sizeof(msg), NULL);
		if(rcvid == 0){
			if(msg.pulse.code == MY_PULSE_CODE){
				printf("Small Integer: %d\n", msg.pulse.value.sival_int);
				FILE *fp = fopen("/dev/local/mydevice", "r");


				if(fscanf(fp, "%s %s\n", status, value) == 2){
					if(strcmp("status", status) == 0){
						printf("Status: %s\n", value);

						if(strncmp("closed", value, 6) == 0){
							name_detach(attach, 0);
							exit(EXIT_SUCCESS);
						}

						fclose(fp);
					}
				} else {
					exit(EXIT_FAILURE);
				}
			}
		} else {
			perror("Error. Please send a pulse.\n");
			exit(EXIT_FAILURE);
		}
	}

	name_detach(attach, 0);
	exit(EXIT_SUCCESS);

}
