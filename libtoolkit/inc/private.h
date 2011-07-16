/*
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

struct window {
	struct fb *fb;
	struct widget *widget;
	handler_t handler;
};

struct widget {
	lua_State *L;
	struct window *window;
	bool dirty, child_dirty;

	char *name;
	int x, y;
	int width, height;
	int realx, realy, realwidth, realheight;

	struct widget *parent, *children;
	struct widget *prev, *next;
};

extern struct window *__rtk_window;

int __rtk_init_freetype();
void __rtk_init_drawing_functions(lua_State *L);

int __rtk_set_attribute(struct widget *widget);
int __rtk_get_attribute(struct widget *widget);
int __rtk_call_lua_function(lua_State *L, int args, int ret);
void __rtk_update_widget();
int __rtk_draw_widget(struct widget *widget, bool force);
struct widget *__rtk_add_widget(const char *widget, struct widget *parent, struct window *window, int x, int y, int width, int height);
void __rtk_free_widget(struct widget *widget);

void __rtk_set_window_size(struct window *window, int width, int height);
