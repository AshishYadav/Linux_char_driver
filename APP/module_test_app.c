#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* The name of the device file */
#define DEVICE_FILE_NAME "/dev/collinsdev-1"

int main()
{
	int file_desc, size;
	char *user_buffer = (char *) calloc(100, sizeof(char));

	/* open() system call */
	file_desc = open(DEVICE_FILE_NAME, O_RDWR);
	if (file_desc < 0) 
	{ 
		perror("r1"); 
		exit(1); 
	}
	
	/* Read() system call */
	size = read(file_desc, user_buffer, 100);
	printf("%d bytes were read from Device \n", size);
	user_buffer[size] = '\0';
	printf("Read Data from Device is : % s\n", user_buffer);

	/* write system call */
	user_buffer = "Collins char device write";
	size = write(file_desc, user_buffer, strlen(user_buffer));
	printf("Bytes asked to write: %d and written to device: %d \n", strlen(user_buffer), size);

	/* close system call */
	close(file_desc);

	return 0;
}
