Adopt
=====

A portable command-line argument parser for C that handles short
(`-a`, `-b foo`, etc) and long (`--arg-a`, `--arg-b=foo`) style 
options.  It is meant to be compatible with GNU getopt in
command-line usage (not as an API) but available under a
permissive license.

Adopt also produces help syntax and usage messages
based on the arguments you accept, so that you don't need to
remember to update your usage messages when you add a new
option.

Types of options
----------------

* Switches are options that do not take a value, for example
  `-h` or `--verbose`.
* Values are options that take a value, for example `-m 2` or
  `--level=2`.
* Literal separators are bare double-dashes, `--` as a lone
  word, indicating that further words will be treated as
  arguments (even if they begin with a dash).
* Arguments are options that are bare words, not prefixed with
  a single or double dash, for example `filename.txt`.
* Argument lists are the remainder of arguments, not prefixed
  with dashes.  For example, an array: `file1.txt`, `file2.txt`,
  ...

Specifying options
------------------

Options should be specified as an array of `adopt_spec`
elements, terminated with an `adopt_spec` initialized to zeroes.

    adopt_spec opt_specs[] = {
        { ADOPT_VALUE, "debug", 'd', NULL, "displays debug information" },
        { ADOPT_SWITCH, "help", 0, NULL, NULL, ADOPT_HIDDEN },
        { ADOPT_VALUE, "verbose", 'v', "level", "sets the verbosity level (default 1)" },
        { ADOPT_VALUE, "channel", 'c', "channel", "sets the channel", ADOPT_VALUE_REQUIRED },
        { ADOPT_LITERAL },
        { ADOPT_ARG, "file", 'f', NULL, "file path" },
        { 0 },
    };

Parsing arguments
-----------------

After initializing the parser by calling  `adopt_parser_init` 
with the `adopt_spec`s and the command-line arguments given
to your program, you can call `adopt_parser_next` in a loop
to handle each option.
   
    int main(int argc, const char **argv)
    {
        adopt_parser parser;
        adopt_opt opt;
        const char *value;
        const char *file;
        
        adopt_parser_init(&parser, opt_specs, argv + 1, argc - 1);
        
        while (adopt_parser_next(&opt, &parser)) {
            if (opt.spec) {
                printf("'%s' = ", opt.spec->name);
                printf("'%s'\n", opt.value);
            } else {
                fprintf(stderr, "Unknown option: %s\n", opt.value);
                adopt_usage_fprint(stderr, argv[0], opt_specs);
                return 129;
            }
        }
    }

Some example inputs with outputs:

    % a.out --debug foo
    'debug' = '(null)'
    'file' = 'foo'
    % a.out -v 1 -v2 --verbose=3 asdf
    'verbose' = '1'
    'verbose' = '2'
    'verbose' = '3'
    'verbose' = '(null)'
    'file' = 'asdf'
    % a.out file.txt other
    'file' = 'file1'
    Unknown option: other
    usage: ./sample [-d] [--verbose[=<level>]] [-c <channel>] -- [<file>]

Displaying usage
----------------

To display usage, you can use the `adopt_usage_fprint` function,
which prints the usage information to the given file stream.

Inclusion in your product
-------------------------
If you loathe additional namespaces, and want to use adopt inside your
product without using the `adopt_` prefix, you can use the included
`rename.pl` script to give the functions and structs your own prefix.

For example:

    % ./rename.pl ed_opt

Will product `ed_opt.c` and `ed_opt.h` that can be included in an
application.

About Adopt
-----------
Adopt was written by Edward Thomson <ethomson@edwardthomson.com>
and is available under the MIT License.  Please see the
included file `LICENSE` for more information.

Adopt takes its name from the only entry in `/usr/dict/words`
that ends in the letters **opt**.  But if you happen to like adopt,
consider a donation to your local pet shelter or human society.

