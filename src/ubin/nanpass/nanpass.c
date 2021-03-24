/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <unistd.h>
#include <string.h>

#include <sys/utsname.h>

#include <nanvix/accounts.h>
#include <nanvix/config.h>
#include <dev/tty.h>

#if (MULTIUSER == 1)

/**
 * @brief Seeks the password of a specific user
 *  
 * @param name     User name.
 * 
 * @returns One if the user exists and zero otherwise.
 */
static int fetch_password(const char *name)
{
	int file;         /* Passwords file.  */
	struct account a; /* Working account. */
	
	/* Open passwords file. */
	if ((file = open("/etc/passwords", O_RDONLY)) == -1)
	{
		fprintf(stderr, "cannot open password file\n");
		return (0);
	}
	
	/* Search in the  passwords file. */
	while (read(file, &a, sizeof(struct account)))
	{
		account_decrypt(a.name, USERNAME_MAX, KERNEL_HASH);
	
		/* No this user. */
		if (strcmp(name, a.name))
			continue;
			
		account_decrypt(a.password, PASSWORD_MAX, KERNEL_HASH);

		printf("%s password is: %s\n", a.name, a.password);
		
		close(file);

		return 1;
	}

	fprintf(stderr, "\nuser does not exist\n\n");
	return 0;
}

/**
 * @brief Seeks the password of a specific user
 * 
 * @returns One if user exists and false otherwise.
 */
static int nanpass(void)
{
	char name[USERNAME_MAX];
	
	printf("  _   _             _____           _____ \n");
	printf(" | \\ | |           |  __ \\         / ____|\n");
	printf(" |  \\| | __ _ _ __ | |__) |_ _ ___| (___  \n");
	printf(" | . ` |/ _` | '_ \\|  ___/ _` / __|\\___ \\ \n");
	printf(" | |\\  | (_| | | | | |  | (_| \\__ \\____) |\n");
	printf(" |_| \\_|\\__,_|_| |_|_|   \\__,_|___/_____/ \n\n");
	printf("\t-- The Nanvix Password Seeker --\n\n");
	printf("login: ");
	fgets(name, USERNAME_MAX, stdin);
	
	return (fetch_password(name));
}

#endif

/*
 * Seeks a user's password.
 */
int main(int argc, char *const argv[])
{
	struct utsname name; /* System information. */
	
	((void)argc);
	((void)argv);
	
	(void) setvbuf(stdin, NULL, _IOLBF, 0);
	(void) setvbuf(stdout, NULL, _IONBF, 0);
	
	ioctl(fileno(stdout), TTY_CLEAR);
	
	/* Get system information. */
	if (uname(&name) != 0)
	{
		fprintf(stderr, "cannot uname()");
		return (EXIT_FAILURE);
	}
	
	printf("%s %s on %s\n\n", name.sysname, name.version, name.nodename);

#if (MULTIUSER == 1)
	
	nanpass();

#endif
	
	return (EXIT_SUCCESS);
}
