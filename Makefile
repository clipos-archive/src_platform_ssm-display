# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright © 2015-2018 ANSSI. All Rights Reserved.
CC = gcc
CFLAGS += -fPIC -pie -Wall -Wextra -Werror 
PREFIX ?= /usr

all:
	$(CC) $(CFLAGS)  -o pinentry-client pinentry-client.c

clean:
	rm -f pinentry-client

install:
	install -m 0756 -D pinentry-client $(DESTDIR)/$(PREFIX)/proxy_bin/pinentry-client
	install -m 0755 -D pinentry-server $(DESTDIR)/$(PREFIX)/bin/pinentry-server
	install -m 0755 -D ssm-manage-key  $(DESTDIR)/$(PREFIX)/bin/ssm-manage-key
