/*
   Stand-alone MAPI testsuite

   OpenChange Project

   Copyright (C) Julien Kerihuel 2008

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libmapi/libmapi.h>
#include "utils/mapitest/mapitest.h"

/**
	\file
	mapitest statistics functions

	mapitest records and prints the results of each test
	using these functions
*/

/**
   \details Initialize the mapitest statistic structure
   
   \param mem_ctx memory allocation context

   \return Allocated stat structure on success, otherwise NULL
 */
_PUBLIC_ struct mapitest_stat *mapitest_stat_init(TALLOC_CTX *mem_ctx)
{
	struct mapitest_stat	*stat = NULL;

	/* Sanity check */
	if (!mem_ctx) return NULL;

	stat = talloc_zero(mem_ctx, struct mapitest_stat);
	stat->success = 0;
	stat->failure = 0;
	stat->failure_info = NULL;
	stat->enabled = false;

	return stat;
}


/**
   \details Add test result to the suite statistic parameter

   \param suite the suite container
   \param name the test name
   \param result the test result

   \return MAPITEST_SUCCESS on success, otherwise MAPITEST_ERROR
 */
_PUBLIC_ uint32_t mapitest_stat_add_result(struct mapitest_suite *suite,
					   const char *name,
					   bool result)
{
	struct mapitest_unit	*el = NULL;

	/* Sanity check */
	if (!suite || !suite->stat || !name) return MAPITEST_ERROR;

	if (result == true) {
		suite->stat->success++;
	} else {
		suite->stat->failure++;
		el = talloc_zero((TALLOC_CTX *) suite->stat, struct mapitest_unit);
		el->name = talloc_strdup((TALLOC_CTX *)el, (char *)name);
		DLIST_ADD_END(suite->stat->failure_info, el, struct mapitest_unit *);
	}

	suite->stat->enabled = true;

	return MAPITEST_SUCCESS;
}


/**
   \details Dump mapitest statistics about test failures

   \param mt the global mapitest structure

   \return the number of test steps that failed (i.e. 0 for all success)
 */
_PUBLIC_ int32_t mapitest_stat_dump(struct mapitest *mt)
{
	struct mapitest_suite	*suite;
	struct mapitest_unit	*el;
	int32_t 		num_passed_tests = 0;
	int32_t 		num_failed_tests = 0;

	mapitest_print_title(mt, MT_STAT_TITLE, MODULE_TITLE_DELIM);

	for (suite = mt->mapi_suite; suite; suite = suite->next) {
		if (suite->stat->enabled == true) {
			num_passed_tests += suite->stat->success;
			num_failed_tests += suite->stat->failure;
			if (suite->stat->failure) {
				for (el = suite->stat->failure_info; el; el = el->next) {
					mapitest_print(mt, MT_STAT_FAILURE, suite->name, el->name);
				}
			}
		}
	}

	mapitest_print_test_title_end(mt);

	mapitest_print_title(mt, MT_SUMMARY_TITLE, MODULE_TITLE_DELIM);
	mapitest_print(mt, "Number of passing tests: %i\n", num_passed_tests);
	mapitest_print(mt, "Number of failing tests: %i\n", num_failed_tests);
	mapitest_print_test_title_end(mt);

	return num_failed_tests;
}
