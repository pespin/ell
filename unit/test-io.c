/*
 *
 *  Embedded Linux library
 *
 *  Copyright (C) 2011  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include <ell/ell.h>

static void do_log(int priority, const char *format, va_list ap)
{
	vprintf(format, ap);
}

static void do_debug(const char *str, void *user_data)
{
	const char *prefix = user_data;

	l_info("%s%s", prefix, str);
}

static bool write_handler(struct l_io *io, void *user_data)
{
	int fd = l_io_get_fd(io);
	char *str = "Hello";
	ssize_t written;

	written = write(fd, str, strlen(str));

	l_info("%zd bytes written", written);

	return false;
}

static void read_handler(struct l_io *io, void *user_data)
{
	int fd = l_io_get_fd(io);
	char str[32];
	ssize_t result;

	result = read(fd, str, sizeof(str));

	l_info("%zd bytes read", result);

	l_main_quit();
}

static void disconnect_handler(struct l_io *io, void *user_data)
{
	l_info("disconnect");
}

int main(int argc, char *argv[])
{
	struct l_io *io1, *io2;
	int fd[2];

	l_log_set_handler(do_log);

	if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, fd) < 0) {
		perror("Failed to create socket pair");
		return 0;
	}

	io1 = l_io_new(fd[0]);
	l_io_set_close_on_destroy(io1, true);
	l_io_set_debug(io1, do_debug, "[IO-1] ", NULL);
	l_io_set_read_handler(io1, read_handler, NULL, NULL);
	l_io_set_disconnect_handler(io1, disconnect_handler, NULL, NULL);

	io2 = l_io_new(fd[1]);
	l_io_set_close_on_destroy(io2, true);
	l_io_set_debug(io2, do_debug, "[IO-2] ", NULL);
	l_io_set_write_handler(io2, write_handler, NULL, NULL);
	l_io_set_disconnect_handler(io2, disconnect_handler, NULL, NULL);

	l_main_run();

	l_io_destroy(io2);
	l_io_destroy(io1);

	return 0;
}
