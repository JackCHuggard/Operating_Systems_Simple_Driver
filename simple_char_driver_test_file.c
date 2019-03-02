#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#define BUFFERSIZE 1024
#define DEVICENAME "/dev/simple_char_driver"
int main ()
{
	char userinput, buffer[1024];//for our commands and to allocate mem
	int whence, offset;
	size_t length;
	int file = open(DEVICENAME, O_RDWR);
	bool val = true;
	
	while (val)
	{	
		printf("Press r to read from device\nPress w to write to device\nPress s to seek in device\nPress e to exit from the device\nPress anything else to keep reading or writing from the device\n Enter command: ");
		scanf("%c", &userinput);
		userinput = tolower(userinput);
	switch(userinput)
	{
		case 'r':
			printf("Enter the amount of bytes you want to read\n");
			scanf("%zu", &length);
			read(file, buffer, length); // reads from device file 
			printf("Data read from the device: %s\n", buffer);
			while(getchar() != '\n');

		break;
		case 'w':
			printf("Enter data you want to write to the device\n");
			scanf("%s", buffer);
			length = strlen(buffer);
			write(file, buffer, length);
			while(getchar() != '\n');		

		break;
		case 's':
			printf("Enter and offset value:\n");
			scanf("%d", &offset);
			printf("Enter a value for whence (third parameter):\n");
			scanf("%d", &whence);
			lseek(file, offset, whence);
			while(getchar() != '\n');
		break;
		case 'e':
			printf("Terminating\n");
			val = false;
		break;
		default:
			printf("Not a valid command, use e to exit\n");
		break;
	}
	//to erase buffer
	memset(buffer, 0, 1024);
//end of while
	}	
close(file);

return 0;
}
