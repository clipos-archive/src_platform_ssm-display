// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2015-2018 ANSSI. All Rights Reserved.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#define BUFF_SIZE 1024

void usage()
{
	fprintf(stderr, " Usage: pinentry-client socket opcode\n");
	fprintf(stderr, " Opcodes:\n");
	fprintf(stderr, "  a: SSM password\n");
	fprintf(stderr, "  b: PKCS#11 PIN\n");
	fprintf(stderr, "  c: PKCS#12 export password\n");
	exit(EXIT_FAILURE);
}

int init(const char* socketPath)
{
	struct sockaddr_un addr;
	int fd;

	if ((strlen(socketPath) + 1) >= UNIX_PATH_MAX) // see addr.sun_path
		usage();
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	memcpy(addr.sun_path, socketPath, strlen(socketPath));
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(EXIT_FAILURE);
	}
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("connect error");
		exit(EXIT_FAILURE);
	}
	return  (fd);
}

int parseResult(const char* buffer)
{
	if (buffer[0] != 'o')
	{
		if (buffer[0] == 'c')
			fprintf(stderr, "Input cancelled by the user\n");
		else
			fprintf(stderr, "pinentry-server returned an error\n");
		return (EXIT_FAILURE); // Maybe we should be more specific
	}
	printf("%s", buffer + 1);
	return (EXIT_SUCCESS);
}

int talk(int fd, const char opcode)
{
	char buffer[BUFF_SIZE];
	int got = 0;
	int ret = 0;

	if (write(fd, &opcode, 1) != 1)
	{
		perror("opcode sending error");
		return (EXIT_FAILURE);
	}
	while (got < BUFF_SIZE)
	{
		if ((ret = read(fd, buffer + got, BUFF_SIZE - got)) < 1)
		{
			perror("read error on the socket");
			return(EXIT_FAILURE);
		}
		got += ret;
		if (buffer[got - 1] == '\0')
			return parseResult(buffer);
		if (got == BUFF_SIZE)
		{
			fprintf(stderr, "Error: the response is too long.\n");
			return (EXIT_FAILURE);
		}
	
	}
	return (EXIT_FAILURE); // Should never go here !

}

int main(const int argc, const char** argv)
{
	if (argc != 3)
		usage();
	if ((strlen(argv[2]) != 1) || (argv[2][0] < 'a') || (argv[2][0] > 'c'))
		usage();
	return talk(init(argv[1]), argv[2][0]);
}
