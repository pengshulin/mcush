/*
 * Copyright (c), Edward Thomson <ethomson@edwardthomson.com>
 * All rights reserved.
 *
 * This file is part of adopt, distributed under the MIT license.
 * For full terms and conditions, see the included LICENSE file.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "adopt.h"

#ifdef _WIN32
# include <Windows.h>
#else
# include <fcntl.h>
# include <sys/ioctl.h>
#endif

#ifdef _MSC_VER
# define INLINE(type) static __inline type
#else
# define INLINE(type) static inline type
#endif

INLINE(const adopt_spec *) spec_byname(
	adopt_parser *parser, const char *name, size_t namelen)
{
	const adopt_spec *spec;

	for (spec = parser->specs; spec->type; ++spec) {
		if (spec->type == ADOPT_LITERAL && namelen == 0)
			return spec;

		if ((spec->type == ADOPT_SWITCH || spec->type == ADOPT_VALUE) &&
			spec->name &&
			strlen(spec->name) == namelen &&
			strncmp(name, spec->name, namelen) == 0)
			return spec;
	}

	return NULL;
}

INLINE(const adopt_spec *) spec_byalias(adopt_parser *parser, char alias)
{
	const adopt_spec *spec;

	for (spec = parser->specs; spec->type; ++spec) {
		if ((spec->type == ADOPT_SWITCH || spec->type == ADOPT_VALUE) &&
			alias == spec->alias)
			return spec;
	}

	return NULL;
}

INLINE(const adopt_spec *) spec_nextarg(adopt_parser *parser)
{
	const adopt_spec *spec;
	size_t args = 0;
	
	for (spec = parser->specs; spec->type; ++spec) {
		if (spec->type == ADOPT_ARG) {
			if (args == parser->arg_idx) {
				parser->arg_idx++;
				return spec;
			}
			
			args++;
		}
	}
	
	return NULL;
}

static void parse_long(adopt_opt *opt, adopt_parser *parser)
{
	const adopt_spec *spec;
	const char *arg = parser->args[parser->idx++], *name = arg + 2, *eql;
	size_t namelen;

	namelen = (eql = strrchr(arg, '=')) ? (size_t)(eql - name) : strlen(name);

	if ((spec = spec_byname(parser, name, namelen)) == NULL) {
		opt->spec = NULL;
		opt->value = arg;

		return;
	}

	opt->spec = spec;

	/* Future options parsed as literal */
	if (spec->type == ADOPT_LITERAL) {
		parser->in_literal = 1;
	}

	/* Parse values as "--foo=bar" or "--foo bar" */
	if (spec->type == ADOPT_VALUE) {
		if (eql)
			opt->value = eql + 1;
		else if ((parser->idx + 1) <= parser->args_len)
			opt->value = parser->args[parser->idx++];
	}
}

static void parse_short(adopt_opt *opt, adopt_parser *parser)
{
	const adopt_spec *spec;
	const char *arg = parser->args[parser->idx++], alias = *(arg + 1);

	if ((spec = spec_byalias(parser, alias)) == NULL) {
		opt->spec = NULL;
		opt->value = arg;

		return;
	}

	opt->spec = spec;

	/* Parse values as "-ifoo" or "-i foo" */
	if (spec->type == ADOPT_VALUE) {
		if (strlen(arg) > 2)
			opt->value = arg + 2;
		else if ((parser->idx + 1) <= parser->args_len)
			opt->value = parser->args[parser->idx++];
	}
}

static void parse_arg(adopt_opt *opt, adopt_parser *parser)
{
	opt->spec = spec_nextarg(parser);
	opt->value = parser->args[parser->idx++];
}

void adopt_parser_init(
	adopt_parser *parser,
	const adopt_spec specs[],
	const char **args,
	size_t args_len)
{
	assert(parser);

	memset(parser, 0x0, sizeof(adopt_parser));

	parser->specs = specs;
	parser->args = args;
	parser->args_len = args_len;
}

int adopt_parser_next(adopt_opt *opt, adopt_parser *parser)
{
	assert(opt && parser);

	memset(opt, 0x0, sizeof(adopt_opt));

	if (parser->idx >= parser->args_len)
		return 0;

	/* Handle arguments in long form, those beginning with "--" */
	if (strncmp(parser->args[parser->idx], "--", 2) == 0 &&
		!parser->in_literal)
		parse_long(opt, parser);

	/* Handle arguments in short form, those beginning with "-" */
	else if (strncmp(parser->args[parser->idx], "-", 1) == 0 &&
		!parser->in_literal)
		parse_short(opt, parser);

	/* Handle "free" arguments, those without a dash */
	else
		parse_arg(opt, parser);

	return 1;
}

int adopt_usage_fprint(
	FILE *file,
	const char *command,
	const adopt_spec specs[])
{
	const adopt_spec *spec;
	int error;

	if ((error = fprintf(file, "usage: %s", command)) < 0)
		goto done;

	for (spec = specs; spec->type; ++spec) {
		int required = (spec->usage & ADOPT_USAGE_REQUIRED);
		int value_required = (spec->usage & ADOPT_USAGE_VALUE_REQUIRED);

		if (spec->usage & ADOPT_USAGE_HIDDEN)
			continue;

		if ((error = fprintf(file, " ")) < 0)
			goto done;

		if (spec->type == ADOPT_VALUE && value_required && spec->alias)
			error = fprintf(file, "[-%c <%s>]", spec->alias, spec->value);
		else if (spec->type == ADOPT_VALUE && value_required)
			error = fprintf(file, "[--%s=<%s>]", spec->name, spec->value);
		else if (spec->type == ADOPT_VALUE)
			error = fprintf(file, "[--%s[=<%s>]]", spec->name, spec->value);
		else if (spec->type == ADOPT_ARG && required)
			error = fprintf(file, "<%s>", spec->name);
		else if (spec->type == ADOPT_ARG)
			error = fprintf(file, "[<%s>]", spec->name);
		else if (spec->type == ADOPT_ARGS && required)
			error = fprintf(file, "<%s...>", spec->name);
		else if (spec->type == ADOPT_ARGS)
			error = fprintf(file, "[<%s...>]", spec->name);
		else if (spec->type == ADOPT_LITERAL)
			error = fprintf(file, "--");
		else if (spec->alias)
			error = fprintf(file, "[-%c]", spec->alias);
		else
			error = fprintf(file, "[--%s]", spec->name);

		if (error < 0)
			goto done;
	}

	error = fprintf(file, "\n");

done:
	error = (error < 0) ? -1 : 0;
	return error;
}

