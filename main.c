/* bambooInkBtButton.c
 *
 * Copyright (C) 2014 Robert Milasan <rmilasan@suse.com>
 * Copyright (C) 2020 Massimiliano Minarelli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This example monitors udev events for 'net' subsystem using libudev API.
 *
 */

// Includes ____________________________________________________________________
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <libudev.h>
#include <sys/types.h>
#include <sys/wait.h>

// Macros ______________________________________________________________________
#define BT_DEVICE_NAME					"hci0:129"

#define PROGRAM_NAME_LEN				256
#define PROGRAM_RPATH					".config/btpenbutton"
#define PROGRAM_NAME					"btpenbutton.sh"
#define PROGRAM_ARGS					""

typedef enum
{
	EXIT_ERR_GETENV=1,
	EXIT_ERR_UDEV,
	EXIT_ERR_FORK,
} exit_error_t;


// Functions ___________________________________________________________________
int main()
{
	char program[PROGRAM_NAME_LEN] = {0};
	struct udev *udev;
	struct udev_device *dev;
	struct udev_monitor *mon;
	int fd;
	pid_t pid;

	/* Get home directory from env*/
	char * home = getenv("HOME");
	if(home == NULL)
		exit(EXIT_ERR_GETENV);
	strcat(program, home);
	strcat(program, "/" PROGRAM_RPATH "/" PROGRAM_NAME);
	printf("%s\n", program);

	/* create udev object */
	udev = udev_new();
	if (!udev)
	{
		fprintf(stderr, "Can't create udev\n");
		return EXIT_ERR_UDEV;
	}

	mon = udev_monitor_new_from_netlink(udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mon, "bluetooth", NULL);
	udev_monitor_enable_receiving(mon);
	fd = udev_monitor_get_fd(mon);

	while (1)
	{
		fd_set fds;
		struct timeval tv;
		int ret;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		ret = select(fd + 1, &fds, NULL, NULL, &tv);
		if (ret > 0 && FD_ISSET(fd, &fds))
		{
			dev = udev_monitor_receive_device(mon);
			if (dev)
			{
				printf("%s \n %s \n", udev_device_get_sysname(dev), 
						udev_device_get_devtype(dev));
				if (strcmp(udev_device_get_action(dev), "add") == 0 &&
				    strcmp(udev_device_get_sysname(dev), BT_DEVICE_NAME) == 0)
				{
					pid = fork();
					if (pid < 0)
					{
						printf("Error in forking. \n");
						return EXIT_ERR_FORK;
					}
					else if (pid == 0)
					{
						execlp(program, program, PROGRAM_ARGS, NULL);
					}
				}
				/* free dev */
				udev_device_unref(dev);
			}
		}
		/* 500 milliseconds */
		usleep(500 * 1000);
	}
	/* free udev */
	udev_unref(udev);

	return 0;
}
