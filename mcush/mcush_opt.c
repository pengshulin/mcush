/* mcush uses adopt (renamed as mcush_opt here) library 
 * to parse command arguments, it's small and efficient
 * MCUSH designed by Peng Shulin, all rights reserved. */
/*
 * Copyright (c), Edward Thomson <ethomson@edwardthomson.com>
 * All rights reserved.
 *
 * This file is part of adopt, distributed under the MIT license.
 * For full terms and conditions, see the included LICENSE file.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "mcush_opt.h"
#include "shell.h"

#define INLINE(type) static inline type

INLINE(const mcush_opt_spec *) spec_byname(
	mcush_opt_parser *parser, const char *name, size_t namelen)
{
	const mcush_opt_spec *spec;

	for (spec = parser->specs; spec->type; ++spec) {
		if (spec->type == MCUSH_OPT_LITERAL && namelen == 0)
			return spec;

		if ((spec->type == MCUSH_OPT_SWITCH || spec->type == MCUSH_OPT_VALUE) &&
			spec->name &&
			strlen(spec->name) == namelen &&
			strncmp(name, spec->name, namelen) == 0)
			return spec;
	}

	return NULL;
}

INLINE(const mcush_opt_spec *) spec_byalias(mcush_opt_parser *parser, char alias)
{
	const mcush_opt_spec *spec;

	for (spec = parser->specs; spec->type; ++spec) {
		if ((spec->type == MCUSH_OPT_SWITCH || spec->type == MCUSH_OPT_VALUE) &&
			alias == spec->alias)
			return spec;
	}

	return NULL;
}

INLINE(const mcush_opt_spec *) spec_nextarg(mcush_opt_parser *parser)
{
	const mcush_opt_spec *spec;
	size_t args = 0;
	
	for (spec = parser->specs; spec->type; ++spec) {
		if (spec->type == MCUSH_OPT_ARG) {
			if (args == parser->arg_idx) {
				parser->arg_idx++;
				return spec;
			}
			
			args++;
		}
	}
	
	return NULL;
}

static void parse_long(mcush_opt *opt, mcush_opt_parser *parser)
{
	const mcush_opt_spec *spec;
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
	if (spec->type == MCUSH_OPT_LITERAL) {
		parser->in_literal = 1;
	}

	/* Parse values as "--foo=bar" or "--foo bar" */
	if (spec->type == MCUSH_OPT_VALUE) {
		if (eql)
			opt->value = eql + 1;
		else if ((parser->idx + 1) <= parser->args_len)
			opt->value = parser->args[parser->idx++];
	}
}

static void parse_short(mcush_opt *opt, mcush_opt_parser *parser)
{
	const mcush_opt_spec *spec;
	const char *arg = parser->args[parser->idx++], alias = *(arg + 1);

	if ((spec = spec_byalias(parser, alias)) == NULL) {
		opt->spec = NULL;
		opt->value = arg;

		return;
	}

	opt->spec = spec;

	/* Parse values as "-ifoo" or "-i foo" */
	if (spec->type == MCUSH_OPT_VALUE) {
		if (strlen(arg) > 2)
			opt->value = arg + 2;
		else if ((parser->idx + 1) <= parser->args_len)
			opt->value = parser->args[parser->idx++];
	}
}

static void parse_arg(mcush_opt *opt, mcush_opt_parser *parser)
{
	opt->spec = spec_nextarg(parser);
	opt->value = parser->args[parser->idx++];
}

void mcush_opt_parser_init(
	mcush_opt_parser *parser,
	const mcush_opt_spec specs[],
	const char **args,
	size_t args_len)
{
	assert(parser);

	memset(parser, 0x0, sizeof(mcush_opt_parser));

	parser->specs = specs;
	parser->args = args;
	parser->args_len = args_len;
}

int mcush_opt_parser_next(mcush_opt *opt, mcush_opt_parser *parser)
{
	assert(opt && parser);

	memset(opt, 0x0, sizeof(mcush_opt));

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

int mcush_opt_usage_print(
	const char *command,
	const mcush_opt_spec specs[])
{
	const mcush_opt_spec *spec;
	int error;

	if ((error = shell_printf("usage: %s", command)) < 0)
		goto done;

	for (spec = specs; spec->type; ++spec) {
		int required = (spec->usage & MCUSH_OPT_USAGE_REQUIRED);
		int value_required = (spec->usage & MCUSH_OPT_USAGE_VALUE_REQUIRED);

		if (spec->usage & MCUSH_OPT_USAGE_HIDDEN)
			continue;

		if ((error = shell_printf(" ")) < 0)
			goto done;

		if (spec->type == MCUSH_OPT_VALUE && value_required && spec->alias)
			error = shell_printf("[-%c <%s>]", spec->alias, spec->value);
		else if (spec->type == MCUSH_OPT_VALUE && value_required)
			error = shell_printf("[--%s=<%s>]", spec->name, spec->value);
		else if (spec->type == MCUSH_OPT_VALUE)
			error = shell_printf("[--%s[=<%s>]]", spec->name, spec->value);
		else if (spec->type == MCUSH_OPT_ARG && required)
			error = shell_printf("<%s>", spec->name);
		else if (spec->type == MCUSH_OPT_ARG)
			error = shell_printf("[<%s>]", spec->name);
		else if (spec->type == MCUSH_OPT_ARGS && required)
			error = shell_printf("<%s...>", spec->name);
		else if (spec->type == MCUSH_OPT_ARGS)
			error = shell_printf("[<%s...>]", spec->name);
		else if (spec->type == MCUSH_OPT_LITERAL)
			error = shell_printf("--");
		else if (spec->alias)
			error = shell_printf("[-%c]", spec->alias);
		else
			error = shell_printf("[--%s]", spec->name);

		if (error < 0)
			goto done;
	}

	error = shell_printf("\n");

done:
	error = (error < 0) ? -1 : 0;
	return error;
}

