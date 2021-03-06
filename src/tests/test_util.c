/* This file is part of the chess engine lisco.
 *
 * Copyright (C) 2002-2021 cantanea EOOD.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <check.h>

#include "xalloc.h"
#include "xmalloca.h"

#include "util.h"

START_TEST(test_trim)
{
	char *string;
	char *wanted;
	char *got;

	string = xstrdup("foo  bar\tbaz");
	wanted = string;
	got = trim(string);
	ck_assert_str_eq(got, string);
	ck_assert_ptr_eq(got, string);
	free(string);

	string = xstrdup("\t  foo  bar\tbaz   \t\t   ");
	wanted = "foo  bar\tbaz";
	got = trim(string);
	ck_assert_str_eq(got, wanted);
	ck_assert_ptr_eq(got, string + 3);
	free(string);

	string = NULL;
	wanted = NULL;
	got = trim(NULL);
	ck_assert_ptr_null(got);
}
END_TEST

START_TEST(test_ltrim)
{
	const char *string;
	const char *wanted;
	const char *got;

	string = "foo  bar\tbaz";
	wanted = string;
	got = ltrim(string);
	ck_assert_str_eq(got, string);
	ck_assert_ptr_eq(got, string);

	string = "\t  foo  bar\tbaz   \t\t   ";
	wanted = "foo  bar\tbaz   \t\t   ";
	got = ltrim(string);
	ck_assert_str_eq(got, wanted);
	ck_assert_ptr_eq(got, string + 3);

	string = NULL;
	wanted = NULL;
	got = trim(NULL);
	ck_assert_ptr_null(got);
}
END_TEST

START_TEST(test_num_cpus)
{
	ck_assert_int_gt(num_cpus(), 0);
}
END_TEST

Suite *
util_suite(void)
{
	Suite *suite;
	TCase *tc_trim;
	TCase *tc_os;

	suite = suite_create("Utility Functions");

	tc_trim = tcase_create("Trimming");
	tcase_add_test(tc_trim, test_trim);
	tcase_add_test(tc_trim, test_ltrim);
	suite_add_tcase(suite, tc_trim);

	tc_os = tcase_create("OS Features");
	tcase_add_test(tc_os, test_num_cpus);
	suite_add_tcase(suite, tc_os);

	return suite;
}
