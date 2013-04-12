#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>


#define TERM_DEVICE "/dev/ttyACM0"
#define TERM_SPEED B9600

void sendingdata(void){
	int fd, old_flags;
	ssize_t length;
	char buffer[16];
	struct termios term_attr;
	fd_set input_fdset;

	if ((fd = open(TERM_DEVICE, O_RDWR)) == -1){
		perror("terminal: Can't open device " TERM_DEVICE);
		return(1);
		}

	if (tcgetattr(fd, &term_attr) != 0){
		perror("termina: tcgetattr() failed");
		return(1);
		}
	term_attr.c_cflag = TERM_SPEED | CS8 | CRTSCTS
						| CLOCAL | CREAD;
	term_attr.c_iflag = 0;
	term_attr.c_oflag = OPOST | ONLCR;
	term_attr.c_lflag = 0;
	if (tcsetattr(fd, TCSAFLUSH, &term_attr) != 0){
		perror("terminal: tcsetattr() failed");
	}
		//Std.-Eingabe anpassen
	if (tcgetattr(STDIN_FILENO, &term_attr) != 0){
		perror("terminal: tcgetattr() failed");
		return(1);
		}
		//alte Einstellung sichern
	old_flags = term_attr.c_lflag;
	term_attr.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr) != 0){
		perror("terminal:tcsetattr() failed");
		}
	while (1){
		FD_ZERO(&input_fdset);
		FD_SET(STDIN_FILENO, &input_fdset);
		FD_SET(fd, &input_fdset);
		if (select(fd+1, &input_fdset, NULL, NULL, NULL) == -1){
			perror("terminal: select() failed");
			}
		if ((FD_ISSET(STDIN_FILENO, &input_fdset)) == -1){
			perror("terminal: read() failed");
			}
			else if (buffer[0] == '\33'){
				break;
				}
			else{
				write(fd, buffer, length);
				}
		if (FD_ISSET(fd, &input_fdset)){
			if ((length = read(fd, buffer, 16)) == -1){
				perror("terminal: read() failed");
			}
			else {
				write(STDOUT_FILENO, buffer, length);
				}
			}
		}
				//Std.-Eingabe wie vorher
	term_attr.c_lflag = old_flags;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr) != 0){
	perror("terminal: tcsetattr() failed");
	}

	printf("Aborted. \n");
	close(fd);
}
