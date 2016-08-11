/*
 * Copyright (c), Edward Thomson <ethomson@edwardthomson.com>
 * All rights reserved.
 *
 * This file is part of adopt, distributed under the MIT license.
 * For full terms and conditions, see the included LICENSE file.
 */

#include <stdio.h>

#include "adopt.h"

adopt_spec opt_specs[] = {
	{ ADOPT_VALUE, "debug", 'd', NULL, "display debug information" },
	{ ADOPT_SWITCH, "help", 0, NULL, NULL, ADOPT_USAGE_HIDDEN },
	{ ADOPT_VALUE, "verbose", 'v', "level", "sets the verbosity level (default 1)" },
	{ ADOPT_VALUE, "channel", 'c', "channel", "sets the channel", ADOPT_USAGE_REQUIRED },
	{ ADOPT_LITERAL },
	{ ADOPT_ARG, "file", 'f', NULL, "file path" },
};

int main(int argc, const char **argv)
{
	adopt_parser parser;
	adopt_opt opt;

	adopt_parser_init(&parser, opt_specs, argv + 1, argc - 1);

	while (adopt_parser_next(&opt, &parser)) {
		if (opt.spec) {
			printf("'%s' = ", opt.spec->name);
			printf("'%s'\n", opt.value ? opt.value : "(null)");
		} else {
			fprintf(stderr, "Unknown option: %s\n", opt.value);
			adopt_usage_fprint(stderr, argv[0], opt_specs);
			return 129;
		}
	}
}

