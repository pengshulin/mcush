/* 19aug13abu
 * (c) Software Lab. Alexander Burger
 */

#include "pico.h"

static void divErr(any ex) {err(ex,NULL,"Div/0");}

/* Number of bytes */
int numBytes(any x) {
   int n = 4;
   word w = (word)x >> 2;

   if ((w & 0xFF000000) == 0) {
      --n;
      if ((w & 0xFF0000) == 0) {
         --n;
         if ((w & 0xFF00) == 0)
            --n;
      }
   }
   return n;
}

/* Make number from symbol */
any symToNum(any s, int scl, int sep, int ign) {
   unsigned c;
   int i;
   word w;
   bool sign, frac;
   long n;

   if (!(c = getByte1(&i, &w, &s)))
      return NULL;
   while (c <= ' ')  /* Skip white space */
      if (!(c = getByte(&i, &w, &s)))
         return NULL;
   sign = NO;
   if (c == '+'  ||  c == '-' && (sign = YES))
      if (!(c = getByte(&i, &w, &s)))
         return NULL;
   if ((c -= '0') > 9)
      return NULL;
   frac = NO;
   n = c;
   while ((c = getByte(&i, &w, &s))  &&  (!frac || scl)) {
      if ((int)c == sep) {
         if (frac)
            return NULL;
         frac = YES;
      }
      else if ((int)c != ign) {
         if ((c -= '0') > 9)
            return NULL;
         n = n * 10 + c;
         if (frac)
            --scl;
      }
   }
   if (c) {
      if ((c -= '0') > 9)
         return NULL;
      if (c >= 5)
         n += 1;
      while (c = getByte(&i, &w, &s)) {
         if ((c -= '0') > 9)
            return NULL;
      }
   }
   if (frac)
      while (--scl >= 0)
         n *= 10;
   return box(sign? -n : n);
}

/* Make symbol from number */
any numToSym(any x, int scl, int sep, int ign) {
   int i;
   word w;
   cell c1;
   long n;
   byte *p, buf[BITS/2];

   n = unBox(x);
   putByte0(&i, &w, &x);
   if (n < 0) {
      n = -n;
      putByte('-', &i, &w, &x, &c1);
   }
   for (p = buf;;) {
      *p = n % 10;
      if ((n /= 10) == 0)
         break;
      ++p;
   }
   if ((scl = p - buf - scl) < 0) {
      putByte('0', &i, &w, &x, &c1);
      putByte(sep, &i, &w, &x, &c1);
      while (scl < -1)
         putByte('0', &i, &w, &x, &c1),  ++scl;
   }
   for (;;) {
      putByte(*p + '0', &i, &w, &x, &c1);
      if (--p < buf)
         return popSym(i, w, x, &c1);
      if (scl == 0)
         putByte(sep, &i, &w, &x, &c1);
      else if (ign  &&  scl > 0  &&  scl % 3 == 0)
         putByte(ign, &i, &w, &x, &c1);
      --scl;
   }
}

// (format 'num ['num ['sym1 ['sym2]]]) -> sym
// (format 'sym ['num ['sym1 ['sym2]]]) -> num
any doFormat(any ex) {
   int scl, sep, ign;
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedAtom(ex,data(c1));
   x = cdr(x),  y = EVAL(car(x));
   scl = isNil(y)? 0 : xNum(ex, y);
   sep = '.';
   ign = 0;
   if (isCell(x = cdr(x))) {
      y = EVAL(car(x));
      NeedSymb(ex,y);
      sep = firstByte(y);
      if (isCell(x = cdr(x))) {
         y = EVAL(car(x));
         NeedSymb(ex,y);
         ign = firstByte(y);
      }
   }
   data(c1) = isNum(data(c1))?
      numToSym(data(c1), scl, sep, ign) :
      symToNum(name(data(c1)), scl, sep, ign) ?: Nil;
   return Pop(c1);
}

// (+ 'num ..) -> num
any doAdd(any ex) {
   any x, y;
   long n;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   n = unBox(y);
   while (isCell(x = cdr(x))) {
      if (isNil(y = EVAL(car(x))))
         return Nil;
      NeedNum(ex,y);
      n += unBox(y);
   }
   return box(n);
}

// (- 'num ..) -> num
any doSub(any ex) {
   any x, y;
   long n;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   n = unBox(y);
   if (!isCell(x = cdr(x)))
      return box(-n);
   do {
      if (isNil(y = EVAL(car(x))))
         return Nil;
      NeedNum(ex,y);
      n -= unBox(y);
   } while (isCell(x = cdr(x)));
   return box(n);
}

// (inc 'num) -> num
// (inc 'var ['num]) -> num
any doInc(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex);
   if (isNil(data(c1) = EVAL(car(x))))
      return Nil;
   if (isNum(data(c1)))
      return (any)(num(data(c1)) + 4);
   CheckVar(ex,data(c1));
   if (!isCell(x = cdr(x))) {
      if (isNil(val(data(c1))))
         return Nil;
      NeedNum(ex,val(data(c1)));
      val(data(c1)) = (any)(num(val(data(c1))) + 4);
   }
   else {
      Save(c1);
      y = EVAL(car(x));
      drop(c1);
      if (isNil(val(data(c1))) || isNil(y))
         return Nil;
      NeedNum(ex,val(data(c1)));
      NeedNum(ex,y);
      val(data(c1)) = box(unBox(val(data(c1))) + unBox(y));
   }
   return val(data(c1));
}

// (dec 'num) -> num
// (dec 'var ['num]) -> num
any doDec(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex);
   if (isNil(data(c1) = EVAL(car(x))))
      return Nil;
   if (isNum(data(c1)))
      return (any)(num(data(c1)) - 4);
   CheckVar(ex,data(c1));
   if (!isCell(x = cdr(x))) {
      if (isNil(val(data(c1))))
         return Nil;
      NeedNum(ex,val(data(c1)));
      val(data(c1)) = (any)(num(val(data(c1))) - 4);
   }
   else {
      Save(c1);
      y = EVAL(car(x));
      drop(c1);
      if (isNil(val(data(c1))) || isNil(y))
         return Nil;
      NeedNum(ex,val(data(c1)));
      NeedNum(ex,y);
      val(data(c1)) = box(unBox(val(data(c1))) - unBox(y));
   }
   return val(data(c1));
}

// (* 'num ..) -> num
any doMul(any ex) {
   any x, y;
   long n;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   n = unBox(y);
   while (isCell(x = cdr(x))) {
      if (isNil(y = EVAL(car(x))))
         return Nil;
      NeedNum(ex,y);
      n *= unBox(y);
   }
   return box(n);
}

// (*/ 'num1 ['num2 ..] 'num3) -> num
any doMulDiv(any ex) {
   any x, y;
   long long n;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   n = unBox(y);
   for (;;) {
      x = cdr(x);
      if (isNil(y = EVAL(car(x))))
         return Nil;
      NeedNum(ex,y);
      if (!isCell(cdr(x)))
         break;
      n *= unBox(y);
   }
   if (y == Zero)
      divErr(ex);
   return box((long)((n + unBox(y)/2) / unBox(y)));
}

// (/ 'num ..) -> num
any doDiv(any ex) {
   any x, y;
   long n;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   n = unBox(y);
   while (isCell(x = cdr(x))) {
      if (isNil(y = EVAL(car(x))))
         return Nil;
      NeedNum(ex,y);
      if (y == Zero)
         divErr(ex);
      n /= unBox(y);
   }
   return box(n);
}

// (% 'num ..) -> num
any doRem(any ex) {
   any x, y;
   long n;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   n = unBox(y);
   while (isCell(x = cdr(x))) {
      if (isNil(y = EVAL(car(x))))
         return Nil;
      NeedNum(ex,y);
      if (y == Zero)
         divErr(ex);
      n %= unBox(y);
   }
   return box(n);
}

// (>> 'num 'num) -> num
any doShift(any ex) {
   any x, y;
   long n, m;

   x = cdr(ex),  n = evNum(ex,x);
   x = cdr(x);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   if ((m = unBox(y)) >= 0)
      m = n >= 0? m >> n : m << -n;
   else
      m = -(n >= 0? -m >> n : -m << -n);
   return box(m);
}

// (lt0 'any) -> num | NIL
any doLt0(any x) {
   x = cdr(x);
   return isNum(x = EVAL(car(x))) && num(x)<0? x : Nil;
}

// (le0 'any) -> num | NIL
any doLe0(any x) {
   x = cdr(x);
   return isNum(x = EVAL(car(x))) && num(x)<=num(Zero)? x : Nil;
}

// (ge0 'any) -> num | NIL
any doGe0(any x) {
   x = cdr(x);
   return isNum(x = EVAL(car(x))) && num(x)>=0? x : Nil;
}

// (gt0 'any) -> num | NIL
any doGt0(any x) {
   x = cdr(x);
   return isNum(x = EVAL(car(x))) && num(x)>num(Zero)? x : Nil;
}

// (abs 'num) -> num
any doAbs(any ex) {
   any x;

   x = cdr(ex);
   if (isNil(x = EVAL(car(x))))
      return Nil;
   NeedNum(ex,x);
   return num(x)<0? box(-unBox(x)) : x;
}

// (bit? 'num ..) -> num | NIL
any doBitQ(any ex) {
   any x, y, z;

   x = cdr(ex),  y = EVAL(car(x));
   NeedNum(ex,y);
   while (isCell(x = cdr(x))) {
      if (isNil(z = EVAL(car(x))))
         return Nil;
      NeedNum(ex,z);
      if ((unBox(y) & unBox(z)) != unBox(y))
         return Nil;
   }
   return y;
}

// (& 'num ..) -> num
any doBitAnd(any ex) {
   any x, y, z;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   while (isCell(x = cdr(x))) {
      if (isNil(z = EVAL(car(x))))
         return Nil;
      NeedNum(ex,z);
      y = box(unBox(y) & unBox(z));
   }
   return y;
}

// (| 'num ..) -> num
any doBitOr(any ex) {
   any x, y, z;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   while (isCell(x = cdr(x))) {
      if (isNil(z = EVAL(car(x))))
         return Nil;
      NeedNum(ex,z);
      y = box(unBox(y) | unBox(z));
   }
   return y;
}

// (x| 'num ..) -> num
any doBitXor(any ex) {
   any x, y, z;

   x = cdr(ex);
   if (isNil(y = EVAL(car(x))))
      return Nil;
   NeedNum(ex,y);
   while (isCell(x = cdr(x))) {
      if (isNil(z = EVAL(car(x))))
         return Nil;
      NeedNum(ex,z);
      y = box(unBox(y) ^ unBox(z));
   }
   return y;
}

// (sqrt 'num ['flg|num]) -> num
any doSqrt(any ex) {
   any x;
	long m, n, r;

   x = cdr(ex);
   if (isNil(x = EVAL(car(x))))
      return Nil;
   NeedNum(ex,x);
	if ((n = unBox(x)) < 0)
      argError(ex, x);
   x = cddr(ex);
   if (isNum(x = EVAL(car(x))))
      n *= unBox(x);
	m = 1L << BITS-4;
	r = 0;
	do {
		if ((r += m) > n)
         r -= m;
      else
			n -= r,  r += m;
		r >>= 1;
	} while (m >>= 2);
   if (!isNil(x) && n > r)
      ++r;
	return box(r);
}

static uint64_t Seed;
#define hi(t)     (word)((t) >> 32)

// (seed 'num) -> num
any doSeed(any ex) {
   return box(hi(Seed = evNum(ex,cdr(ex)) * 6364136223846793005LL));
}

// (rand ['num1 'num2] | ['T]) -> num | flg
any doRand(any ex) {
   any x;
   long n;

   x = cdr(ex);
   Seed = Seed * 6364136223846793005LL + 1;
   if (isNil(x = EVAL(car(x))))
      return box(hi(Seed));
   if (x == T)
      return hi(Seed) & 1 ? T : Nil;
   n = xNum(ex,x);
   return box(n + hi(Seed) % (evNum(ex,cddr(ex)) + 1 - n));
}
