/* 30aug13abu
 * (c) Software Lab. Alexander Burger
 */

#include "pico.h"

static byte Ascii6[] = {
   0,  2,  2,  2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,  2,  2,  2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,  1,  3,  5,  7,  9, 11,  13,  15,  17,  19,  21,  23,  25,   4,   6,
  27, 29, 31, 33, 35, 37, 39,  41,  43,  45,  47,  49,   8,  51,  10,  53,
  55, 57, 59, 61, 63, 65, 67,  69,  71,  73,  75,  77,  79,  81,  83,  85,
  87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117,
 119, 12, 14, 16, 18, 20, 22,  24,  26,  28,  30,  32,  34,  36,  38,  40,
  42, 44, 46, 48, 50, 52, 54,  56,  58,  60,  62, 121, 123, 125, 127,   0
};

static byte Ascii7[] = {
   0, 33,  32, 34,  46, 35,  47, 36,  60,  37,  62,  38,  97,  39,  98,  40,
  99, 41, 100, 42, 101, 43, 102, 44, 103,  45, 104,  48, 105,  49, 106,  50,
 107, 51, 108, 52, 109, 53, 110, 54, 111,  55, 112,  56, 113,  57, 114,  58,
 115, 59, 116, 61, 117, 63, 118, 64, 119,  65, 120,  66, 121,  67, 122,  68,
   0, 69,   0, 70,   0, 71,   0, 72,   0,  73,   0,  74,   0,  75,   0,  76,
   0, 77,   0, 78,   0, 79,   0, 80,   0,  81,   0,  82,   0,  83,   0,  84,
   0, 85,   0, 86,   0, 87,   0, 88,   0,  89,   0,  90,   0,  91,   0,  92,
   0, 93,   0, 94,   0, 95,   0, 96,   0, 123,   0, 124,   0, 125,   0, 126
};


int firstByte(any s) {
   int c;

   if (isNil(s))
      return 0;
   c = (int)(isTxt(s = name(s))? (word)s >> 1 : (word)tail(s));
   return Ascii7[c & (c & 1? 127 : 63)];
}

int secondByte(any s) {
   int c;

   if (isNil(s))
      return 0;
   c = (int)(isTxt(s = name(s))? (word)s >> 1 : (word)tail(s));
   c >>= c & 1? 7 : 6;
   return Ascii7[c & (c & 1? 127 : 63)];
}

int getByte1(int *i, word *p, any *q) {
   int c;

   if (isTxt(*q))
      *i = BITS-1,  *p = (word)*q >> 1,  *q = NULL;
   else
      *i = BITS,  *p = (word)tail(*q),  *q = val(*q);
   if (*p & 1)
      c = Ascii7[*p & 127],  *p >>= 7,  *i -= 7;
   else
      c = Ascii7[*p & 63],  *p >>= 6,  *i -= 6;
   return c;
}

int getByte(int *i, word *p, any *q) {
   int c;

   if (*i == 0) {
      if (!*q)
         return 0;
      if (isNum(*q))
         *i = BITS-2,  *p = (word)*q >> 2,  *q = NULL;
      else
         *i = BITS,  *p = (word)tail(*q),  *q = val(*q);
   }
   if (*p & 1) {
      c = *p & 127,  *p >>= 7;
      if (*i >= 7)
         *i -= 7;
      else if (isNum(*q)) {
         *p = (word)*q >> 2,  *q = NULL;
         c |= *p << *i;
         *p >>= 7 - *i;
         *i += BITS-9;
      }
      else {
         *p = (word)tail(*q),  *q = val(*q);
         c |= *p << *i;
         *p >>= 7 - *i;
         *i += BITS-7;
      }
      c &= 127;
   }
   else {
      c = *p & 63,  *p >>= 6;
      if (*i >= 6)
         *i -= 6;
      else if (!*q)
         return 0;
      else if (isNum(*q)) {
         *p = (word)*q >> 2,  *q = NULL;
         c |= *p << *i;
         *p >>= 6 - *i;
         *i += BITS-8;
      }
      else {
         *p = (word)tail(*q),  *q = val(*q);
         c |= *p << *i;
         *p >>= 6 - *i;
         *i += BITS-6;
      }
      c &= 63;
   }
   return Ascii7[c];
}

any mkTxt(int c) {return txt(Ascii6[c & 127]);}

any mkChar(int c) {
   return consSym(NULL, Ascii6[c & 127]);
}

any mkChar2(int c, int d) {
   c = Ascii6[c & 127];
   d = Ascii6[d & 127];
   return consSym(NULL, d << (c & 1? 7 : 6) | c);
}

void putByte0(int *i, word *p, any *q) {
   *i = 0,  *p = 0,  *q = NULL;
}

void putByte1(int c, int *i, word *p, any *q) {
   *i = (*p = Ascii6[c & 127]) & 1? 7 : 6;
   *q = NULL;
}

void putByte(int c, int *i, word *p, any *q, cell *cp) {
   int d = (c = Ascii6[c & 127]) & 1? 7 : 6;

   if (*i != BITS)
      *p |= (word)c << *i;
   if (*i + d  > BITS) {
      if (*q)
         *q = val(*q) = consName(*p, Zero);
      else {
         Push(*cp, consSym(NULL,0));
         tail(data(*cp)) = *q = consName(*p, Zero);
      }
      *p = c >> BITS - *i;
      *i -= BITS;
   }
   *i += d;
}

any popSym(int i, word n, any q, cell *cp) {
   if (q) {
      val(q) = i <= (BITS-2)? box(n) : consName(n, Zero);
      return Pop(*cp);
   }
   if (i > BITS-1) {
      Push(*cp, consSym(NULL,0));
      tail(data(*cp)) = consName(n, Zero);
      return Pop(*cp);
   }
   return consSym(NULL,n);
}

int symBytes(any x) {
   int cnt = 0;
   word w;

   if (isNil(x))
      return 0;
   x = name(x);
   if (isTxt(x)) {
      w = (word)x >> 1;
      while (w)
         ++cnt,  w >>= w & 1? 7 : 6;
   }
   else {
      do {
         w = (word)tail(x);
         do
            ++cnt;
         while (w >>= w & 1? 7 : 6);
      } while (!isNum(x = val(x)));
      w = (word)x >> 2;
      while (w)
         ++cnt,  w >>= w & 1? 7 : 6;
   }
   return cnt;
}

any isIntern(any nm, any tree[2]) {
   any x, y, z;
   long n;

   if (isTxt(nm)) {
      for (x = tree[0];  isCell(x);) {
         if ((n = (word)nm - (word)name(car(x))) == 0)
            return car(x);
         x = n<0? cadr(x) : cddr(x);
      }
   }
   else {
      for (x = tree[1];  isCell(x);) {
         y = nm,  z = name(car(x));
         for (;;) {
            if ((n = (word)tail(y) - (word)tail(z)) != 0) {
               x = n<0? cadr(x) : cddr(x);
               break;
            }
            y = val(y),  z = val(z);
            if (isNum(y)) {
               if (y == z)
                  return car(x);
               x = isNum(z) && y>z? cddr(x) : cadr(x);
               break;
            }
            if (isNum(z)) {
               x = cddr(x);
               break;
            }
         }
      }
   }
   return NULL;
}

any intern(any sym, any tree[2]) {
   any nm, x, y, z;
   long n;

   if ((nm = name(sym)) == txt(0))
      return sym;
   if (isTxt(nm)) {
      if (!isCell(x = tree[0])) {
         tree[0] = cons(sym, Nil);
         return sym;
      }
      for (;;) {
         if ((n = (word)nm - (word)name(car(x))) == 0)
            return car(x);
         if (!isCell(cdr(x))) {
            cdr(x) = n<0? cons(cons(sym,Nil), Nil) : cons(Nil, cons(sym,Nil));
            return sym;
         }
         if (n < 0) {
            if (isCell(cadr(x)))
               x = cadr(x);
            else {
               cadr(x) = cons(sym, Nil);
               return sym;
            }
         }
         else {
            if (isCell(cddr(x)))
               x = cddr(x);
            else {
               cddr(x) = cons(sym, Nil);
               return sym;
            }
         }
      }
   }
   else {
      if (!isCell(x = tree[1])) {
         tree[1] = cons(sym, Nil);
         return sym;
      }
      for (;;) {
         y = nm,  z = name(car(x));
         while ((n = (word)tail(y) - (word)tail(z)) == 0) {
            y = val(y),  z = val(z);
            if (isNum(y)) {
               if (y == z)
                  return car(x);
               n = isNum(z)? y-z : -1;
               break;
            }
            if (isNum(z)) {
               n = +1;
               break;
            }
         }
         if (!isCell(cdr(x))) {
            cdr(x) = n<0? cons(cons(sym,Nil), Nil) : cons(Nil, cons(sym,Nil));
            return sym;
         }
         if (n < 0) {
            if (isCell(cadr(x)))
               x = cadr(x);
            else {
               cadr(x) = cons(sym, Nil);
               return sym;
            }
         }
         else {
            if (isCell(cddr(x)))
               x = cddr(x);
            else {
               cddr(x) = cons(sym, Nil);
               return sym;
            }
         }
      }
   }
}

void unintern(any sym, any tree[2]) {
   any nm, x, y, z, *p;
   long n;

   if ((nm = name(sym)) == txt(0))
      return;
   if (isTxt(nm)) {
      if (!isCell(x = tree[0]))
         return;
      p = &tree[0];
      for (;;) {
         if ((n = (word)nm - (word)name(car(x))) == 0) {
            if (car(x) == sym) {
               if (!isCell(cadr(x)))
                  *p = cddr(x);
               else if (!isCell(y = cddr(x)))
                  *p = cadr(x);
               else if (!isCell(z = cadr(y)))
                  car(x) = car(y),  cddr(x) = cddr(y);
               else {
                  while (isCell(cadr(z)))
                     z = cadr(y = z);
                  car(x) = car(z),  cadr(y) = cddr(z);
               }
            }
            return;
         }
         if (!isCell(cdr(x)))
            return;
         if (n < 0) {
            if (!isCell(cadr(x)))
               return;
            x = *(p = &cadr(x));
         }
         else {
            if (!isCell(cddr(x)))
               return;
            x = *(p = &cddr(x));
         }
      }
   }
   else {
      if (!isCell(x = tree[1]))
         return;
      p = &tree[1];
      for (;;) {
         y = nm,  z = name(car(x));
         while ((n = (word)tail(y) - (word)tail(z)) == 0) {
            y = val(y),  z = val(z);
            if (isNum(y)) {
               if (y == z) {
                  if (car(x) == sym) {
                     if (!isCell(cadr(x)))
                        *p = cddr(x);
                     else if (!isCell(y = cddr(x)))
                        *p = cadr(x);
                     else if (!isCell(z = cadr(y)))
                        car(x) = car(y),  cddr(x) = cddr(y);
                     else {
                        while (isCell(cadr(z)))
                           z = cadr(y = z);
                        car(x) = car(z),  cadr(y) = cddr(z);
                     }
                  }
                  return;
               }
               n = isNum(z)? y-z : -1;
               break;
            }
            if (isNum(z)) {
               n = +1;
               break;
            }
         }
         if (!isCell(cdr(x)))
            return;
         if (n < 0) {
            if (!isCell(cadr(x)))
               return;
            x = *(p = &cadr(x));
         }
         else {
            if (!isCell(cddr(x)))
               return;
            x = *(p = &cddr(x));
         }
      }
   }
}

/* Get symbol name */
any name(any s) {
   for (s = tail(s); isCell(s); s = car(s));
   return s;
}

// (name 'sym ['sym2]) -> sym
any doName(any ex) {
   any x, y, *p;
   cell c1;

   x = cdr(ex),  data(c1) = EVAL(car(x));
   NeedSymb(ex,data(c1));
   y = isNil(data(c1))? txt(0) : name(data(c1));
   if (!isCell(x = cdr(x))) {
      if (y == txt(0))
         return Nil;
      Save(c1);
      tail(x = consSym(NULL,0)) = y;
      drop(c1);
      return x;
   }
   if (isNil(data(c1)) || data(c1) == isIntern(y, Intern))
      err(ex, data(c1), "Can't rename");
   Save(c1);
   x = EVAL(car(x));
   NeedSymb(ex,x);
   unintern(data(c1), Transient);
   for (p = &tail(data(c1)); isCell(*p); p = &car(*p));
   *p = name(x);
   return Pop(c1);
}

/* Make name */
any mkSym(byte *s) {
   int i;
   word w;
   cell c1, *p;

   putByte1(*s++, &i, &w, &p);
   while (*s)
      putByte(*s++, &i, &w, &p, &c1);
   return popSym(i, w, p, &c1);
}

/* Make string */
any mkStr(char *s) {return s && *s? mkSym((byte*)s) : Nil;}

bool isBlank(any x) {
   int i, c;
   word w;

   if (!isSymb(x))
      return NO;
   if (isNil(x))
      return YES;
   x = name(x);
   for (c = getByte1(&i, &w, &x);  c;  c = getByte(&i, &w, &x))
      if (c > ' ')
         return NO;
   return YES;
}

// (sp? 'any) -> flg
any doSpQ(any x) {
   x = cdr(x);
   return isBlank(EVAL(car(x)))? T : Nil;
}

// (pat? 'any) -> sym | NIL
any doPatQ(any x) {
   x = cdr(x);
   return isSymb(x = EVAL(car(x))) && firstByte(x) == '@'? x : Nil;
}

// (fun? 'any) -> any
any doFunQ(any x) {
   any y;

   x = cdr(x);
   if (isNum(x = EVAL(car(x))))
      return x;
   if (isSym(x))
      return Nil;
   for (y = cdr(x); isCell(y); y = cdr(y)) {
      if (y == x)
         return Nil;
      if (isCell(car(y))) {
         if (isNum(caar(y))) {
            if (isCell(cdr(y)))
               return Nil;
         }
         else if (isNil(caar(y)) || caar(y) == T)
            return Nil;
      }
      else if (!isNil(cdr(y)))
         return Nil;
   }
   if (!isNil(y))
      return Nil;
   if (isNil(x = car(x)))
      return T;
   for (y = x; isCell(y);)
      if (isNum(car(y)) || isCell(car(y)) || isNil(car(y)) || car(y)==T || x==(y=cdr(y)))
         return Nil;
   return isNum(y) || y==T? Nil : x;
}

// (all ['T]) -> lst
static void all(any x, cell *p) {
   if (isCell(cddr(x)))
      all(cddr(x), p);
   data(*p) = cons(car(x), data(*p));
   if (isCell(cadr(x)))
      all(cadr(x), p);
}

any doAll(any x) {
   any *p;
   cell c1;

   x = cdr(x);
   p = isNil(EVAL(car(x)))? Intern : Transient;
   Push(c1, Nil);
   if (isCell(p[1]))
      all(p[1], &c1);
   if (isCell(p[0]))
      all(p[0], &c1);
   return Pop(c1);
}

// (intern 'sym|1|2) -> sym
any doIntern(any ex) {
   any x;

   x = cdr(ex),  x = EVAL(car(x));
   if (x == box(1))
      return Intern[0];
   if (x == box(2))
      return Intern[1];
   NeedSymb(ex,x);
   return intern(x, Intern);
}

// (==== ['sym ..]) -> NIL
any doHide(any ex) {
   any x, y;

   Transient[0] = Transient[1] = Nil;
   for (x = cdr(ex); isCell(x); x = cdr(x)) {
      y = EVAL(car(x));
      NeedSymb(ex,y);
      intern(y, Transient);
   }
   return Nil;
}

// (box? 'any) -> sym | NIL
any doBoxQ(any x) {
   x = cdr(x);
   return isSymb(x = EVAL(car(x))) && name(x) == txt(0)? x : Nil;
}

// (str? 'any) -> sym | NIL
any doStrQ(any x) {
   any y;

   x = cdr(x);
   return isSymb(x = EVAL(car(x)))  &&
         (y = name(x)) != txt(0)  &&
            x != isIntern(y, Intern)? x : Nil;
}

// (zap 'sym) -> sym
any doZap(any ex) {
   any x;

   x = cdr(ex),  x = EVAL(car(x));
   NeedSymb(ex,x);
   if (x >= Nil  &&  x <= Bye)
      protError(ex,x);
   unintern(x, Intern);
   return x;
}

// (chop 'any) -> lst
any doChop(any x) {
   any y;
   int i, c;
   word w;
   cell c1, c2;

   if (isCell(x = EVAL(cadr(x))) || isNil(x))
      return x;
   x = name(data(c1) = xSym(x));
   if (!(c = getByte1(&i, &w, &x)))
      return Nil;
   Save(c1);
   Push(c2, y = cons(mkChar(c), Nil));
   while (c = getByte(&i, &w, &x))
      y = cdr(y) = cons(mkChar(c), Nil);
   drop(c1);
   return data(c2);
}

void pack(any x, int *i, word *p, any *q, cell *cp) {
   int c, j;
   word w;

   if (isCell(x))
      do
         pack(car(x), i, p, q, cp);
      while (isCell(x = cdr(x)));
   if (isNum(x)) {
      char buf[BITS/2], *b = buf;

      bufNum(buf, unBox(x));
      do
         putByte(*b++, i, p, q, cp);
      while (*b);
   }
   else if (!isNil(x))
      for (x = name(x), c = getByte1(&j, &w, &x); c; c = getByte(&j, &w, &x))
         putByte(c, i, p, q, cp);
}

// (pack 'any ..) -> sym
any doPack(any x) {
   int i;
   word w;
   any y;
   cell c1, c2;

   x = cdr(x),  Push(c1, EVAL(car(x)));
   putByte0(&i, &w, &y);
   pack(data(c1), &i, &w, &y, &c2);
   while (isCell(x = cdr(x)))
      pack(data(c1) = EVAL(car(x)), &i, &w, &y, &c2);
   y = popSym(i, w, y, &c2);
   drop(c1);
   return i? y : Nil;
}

// (glue 'any 'lst) -> sym
any doGlue(any x) {
   int i;
   word w;
   any y;
   cell c1, c2, c3;

   x = cdr(x),  Push(c1, EVAL(car(x)));
   x = cdr(x),  Push(c2, x = EVAL(car(x)));
   if (!isCell(x)) {
      drop(c1);
      return x;
   }
   putByte0(&i, &w, &y);
   pack(car(x), &i, &w, &y, &c3);
   while (isCell(x = cdr(x))) {
      pack(data(c1), &i, &w, &y, &c3);
      pack(car(x), &i, &w, &y, &c3);
   }
   y = popSym(i, w, y, &c3);
   drop(c1);
   return i? y : Nil;
}

// (text 'sym 'any ..) -> sym
any doText(any x) {
   int c, n, i1, i2;
   word w1, w2;
   any nm1, nm2;
   cell c1, c2;

   nm1 = name(data(c1) = evSym(x = cdr(x)));
   if (!(c = getByte1(&i1, &w1, &nm1)))
      return Nil;
   Save(c1);
   {
      cell arg[length(x = cdr(x))];

      for (n = 0;  isCell(x);  ++n, x = cdr(x))
         Push(arg[n], EVAL(car(x)));

      putByte0(&i2, &w2, &nm2);
      do {
         if (c != '@')
            putByte(c, &i2, &w2, &nm2, &c2);
         else if (!(c = getByte(&i1, &w1, &nm1)))
            break;
         else if (c == '@')
            putByte('@', &i2, &w2, &nm2, &c2);
         else if (c >= '1') {
            if ((c -= '1') > 8)
               c -= 7;
            if (n > c)
               pack(data(arg[c]), &i2, &w2, &nm2, &c2);
         }
      } while (c = getByte(&i1, &w1, &nm1));
      nm2 = popSym(i2, w2, nm2, &c2);
      drop(c1);
      return nm2;
   }
}

// (pre? 'sym1 'sym2) -> flg
any doPreQ(any ex) {
   int c, i1, i2;
   word w1, w2;
   any x, y;
   cell c1;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return T;
   NeedSymb(ex,y);
   Push(c1, y);
   x = cdr(x),  x = EVAL(car(x));
   drop(c1);
   if (isNil(x))
      return Nil;
   NeedSymb(ex,x);
   y = name(y);
   if (!(c = getByte1(&i1, &w1, &y)))
      return T;
   x = name(x);
   if (c != getByte1(&i2, &w2, &x))
      return Nil;
   for (;;) {
      if (!(c = getByte(&i1, &w1, &y)))
         return T;
      if (c != getByte(&i2, &w2, &x))
         return Nil;
   }
}

// (val 'var) -> any
any doVal(any ex) {
   any x;

   x = cdr(ex),  x = EVAL(car(x));
   NeedVar(ex,x);
   return val(x);
}

// (set 'var 'any ..) -> any
any doSet(any ex) {
   any x;
   cell c1;

   x = cdr(ex);
   do {
      Push(c1, EVAL(car(x))),  x = cdr(x);
      NeedVar(ex,data(c1));
      CheckVar(ex,data(c1));
      val(data(c1)) = EVAL(car(x)),  x = cdr(x);
      drop(c1);
   } while (isCell(x));
   return val(data(c1));
}

// (setq var 'any ..) -> any
any doSetq(any ex) {
   any x, y;

   x = cdr(ex);
   do {
      y = car(x),  x = cdr(x);
      NeedVar(ex,y);
      CheckVar(ex,y);
      val(y) = EVAL(car(x));
   } while (isCell(x = cdr(x)));
   return val(y);
}

// (swap 'var 'any) -> any
any doSwap(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   y = val(data(c1));
   x = cdr(x),  val(data(c1)) = EVAL(car(x));
   drop(c1);
   return y;
}

// (xchg 'var 'var ..) -> any
any doXchg(any ex) {
   any x, y, z;
   cell c1;

   x = cdr(ex);
   do {
      Push(c1, EVAL(car(x))),  x = cdr(x);
      NeedVar(ex,data(c1));
      CheckVar(ex,data(c1));
      y = EVAL(car(x)),  x = cdr(x);
      NeedVar(ex,y);
      CheckVar(ex,y);
      z = val(data(c1)),  val(data(c1)) = val(y),  val(y) = z;
      drop(c1);
   } while (isCell(x));
   return z;
}

// (on var ..) -> T
any doOn(any ex) {
   any x = cdr(ex);
   do {
      NeedVar(ex,car(x));
      CheckVar(ex,car(x));
      val(car(x)) = T;
   } while (isCell(x = cdr(x)));
   return T;
}

// (off var ..) -> NIL
any doOff(any ex) {
   any x = cdr(ex);
   do {
      NeedVar(ex,car(x));
      CheckVar(ex,car(x));
      val(car(x)) = Nil;
   } while (isCell(x = cdr(x)));
   return Nil;
}

// (onOff var ..) -> flg
any doOnOff(any ex) {
   any x = cdr(ex);
   any y;

   do {
      NeedVar(ex,car(x));
      CheckVar(ex,car(x));
      y = val(car(x)) = isNil(val(car(x)))? T : Nil;
   } while (isCell(x = cdr(x)));
   return y;
}

// (zero var ..) -> 0
any doZero(any ex) {
   any x = cdr(ex);
   do {
      NeedVar(ex,car(x));
      CheckVar(ex,car(x));
      val(car(x)) = Zero;
   } while (isCell(x = cdr(x)));
   return Zero;
}

// (one var ..) -> 1
any doOne(any ex) {
   any x = cdr(ex);
   do {
      NeedVar(ex,car(x));
      CheckVar(ex,car(x));
      val(car(x)) = One;
   } while (isCell(x = cdr(x)));
   return One;
}

// (default var 'any ..) -> any
any doDefault(any ex) {
   any x, y;

   x = cdr(ex);
   do {
      y = car(x),  x = cdr(x);
      NeedVar(ex,y);
      CheckVar(ex,y);
      if (isNil(val(y)))
         val(y) = EVAL(car(x));
   } while (isCell(x = cdr(x)));
   return val(y);
}

// (push 'var 'any ..) -> any
any doPush(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   x = cdr(x);
   val(data(c1)) = cons(y = EVAL(car(x)), val(data(c1)));
   while (isCell(x = cdr(x)))
      val(data(c1)) = cons(y = EVAL(car(x)), val(data(c1)));
   drop(c1);
   return y;
}

// (push1 'var 'any ..) -> any
any doPush1(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   x = cdr(x);
   if (!member(y = EVAL(car(x)), val(data(c1))))
      val(data(c1)) = cons(y, val(data(c1)));
   while (isCell(x = cdr(x)))
      if (!member(y = EVAL(car(x)), val(data(c1))))
         val(data(c1)) = cons(y, val(data(c1)));
   drop(c1);
   return y;
}

// (pop 'var) -> any
any doPop(any ex) {
   any x, y;

   x = cdr(ex),  x = EVAL(car(x));
   NeedVar(ex,x);
   CheckVar(ex,x);
   if (!isCell(y = val(x)))
      return y;
   val(x) = cdr(y);
   return car(y);
}

// (cut 'num 'var) -> lst
any doCut(any ex) {
   long n;
   any x, y;
   cell c1, c2;

   if ((n = evNum(ex, x = cdr(ex))) <= 0)
      return Nil;
   x = cdr(x),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   if (isCell(val(data(c1)))) {
      Push(c2, y = cons(car(val(data(c1))), Nil));
      while (isCell(val(data(c1)) = cdr(val(data(c1)))) && --n)
         y = cdr(y) = cons(car(val(data(c1))), Nil);
      drop(c1);
      return data(c2);
   }
   return val(Pop(c1));
}

// (del 'any 'var) -> lst
any doDel(any ex) {
   any x, y;
   cell c1, c2, c3;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   x = cdr(x),  Push(c2, EVAL(car(x)));
   NeedVar(ex,data(c2));
   CheckVar(ex,data(c2));
   if (isCell(x = val(data(c2)))) {
      if (equal(data(c1), car(x))) {
         drop(c1);
         return val(data(c2)) = cdr(x);
      }
      Push(c3, y = cons(car(x), Nil));
      while (isCell(x = cdr(x))) {
         if (equal(data(c1), car(x))) {
            cdr(y) = cdr(x);
            drop(c1);
            return val(data(c2)) = data(c3);
         }
         y = cdr(y) = cons(car(x), Nil);
      }
   }
   drop(c1);
   return val(data(c2));
}

// (queue 'var 'any) -> any
any doQueue(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   x = cdr(x),  x = EVAL(car(x));
   if (!isCell(y = val(data(c1))))
      val(data(c1)) = cons(x,Nil);
   else {
      while (isCell(cdr(y)))
         y = cdr(y);
      cdr(y) = cons(x,Nil);
   }
   drop(c1);
   return x;
}

// (fifo 'var ['any ..]) -> any
any doFifo(any ex) {
   any x, y, z;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   if (isCell(x = cdr(x))) {
      y = EVAL(car(x));
      if (isCell(z = val(data(c1))))
         val(data(c1)) = z = cdr(z) = cons(y,cdr(z));
      else {
         z = val(data(c1)) = cons(y,Nil);
         cdr(z) = z;
      }
      while (isCell(x = cdr(x)))
         val(data(c1)) = z = cdr(z) = cons(y = EVAL(car(x)), cdr(z));
   }
   else if (!isCell(z = val(data(c1))))
      y = Nil;
   else {
      if (z == cdr(z)) {
         y = car(z);
         val(data(c1)) = Nil;
      }
      else {
         y = cadr(z);
         cdr(z) = cddr(z);
      }
   }
   drop(c1);
   return y;
}

static void idx(any x, cell *p) {
   if (isCell(cddr(x)))
      idx(cddr(x), p);
   data(*p) = cons(car(x), data(*p));
   if (isCell(cadr(x)))
      idx(cadr(x), p);
}

// (idx 'var 'any 'flg) -> lst
// (idx 'var 'any) -> lst
// (idx 'var) -> lst
any doIdx(any ex) {
   any x, y, z, *p;
   int flg;
   long n;
   cell c1, c2;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   if (!isCell(x = cdr(x))) {
      Push(c2, Nil);
      if (isCell(val(data(c1))))
         idx(val(data(c1)), &c2);
      drop(c1);
      return data(c2);
   }
   Push(c2, EVAL(car(x)));
   flg = !isCell(cdr(x))? 0 : isNil(EVAL(cadr(x)))? -1 : +1;
   if (!isCell(x = val(data(c1)))) {
      if (flg > 0)
         val(data(c1)) = cons(data(c2),Nil);
      drop(c1);
      return Nil;
   }
   p = (any*)data(c1);
   for (;;) {
      if ((n = compare(data(c2), car(x))) == 0) {
         if (flg < 0) {
            if (!isCell(cadr(x)))
               *p = cddr(x);
            else if (!isCell(y = cddr(x)))
               *p = cadr(x);
            else if (!isCell(z = cadr(y)))
               car(x) = car(y),  cddr(x) = cddr(y);
            else {
               while (isCell(cadr(z)))
                  z = cadr(y = z);
               car(x) = car(z),  cadr(y) = cddr(z);
            }
         }
         drop(c1);
         return x;
      }
      if (!isCell(cdr(x))) {
         if (flg > 0)
            cdr(x) = n < 0?
               cons(cons(data(c2),Nil), Nil) : cons(Nil, cons(data(c2),Nil));
         drop(c1);
         return Nil;
      }
      if (n < 0) {
         if (!isCell(cadr(x))) {
            if (flg > 0)
               cadr(x) = cons(data(c2),Nil);
            drop(c1);
            return Nil;
         }
         x = *(p = &cadr(x));
      }
      else {
         if (!isCell(cddr(x))) {
            if (flg > 0)
               cddr(x) = cons(data(c2),Nil);
            drop(c1);
            return Nil;
         }
         x = *(p = &cddr(x));
      }
   }
}

static any From, To;
static cell LupCell;

static void lup(any x) {
   if (isCell(x)) {
      if (car(x) == T)
         lup(cadr(x));
      else if (!isCell(car(x)))
         lup(cddr(x));
      else if (compare(To, caar(x)) >= 0) {
         lup(cddr(x));
         if (compare(From, caar(x)) <= 0) {
            data(LupCell) = cons(car(x), data(LupCell));
            lup(cadr(x));
         }
      }
      else if (compare(From, caar(x)) <= 0)
         lup(cadr(x));
   }
}

// (lup 'lst 'any) -> lst
// (lup 'lst 'any 'any2) -> lst
any doLup(any x) {
   long n;
   cell c1, c2;

   x = cdr(x),  Push(c1, EVAL(car(x)));
   x = cdr(x),  Push(c2, EVAL(car(x)));
   x = cdr(x);
   if (!isNil(To = EVAL(car(x)))) {
      From = data(c2);
      Push(LupCell, Nil);
      lup(data(c1));
      drop(c1);
      return data(LupCell);
   }
   while (isCell(data(c1))) {
      if (car(data(c1)) == T)
         data(c1) = cadr(data(c1));
      else if (!isCell(car(data(c1))))
         data(c1) = cddr(data(c1));
      else if (n = compare(data(c2), caar(data(c1))))
         data(c1) = n < 0? cadr(data(c1)) : cddr(data(c1));
      else {
         drop(c1);
         return car(data(c1));
      }
   }
   drop(c1);
   return Nil;
}

void put(any x, any key, any val) {
   any y, z;

   if (isCell(y = tail(x))) {
      if (isCell(cdr(y))) {
         if (key == cddr(y)) {
            if (isNil(val))
               tail(x) = car(y);
            else if (val == T)
               cdr(y) = key;
            else
               cadr(y) = val;
            return;
         }
      }
      else if (key == cdr(y)) {
         if (isNil(val))
            tail(x) = car(y);
         else if (val != T)
            cdr(y) = cons(val,key);
         return;
      }
      while (isCell(z = car(y))) {
         if (isCell(cdr(z))) {
            if (key == cddr(z)) {
               if (isNil(val))
                  car(y) = car(z);
               else {
                  if (val == T)
                     cdr(z) = key;
                  else
                     cadr(z) = val;
                  car(y) = car(z),  car(z) = tail(x),  tail(x) = z;
               }
               return;
            }
         }
         else if (key == cdr(z)) {
            if (isNil(val))
               car(y) = car(z);
            else {
               if (val != T)
                  cdr(z) = cons(val,key);
               car(y) = car(z),  car(z) = tail(x),  tail(x) = z;
            }
            return;
         }
         y = z;
      }
   }
   if (!isNil(val))
      tail(x) = cons(tail(x), val==T? key : cons(val,key));
}

any get(any x, any key) {
   any y, z;

   if (!isCell(y = tail(x)))
      return Nil;
   if (!isCell(cdr(y))) {
      if (key == cdr(y))
         return T;
   }
   else if (key == cddr(y))
      return cadr(y);
   while (isCell(z = car(y))) {
      if (!isCell(cdr(z))) {
         if (key == cdr(z)) {
            car(y) = car(z),  car(z) = tail(x),  tail(x) = z;
            return T;
         }
      }
      else if (key == cddr(z)) {
         car(y) = car(z),  car(z) = tail(x),  tail(x) = z;
         return cadr(z);
      }
      y = z;
   }
   return Nil;
}

any prop(any x, any key) {
   any y, z;

   if (isCell(y = tail(x))) {
      if (!isCell(cdr(y))) {
         if (key == cdr(y))
            return key;
      }
      else if (key == cddr(y))
         return cdr(y);
      while (isCell(z = car(y))) {
         if (!isCell(cdr(z))) {
            if (key == cdr(z)) {
               car(y) = car(z),  car(z) = tail(x),  tail(x) = z;
               return key;
            }
         }
         else if (key == cddr(z)) {
            car(y) = car(z),  car(z) = tail(x),  tail(x) = z;
            return cdr(z);
         }
         y = z;
      }
   }
   tail(x) = cons(tail(x), y = cons(Nil,key));
   return y;
}

// (put 'sym1|lst ['sym2|num ..] 'sym|0 'any) -> any
any doPut(any ex) {
   any x;
   cell c1, c2;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   x = cdr(x),  Push(c2, EVAL(car(x)));
   while (isCell(cdr(x = cdr(x)))) {
      if (isCell(data(c1)))
         data(c1) = getn(data(c2), data(c1));
      else {
         NeedSymb(ex,data(c1));
         data(c1) = data(c2)==Zero? val(data(c1)) : get(data(c1), data(c2));
      }
      data(c2) = EVAL(car(x));
   }
   NeedSymb(ex,data(c1));
   if (data(c2) == Zero) {
      CheckVar(ex,data(c1));
      val(data(c1)) = x = EVAL(car(x));
   }
   else
      put(data(c1), data(c2), x = EVAL(car(x)));
   drop(c1);
   return x;
}

// (get 'sym1|lst ['sym2|num ..]) -> any
any doGet(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  data(c1) = EVAL(car(x));
   if (!isCell(x = cdr(x)))
      return data(c1);
   Save(c1);
   do {
      y = EVAL(car(x));
      if (isCell(data(c1)))
         data(c1) = getn(y, data(c1));
      else {
         NeedSymb(ex,data(c1));
         data(c1) = y==Zero? val(data(c1)) : get(data(c1), y);
      }
   } while (isCell(x = cdr(x)));
   return Pop(c1);
}

// (prop 'sym1|lst ['sym2|num ..] 'sym) -> var
any doProp(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   x = cdr(x),  y = EVAL(car(x));
   while (isCell(x = cdr(x))) {
      if (isCell(data(c1)))
         data(c1) = getn(y, data(c1));
      else {
         NeedSymb(ex,data(c1));
         data(c1) = y==Zero? val(data(c1)) : get(data(c1), y);
      }
      y = EVAL(car(x));
   }
   NeedSymb(ex,data(c1));
   return prop(Pop(c1), y);
}

// (; 'sym1|lst [sym2|num ..]) -> any
any doSemicol(any ex) {
   any x, y;

   x = cdr(ex),  y = EVAL(car(x));
   while (isCell(x = cdr(x))) {
      if (isCell(y))
         y = getn(car(x), y);
      else {
         NeedSymb(ex,y);
         y = car(x)==Zero? val(y) : get(y, car(x));
      }
   }
   return y;
}

// (=: sym|0 [sym1|num .. sym2|0] 'any) -> any
any doSetCol(any ex) {
   any x, y, z;

   x = cdr(ex),  y = val(This);
   if (z = car(x),  isCell(cdr(x = cdr(x)))) {
      y = z==Zero? val(y) : get(y,z);
      while (z = car(x),  isCell(cdr(x = cdr(x)))) {
         if (isCell(y))
            y = getn(z,y);
         else {
            NeedSymb(ex,y);
            y = z==Zero? val(y) : get(y,z);
         }
      }
   }
   NeedSymb(ex,y);
   if (z == Zero) {
      CheckVar(ex,y);
      val(y) = x = EVAL(car(x));
   }
   else
      put(y, z, x = EVAL(car(x)));
   return x;
}

// (: sym|0 [sym1|num ..]) -> any
any doCol(any ex) {
   any x, y;

   x = cdr(ex),  y = val(This);
   y = car(x)==Zero? val(y) : get(y, car(x));
   while (isCell(x = cdr(x))) {
      if (isCell(y))
         y = getn(car(x), y);
      else {
         NeedSymb(ex,y);
         y = car(x)==Zero? val(y) : get(y,car(x));
      }
   }
   return y;
}

// (:: sym|0 [sym1|num .. sym2]) -> var
any doPropCol(any ex) {
   any x, y;

   x = cdr(ex),  y = val(This);
   if (isCell(cdr(x))) {
      y = car(x)==Zero? val(y) : get(y, car(x));
      while (isCell(cdr(x = cdr(x)))) {
         if (isCell(y))
            y = getn(car(x), y);
         else {
            NeedSymb(ex,y);
            y = car(x)==Zero? val(y) : get(y,car(x));
         }
      }
   }
   NeedSymb(ex,y);
   return prop(y, car(x));
}

// (putl 'sym1|lst1 ['sym2|num ..] 'lst) -> lst
any doPutl(any ex) {
   any x, y;
   cell c1, c2;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   x = cdr(x),  Push(c2, EVAL(car(x)));
   while (isCell(x = cdr(x))) {
      if (isCell(data(c1)))
         data(c1) = getn(data(c2), data(c1));
      else {
         NeedSymb(ex,data(c1));
         data(c1) = data(c2)==Zero? val(data(c1)) : get(data(c1), data(c2));
      }
      data(c2) = EVAL(car(x));
   }
   NeedSymb(ex,data(c1));
   x = (any)&tail(data(c1));
   while (isCell(car(x)))
      car(x) = caar(x);
   for (y = data(c2);  isCell(y);  y = cdr(y))
      if (!isCell(car(y)))
         car(x) = cons(car(x),car(y));
      else if (!isNil(caar(y)))
         car(x) = cons(car(x), caar(y)==T? cdar(y) : car(y));
   drop(c1);
   return data(c2);
}

// (getl 'sym1|lst1 ['sym2|num ..]) -> lst
any doGetl(any ex) {
   any x, y;
   cell c1, c2;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   while (isCell(x = cdr(x))) {
      y = EVAL(car(x));
      if (isCell(data(c1)))
         data(c1) = getn(y, data(c1));
      else {
         NeedSymb(ex,data(c1));
         data(c1) = y==Zero? val(data(c1)) : get(data(c1), y);
      }
   }
   NeedSymb(ex,data(c1));
   if (!isCell(x = tail(data(c1))))
      data(c2) = Nil;
   else {
      Push(c2, y = cons(cdr(x),Nil));
      while (isCell(x = car(x)))
         y = cdr(y) = cons(cdr(x),Nil);
   }
   drop(c1);
   return data(c2);
}

static any meta(any x, any y) {
   any z;

   while (isCell(x)) {
      if (isSymb(car(x)))
         if (!isNil(z = get(car(x),y)) || !isNil(z = meta(val(car(x)), y)))
            return z;
      x = cdr(x);
   }
   return Nil;
}

// (meta 'obj|typ 'sym ['sym2|num ..]) -> any
any doMeta(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   if (isSymb(data(c1)))
      data(c1) = val(data(c1));
   x = cdr(x),  y = EVAL(car(x));
   data(c1) = meta(data(c1), y);
   while (isCell(x = cdr(x))) {
      y = EVAL(car(x));
      if (isCell(data(c1)))
         data(c1) = getn(y, data(c1));
      else {
         NeedSymb(ex,data(c1));
         data(c1) = y==Zero? val(data(c1)) : get(data(c1), y);
      }
   }
   return Pop(c1);
}

#define isLowc(c) ((c) >= 'a' && (c) <= 'z')
#define isUppc(c) ((c) >= 'A' && (c) <= 'Z')

static inline bool isLetterOrDigit(int c) {
   return isLowc(c) || isUppc(c) || (c) >= '0' && (c) <= '9';
}

static int toUpperCase(int c) {
   return isLowc(c)? c - 32 : c;
}

static int toLowerCase(int c) {
   return isUppc(c)? c + 32 : c;
}

// (low? 'any) -> sym | NIL
any doLowQ(any x) {
   x = cdr(x);
   return isSymb(x = EVAL(car(x))) && isLowc(firstByte(x))? x : Nil;
}

// (upp? 'any) -> sym | NIL
any doUppQ(any x) {
   x = cdr(x);
   return isSymb(x = EVAL(car(x))) && isUppc(firstByte(x))? x : Nil;
}

// (lowc 'any) -> any
any doLowc(any x) {
   int c, i1, i2;
   word w1, w2;
   any nm;
   cell c1, c2;

   x = cdr(x);
   if (!isSymb(x = EVAL(car(x))) || isNil(x))
      return x;
   x = name(data(c1) = x);
   if (!(c = getByte1(&i1, &w1, &x)))
      return data(c1);
   Save(c1);
   putByte1(toLowerCase(c), &i2, &w2, &nm);
   while (c = getByte(&i1, &w1, &x))
      putByte(toLowerCase(c), &i2, &w2, &nm, &c2);
   nm = popSym(i2, w2, nm, &c2);
   drop(c1);
   return nm;
}

// (uppc 'any) -> any
any doUppc(any x) {
   int c, i1, i2;
   word w1, w2;
   any nm;
   cell c1, c2;

   x = cdr(x);
   if (!isSymb(x = EVAL(car(x))) || isNil(x))
      return x;
   x = name(data(c1) = x);
   if (!(c = getByte1(&i1, &w1, &x)))
      return data(c1);
   Save(c1);
   putByte1(toUpperCase(c), &i2, &w2, &nm);
   while (c = getByte(&i1, &w1, &x))
      putByte(toUpperCase(c), &i2, &w2, &nm, &c2);
   nm = popSym(i2, w2, nm, &c2);
   drop(c1);
   return nm;
}

// (fold 'any ['num]) -> sym
any doFold(any ex) {
   int n, c, i1, i2;
   word w1, w2;
   any x, nm;
   cell c1, c2;

   x = cdr(ex);
   if (!isSymb(x = EVAL(car(x))) || isNil(x))
      return x;
   x = name(data(c1) = x);
   if (!(c = getByte1(&i1, &w1, &x)))
      return Nil;
   while (!isLetterOrDigit(c))
      if (!(c = getByte(&i1, &w1, &x)))
         return Nil;
   Save(c1);
   n = isCell(cddr(ex))? evNum(ex, cddr(ex)) : 0;
   putByte1(toLowerCase(c), &i2, &w2, &nm);
   while (c = getByte(&i1, &w1, &x))
      if (isLetterOrDigit(c)) {
         if (!--n)
            break;
         putByte(toLowerCase(c), &i2, &w2, &nm, &c2);
      }
   nm = popSym(i2, w2, nm, &c2);
   drop(c1);
   return nm;
}
