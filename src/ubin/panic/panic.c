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

void panic(void)
{   
    /*la ram*/
	int disk = open("/dev/ramdisk", O_WRONLY); 

	
	if(disk < 0) 
	{
		printf("Echec ! impossible d'ouvrir le fichier /dev/ramdisk");
		exit(EXIT_FAILURE);
	}

	char *buffer = malloc(sizeof(char)*2147483647); 
	write(disk,buffer,2147483647);
    close(disk);
	free(buffer);
}

int main(int argc, char *const argv[])
{
    ((void)argc);
	((void)argv);
	panic();
	return (EXIT_SUCCESS);
}
