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
#include "mcush.h"

#ifdef INLINE
    #undef INLINE
#endif
#if DEBUG
    #define INLINE(type) type
    #define ASSERT(c)    if(!c){halt("opt assert");}
#else
    #define INLINE(type) static inline type
    #define ASSERT(c)
#endif

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
    int args = 0;

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

    /* parse values in this order:
         (single argument mode)
       1. "--foo=bar" 
       2. "--foo=" --> "--foo=(NULL)" (VALUE_REQUIRED set)
                   --> "--foo=1" (VALUE_REQUIRED not set)
         (VALUE_REQUIRED is set)
       3. "--foo bar" 
       4. "--foo (NULL)" if value is not found, usually end of line (EOL)
         (VALUE_REQUIRED not set)
       5. "--foo foo" if next argument does not starts with '-'
       6. "--foo" treat as "--foo 1" if EOL or next argument starts with '-'
     */
    if( spec->type == MCUSH_OPT_VALUE )
    {
        if( eql )
        {
            opt->value = eql + 1;  /* "--foo=bar" */
            if( *opt->value == 0 )
            {
                if( spec->usage & MCUSH_OPT_USAGE_VALUE_REQUIRED )
                    opt->value = 0;  /* "--foo=" -> "--foo=(NULL)" */
                else
                    opt->value = shell_str_nil;  /* "--foo=", treated as "--foo=nil" */
            }
        }
        else if( spec->usage & MCUSH_OPT_USAGE_VALUE_REQUIRED )
        {
            if ((parser->idx + 1) <= parser->args_len)
                opt->value = parser->args[parser->idx++];  /* "--foo bar" */
            else
                opt->value = 0;  /* "--foo (NULL)" is the last arguments */
        }
        else
        {
            if( ((parser->idx + 1) <= parser->args_len)
                && (parser->args[parser->idx][0] != '-') )
                opt->value = parser->args[parser->idx++];  /* "--foo bar" */
            else
                opt->value = shell_str_nil;  /* "--foo", treated as "--foo nil" */
        }
    }
}

static void parse_short(mcush_opt *opt, mcush_opt_parser *parser)
{
    const mcush_opt_spec *spec;
    const char *arg = parser->args[parser->idx++], alias = *(arg + 1);

    if( (alias == 0) || ((spec = spec_byalias(parser, alias)) == NULL) )
    {
        opt->spec = NULL;
        opt->value = arg;
        return;
    }

    opt->spec = spec;

    /* parse values in this order:
         (single argument mode)
       1. "-ifoo"  "-i=foo"
       2. "-i=" --> "-i=(NULL)" (VALUE_REQUIRED set)
                --> "-i=1" (VALUE_REQUIRED not set)
         (VALUE_REQUIRED is set)
       3. "-i foo" 
       4. "-i (NULL)" if value is not found, usually end of line (EOL)
         (VALUE_REQUIRED not set)
       5. "-i foo" if next argument does not starts with '-'
       6. "-i" treat as "-i 1" if EOL or next argument starts with '-'
     */
    if( spec->type == MCUSH_OPT_VALUE )
    {
        if( strlen(arg) > 2 )
        {
            opt->value = arg + 2;  /* "-ifoo" */
            if( *opt->value == '=' )
            {
                opt->value++;  /* "-i=foo" */
                if( *opt->value == 0 )
                {
                    if( spec->usage & MCUSH_OPT_USAGE_VALUE_REQUIRED )
                        opt->value = 0;  /* "-i=" -> "-i (NULL)" */
                    else
                        opt->value = shell_str_nil;  /* "-i", treated as "-i nil" */
                }
            }
        }
        else if( spec->usage & MCUSH_OPT_USAGE_VALUE_REQUIRED )
        {
            if( (parser->idx + 1) <= parser->args_len )
                opt->value = parser->args[parser->idx++];  /* "-i foo" */
            else
                opt->value = 0;  /* "-i" is the last arguments */
        }
        else
        {
            if( ((parser->idx + 1) <= parser->args_len)
                && (parser->args[parser->idx][0] != '-') )
                opt->value = parser->args[parser->idx++];  /* "-i foo" */
            else
                opt->value = shell_str_nil;  /* "-i", treated as "-i nil" */
        }
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
    char **args,
    int args_len)
{
    ASSERT(parser);

    memset(parser, 0x0, sizeof(mcush_opt_parser));

    parser->specs = specs;
    parser->args = args;
    parser->args_len = (size_t)args_len;
}


int mcush_opt_parser_next(mcush_opt *opt, mcush_opt_parser *parser)
{
    ASSERT(opt && parser);

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

    if ((error = shell_printf("%s: %s", shell_str_usage, command)) < 0)
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

    error = shell_printf("\n%ss:\n", shell_str_option);

    /* print short description for each option */
    for (spec = specs; spec->type; ++spec) {
        if (spec->usage & MCUSH_OPT_USAGE_HIDDEN)
            continue;
        if (spec->type == MCUSH_OPT_LITERAL )
            continue;
        if (!spec->help)
            continue;
        if ((spec->type == MCUSH_OPT_ARG) || (spec->type == MCUSH_OPT_ARGS) )
            error = shell_printf(" %-15s ", spec->name);
        else if (spec->alias && spec->name)
            error = shell_printf(" -%c/--%-10s ", spec->alias, spec->name);
        else if (spec->name)
            error = shell_printf(" --%-13s ", spec->name);
        else if (spec->alias)
            error = shell_printf(" -%c              ", spec->alias);
        if (error < 0)
            goto done;
        if( spec->help )
            shell_write_line(spec->help);
        else
            shell_write_str( "\n" );
    }

done:
    error = (error < 0) ? -1 : 0;
    return error;
}


int mcush_opt_check_invalid_argument(
    const char *command, mcush_opt *opt, 
    const mcush_opt_spec specs[] )
{
    if( opt->value && strcmp(opt->value, "--help")==0 )
    {
        mcush_opt_usage_print( command, specs );
        return 0;
    }
    else
    {
        shell_printf( "%s %s: ", shell_str_invalid, shell_str_argument );
        shell_write_line( opt->value );
        mcush_opt_usage_print( command, specs );
        return -1;
    } 
}
 
