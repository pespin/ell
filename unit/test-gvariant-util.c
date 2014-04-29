/*
 *
 *  Embedded Linux library
 *
 *  Copyright (C) 2011-2014  Intel Corporation. All rights reserved.
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

#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdio.h>

#include <ell/ell.h>
#include "ell/gvariant-private.h"

struct signature_test {
	bool valid;
	const char *signature;
};

#define SIGNATURE_TEST(v, sig, i)				\
	static struct signature_test sig_test##i = {		\
		.valid = v,					\
		.signature = sig,				\
	}

SIGNATURE_TEST(false, "a", 1);
SIGNATURE_TEST(false, "a{vs}", 2);
SIGNATURE_TEST(true, "(ss)", 3);
SIGNATURE_TEST(true, "(s(ss))", 4);
SIGNATURE_TEST(true, "as", 5);
SIGNATURE_TEST(true, "ab", 6);
SIGNATURE_TEST(true, "aas", 7);
SIGNATURE_TEST(true, "a(ss)", 8);
SIGNATURE_TEST(true, "asas", 9);
SIGNATURE_TEST(true, "av", 10);
SIGNATURE_TEST(true, "a{sv}", 11);
SIGNATURE_TEST(true, "v", 12);
SIGNATURE_TEST(true, "oa{sv}", 13);
SIGNATURE_TEST(true, "a(oa{sv})", 14);
SIGNATURE_TEST(true, "(sa{sv})sa{ss}us", 15);
SIGNATURE_TEST(true, "(bba{ss})", 16);
SIGNATURE_TEST(true, "{sv}", 17);
SIGNATURE_TEST(false, "{vu}", 18);
SIGNATURE_TEST(false, "{uv", 19);
SIGNATURE_TEST(false, "(ss", 20);
SIGNATURE_TEST(false, "aaaaa", 21);
SIGNATURE_TEST(true, "()", 22);

static void test_signature(const void *test_data)
{
	const struct signature_test *test = test_data;
	bool valid;

	valid = _gvariant_valid_signature(test->signature);

	assert(valid == test->valid);
}

struct alignment_test {
	int alignment;
	const char *signature;
};

#define ALIGNMENT_TEST(sig, a, i)				\
	static struct alignment_test align_test##i = {		\
		.alignment = a,					\
		.signature = sig,				\
	}

ALIGNMENT_TEST("()", 1, 1);
ALIGNMENT_TEST("y", 1, 2);
ALIGNMENT_TEST("b", 1, 3);
ALIGNMENT_TEST("s", 1, 4);
ALIGNMENT_TEST("o", 1, 5);
ALIGNMENT_TEST("g", 1, 6);
ALIGNMENT_TEST("q", 2, 7);
ALIGNMENT_TEST("n", 2, 8);
ALIGNMENT_TEST("u", 4, 9);
ALIGNMENT_TEST("h", 4, 10);
ALIGNMENT_TEST("i", 4, 11);
ALIGNMENT_TEST("v", 8, 12);
ALIGNMENT_TEST("t", 8, 13);
ALIGNMENT_TEST("x", 8, 14);
ALIGNMENT_TEST("d", 8, 15);
ALIGNMENT_TEST("ay", 1, 16);
ALIGNMENT_TEST("as", 1, 17);
ALIGNMENT_TEST("au", 4, 18);
ALIGNMENT_TEST("an", 2, 19);
ALIGNMENT_TEST("ans", 2, 20);
ALIGNMENT_TEST("ant", 8, 21);
ALIGNMENT_TEST("(ss)", 1, 22);
ALIGNMENT_TEST("(ssu)", 4, 23);
ALIGNMENT_TEST("a(ssu)", 4, 24);
ALIGNMENT_TEST("(u)", 4, 25);
ALIGNMENT_TEST("(uuuuy)", 4, 26);
ALIGNMENT_TEST("(uusuuy)", 4, 27);
ALIGNMENT_TEST("a{ss}", 1, 28);
ALIGNMENT_TEST("((u)yyy(b(iiii)))", 4, 29);
ALIGNMENT_TEST("((u)yyy(b(iiivi)))", 8, 30);
ALIGNMENT_TEST("((b)(t))", 8, 31);
ALIGNMENT_TEST("((b)(b)(t))", 8, 32);
ALIGNMENT_TEST("(bt)", 8, 33);
ALIGNMENT_TEST("((t)(b))", 8, 34);
ALIGNMENT_TEST("(tb)", 8, 35);
ALIGNMENT_TEST("((b)(b))", 1, 36);
ALIGNMENT_TEST("((t)(t))", 8, 37);

static void test_alignment(const void *test_data)
{
	const struct alignment_test *test = test_data;
	int alignment;

	alignment = _gvariant_get_alignment(test->signature);

	assert(alignment == test->alignment);
}

struct is_fixed_size_test {
	bool fixed_size;
	const char *signature;
};

struct get_fixed_size_test {
	int size;
	const char *signature;
};

#define IS_FIXED_SIZE_TEST(sig, v, i)					\
	static struct is_fixed_size_test is_fixed_size_test##i = {	\
		.fixed_size = v,					\
		.signature = sig,					\
	}

IS_FIXED_SIZE_TEST("", true, 1);
IS_FIXED_SIZE_TEST("()", true, 2);
IS_FIXED_SIZE_TEST("y", true, 3);
IS_FIXED_SIZE_TEST("u", true, 4);
IS_FIXED_SIZE_TEST("b", true, 5);
IS_FIXED_SIZE_TEST("n", true, 6);
IS_FIXED_SIZE_TEST("q", true, 7);
IS_FIXED_SIZE_TEST("i", true, 8);
IS_FIXED_SIZE_TEST("t", true, 9);
IS_FIXED_SIZE_TEST("d", true, 10);
IS_FIXED_SIZE_TEST("s", false, 11);
IS_FIXED_SIZE_TEST("o", false, 12);
IS_FIXED_SIZE_TEST("g", false, 13);
IS_FIXED_SIZE_TEST("h", true, 14);
IS_FIXED_SIZE_TEST("ay", false, 15);
IS_FIXED_SIZE_TEST("v", false, 16);
IS_FIXED_SIZE_TEST("(u)", true, 17);
IS_FIXED_SIZE_TEST("(uuuuy)", true, 18);
IS_FIXED_SIZE_TEST("(uusuuy)", false, 19);
IS_FIXED_SIZE_TEST("a{ss}", false, 20);
IS_FIXED_SIZE_TEST("((u)yyy(b(iiii)))", true, 21);
IS_FIXED_SIZE_TEST("((u)yyy(b(iiivi)))", false, 22);

static void test_is_fixed_size(const void *test_data)
{
	const struct is_fixed_size_test *test = test_data;
	bool fixed_size;

	fixed_size = _gvariant_is_fixed_size(test->signature);

	assert(fixed_size == test->fixed_size);
}

#define GET_FIXED_SIZE_TEST(sig, n, i)				\
	static struct get_fixed_size_test size_test##i = {	\
		.size = n,					\
		.signature = sig,				\
	}

GET_FIXED_SIZE_TEST("", 0, 1);
GET_FIXED_SIZE_TEST("()", 1, 2);
GET_FIXED_SIZE_TEST("y", 1, 3);
GET_FIXED_SIZE_TEST("u", 4, 4);
GET_FIXED_SIZE_TEST("b", 1, 5);
GET_FIXED_SIZE_TEST("n", 2, 6);
GET_FIXED_SIZE_TEST("q", 2, 7);
GET_FIXED_SIZE_TEST("i", 4, 8);
GET_FIXED_SIZE_TEST("t", 8, 9);
GET_FIXED_SIZE_TEST("d", 8, 10);
GET_FIXED_SIZE_TEST("s", 0, 11);
GET_FIXED_SIZE_TEST("o", 0, 12);
GET_FIXED_SIZE_TEST("g", 0, 13);
GET_FIXED_SIZE_TEST("h", 4, 14);
GET_FIXED_SIZE_TEST("ay", 0, 15);
GET_FIXED_SIZE_TEST("v", 0, 16);
GET_FIXED_SIZE_TEST("(u)", 4, 17);
GET_FIXED_SIZE_TEST("(uuuuy)", 20, 18);
GET_FIXED_SIZE_TEST("(uusuuy)", 0, 19);
GET_FIXED_SIZE_TEST("a{ss}", 0, 20);
GET_FIXED_SIZE_TEST("((u)yyy(b(iiii)))", 28, 21);
GET_FIXED_SIZE_TEST("((u)yyy(b(iiivi)))", 0, 22);
GET_FIXED_SIZE_TEST("((b)(t))", 16, 23);
GET_FIXED_SIZE_TEST("((b)(b)(t))", 16, 24);
GET_FIXED_SIZE_TEST("(bt)", 16, 25);
GET_FIXED_SIZE_TEST("((t)(b))", 16, 26);
GET_FIXED_SIZE_TEST("(tb)", 16, 27);
GET_FIXED_SIZE_TEST("((b)(b))", 2, 28);
GET_FIXED_SIZE_TEST("((t)(t))", 16, 29);

static void test_get_fixed_size(const void *test_data)
{
	const struct get_fixed_size_test *test = test_data;
	int size;

	size = _gvariant_get_fixed_size(test->signature);

	assert(size == test->size);
}

struct parser_data {
	const char *data;
	size_t len;
	const char *signature;
};

static const unsigned char basic_data_1[] = {
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x6f, 0x6f, 0x62,
	0x61, 0x72, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x19,
};

static struct parser_data parser_data_1 = {
	.data = basic_data_1,
	.len = 33,
	.signature = "(uvu)i",
};

static void test_iter_basic_1(const void *test_data)
{
	const struct parser_data *test = test_data;
	struct gvariant_iter iter;

	_gvariant_iter_init(&iter, test->signature,
				test->signature + strlen(test->signature),
				test->data, test->len);
	_gvariant_iter_free(&iter);
}

static const unsigned char basic_data_2[] = {
	0x05, 0x00, 0x00, 0x00, 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72, 0x00,
};

static struct parser_data parser_data_2 = {
	.data = basic_data_2,
	.len = 11,
	.signature = "is",
};

static void test_iter_basic_2(const void *test_data)
{
	const struct parser_data *test = test_data;
	struct gvariant_iter iter;
	const char *s;
	int i;
	bool ret;

	_gvariant_iter_init(&iter, test->signature,
				test->signature + strlen(test->signature),
				test->data, test->len);

	ret = _gvariant_iter_next_entry_basic(&iter, 'i', &i);
	assert(ret);
	assert(i == 5);

	ret = _gvariant_iter_next_entry_basic(&iter, 's', &s);
	assert(ret);
	assert(!strcmp(s, "foobar"));

	_gvariant_iter_free(&iter);
}

static const unsigned char basic_data_3[] = {
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x14, 0x40, 0xdf, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1c, 0xaf, 0x7d, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x21, 0x7f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xe4, 0xd4, 0x59, 0xfd, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x02, 0xad, 0x31, 0x00, 0x66, 0x6f, 0x6f, 0x62,
	0x61, 0x72, 0x00, 0x00, 0xfe, 0x52, 0xce, 0xff, 0x3f,
};

static struct parser_data parser_data_3 = {
	.data = basic_data_3,
	.len = 69,
	.signature = "bdntqxyusi",
};

static void test_iter_basic_3(const void *test_data)
{
	const struct parser_data *test = test_data;
	struct gvariant_iter iter;
	bool b;
	double d;
	int16_t n;
	uint64_t t;
	uint16_t q;
	int64_t x;
	uint8_t y;
	uint32_t u;
	const char *s;
	int32_t i;
	bool ret;

	_gvariant_iter_init(&iter, test->signature,
				test->signature + strlen(test->signature),
				test->data, test->len);

	ret = _gvariant_iter_next_entry_basic(&iter, 'd', &b);
	assert(ret == false);

	ret = _gvariant_iter_next_entry_basic(&iter, 'b', &b);
	assert(ret);
	assert(b == true);

	ret = _gvariant_iter_next_entry_basic(&iter, 'd', &d);
	assert(ret);
	assert(fabs(d - 5.0) < DBL_EPSILON);
	assert(d == 5.0);

	ret = _gvariant_iter_next_entry_basic(&iter, 'n', &n);
	assert(ret);
	assert(n == -32545);

	ret = _gvariant_iter_next_entry_basic(&iter, 't', &t);
	assert(ret);
	assert(t == 444444444LL);

	ret == _gvariant_iter_next_entry_basic(&iter, 'q', &q);
	assert(ret);
	assert(q == 32545);

	ret = _gvariant_iter_next_entry_basic(&iter, 'x', &x);
	assert(ret);
	assert(x == -44444444LL);

	ret = _gvariant_iter_next_entry_basic(&iter, 'y', &y);
	assert(ret);
	assert(y == 255);

	ret = _gvariant_iter_next_entry_basic(&iter, 'u', &u);
	assert(ret);
	assert(u == 3255554);

	ret = _gvariant_iter_next_entry_basic(&iter, 's', &s);
	assert(ret);
	assert(!strcmp(s, "foobar"));

	ret = _gvariant_iter_next_entry_basic(&iter, 'i', &i);
	assert(ret);
	assert(i == -3255554);

	_gvariant_iter_free(&iter);
}

static const unsigned char fixed_struct_data_1[] = {
	0x0a, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00,
};

static struct parser_data fixed_struct_1 = {
	.data = fixed_struct_data_1,
	.len = 8,
	.signature = "i(yy)",
};

static void test_iter_fixed_struct_1(const void *test_data)
{
	const struct parser_data *test = test_data;
	struct gvariant_iter iter;
	int32_t i;
	uint8_t y;
	bool ret;
	struct gvariant_iter structure;

	_gvariant_iter_init(&iter, test->signature,
				test->signature + strlen(test->signature),
				test->data, test->len);

	ret = _gvariant_iter_next_entry_basic(&iter, 'i', &i);
	assert(ret);
	assert(i == 10);

	ret = _gvariant_iter_enter_struct(&iter, &structure);
	assert(ret);

	ret = _gvariant_iter_next_entry_basic(&structure, 'y', &y);
	assert(ret);
	assert(y == 255);

	ret = _gvariant_iter_next_entry_basic(&structure, 'y', &y);
	assert(ret);
	assert(y == 1);

	_gvariant_iter_free(&structure);
	_gvariant_iter_free(&iter);
}

static const unsigned char variant_data_1[] = {
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x6f, 0x6f, 0x62,
	0x61, 0x72, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x19,
};

static struct parser_data variant_1 = {
	.data = variant_data_1,
	.len = 33,
	.signature = "(uvu)i",
};

static void test_iter_variant_1(const void *test_data)
{
	const struct parser_data *test = test_data;
	struct gvariant_iter iter;
	struct gvariant_iter structure;
	struct gvariant_iter variant;
	int32_t i;
	uint32_t u;
	const char *s;
	bool ret;

	_gvariant_iter_init(&iter, test->signature,
				test->signature + strlen(test->signature),
				test->data, test->len);

	ret = _gvariant_iter_enter_struct(&iter, &structure);
	assert(ret);

	ret = _gvariant_iter_next_entry_basic(&structure, 'u', &u);
	assert(ret);
	assert(u == 5);

	ret = _gvariant_iter_enter_variant(&structure, &variant);
	assert(ret);

	ret = _gvariant_iter_next_entry_basic(&variant, 's', &s);
	assert(ret);
	assert(!strcmp(s, "foobar"));

	ret = _gvariant_iter_next_entry_basic(&structure, 'u', &u);
	assert(ret);
	assert(u == 5);

	ret = _gvariant_iter_next_entry_basic(&iter, 'i', &i);
	assert(ret);
	assert(i == 5);

	_gvariant_iter_free(&variant);
	_gvariant_iter_free(&structure);
	_gvariant_iter_free(&iter);
}

static const unsigned char variant_data_2[] = {
	0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
	0xff, 0x07, 0x00, 0x28, 0x73, 0x75, 0x79, 0x29,
};

static struct parser_data variant_2 = {
	.data = variant_data_2,
	.len = 20,
	.signature = "v",
};

static void test_iter_variant_2(const void *test_data)
{
	const struct parser_data *test = test_data;
	struct gvariant_iter iter;
	struct gvariant_iter variant;
	struct gvariant_iter structure;
	uint8_t y;
	uint32_t u;
	const char *s;
	bool ret;

	_gvariant_iter_init(&iter, test->signature,
				test->signature + strlen(test->signature),
				test->data, test->len);

	ret = _gvariant_iter_enter_variant(&iter, &variant);
	assert(ret);

	ret = _gvariant_iter_enter_struct(&variant, &structure);

	ret = _gvariant_iter_next_entry_basic(&structure, 's', &s);
	assert(ret);
	assert(!strcmp(s, "foobar"));

	ret = _gvariant_iter_next_entry_basic(&structure, 'u', &u);
	assert(ret);
	assert(u == 20);

	ret = _gvariant_iter_next_entry_basic(&structure, 'y', &y);
	assert(ret);
	assert(y == 255);

	_gvariant_iter_free(&structure);
	_gvariant_iter_free(&variant);
	_gvariant_iter_free(&iter);
}

int main(int argc, char *argv[])
{
	l_test_init(&argc, &argv);

	l_test_add("Signature Test 1", test_signature, &sig_test1);
	l_test_add("Signature test 2", test_signature, &sig_test2);
	l_test_add("Signature test 3", test_signature, &sig_test3);
	l_test_add("Signature test 4", test_signature, &sig_test4);
	l_test_add("Signature test 5", test_signature, &sig_test5);
	l_test_add("Signature test 6", test_signature, &sig_test6);
	l_test_add("Signature test 7", test_signature, &sig_test7);
	l_test_add("Signature test 8", test_signature, &sig_test8);
	l_test_add("Signature test 9", test_signature, &sig_test9);
	l_test_add("Signature test 10", test_signature, &sig_test10);
	l_test_add("Signature test 11", test_signature, &sig_test11);
	l_test_add("Signature test 12", test_signature, &sig_test12);
	l_test_add("Signature test 13", test_signature, &sig_test13);
	l_test_add("Signature test 14", test_signature, &sig_test14);
	l_test_add("Signature test 15", test_signature, &sig_test15);
	l_test_add("Signature test 16", test_signature, &sig_test16);
	l_test_add("Signature test 17", test_signature, &sig_test17);
	l_test_add("Signature test 18", test_signature, &sig_test18);
	l_test_add("Signature test 19", test_signature, &sig_test19);
	l_test_add("Signature test 20", test_signature, &sig_test20);
	l_test_add("Signature test 21", test_signature, &sig_test21);
	l_test_add("Signature test 22", test_signature, &sig_test22);

	l_test_add("Alignment test 1", test_alignment, &align_test1);
	l_test_add("Alignment test 2", test_alignment, &align_test2);
	l_test_add("Alignment test 3", test_alignment, &align_test3);
	l_test_add("Alignment test 4", test_alignment, &align_test4);
	l_test_add("Alignment test 5", test_alignment, &align_test5);
	l_test_add("Alignment test 6", test_alignment, &align_test6);
	l_test_add("Alignment test 7", test_alignment, &align_test7);
	l_test_add("Alignment test 8", test_alignment, &align_test8);
	l_test_add("Alignment test 9", test_alignment, &align_test9);
	l_test_add("Alignment test 10", test_alignment, &align_test10);
	l_test_add("Alignment test 11", test_alignment, &align_test11);
	l_test_add("Alignment test 12", test_alignment, &align_test12);
	l_test_add("Alignment test 13", test_alignment, &align_test13);
	l_test_add("Alignment test 14", test_alignment, &align_test14);
	l_test_add("Alignment test 15", test_alignment, &align_test15);
	l_test_add("Alignment test 16", test_alignment, &align_test16);
	l_test_add("Alignment test 17", test_alignment, &align_test17);
	l_test_add("Alignment test 18", test_alignment, &align_test18);
	l_test_add("Alignment test 19", test_alignment, &align_test19);
	l_test_add("Alignment test 20", test_alignment, &align_test20);
	l_test_add("Alignment test 21", test_alignment, &align_test21);
	l_test_add("Alignment test 22", test_alignment, &align_test22);
	l_test_add("Alignment test 23", test_alignment, &align_test23);
	l_test_add("Alignment test 24", test_alignment, &align_test24);
	l_test_add("Alignment test 25", test_alignment, &align_test25);
	l_test_add("Alignment test 26", test_alignment, &align_test26);
	l_test_add("Alignment test 27", test_alignment, &align_test27);
	l_test_add("Alignment test 28", test_alignment, &align_test28);
	l_test_add("Alignment test 29", test_alignment, &align_test29);
	l_test_add("Alignment test 30", test_alignment, &align_test30);
	l_test_add("Alignment test 31", test_alignment, &align_test31);
	l_test_add("Alignment test 32", test_alignment, &align_test32);
	l_test_add("Alignment test 33", test_alignment, &align_test33);
	l_test_add("Alignment test 34", test_alignment, &align_test34);
	l_test_add("Alignment test 35", test_alignment, &align_test35);
	l_test_add("Alignment test 36", test_alignment, &align_test36);
	l_test_add("Alignment test 37", test_alignment, &align_test37);

	l_test_add("Is Fixed Size test 1", test_is_fixed_size,
			&is_fixed_size_test1);
	l_test_add("Is Fixed Size test 2", test_is_fixed_size,
			&is_fixed_size_test2);
	l_test_add("Is Fixed Size test 3", test_is_fixed_size,
			&is_fixed_size_test3);
	l_test_add("Is Fixed Size test 4", test_is_fixed_size,
			&is_fixed_size_test4);
	l_test_add("Is Fixed Size test 4", test_is_fixed_size,
			&is_fixed_size_test4);
	l_test_add("Is Fixed Size test 5", test_is_fixed_size,
			&is_fixed_size_test5);
	l_test_add("Is Fixed Size test 6", test_is_fixed_size,
			&is_fixed_size_test6);
	l_test_add("Is Fixed Size test 7", test_is_fixed_size,
			&is_fixed_size_test7);
	l_test_add("Is Fixed Size test 8", test_is_fixed_size,
			&is_fixed_size_test8);
	l_test_add("Is Fixed Size test 9", test_is_fixed_size,
			&is_fixed_size_test9);
	l_test_add("Is Fixed Size test 10", test_is_fixed_size,
			&is_fixed_size_test10);
	l_test_add("Is Fixed Size test 11", test_is_fixed_size,
			&is_fixed_size_test11);
	l_test_add("Is Fixed Size test 12", test_is_fixed_size,
			&is_fixed_size_test12);
	l_test_add("Is Fixed Size test 13", test_is_fixed_size,
			&is_fixed_size_test13);
	l_test_add("Is Fixed Size test 14", test_is_fixed_size,
			&is_fixed_size_test14);
	l_test_add("Is Fixed Size test 15", test_is_fixed_size,
			&is_fixed_size_test15);
	l_test_add("Is Fixed Size test 16", test_is_fixed_size,
			&is_fixed_size_test16);
	l_test_add("Is Fixed Size test 17", test_is_fixed_size,
			&is_fixed_size_test17);
	l_test_add("Is Fixed Size test 18", test_is_fixed_size,
			&is_fixed_size_test18);
	l_test_add("Is Fixed Size test 19", test_is_fixed_size,
			&is_fixed_size_test19);
	l_test_add("Is Fixed Size test 20", test_is_fixed_size,
			&is_fixed_size_test20);
	l_test_add("Is Fixed Size test 21", test_is_fixed_size,
			&is_fixed_size_test21);
	l_test_add("Is Fixed Size test 22", test_is_fixed_size,
			&is_fixed_size_test22);

	l_test_add("Get Fixed Size test 1", test_get_fixed_size, &size_test1);
	l_test_add("Get Fixed Size test 2", test_get_fixed_size, &size_test2);
	l_test_add("Get Fixed Size test 3", test_get_fixed_size, &size_test3);
	l_test_add("Get Fixed Size test 4", test_get_fixed_size, &size_test4);
	l_test_add("Get Fixed Size test 5", test_get_fixed_size, &size_test5);
	l_test_add("Get Fixed Size test 6", test_get_fixed_size, &size_test6);
	l_test_add("Get Fixed Size test 7", test_get_fixed_size, &size_test7);
	l_test_add("Get Fixed Size test 8", test_get_fixed_size, &size_test8);
	l_test_add("Get Fixed Size test 9", test_get_fixed_size, &size_test9);
	l_test_add("Get Fixed Size test 10", test_get_fixed_size, &size_test10);
	l_test_add("Get Fixed Size test 11", test_get_fixed_size, &size_test11);
	l_test_add("Get Fixed Size test 12", test_get_fixed_size, &size_test12);
	l_test_add("Get Fixed Size test 13", test_get_fixed_size, &size_test13);
	l_test_add("Get Fixed Size test 14", test_get_fixed_size, &size_test14);
	l_test_add("Get Fixed Size test 15", test_get_fixed_size, &size_test15);
	l_test_add("Get Fixed Size test 16", test_get_fixed_size, &size_test16);
	l_test_add("Get Fixed Size test 17", test_get_fixed_size, &size_test17);
	l_test_add("Get Fixed Size test 18", test_get_fixed_size, &size_test18);
	l_test_add("Get Fixed Size test 19", test_get_fixed_size, &size_test19);
	l_test_add("Get Fixed Size test 20", test_get_fixed_size, &size_test20);
	l_test_add("Get Fixed Size test 21", test_get_fixed_size, &size_test21);
	l_test_add("Get Fixed Size test 22", test_get_fixed_size, &size_test22);
	l_test_add("Get Fixed Size test 23", test_get_fixed_size, &size_test23);
	l_test_add("Get Fixed Size test 24", test_get_fixed_size, &size_test24);
	l_test_add("Get Fixed Size test 25", test_get_fixed_size, &size_test25);
	l_test_add("Get Fixed Size test 26", test_get_fixed_size, &size_test26);
	l_test_add("Get Fixed Size test 27", test_get_fixed_size, &size_test27);
	l_test_add("Get Fixed Size test 28", test_get_fixed_size, &size_test28);
	l_test_add("Get Fixed Size test 29", test_get_fixed_size, &size_test29);

	l_test_add("Iter Test Basic 1", test_iter_basic_1, &parser_data_1);
	l_test_add("Iter Test Basic 2", test_iter_basic_2, &parser_data_2);
	l_test_add("Iter Test Basic 3", test_iter_basic_3, &parser_data_3);
	l_test_add("Iter Test Fixed Struct 1", test_iter_fixed_struct_1,
			&fixed_struct_1);

	l_test_add("Iter Test Variant 1", test_iter_variant_1, &variant_1);
	l_test_add("Iter Test Variant 2", test_iter_variant_2, &variant_2);

	return l_test_run();
}