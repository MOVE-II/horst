/* Copyright (c) 2012 Yoran Heling

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "dbusuv.h"
#include <stdlib.h>


/* dbus <-> libuv poll watchers */

static void poll_handle(uv_poll_t *p, int status, int revents) {
	dbus_watch_handle(p->data, (revents & UV_READABLE ? DBUS_WATCH_READABLE : 0) | (revents & UV_WRITABLE ? DBUS_WATCH_WRITABLE : 0));
}


static void poll_toggle(DBusWatch *w, void *loop) {
	uv_poll_t *p = dbus_watch_get_data(w);
	if(dbus_watch_get_enabled(w)) {
		int f = dbus_watch_get_flags(w);
		uv_poll_start(p, (f & DBUS_WATCH_READABLE ? UV_READABLE : 0) | (f & DBUS_WATCH_WRITABLE ? UV_WRITABLE : 0), poll_handle);
	} else
		uv_poll_stop(p);
}


static dbus_bool_t poll_add(DBusWatch *w, void *loop) {
	uv_poll_t *p = malloc(sizeof(uv_poll_t));
	if(!p)
		return FALSE;
	p->data = w;
	dbus_watch_set_data(w, p, NULL);
	uv_poll_init(loop, p, dbus_watch_get_unix_fd(w));
	poll_toggle(w, loop);
	return TRUE;
}


static void poll_remove(DBusWatch *w, void *loop) {
	uv_close(dbus_watch_get_data(w), (uv_close_cb)free);
}



/* dbus <-> libuv timeout functions */

static void timer_handle(uv_timer_t *timer, int status) {
	dbus_timeout_handle(timer->data);
}


static void timer_toggle(DBusTimeout *t, void *loop) {
	uv_timer_t *timer = dbus_timeout_get_data(t);
	if(dbus_timeout_get_enabled(t))
		uv_timer_start(timer, timer_handle, (int64_t)(dbus_timeout_get_interval(t))/1000.0, 0);
	else
		uv_timer_stop(timer);
}


static dbus_bool_t timer_add(DBusTimeout *t, void *loop) {
	uv_timer_t *timer = malloc(sizeof(uv_timer_t));
	if(!timer)
		return FALSE;
	timer->data = t;
	dbus_timeout_set_data(t, timer, NULL);
	uv_timer_init(loop, timer);
	timer_toggle(t, loop);
	return TRUE;
}


static void timer_remove(DBusTimeout *t, void *loop) {
	uv_close(dbus_timeout_get_data(t), (uv_close_cb)free);
}



/* dbus <-> libuv dispatch handling */

static void dispatch(uv_idle_t *idl, int status) {
	if(dbus_connection_dispatch(idl->data) == DBUS_DISPATCH_COMPLETE)
		uv_idle_stop(idl);
}


static void dispatch_change(DBusConnection *con, DBusDispatchStatus s, void *data) {
	uv_idle_start(data, dispatch);
}



/* dbus <-> libuv async main loop wakeup */

static void async_handle(uv_async_t *async, int revents) {
	/* Nothing to do here, if this function is called then the uv_loop has already woken up. */
}


static void async_wakeup(void *data) {
	uv_async_send((uv_async_t *)data);
}



static void handle_close(void *data) {
	uv_close(data, (uv_close_cb)free);
}


void dbusuv_register(uv_loop_t *loop, DBusConnection *con) {
	dbus_connection_set_watch_functions(con, poll_add, poll_remove, poll_toggle, loop, NULL);
	dbus_connection_set_timeout_functions(con, timer_add, timer_remove, timer_toggle, loop, NULL);

	uv_idle_t *dispatcher = malloc(sizeof(uv_idle_t));
	uv_idle_init(loop, dispatcher);
	dispatcher->data = con;
	dbus_connection_set_dispatch_status_function(con, dispatch_change, dispatcher, handle_close);

	uv_async_t *async = malloc(sizeof(uv_async_t));
	uv_async_init(loop, async, async_handle);
	dbus_connection_set_wakeup_main_function(con, async_wakeup, async, handle_close);
}

/* vim: set noet sw=4 ts=4: */
