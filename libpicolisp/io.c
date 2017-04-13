/* 10jul13abu
 * (c) Software Lab. Alexander Burger
 */

#include "pico.h"

static any read0(bool);

static int StrI;
static cell StrCell, *StrP;
static word StrW;
static void (*PutSave)(int);
static char Delim[] = " \t\n\r\"'(),[]`~{}";

static void openErr(any ex, char *s) {err(ex, NULL, "%s open: %s", s, strerror(errno));}
static void eofErr(void) {err(NULL, NULL, "EOF Overrun");}

/* Buffer size */
int bufSize(any x) {return symBytes(x) + 1;}

int pathSize(any x) {
   int c = firstByte(x);

   if (c != '@'  &&  (c != '+' || secondByte(x) != '@'))
      return bufSize(x);
   if (!Home)
      return symBytes(x);
   return strlen(Home) + symBytes(x);
}

void bufString(any x, char *p) {
   int c, i;
   word w;

   if (!isNil(x)) {
      for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x)) {
         if (c == '^') {
            if ((c = getByte(&i, &w, &x)) == '?')
               c = 127;
            else
               c &= 0x1F;
         }
         *p++ = c;
      }
   }
   *p = '\0';
}

void pathString(any x, char *p) {
   int c, i;
   word w;
   char *h;

   x = name(x);
   if ((c = getByte1(&i, &w, &x)) == '+')
      *p++ = c,  c = getByte(&i, &w, &x);
   if (c != '@')
      while (*p++ = c)
         c = getByte(&i, &w, &x);
   else {
      if (h = Home)
         do
            *p++ = *h++;
         while (*h);
      while (*p++ = getByte(&i, &w, &x));
   }
}

// (path 'any) -> sym
any doPath(any x) {
   x = evSym(cdr(x));
   {
      char nm[pathSize(x)];

      pathString(x,nm);
      return mkStr(nm);
   }
}

void rdOpen(any ex, any x, inFrame *f) {
   NeedSymb(ex,x);
   if (isNil(x))
      f->fp = stdin;
   else {
      char nm[pathSize(x)];

      pathString(x,nm);
      if (nm[0] == '+') {
         if (!(f->fp = fopen(nm+1, "a+")))
            openErr(ex, nm);
         fseek(f->fp, 0L, SEEK_SET);
      }
      else if (!(f->fp = fopen(nm, "r")))
         openErr(ex, nm);
   }
}

void wrOpen(any ex, any x, outFrame *f) {
   NeedSymb(ex,x);
   if (isNil(x))
      f->fp = stdout;
   else {
      char nm[pathSize(x)];

      pathString(x,nm);
      if (nm[0] == '+') {
         if (!(f->fp = fopen(nm+1, "a")))
            openErr(ex, nm);
      }
      else if (!(f->fp = fopen(nm, "w")))
         openErr(ex, nm);
   }
}

/*** Reading ***/
void getStdin(void) {Chr = getc(InFile);}

static void getParse(void) {
   if ((Chr = getByte(&Env.parser->i, &Env.parser->w, &Env.parser->nm)) == 0)
      Chr = ']';
}

void pushInFiles(inFrame *f) {
   f->next = Chr,  Chr = 0;
   InFile = f->fp;
   f->get = Env.get,  Env.get = getStdin;
   f->link = Env.inFrames,  Env.inFrames = f;
}

void pushOutFiles(outFrame *f) {
   OutFile = f->fp;
   f->put = Env.put,  Env.put = putStdout;
   f->link = Env.outFrames,  Env.outFrames = f;
}

void popInFiles(void) {
   if (InFile != stdin)
      fclose(InFile);
   Chr = Env.inFrames->next;
   Env.get = Env.inFrames->get;
   InFile = (Env.inFrames = Env.inFrames->link)?  Env.inFrames->fp : stdin;
}

void popOutFiles(void) {
   if (OutFile != stdout && OutFile != stderr)
      fclose(OutFile);
   Env.put = Env.outFrames->put;
   OutFile = (Env.outFrames = Env.outFrames->link)? Env.outFrames->fp : stdout;
}

/* Skip White Space and Comments */
static int skipc(int c) {
   if (Chr < 0)
      return Chr;
   for (;;) {
      while (Chr <= ' ') {
         Env.get();
         if (Chr < 0)
            return Chr;
      }
      if (Chr != c)
         return Chr;
      Env.get();
      while (Chr != '\n') {
         if (Chr < 0)
            return Chr;
         Env.get();
      }
   }
}

static void comment(void) {
   Env.get();
   if (Chr != '{') {
      while (Chr != '\n') {
         if (Chr < 0)
            return;
         Env.get();
      }
   }
   else {
      for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
         Env.get();
         if (Chr < 0)
            return;
         if (Chr == '}' && (Env.get(), Chr == '#'))
            break;
      }
      Env.get();
   }
}

static int skip(void) {
   for (;;) {
      if (Chr < 0)
         return Chr;
      while (Chr <= ' ') {
         Env.get();
         if (Chr < 0)
            return Chr;
      }
      if (Chr != '#')
         return Chr;
      comment();
   }
}

/* Test for escaped characters */
static bool testEsc(void) {
   for (;;) {
      if (Chr < 0)
         return NO;
      if (Chr != '\\')
         return YES;
      if (Env.get(), Chr != '\n')
         return YES;
      do
         Env.get();
      while (Chr == ' '  ||  Chr == '\t');
   }
}

/* Read a list */
static any rdList(void) {
   any x;
   cell c1;

   for (;;) {
      if (skip() == ')') {
         Env.get();
         return Nil;
      }
      if (Chr == ']')
         return Nil;
      if (Chr != '~') {
         Push(c1, x = cons(read0(NO),Nil));
         break;
      }
      Env.get();
      Push(c1, read0(NO));
      if (isCell(x = data(c1) = EVAL(data(c1)))) {
         while (isCell(cdr(x)))
            x = cdr(x);
         break;
      }
      drop(c1);
   }
   for (;;) {
      if (skip() == ')') {
         Env.get();
         break;
      }
      if (Chr == ']')
         break;
      if (Chr == '.') {
         Env.get();
         cdr(x) = skip()==')' || Chr==']'? data(c1) : read0(NO);
         if (skip() == ')')
            Env.get();
         else if (Chr != ']')
            err(NULL, x, "Bad dotted pair");
         break;
      }
      if (Chr != '~')
         x = cdr(x) = cons(read0(NO),Nil);
      else {
         Env.get();
         cdr(x) = read0(NO);
         cdr(x) = EVAL(cdr(x));
         while (isCell(cdr(x)))
            x = cdr(x);
      }
   }
   return Pop(c1);
}

/* Try for anonymous symbol */
static any anonymous(any s) {
   int c, i;
   word w;
   unsigned long n;
   heap *h;

   if ((c = getByte1(&i, &w, &s)) != '$')
      return NULL;
   n = 0;
   while (c = getByte(&i, &w, &s)) {
      if (c < '0' || c > '9')
         return NULL;
      n = n * 10 + c - '0';
   }
   n *= sizeof(cell);
   h = Heaps;
   do
      if ((any)n > h->cells  &&  (any)n < h->cells + CELLS)
         return symPtr((any)n);
   while (h = h->next);
   return NULL;
}

/* Relocate anonymous symbol */
static any reloc(any key) {
   any x, y;
   int n;

   if (!isCell(x = Reloc)) {
      Reloc = cons(cons(key, y = consSym(Nil,0)), Nil);
      return y;
   }
   for (;;) {
      if ((n = num(key) - num(caar(x))) == 0)
         return cdar(x);
      if (!isCell(cdr(x))) {
         key = cons(cons(key, y = consSym(Nil,0)), Nil);
         cdr(x) = n<0? cons(key,Nil) : cons(Nil,key);
         return y;
      }
      if (n < 0) {
         if (!isCell(cadr(x))) {
            cadr(x) = cons(cons(key, y = consSym(Nil,0)), Nil);
            return y;
         }
         x = cadr(x);
      }
      else {
         if (!isCell(cddr(x))) {
            cddr(x) = cons(cons(key, y = consSym(Nil,0)), Nil);
            return y;
         }
         x = cddr(x);
      }
   }
}

/* Read one expression */
static any read0(bool top) {
   int i;
   word w;
   any x, y;
   cell c1, *p;

   if (skip() < 0) {
      if (top)
         return Nil;
      eofErr();
   }
   if (Chr == '(') {
      Env.get();
      x = rdList();
      if (top  &&  Chr == ']')
         Env.get();
      return x;
   }
   if (Chr == '[') {
      Env.get();
      x = rdList();
      if (Chr != ']')
         err(NULL, x, "Super parentheses mismatch");
      Env.get();
      return x;
   }
   if (Chr == '\'') {
      Env.get();
      return cons(Quote, read0(top));
   }
   if (Chr == ',') {
      Env.get();
      return read0(top);
   }
   if (Chr == '`') {
      Env.get();
      Push(c1, read0(top));
      x = EVAL(data(c1));
      drop(c1);
      return x;
   }
   if (Chr == '\\') {
      Env.get();
      Push(c1, read0(top));
      if (isNum(x = data(c1)))
         x = reloc(x);
      else if (isCell(x)) {
         Transient[0] = Transient[1] = Nil;
         if (isNum(x = car(y = x)))
            x = car(y) = reloc(x);
         if (isCell(y = cdr(y))) {
            val(x) = car(y);
            p = (any)&tail(x);
            while (isCell(car(p)))
               car(p) = caar(p);
            while (isCell(y = cdr(y)))
               car(p) = cons(car(p),car(y)),  p = car(p);
         }
      }
      drop(c1);
      return x;
   }
   if (Chr == '"') {
      Env.get();
      if (Chr == '"') {
         Env.get();
         return Nil;
      }
      if (!testEsc())
         eofErr();
      putByte1(Chr, &i, &w, &p);
      while (Env.get(), Chr != '"') {
         if (!testEsc())
            eofErr();
         putByte(Chr, &i, &w, &p, &c1);
      }
      y = popSym(i, w, p, &c1),  Env.get();
      if (x = isIntern(tail(y), Transient))
         return x;
      intern(y, Transient);
      return y;
   }
   if (strchr(Delim, Chr))
      err(NULL, NULL, "Bad input '%c' (%d)", isprint(Chr)? Chr:'?', Chr);
   if (Chr == '\\')
      Env.get();
   putByte1(Chr, &i, &w, &p);
   for (;;) {
      Env.get();
      if (strchr(Delim, Chr))
         break;
      if (Chr == '\\')
         Env.get();
      putByte(Chr, &i, &w, &p, &c1);
   }
   y = popSym(i, w, p, &c1);
   if (x = symToNum(tail(y), (int)unBox(val(Scl)), '.', 0))
      return x;
   if (x = anonymous(name(y)))
      return x;
   if (x = isIntern(tail(y), Intern))
      return x;
   intern(y, Intern);
   val(y) = Nil;
   return y;
}

any read1(int end) {
   if (!Chr)
      Env.get();
   if (Chr == end)
      return Nil;
   return read0(YES);
}

/* Read one token */
any token(any x, int c) {
   int i;
   word w;
   any y;
   cell c1, *p;

   if (!Chr)
      Env.get();
   if (skipc(c) < 0)
      return Nil;
   if (Chr == '"') {
      Env.get();
      if (Chr == '"') {
         Env.get();
         return Nil;
      }
      if (!testEsc())
         return Nil;
      Push(c1, y =  cons(mkChar(Chr), Nil));
      while (Env.get(), Chr != '"' && testEsc())
         y = cdr(y) = cons(mkChar(Chr), Nil);
      Env.get();
      return Pop(c1);
   }
   if (Chr >= '0' && Chr <= '9') {
      putByte1(Chr, &i, &w, &p);
      while (Env.get(), Chr >= '0' && Chr <= '9' || Chr == '.')
         putByte(Chr, &i, &w, &p, &c1);
      return symToNum(tail(popSym(i, w, p, &c1)), (int)unBox(val(Scl)), '.', 0);
   }
   if (Chr != '+' && Chr != '-') {
      char nm[bufSize(x)];

      bufString(x, nm);
      if (Chr >= 'A' && Chr <= 'Z' || Chr == '\\' || Chr >= 'a' && Chr <= 'z' || strchr(nm,Chr)) {
         if (Chr == '\\')
            Env.get();
         putByte1(Chr, &i, &w, &p);
         while (Env.get(),
               Chr >= '0' && Chr <= '9' || Chr >= 'A' && Chr <= 'Z' ||
               Chr == '\\' || Chr >= 'a' && Chr <= 'z' || strchr(nm,Chr) ) {
            if (Chr == '\\')
               Env.get();
            putByte(Chr, &i, &w, &p, &c1);
         }
         y = popSym(i, w, p, &c1);
         if (x = isIntern(tail(y), Intern))
            return x;
         intern(y, Intern);
         val(y) = Nil;
         return y;
      }
   }
   y = mkTxt(c = Chr);
   Env.get();
   return mkChar(c);
}

// (read ['sym1 ['sym2]]) -> any
any doRead(any ex) {
   any x, y;

   if (!isCell(x = cdr(ex)))
      x = read1(0),  Reloc = Nil;
   else {
      y = EVAL(car(x));
      NeedSym(ex,y);
      x = cdr(x),  x = EVAL(car(x));
      NeedSym(ex,x);
      x = token(y, firstByte(x));
   }
   if (InFile == stdin  &&  Chr == '\n')
      Chr = 0;
   return x;
}

// (peek) -> sym
any doPeek(any ex __attribute__((unused))) {
   if (!Chr)
      Env.get();
   return Chr<0? Nil : mkChar(Chr);
}

// (char) -> sym
// (char 'num) -> sym
// (char 'sym) -> num
any doChar(any ex) {
   any x = cdr(ex);

   if (!isCell(x)) {
      if (!Chr)
         Env.get();
      x = Chr<0? Nil : mkChar(Chr);
      Env.get();
      return x;
   }
   if (isNum(x = EVAL(car(x)))) {
      int c = (int)unBox(x);

      if (c == 0)
         return Nil;
      if (c == 127)
         return mkChar2('^','?');
      if (c < ' ')
         return mkChar2('^', c + 0x40);
      return mkChar(c);
   }
   if (isSym(x)) {
      int c;

      if ((c = firstByte(x)) != '^')
         return box(c);
      return box((c = secondByte(x)) == '?'? 127 : c & 0x1F);
   }
   atomError(ex,x);
}

// (skip ['any]) -> sym
any doSkip(any x) {
   x = evSym(cdr(x));
   return skipc(firstByte(x))<0? Nil : mkChar(Chr);
}

// (eol) -> flg
any doEol(any ex __attribute__((unused))) {
   return InFile && Chr=='\n' || Chr<=0? T : Nil;
}

// (eof ['flg]) -> flg
any doEof(any x) {
   x = cdr(x);
   if (!isNil(EVAL(car(x)))) {
      Chr = -1;
      return T;
   }
   if (!Chr)
      Env.get();
   return Chr < 0? T : Nil;
}

// (from 'any ..) -> sym
any doFrom(any x) {
   int i, j, ac = length(x = cdr(x)), p[ac];
   cell c[ac];
   char *av[ac];

   if (ac == 0)
      return Nil;
   for (i = 0;;) {
      Push(c[i], evSym(x));
      av[i] = alloc(NULL, bufSize(data(c[i]))),  bufString(data(c[i]), av[i]);
      p[i] = 0;
      if (++i == ac)
         break;
      x = cdr(x);
   }
   if (!Chr)
      Env.get();
   while (Chr >= 0) {
      for (i = 0; i < ac; ++i) {
         for (;;) {
            if (av[i][p[i]] == (byte)Chr) {
               if (av[i][++p[i]])
                  break;
               Env.get();
               x = data(c[i]);
               goto done;
            }
            if (!p[i])
               break;
            for (j = 1; --p[i]; ++j)
               if (memcmp(av[i], av[i]+j, p[i]) == 0)
                  break;
         }
      }
      Env.get();
   }
   x = Nil;
done:
   i = 0;  do
      free(av[i]);
   while (++i < ac);
   drop(c[0]);
   return x;
}

// (till 'any ['flg]) -> lst|sym
any doTill(any ex) {
   any x;
   int i;
   word w;
   cell c1;

   x = evSym(cdr(ex));
   {
      char buf[bufSize(x)];

      bufString(x, buf);
      if (!Chr)
         Env.get();
      if (Chr < 0 || strchr(buf,Chr))
         return Nil;
      x = cddr(ex);
      if (isNil(EVAL(car(x)))) {
         Push(c1, x = cons(mkChar(Chr), Nil));
         while (Env.get(), Chr > 0 && !strchr(buf,Chr))
            x = cdr(x) = cons(mkChar(Chr), Nil);
         return Pop(c1);
      }
      putByte1(Chr, &i, &w, &x);
      while (Env.get(), Chr > 0 && !strchr(buf,Chr))
         putByte(Chr, &i, &w, &x, &c1);
      return popSym(i, w, x, &c1);
   }
}

static inline bool eol(void) {
   if (Chr < 0)
      return YES;
   if (Chr == '\n') {
      Chr = 0;
      return YES;
   }
   if (Chr == '\r') {
      Env.get();
      if (Chr == '\n')
         Chr = 0;
      return YES;
   }
   return NO;
}

// (line 'flg) -> lst|sym
any doLine(any x) {
   any y;
   int i;
   word w;
   cell c1;

   if (!Chr)
      Env.get();
   if (eol())
      return Nil;
   x = cdr(x);
   if (isNil(EVAL(car(x)))) {
      Push(c1, cons(mkChar(Chr), Nil));
      y = data(c1);
      for (;;) {
         if (Env.get(), eol())
            return Pop(c1);
         y = cdr(y) = cons(mkChar(Chr), Nil);
      }
   }
   else {
      putByte1(Chr, &i, &w, &y);
      for (;;) {
         if (Env.get(), eol())
            return popSym(i, w, y, &c1);
         putByte(Chr, &i, &w, &y, &c1);
      }
   }
}

static any parse(any x, bool skp) {
   int c;
   parseFrame *save, parser;
   void (*getSave)(void);
   cell c1;

   save = Env.parser;
   Env.parser = &parser;
   parser.nm = name(parser.sym = x);
   getSave = Env.get,  Env.get = getParse,  c = Chr;
   Push(c1, Env.parser->sym);
   Chr = getByte1(&parser.i, &parser.w, &parser.nm);
   if (skp)
      getParse();
   x = rdList();
   drop(c1);
   Chr = c,  Env.get = getSave,  Env.parser = save;
   return x;
}

static void putString(int c) {
   putByte(c, &StrI, &StrW, &StrP, &StrCell);
}

void begString(void) {
   putByte0(&StrI, &StrW, &StrP);
   PutSave = Env.put,  Env.put = putString;
}

any endString(void) {
   Env.put = PutSave;
   StrP = popSym(StrI, StrW, StrP, &StrCell);
   return StrI? StrP : Nil;
}

// (any 'sym) -> any
any doAny(any ex) {
   any x;

   x = cdr(ex),  x = EVAL(car(x));
   NeedSymb(ex,x);
   if (!isNil(x)) {
      int c;
      parseFrame *save, parser;
      void (*getSave)(void);
      cell c1;

      save = Env.parser;
      Env.parser = &parser;
      parser.nm = name(parser.sym = x);
      getSave = Env.get,  Env.get = getParse,  c = Chr;
      Push(c1, Env.parser->sym);
      Chr = getByte1(&parser.i, &parser.w, &parser.nm);
      x = read0(YES);
      drop(c1);
      Chr = c,  Env.get = getSave,  Env.parser = save;
   }
   return x;
}

// (sym 'any) -> sym
any doSym(any x) {
   x = EVAL(cadr(x));
   begString();
   print(x);
   return endString();
}

// (str 'sym) -> lst
// (str 'lst) -> sym
any doStr(any ex) {
   any x;

   x = cdr(ex);
   if (isNum(x = EVAL(car(x))))
      argError(ex,x);
   if (isSym(x))
      return isNil(x)? Nil : parse(x,NO);
   begString();
   while (print(car(x)), isCell(x = cdr(x)))
      space();
   return endString();
}

any load(any ex, int pr, any x) {
   cell c1, c2;
   inFrame f;

   if (isSymb(x) && firstByte(x) == '-') {
      Push(c1, parse(x,YES));
      x = evList(data(c1));
      drop(c1);
      return x;
   }
   rdOpen(ex, x, &f);
   pushInFiles(&f);
   doHide(Nil);
   x = Nil;
   for (;;) {
      if (InFile != stdin)
         data(c1) = read1(0);
      else {
         if (pr && !Chr)
            Env.put(pr), space(), fflush(OutFile);
         data(c1) = read1('\n');
         while (Chr > 0) {
            if (Chr == '\n') {
               Chr = 0;
               break;
            }
            if (Chr == '#')
               comment();
            else {
               if (Chr > ' ')
                  break;
               Env.get();
            }
         }
      }
      if (isNil(data(c1))) {
         popInFiles();
         doHide(Nil);
         return x;
      }
      Save(c1);
      if (InFile != stdin || Chr || !pr)
         x = EVAL(data(c1));
      else {
         Push(c2, val(At));
         x = val(At) = EVAL(data(c1));
         val(At3) = val(At2),  val(At2) = data(c2);
         outString("-> "),  fflush(OutFile),  print(x),  newline();
      }
      drop(c1);
   }
}

// (load 'any ..) -> any
any doLoad(any ex) {
   any x, y;

   x = cdr(ex);
   do {
      if ((y = EVAL(car(x))) != T)
         y = load(ex, '>', y);
      else
         y = loadAll(ex);
   } while (isCell(x = cdr(x)));
   return y;
}

// (in 'any . prg) -> any
any doIn(any ex) {
   any x;
   inFrame f;

   x = cdr(ex),  x = EVAL(car(x));
   rdOpen(ex,x,&f);
   pushInFiles(&f);
   x = prog(cddr(ex));
   popInFiles();
   return x;
}

// (out 'any . prg) -> any
any doOut(any ex) {
   any x;
   outFrame f;

   x = cdr(ex),  x = EVAL(car(x));
   wrOpen(ex,x,&f);
   pushOutFiles(&f);
   x = prog(cddr(ex));
   popOutFiles();
   return x;
}

/*** Prining ***/
void putStdout(int c) {putc(c, OutFile);}

void newline(void) {Env.put('\n');}
void space(void) {Env.put(' ');}

void outString(char *s) {
   while (*s)
      Env.put(*s++);
}

int bufNum(char buf[BITS/2], long n) {
   return sprintf(buf, "%ld", n);
}

void outNum(long n) {
   char buf[BITS/2];

   bufNum(buf, n);
   outString(buf);
}

void prIntern(any nm) {
   int i, c;
   word w;

   c = getByte1(&i, &w, &nm);
   if (strchr(Delim, c))
      Env.put('\\');
   Env.put(c);
   while (c = getByte(&i, &w, &nm)) {
      if (c == '\\' || strchr(Delim, c))
         Env.put('\\');
      Env.put(c);
   }
}

void prTransient(any nm) {
   int i, c;
   word w;

   Env.put('"');
   c = getByte1(&i, &w, &nm);
   do {
      if (c == '"'  ||  c == '\\')
         Env.put('\\');
      Env.put(c);
   } while (c = getByte(&i, &w, &nm));
   Env.put('"');
}

/* Print one expression */
void print(any x) {
   if (isNum(x))
      outNum(unBox(x));
   else if (isSym(x)) {
      any nm = name(x);

      if (nm == txt(0))
         Env.put('$'),  outNum((word)x/sizeof(cell));
      else if (x == isIntern(nm, Intern))
         prIntern(nm);
      else
         prTransient(nm);
   }
   else if (car(x) == Quote  &&  x != cdr(x))
      Env.put('\''),  print(cdr(x));
   else {
      any y;

      Env.put('(');
      if ((y = circ(x)) == NULL) {
         while (print(car(x)), !isNil(x = cdr(x))) {
            if (!isCell(x)) {
               outString(" . ");
               print(x);
               break;
            }
            space();
         }
      }
      else if (y == x) {
         do
            print(car(x)),  space();
         while (y != (x = cdr(x)));
         Env.put('.');
      }
      else {
         do
            print(car(x)),  space();
         while (y != (x = cdr(x)));
         outString(". (");
         do
            print(car(x)),  space();
         while (y != (x = cdr(x)));
         outString(".)");
      }
      Env.put(')');
   }
}

void prin(any x) {
   if (!isNil(x)) {
      if (isNum(x))
         outNum(unBox(x));
      else if (isSym(x)) {
         int i, c;
         word w;

         for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x)) {
            if (c != '^')
               Env.put(c);
            else if (!(c = getByte(&i, &w, &x)))
               Env.put('^');
            else if (c == '?')
               Env.put(127);
            else
               Env.put(c &= 0x1F);
         }
      }
      else {
         while (prin(car(x)), !isNil(x = cdr(x))) {
            if (!isCell(x)) {
               prin(x);
               break;
            }
         }
      }
   }
}

// (prin 'any ..) -> any
any doPrin(any x) {
   any y = Nil;

   while (isCell(x = cdr(x)))
      prin(y = EVAL(car(x)));
   return y;
}

// (prinl 'any ..) -> any
any doPrinl(any x) {
   any y = Nil;

   while (isCell(x = cdr(x)))
      prin(y = EVAL(car(x)));
   newline();
   return y;
}

// (space ['num]) -> num
any doSpace(any ex) {
   any x;
   int n;

   if (isNil(x = EVAL(cadr(ex)))) {
      Env.put(' ');
      return One;
   }
   for (n = xNum(ex,x); n > 0; --n)
      Env.put(' ');
   return x;
}

// (print 'any ..) -> any
any doPrint(any x) {
   any y;

   x = cdr(x),  print(y = EVAL(car(x)));
   while (isCell(x = cdr(x)))
      space(),  print(y = EVAL(car(x)));
   return y;
}

// (printsp 'any ..) -> any
any doPrintsp(any x) {
   any y;

   x = cdr(x);
   do
      print(y = EVAL(car(x))),  space();
   while (isCell(x = cdr(x)));
   return y;
}

// (println 'any ..) -> any
any doPrintln(any x) {
   any y;

   x = cdr(x),  print(y = EVAL(car(x)));
   while (isCell(x = cdr(x)))
      space(),  print(y = EVAL(car(x)));
   newline();
   return y;
}

/* Save one expression */
static void save(any x) {
   any y, nm;

   if (isNum(x))
      outNum(unBox(x));
   else if (isSym(x)) {
      if (x == isIntern(nm = name(x), Intern))
         prIntern(nm);
      else if (num(y = val(x)) & 1) {
         if (nm == txt(0))
            Env.put('\\'), outNum((word)x/sizeof(cell));
         else
            prTransient(nm);
      }
      else {
         *(long*)&val(x) |= 1;
         if (x == y && nm != txt(0))
            prTransient(nm);
         else {
            outString("\\(");
            if (nm == txt(0))
               outNum((word)x/sizeof(cell));
            else
               prTransient(nm);
            space(), save(y);
            for (y = tail(x); isCell(y); y = car(y))
               space(), save(cdr(y));
            Env.put(')');
         }
      }
   }
   else {
      y = x;
      Env.put('(');
      while (save(car(x)), !isNil(x = cdr(x))) {
         if (x == y) {
            outString(" .");
            break;
         }
         if (!isCell(x)) {
            outString(" . ");
            save(x);
            break;
         }
         space();
      }
      Env.put(')');
   }
}

// (save 'any) -> any
any doSave(any x) {
   any p;
   heap *h;

   x = cdr(x),  save(x = EVAL(car(x))),  newline();
   h = Heaps;
   do {
      p = h->cells + CELLS-1;
      do
         *(long*)&cdr(p) &= ~1;
      while (--p >= h->cells);
   } while (h = h->next);
   return x;
}

// (flush) -> flg
any doFlush(any ex __attribute__((unused))) {
   return fflush(OutFile)? Nil : T;
}
