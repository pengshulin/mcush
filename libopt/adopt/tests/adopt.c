#include "clar.h"
#include "adopt.h"

static void test_parse(adopt_spec *specs, const char *args[], size_t argslen, adopt_opt expected[], size_t expectlen)
{
	adopt_parser parser;
	adopt_opt opt;
	size_t i;

	adopt_parser_init(&parser, specs, args, argslen);

	for (i = 0; i < expectlen; ++i) {
		cl_assert(adopt_parser_next(&opt, &parser) == 1);

		cl_assert_equal_p(expected[i].spec, opt.spec);

		if (expected[i].value)
			cl_assert_equal_s(expected[i].value, opt.value);
		else
			cl_assert(opt.value == NULL);
	}

	cl_assert(adopt_parser_next(&opt, &parser) == 0);
}

void test_adopt__empty(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "bar" },
		{ 0 }
	};

	/* Parse an empty arg list */
	test_parse(specs, NULL, 0, NULL, 0);
}

void test_adopt__args(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "bar" },
		{ 0 }
	};

	const char *args[] = { "bare1", "bare2" };
	adopt_opt expected[] = {
		{ NULL, "bare1" },
		{ NULL, "bare2" },
	};

	/* Parse an arg list with only bare arguments */
	test_parse(specs, args, 2, expected, 2);
}

void test_adopt__unknown(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "bar" },
		{ 0 }
	};

	const char *args[] = { "--unknown-long", "-u" };
	adopt_opt expected[] = {
		{ NULL, "--unknown-long" },
		{ NULL, "-u" },
	};

	/* Parse an arg list with only bare arguments */
	test_parse(specs, args, 2, expected, 2);
}

void test_adopt__long_switches1(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "bar" },
		{ 0 }
	};

	const char *args1[] = { "--foo", "bare1" };
	adopt_opt expected1[] = {
		{ &specs[0], NULL },
		{ NULL, "bare1" },
	};

	/* Parse --foo bare1 */
	test_parse(specs, args1, 2, expected1, 2);
}

void test_adopt__long_switches2(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "bar" },
		{ 0 }
	};

	const char *args2[] = { "--foo", "--bar" };
	adopt_opt expected2[] = {
		{ &specs[0], NULL },
		{ &specs[1], NULL }
	};
	
	/* Parse --foo --bar */
	test_parse(specs, args2, 2, expected2, 2);
}


void test_adopt__long_switches3(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "bar" },
		{ 0 }
	};

	const char *args3[] = { "--foo", "bare2", "--bar", "-u" };
	adopt_opt expected3[] = {
		{ &specs[0], NULL },
		{ NULL, "bare2" },
		{ &specs[1], NULL },
		{ NULL, "-u" },
	};
	
	/* Parse --foo bare2 --bar -u */
	test_parse(specs, args3, 4, expected3, 4);
}

void test_adopt__long_values1(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo" },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};
	
	const char *args1[] = { "--foo", "arg_1" };
	adopt_opt expected1[] = {
		{ &specs[0], "arg_1" },
	};

	/* Parse --foo arg_1 */
	test_parse(specs, args1, 2, expected1, 1);
}

void test_adopt__long_values2(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo" },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};
	
	const char *args2[] = { "--foo", "--bar" };
	adopt_opt expected2[] = {
		{ &specs[0], "--bar" },
	};

	/* Parse --foo --bar */
	test_parse(specs, args2, 2, expected2, 1);
}

void test_adopt__long_values3(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo" },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};
	
	const char *args3[] = { "--foo", "--arg_1", "--bar", "arg_2" };
	adopt_opt expected3[] = {
		{ &specs[0], "--arg_1" },
		{ &specs[1], "arg_2" },
	};

	/* Parse --foo --arg_1 --bar arg_2 */
	test_parse(specs, args3, 4, expected3, 2);
}

void test_adopt__long_values4(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo" },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};
	
	const char *args4[] = { "--foo=bar" };
	adopt_opt expected4[] = {
		{ &specs[0], "bar" },
	};

	/* Parse --foo=bar */
	test_parse(specs, args4, 1, expected4, 1);
}

void test_adopt__long_values5(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo" },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};

	const char *args5[] = { "--bar=" };
	adopt_opt expected5[] = {
		{ &specs[1], "" },
	};

	/* Parse --bar= */
	test_parse(specs, args5, 1, expected5, 1);
}

void test_adopt__short_switches1(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo", 'f' },
		{ ADOPT_SWITCH, "bar", 'b' },
		{ 0 }
	};
	
	const char *args1[] = { "-f", "bare1" };
	adopt_opt expected1[] = {
		{ &specs[0], NULL },
		{ NULL, "bare1" },
	};

	/* Parse -foo bare1 */
	test_parse(specs, args1, 2, expected1, 2);
}

void test_adopt__short_switches2(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo", 'f' },
		{ ADOPT_SWITCH, "bar", 'b' },
		{ 0 }
	};
	
	const char *args2[] = { "-f", "-b" };
	adopt_opt expected2[] = {
		{ &specs[0], NULL },
		{ &specs[1], NULL }
	};

	/* Parse -foo -b */
	test_parse(specs, args2, 2, expected2, 2);
}

void test_adopt__short_switches3(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo", 'f' },
		{ ADOPT_SWITCH, "bar", 'b' },
		{ 0 }
	};

	const char *args3[] = { "-f", "bare2", "-b", "-u" };
	adopt_opt expected3[] = {
		{ &specs[0], NULL },
		{ NULL, "bare2" },
		{ &specs[1], NULL },
		{ NULL, "-u" },
	};

	/* Parse -f bare2 -b -u */
	test_parse(specs, args3, 4, expected3, 4);
}

void test_adopt__short_values1(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo", 'f' },
		{ ADOPT_VALUE, "bar", 'b' },
		{ 0 }
	};
	
	const char *args1[] = { "-f", "arg_1" };
	adopt_opt expected1[] = {
		{ &specs[0], "arg_1" },
	};

	/* Parse -f arg_1 */
	test_parse(specs, args1, 2, expected1, 1);
}

void test_adopt__short_values2(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo", 'f' },
		{ ADOPT_VALUE, "bar", 'b' },
		{ 0 }
	};
	
	const char *args2[] = { "-f", "--bar" };
	adopt_opt expected2[] = {
		{ &specs[0], "--bar" },
	};

	/* Parse -f -b */
	test_parse(specs, args2, 2, expected2, 1);
}

void test_adopt__short_values3(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo", 'f' },
		{ ADOPT_VALUE, "bar", 'b' },
		{ 0 }
	};

	const char *args3[] = { "-f", "--arg_1", "-b", "arg_2" };
	adopt_opt expected3[] = {
		{ &specs[0], "--arg_1" },
		{ &specs[1], "arg_2" },
	};

	/* Parse -f --arg_1 -b arg_2 */
	test_parse(specs, args3, 4, expected3, 2);
}

void test_adopt__short_values4(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo", 'f' },
		{ ADOPT_VALUE, "bar", 'b' },
		{ 0 }
	};

	const char *args4[] = { "-fbar" };
	adopt_opt expected4[] = {
		{ &specs[0], "bar" },
	};

	/* Parse -fbar */
	test_parse(specs, args4, 1, expected4, 1);
}

void test_adopt__short_values5(void)
{
	adopt_spec specs[] = {
		{ ADOPT_VALUE, "foo", 'f' },
		{ ADOPT_VALUE, "bar", 'b' },
		{ 0 }
	};

	const char *args5[] = { "-b" };
	adopt_opt expected5[] = {
		{ &specs[1], NULL },
	};

	/* Parse -b */
	test_parse(specs, args5, 1, expected5, 1);
}

void test_adopt__literal(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_LITERAL },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};

	const char *args1[] = { "--foo", "--", "--bar" };
	adopt_opt expected1[] = {
		{ &specs[0], NULL },
		{ &specs[1], NULL },
		{ NULL, "--bar" },
	};

	/* Parse --foo -- --bar */
	test_parse(specs, args1, 3, expected1, 3);
}

void test_adopt__internal_literal(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, "foo" },
		{ ADOPT_SWITCH, "" },
		{ ADOPT_VALUE, "bar" },
		{ 0 }
	};

	const char *args1[] = { "--foo", "--", "--bar" };
	adopt_opt expected1[] = {
		{ &specs[0], NULL },
		{ &specs[1], NULL },
		{ &specs[2], NULL },
	};

	/* Parse --foo -- --bar */
	test_parse(specs, args1, 3, expected1, 3);
}

void test_adopt__no_long_argument(void)
{
	adopt_spec specs[] = {
		{ ADOPT_SWITCH, NULL, 's' },
		{ ADOPT_SWITCH, "long", 0 },
		{ 0 },
	};

	const char *args1[] = { "-s", "--long" };
	adopt_opt expected1[] = {
		{ &specs[0], NULL },
		{ &specs[1], NULL },
	};

	/* Parse -z */
	test_parse(specs, args1, 2, expected1, 2);
}

