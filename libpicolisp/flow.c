/* 31jul13abu
 * (c) Software Lab. Alexander Burger
 */

#include "pico.h"

static void redefMsg(any x, any y) {
   FILE *oSave = OutFile;

   OutFile = stderr;
   outString("# ");
   print(x);
   if (y)
      space(), print(y);
   outString(" redefined\n");
   OutFile = oSave;
}

static void redefine(any ex, any s, any x) {
   NeedSymb(ex,s);
   CheckVar(ex,s);
   if (!isNil(val(s))  &&  s != val(s)  &&  !equal(x,val(s)))
      redefMsg(s,NULL);
   val(s) = x;
}

// (quote . any) -> any
any doQuote(any x) {return cdr(x);}

// (as 'any1 . any2) -> any2 | NIL
any doAs(any x) {
   x = cdr(x);
   if (isNil(EVAL(car(x))))
      return Nil;
   return cdr(x);
}

// (lit 'any) -> any
any doLit(any x) {
   x = cadr(x);
   if (isNum(x = EVAL(x)) || isNil(x) || x == T || isCell(x) && isNum(car(x)))
      return x;
   return cons(Quote, x);
}

// (eval 'any ['cnt ['lst]]) -> any
any doEval(any x) {
   any y;
   cell c1;
   bindFrame *p;

   x = cdr(x),  Push(c1, EVAL(car(x))),  x = cdr(x);
   if (!isNum(y = EVAL(car(x))) || !(p = Env.bind))
      data(c1) = EVAL(data(c1));
   else {
      int cnt, n, i, j;
      struct {  // bindFrame
         struct bindFrame *link;
         int i, cnt;
         struct {any sym; any val;} bnd[length(x)];
      } f;

      x = cdr(x),  x = EVAL(car(x));
      j = cnt = (int)unBox(y);
      n = f.i = f.cnt = 0;
      do {
         ++n;
         if ((i = p->i) <= 0  &&  (p->i -= cnt, i == 0)) {
            for (i = 0;  i < p->cnt;  ++i) {
               y = val(p->bnd[i].sym);
               val(p->bnd[i].sym) = p->bnd[i].val;
               p->bnd[i].val = y;
            }
            if (p->cnt  &&  p->bnd[0].sym == At  &&  !--j)
               break;
         }
      } while (p = p->link);
      while (isCell(x)) {
         for (p = Env.bind, j = n; ; p = p->link) {
            if (p->i < 0)
               for (i = 0;  i < p->cnt;  ++i) {
                  if (p->bnd[i].sym == car(x)) {
                     f.bnd[f.cnt].val = val(f.bnd[f.cnt].sym = car(x));
                     val(car(x)) = p->bnd[i].val;
                     ++f.cnt;
                     goto next;
                  }
               }
            if (!--j)
               break;
         }
next:    x = cdr(x);
      }
      f.link = Env.bind,  Env.bind = (bindFrame*)&f;
      data(c1) = EVAL(data(c1));
      while (--f.cnt >= 0)
         val(f.bnd[f.cnt].sym) = f.bnd[f.cnt].val;
      Env.bind = f.link;
      do {
         for (p = Env.bind, i = n;  --i;  p = p->link);
         if (p->i < 0  &&  (p->i += cnt) == 0)
            for (i = p->cnt;  --i >= 0;) {
               y = val(p->bnd[i].sym);
               val(p->bnd[i].sym) = p->bnd[i].val;
               p->bnd[i].val = y;
            }
      } while (--n);
   }
   return Pop(c1);
}

// (run 'any ['cnt ['lst]]) -> any
any doRun(any x) {
   any y;
   cell c1;
   bindFrame *p;

   x = cdr(x),  data(c1) = EVAL(car(x)),  x = cdr(x);
   if (!isNum(data(c1))) {
      Save(c1);
      if (!isNum(y = EVAL(car(x))) || !(p = Env.bind))
         data(c1) = isSym(data(c1))? val(data(c1)) : run(data(c1));
      else {
         int cnt, n, i, j;
         struct {  // bindFrame
            struct bindFrame *link;
            int i, cnt;
            struct {any sym; any val;} bnd[length(x)];
         } f;

         x = cdr(x),  x = EVAL(car(x));
         j = cnt = (int)unBox(y);
         n = f.i = f.cnt = 0;
         do {
            ++n;
            if ((i = p->i) <= 0  &&  (p->i -= cnt, i == 0)) {
               for (i = 0;  i < p->cnt;  ++i) {
                  y = val(p->bnd[i].sym);
                  val(p->bnd[i].sym) = p->bnd[i].val;
                  p->bnd[i].val = y;
               }
               if (p->cnt  &&  p->bnd[0].sym == At  &&  !--j)
                  break;
            }
         } while (p = p->link);
         while (isCell(x)) {
            for (p = Env.bind, j = n; ; p = p->link) {
               if (p->i < 0)
                  for (i = 0;  i < p->cnt;  ++i) {
                     if (p->bnd[i].sym == car(x)) {
                        f.bnd[f.cnt].val = val(f.bnd[f.cnt].sym = car(x));
                        val(car(x)) = p->bnd[i].val;
                        ++f.cnt;
                        goto next;
                     }
                  }
               if (!--j)
                  break;
            }
next:       x = cdr(x);
         }
         f.link = Env.bind,  Env.bind = (bindFrame*)&f;
         data(c1) = isSym(data(c1))? val(data(c1)) : prog(data(c1));
         while (--f.cnt >= 0)
            val(f.bnd[f.cnt].sym) = f.bnd[f.cnt].val;
         Env.bind = f.link;
         do {
            for (p = Env.bind, i = n;  --i;  p = p->link);
            if (p->i < 0  &&  (p->i += cnt) == 0)
               for (i = p->cnt;  --i >= 0;) {
                  y = val(p->bnd[i].sym);
                  val(p->bnd[i].sym) = p->bnd[i].val;
                  p->bnd[i].val = y;
               }
         } while (--n);
      }
      drop(c1);
   }
   return data(c1);
}

// (def 'sym 'any) -> sym
// (def 'sym 'sym 'any) -> sym
any doDef(any ex) {
   any x, y;
   cell c1, c2, c3;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedSymb(ex,data(c1));
   x = cdr(x),  Push(c2, EVAL(car(x)));
   if (!isCell(cdr(x))) {
      CheckVar(ex,data(c1));
      if (!isNil(y = val(data(c1)))  &&  y != data(c1)  &&  !equal(data(c2), y))
         redefMsg(data(c1),NULL);
      val(data(c1)) = data(c2);
   }
   else {
      x = cdr(x),  Push(c3, EVAL(car(x)));
      if (!isNil(y = get(data(c1), data(c2)))  &&  !equal(data(c3), y))
         redefMsg(data(c1), data(c2));
      put(data(c1), data(c2), data(c3));
   }
   return Pop(c1);
}

// (de sym . any) -> sym
any doDe(any ex) {
   redefine(ex, cadr(ex), cddr(ex));
   return cadr(ex);
}

// (dm sym . fun) -> sym
// (dm (sym . cls) . fun) -> sym
// (dm (sym sym [. cls]) . fun) -> sym
any doDm(any ex) {
   any x, y, msg, cls;

   x = cdr(ex);
   if (!isCell(car(x)))
      msg = car(x),  cls = val(Class);
   else {
      msg = caar(x);
      cls = !isCell(cdar(x))? cdar(x) :
         get(isNil(cddar(x))? val(Class) : cddar(x), cadar(x));
   }
   if (msg != T)
      redefine(ex, msg, val(Meth));
   if (isSymb(cdr(x))) {
      y = val(cdr(x));
      for (;;) {
         if (!isCell(y) || !isCell(car(y)))
            err(ex, msg, "Bad message");
         if (caar(y) == msg) {
            x = car(y);
            break;
         }
         y = cdr(y);
      }
   }
   for (y = val(cls);  isCell(y) && isCell(car(y));  y = cdr(y))
      if (caar(y) == msg) {
         if (!equal(cdr(x), cdar(y)))
            redefMsg(msg,cls);
         cdar(y) = cdr(x);
         return msg;
      }
   if (!isCell(car(x)))
      val(cls) = cons(x, val(cls));
   else
      val(cls) = cons(cons(msg, cdr(x)), val(cls));
   return msg;
}

/* Evaluate method invocation */
static any evMethod(any o, any expr, any x) {
   any y = car(expr);
   any cls = TheCls, key = TheKey;
   struct {  // bindFrame
      struct bindFrame *link;
      int i, cnt;
      struct {any sym; any val;} bnd[length(y)+3];
   } f;

   f.link = Env.bind,  Env.bind = (bindFrame*)&f;
   f.i = sizeof(f.bnd) / (2*sizeof(any)) - 2;
   f.cnt = 1,  f.bnd[0].sym = At,  f.bnd[0].val = val(At);
   while (isCell(y)) {
      f.bnd[f.cnt].sym = car(y);
      f.bnd[f.cnt].val = EVAL(car(x));
      ++f.cnt, x = cdr(x), y = cdr(y);
   }
   if (isNil(y)) {
      do {
         x = val(f.bnd[--f.i].sym);
         val(f.bnd[f.i].sym) = f.bnd[f.i].val;
         f.bnd[f.i].val = x;
      } while (f.i);
      f.bnd[f.cnt].sym = This,  f.bnd[f.cnt++].val = val(This),  val(This) = o;
      y = cls,  cls = Env.cls;  Env.cls = y;
      y = key,  key = Env.key;  Env.key = y;
      x = prog(cdr(expr));
   }
   else if (y != At) {
      f.bnd[f.cnt].sym = y,  f.bnd[f.cnt++].val = val(y),  val(y) = x;
      do {
         x = val(f.bnd[--f.i].sym);
         val(f.bnd[f.i].sym) = f.bnd[f.i].val;
         f.bnd[f.i].val = x;
      } while (f.i);
      f.bnd[f.cnt].sym = This,  f.bnd[f.cnt++].val = val(This),  val(This) = o;
      y = cls,  cls = Env.cls;  Env.cls = y;
      y = key,  key = Env.key;  Env.key = y;
      x = prog(cdr(expr));
   }
   else {
      int n, cnt;
      cell *arg;
      cell c[n = cnt = length(x)];

      while (--n >= 0)
         Push(c[n], EVAL(car(x))),  x = cdr(x);
      do {
         x = val(f.bnd[--f.i].sym);
         val(f.bnd[f.i].sym) = f.bnd[f.i].val;
         f.bnd[f.i].val = x;
      } while (f.i);
      n = Env.next,  Env.next = cnt;
      arg = Env.arg,  Env.arg = c;
      f.bnd[f.cnt].sym = This,  f.bnd[f.cnt++].val = val(This),  val(This) = o;
      y = cls,  cls = Env.cls;  Env.cls = y;
      y = key,  key = Env.key;  Env.key = y;
      x = prog(cdr(expr));
      if (cnt)
         drop(c[cnt-1]);
      Env.arg = arg,  Env.next = n;
   }
   while (--f.cnt >= 0)
      val(f.bnd[f.cnt].sym) = f.bnd[f.cnt].val;
   Env.bind = f.link;
   Env.cls = cls,  Env.key = key;
   return x;
}

any method(any x) {
   any y, z;

   if (isCell(y = val(x))) {
      while (isCell(z = car(y))) {
         if (car(z) == TheKey)
            return cdr(z);
         if (!isCell(y = cdr(y)))
            return NULL;
      }
      do
         if (x = method(car(TheCls = y)))
            return x;
      while (isCell(y = cdr(y)));
   }
   return NULL;
}

// (box 'any) -> sym
any doBox(any x) {
   x = cdr(x);
   return consSym(EVAL(car(x)),0);
}

// (new ['typ ['any ..]]) -> obj
any doNew(any ex) {
   any x, y;
   cell c1, c2;

   x = cdr(ex);
   Push(c1, consSym(EVAL(car(x)),0));
   TheKey = T,  TheCls = NULL;
   if (y = method(data(c1)))
      evMethod(data(c1), y, cdr(x));
   else {
      Save(c2);
      while (isCell(x = cdr(x))) {
         data(c2) = EVAL(car(x)),  x = cdr(x);
         put(data(c1), data(c2), EVAL(car(x)));
      }
   }
   return Pop(c1);
}

// (type 'any) -> lst
any doType(any ex) {
   any x, y, z;

   x = cdr(ex),  x = EVAL(car(x));
   if (isSymb(x)) {
      z = x = val(x);
      while (isCell(x)) {
         if (!isCell(car(x))) {
            y = x;
            while (isSymb(car(x))) {
               if (!isCell(x = cdr(x)))
                  return isNil(x)? y : Nil;
               if (z == x)
                  return Nil;
            }
            return Nil;
         }
         if (z == (x = cdr(x)))
            return Nil;
      }
   }
   return Nil;
}

static bool isa(any ex, any cls, any x) {
   any z;

   z = x = val(x);
   while (isCell(x)) {
      if (!isCell(car(x))) {
         while (isSymb(car(x))) {
            if (cls == car(x) || isa(ex, cls, car(x)))
               return YES;
            if (!isCell(x = cdr(x)) || z == x)
               return NO;
         }
         return NO;
      }
      if (z == (x = cdr(x)))
         return NO;
   }
   return NO;
}

// (isa 'cls|typ 'any) -> obj | NIL
any doIsa(any ex) {
   any x;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   x = cdr(x),  x = EVAL(car(x));
   drop(c1);
   if (isSymb(x)) {
      if (isSymb(data(c1)))
         return isa(ex, data(c1), x)? x : Nil;
      while (isCell(data(c1))) {
         if (!isa(ex, car(data(c1)), x))
            return Nil;
         data(c1) = cdr(data(c1));
      }
      return x;
   }
   return Nil;
}

// (method 'msg 'obj) -> fun
any doMethod(any ex) {
   any x, y;

   x = cdr(ex),  y = EVAL(car(x));
   x = cdr(x),  x = EVAL(car(x));
   TheKey = y;
   return method(x)? : Nil;
}

// (meth 'obj ..) -> any
any doMeth(any ex) {
   any x, y;
   cell c1;

   x = cdr(ex),  Push(c1, EVAL(car(x)));
   NeedSymb(ex,data(c1));
   for (TheKey = car(ex); ; TheKey = val(TheKey)) {
      if (!isSymb(TheKey))
         err(ex, car(ex), "Bad message");
      if (isNum(val(TheKey))) {
         TheCls = NULL;
         if (y = method(data(c1))) {
            x = evMethod(data(c1), y, cdr(x));
            drop(c1);
            return x;
         }
         err(ex, TheKey, "Bad message");
      }
   }
}

// (send 'msg 'obj ['any ..]) -> any
any doSend(any ex) {
   any x, y;
   cell c1, c2;

   x = cdr(ex),  Push(c1,  EVAL(car(x)));
   NeedSymb(ex,data(c1));
   x = cdr(x),  Push(c2,  EVAL(car(x)));
   NeedSymb(ex,data(c2));
   TheKey = data(c1),  TheCls = NULL;
   if (y = method(data(c2))) {
      x = evMethod(data(c2), y, cdr(x));
      drop(c1);
      return x;
   }
   err(ex, TheKey, "Bad message");
}

// (try 'msg 'obj ['any ..]) -> any
any doTry(any ex) {
   any x, y;
   cell c1, c2;

   x = cdr(ex),  Push(c1,  EVAL(car(x)));
   NeedSymb(ex,data(c1));
   x = cdr(x),  Push(c2,  EVAL(car(x)));
   if (isSymb(data(c2))) {
      TheKey = data(c1),  TheCls = NULL;
      if (y = method(data(c2))) {
         x = evMethod(data(c2), y, cdr(x));
         drop(c1);
         return x;
      }
   }
   drop(c1);
   return Nil;
}

// (super ['any ..]) -> any
any doSuper(any ex) {
   any x, y, cls, key;

   TheKey = Env.key;
   x = val(Env.cls? car(Env.cls) : val(This));
   while (isCell(car(x)))
      x = cdr(x);
   while (isCell(x)) {
      if (y = method(car(TheCls = x))) {
         cls = Env.cls,  Env.cls = TheCls;
         key = Env.key,  Env.key = TheKey;
         x = evExpr(y, cdr(ex));
         Env.key = key,  Env.cls = cls;
         return x;
      }
      x = cdr(x);
   }
   err(ex, TheKey, "Bad super");
}

static any extra(any x) {
   any y;

   for (x = val(x); isCell(car(x)); x = cdr(x));
   while (isCell(x)) {
      if (x == Env.cls  ||  !(y = extra(car(x)))) {
         while (isCell(x = cdr(x)))
            if (y = method(car(TheCls = x)))
               return y;
         return NULL;
      }
      if (y  &&  y != Zero)
         return y;
      x = cdr(x);
   }
   return Zero;
}

// (extra ['any ..]) -> any
any doExtra(any ex) {
   any x, y, cls, key;

   TheKey = Env.key;
   if ((y = extra(val(This)))  &&  y != Zero) {
      cls = Env.cls,  Env.cls = TheCls;
      key = Env.key,  Env.key = TheKey;
      x = evExpr(y, cdr(ex));
      Env.key = key,  Env.cls = cls;
      return x;
   }
   err(ex, TheKey, "Bad extra");
}

// (with 'sym . prg) -> any
any doWith(any ex) {
   any x;
   bindFrame f;

   x = cdr(ex);
   if (isNil(x = EVAL(car(x))))
      return Nil;
   NeedSymb(ex,x);
   Bind(This,f),  val(This) = x;
   x = prog(cddr(ex));
   Unbind(f);
   return x;
}

// (bind 'sym|lst . prg) -> any
any doBind(any ex) {
   any x, y;

   x = cdr(ex);
   if (isNum(y = EVAL(car(x))))
      argError(ex, y);
   if (isNil(y))
      return prog(cdr(x));
   if (isSym(y)) {
      bindFrame f;

      Bind(y,f);
      x = prog(cdr(x));
      Unbind(f);
      return x;
   }
   {
      struct {  // bindFrame
         struct bindFrame *link;
         int i, cnt;
         struct {any sym; any val;} bnd[length(y)];
      } f;

      f.link = Env.bind,  Env.bind = (bindFrame*)&f;
      f.i = f.cnt = 0;
      do {
         if (isNum(car(y)))
            argError(ex, car(y));
         if (isSym(car(y))) {
            f.bnd[f.cnt].sym = car(y);
            f.bnd[f.cnt].val = val(car(y));
         }
         else {
            f.bnd[f.cnt].sym = caar(y);
            f.bnd[f.cnt].val = val(caar(y));
            val(caar(y)) = cdar(y);
         }
         ++f.cnt;
      } while (isCell(y = cdr(y)));
      x = prog(cdr(x));
      while (--f.cnt >= 0)
         val(f.bnd[f.cnt].sym) = f.bnd[f.cnt].val;
      Env.bind = f.link;
      return x;
   }
}

// (job 'lst . prg) -> any
any doJob(any ex) {
   any x = cdr(ex);
   any y = EVAL(car(x));
   cell c1;
   struct {  // bindFrame
      struct bindFrame *link;
      int i, cnt;
      struct {any sym; any val;} bnd[length(y)];
   } f;

   Push(c1,y);
   f.link = Env.bind,  Env.bind = (bindFrame*)&f;
   f.i = f.cnt = 0;
   while (isCell(y)) {
      f.bnd[f.cnt].sym = caar(y);
      f.bnd[f.cnt].val = val(caar(y));
      val(caar(y)) = cdar(y);
      ++f.cnt,  y = cdr(y);
   }
   x = prog(cdr(x));
   for (f.cnt = 0, y = Pop(c1);  isCell(y);  ++f.cnt, y = cdr(y)) {
      cdar(y) = val(caar(y));
      val(caar(y)) = f.bnd[f.cnt].val;
   }
   Env.bind = f.link;
   return x;
}

// (let sym 'any . prg) -> any
// (let (sym 'any ..) . prg) -> any
any doLet(any x) {
   any y;

   x = cdr(x);
   if (!isCell(y = car(x))) {
      bindFrame f;

      x = cdr(x),  Bind(y,f),  val(y) = EVAL(car(x));
      x = prog(cdr(x));
      Unbind(f);
   }
   else {
      struct {  // bindFrame
         struct bindFrame *link;
         int i, cnt;
         struct {any sym; any val;} bnd[(length(y)+1)/2];
      } f;

      f.link = Env.bind,  Env.bind = (bindFrame*)&f;
      f.i = f.cnt = 0;
      do {
         f.bnd[f.cnt].sym = car(y);
         f.bnd[f.cnt].val = val(car(y));
         ++f.cnt;
         val(car(y)) = EVAL(cadr(y));
      } while (isCell(y = cddr(y)));
      x = prog(cdr(x));
      while (--f.cnt >= 0)
         val(f.bnd[f.cnt].sym) = f.bnd[f.cnt].val;
      Env.bind = f.link;
   }
   return x;
}

// (let? sym 'any . prg) -> any
any doLetQ(any x) {
   any y, z;
   bindFrame f;

   x = cdr(x),  y = car(x),  x = cdr(x);
   if (isNil(z = EVAL(car(x))))
      return Nil;
   Bind(y,f),  val(y) = z;
   x = prog(cdr(x));
   Unbind(f);
   return x;
}

// (use sym . prg) -> any
// (use (sym ..) . prg) -> any
any doUse(any x) {
   any y;

   x = cdr(x);
   if (!isCell(y = car(x))) {
      bindFrame f;

      Bind(y,f);
      x = prog(cdr(x));
      Unbind(f);
   }
   else {
      struct {  // bindFrame
         struct bindFrame *link;
         int i, cnt;
         struct {any sym; any val;} bnd[length(y)];
      } f;

      f.link = Env.bind,  Env.bind = (bindFrame*)&f;
      f.i = f.cnt = 0;
      do {
         f.bnd[f.cnt].sym = car(y);
         f.bnd[f.cnt].val = val(car(y));
         ++f.cnt;
      } while (isCell(y = cdr(y)));
      x = prog(cdr(x));
      while (--f.cnt >= 0)
         val(f.bnd[f.cnt].sym) = f.bnd[f.cnt].val;
      Env.bind = f.link;
   }
   return x;
}

// (and 'any ..) -> any
any doAnd(any x) {
   any a;

   x = cdr(x);
   do {
      if (isNil(a = EVAL(car(x))))
         return Nil;
      val(At) = a;
   } while (isCell(x = cdr(x)));
   return a;
}

// (or 'any ..) -> any
any doOr(any x) {
   any a;

   x = cdr(x);
   do
      if (!isNil(a = EVAL(car(x))))
         return val(At) = a;
   while (isCell(x = cdr(x)));
   return Nil;
}

// (nand 'any ..) -> flg
any doNand(any x) {
   any a;

   x = cdr(x);
   do {
      if (isNil(a = EVAL(car(x))))
         return T;
      val(At) = a;
   } while (isCell(x = cdr(x)));
   return Nil;
}

// (nor 'any ..) -> flg
any doNor(any x) {
   any a;

   x = cdr(x);
   do
      if (!isNil(a = EVAL(car(x)))) {
         val(At) = a;
         return Nil;
      }
   while (isCell(x = cdr(x)));
   return T;
}

// (xor 'any 'any) -> flg
any doXor(any x) {
   bool f;

   x = cdr(x),  f = isNil(EVAL(car(x))),  x = cdr(x);
   return  f ^ isNil(EVAL(car(x)))?  T : Nil;
}

// (bool 'any) -> flg
any doBool(any x) {return isNil(EVAL(cadr(x)))? Nil : T;}

// (not 'any) -> flg
any doNot(any x) {
   any a;

   if (isNil(a = EVAL(cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (nil . prg) -> NIL
any doNil(any x) {
   while (isCell(x = cdr(x)))
      if (isCell(car(x)))
         evList(car(x));
   return Nil;
}

// (t . prg) -> T
any doT(any x) {
   while (isCell(x = cdr(x)))
      if (isCell(car(x)))
         evList(car(x));
   return T;
}

// (prog . prg) -> any
any doProg(any x) {return prog(cdr(x));}

// (prog1 'any1 . prg) -> any1
any doProg1(any x) {
   cell c1;

   x = cdr(x),  Push(c1, val(At) = EVAL(car(x)));
   while (isCell(x = cdr(x)))
      if (isCell(car(x)))
         evList(car(x));
   return Pop(c1);
}

// (prog2 'any1 'any2 . prg) -> any2
any doProg2(any x) {
   cell c1;

   x = cdr(x),  EVAL(car(x));
   x = cdr(x),  Push(c1, val(At) = EVAL(car(x)));
   while (isCell(x = cdr(x)))
      if (isCell(car(x)))
         evList(car(x));
   return Pop(c1);
}

// (if 'any1 'any2 . prg) -> any
any doIf(any x) {
   any a;

   x = cdr(x);
   if (isNil(a = EVAL(car(x))))
      return prog(cddr(x));
   val(At) = a;
   x = cdr(x);
   return EVAL(car(x));
}

// (if2 'any1 'any2 'any3 'any4 'any5 . prg) -> any
any doIf2(any x) {
   any a;

   x = cdr(x);
   if (isNil(a = EVAL(car(x)))) {
      x = cdr(x);
      if (isNil(a = EVAL(car(x))))
         return prog(cddddr(x));
      val(At) = a;
      x = cdddr(x);
      return EVAL(car(x));
   }
   val(At) = a;
   x = cdr(x);
   if (isNil(a = EVAL(car(x)))) {
      x = cddr(x);
      return EVAL(car(x));
   }
   val(At) = a;
   x = cdr(x);
   return EVAL(car(x));
}

// (ifn 'any1 'any2 . prg) -> any
any doIfn(any x) {
   any a;

   x = cdr(x);
   if (!isNil(a = EVAL(car(x)))) {
      val(At) = a;
      return prog(cddr(x));
   }
   x = cdr(x);
   return EVAL(car(x));
}

// (when 'any . prg) -> any
any doWhen(any x) {
   any a;

   x = cdr(x);
   if (isNil(a = EVAL(car(x))))
      return Nil;
   val(At) = a;
   return prog(cdr(x));
}

// (unless 'any . prg) -> any
any doUnless(any x) {
   any a;

   x = cdr(x);
   if (!isNil(a = EVAL(car(x)))) {
      val(At) = a;
      return Nil;
   }
   return prog(cdr(x));
}

// (cond ('any1 . prg1) ('any2 . prg2) ..) -> any
any doCond(any x) {
   any a;

   while (isCell(x = cdr(x))) {
      if (!isNil(a = EVAL(caar(x)))) {
         val(At) = a;
         return prog(cdar(x));
      }
   }
   return Nil;
}

// (nond ('any1 . prg1) ('any2 . prg2) ..) -> any
any doNond(any x) {
   any a;

   while (isCell(x = cdr(x))) {
      if (isNil(a = EVAL(caar(x))))
         return prog(cdar(x));
      val(At) = a;
   }
   return Nil;
}

// (case 'any (any1 . prg1) (any2 . prg2) ..) -> any
any doCase(any x) {
   any y, z;

   x = cdr(x),  val(At) = EVAL(car(x));
   while (isCell(x = cdr(x))) {
      y = car(x),  z = car(y);
      if (z == T  ||  equal(val(At), z))
         return prog(cdr(y));
      if (isCell(z)) {
         do
            if (equal(val(At), car(z)))
               return prog(cdr(y));
         while (isCell(z = cdr(z)));
      }
   }
   return Nil;
}

// (casq 'any (any1 . prg1) (any2 . prg2) ..) -> any
any doCasq(any x) {
   any y, z;

   x = cdr(x),  val(At) = EVAL(car(x));
   while (isCell(x = cdr(x))) {
      y = car(x),  z = car(y);
      if (z == T  ||  z == val(At))
         return prog(cdr(y));
      if (isCell(z)) {
         do
            if (car(z) == val(At))
               return prog(cdr(y));
         while (isCell(z = cdr(z)));
      }
   }
   return Nil;
}

// (state 'var (sym|lst exe [. prg]) ..) -> any
any doState(any ex) {
   any x, y, a;
   cell c1;

   x = cdr(ex);
   Push(c1, EVAL(car(x)));
   NeedVar(ex,data(c1));
   CheckVar(ex,data(c1));
   while (isCell(x = cdr(x))) {
      y = car(x);
      if (car(y) == T || memq(val(data(c1)), car(y))) {
         y = cdr(y);
         if (!isNil(a = EVAL(car(y)))) {
            val(At) = val(data(c1)) = a;
            drop(c1);
            return prog(cdr(y));
         }
      }
   }
   drop(c1);
   return Nil;
}

// (while 'any . prg) -> any
any doWhile(any x) {
   any cond, a;
   cell c1;

   cond = car(x = cdr(x)),  x = cdr(x);
   Push(c1, Nil);
   while (!isNil(a = EVAL(cond))) {
      val(At) = a;
      data(c1) = prog(x);
   }
   return Pop(c1);
}

// (until 'any . prg) -> any
any doUntil(any x) {
   any cond, a;
   cell c1;

   cond = car(x = cdr(x)),  x = cdr(x);
   Push(c1, Nil);
   while (isNil(a = EVAL(cond)))
      data(c1) = prog(x);
   val(At) = a;
   return Pop(c1);
}

// (loop ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doLoop(any ex) {
   any x, y, a;

   for (;;) {
      x = cdr(ex);
      do {
         if (isCell(y = car(x))) {
            if (isNil(car(y))) {
               y = cdr(y);
               if (isNil(a = EVAL(car(y))))
                  return prog(cdr(y));
               val(At) = a;
            }
            else if (car(y) == T) {
               y = cdr(y);
               if (!isNil(a = EVAL(car(y)))) {
                  val(At) = a;
                  return prog(cdr(y));
               }
            }
            else
               evList(y);
         }
      } while (isCell(x = cdr(x)));
   }
}

// (do 'flg|num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doDo(any x) {
   any f, y, z, a;

   x = cdr(x);
   if (isNil(f = EVAL(car(x))))
      return Nil;
   if (isNum(f) && num(f) < 0)
      return Nil;
   x = cdr(x),  z = Nil;
   for (;;) {
      if (isNum(f)) {
         if (f == Zero)
            return z;
         f = (any)(num(f) - 4);
      }
      y = x;
      do {
         if (!isNum(z = car(y))) {
            if (isSym(z))
               z = val(z);
            else if (isNil(car(z))) {
               z = cdr(z);
               if (isNil(a = EVAL(car(z))))
                  return prog(cdr(z));
               val(At) = a;
               z = Nil;
            }
            else if (car(z) == T) {
               z = cdr(z);
               if (!isNil(a = EVAL(car(z)))) {
                  val(At) = a;
                  return prog(cdr(z));
               }
               z = Nil;
            }
            else
               z = evList(z);
         }
      } while (isCell(y = cdr(y)));
   }
}

// (at '(cnt1 . cnt2|NIL) . prg) -> any
any doAt(any ex) {
   any x;

   x = cdr(ex),  x = EVAL(car(x));
   NeedPair(ex,x);
   if (isNil(cdr(x)))
      return Nil;
   NeedNum(ex,car(x));
   NeedNum(ex,cdr(x));
   if (num(car(x) = (any)(num(car(x)) + 4)) < num(cdr(x)))
      return Nil;
   car(x) = Zero;
   return prog(cddr(ex));
}

// (for sym 'num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for sym|(sym2 . sym) 'lst ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for (sym|(sym2 . sym) 'any1 'any2 [. prg]) ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doFor(any x) {
   any y, body, cond, a;
   cell c1;
   struct {  // bindFrame
      struct bindFrame *link;
      int i, cnt;
      struct {any sym; any val;} bnd[2];
   } f;

   f.link = Env.bind,  Env.bind = (bindFrame*)&f;
   f.i = 0;
   if (!isCell(y = car(x = cdr(x))) || !isCell(cdr(y))) {
      if (!isCell(y)) {
         f.cnt = 1;
         f.bnd[0].sym = y;
         f.bnd[0].val = val(y);
      }
      else {
         f.cnt = 2;
         f.bnd[0].sym = cdr(y);
         f.bnd[0].val = val(cdr(y));
         f.bnd[1].sym = car(y);
         f.bnd[1].val = val(car(y));
         val(f.bnd[1].sym) = Zero;
      }
      y = Nil;
      x = cdr(x),  Push(c1, EVAL(car(x)));
      if (isNum(data(c1)))
         val(f.bnd[0].sym) = Zero;
      body = x = cdr(x);
      for (;;) {
         if (isNum(data(c1))) {
            val(f.bnd[0].sym) = (any)(num(val(f.bnd[0].sym)) + 4);
            if (num(val(f.bnd[0].sym)) > num(data(c1)))
               break;
         }
         else {
            if (!isCell(data(c1)))
               break;
            val(f.bnd[0].sym) = car(data(c1));
            if (!isCell(data(c1) = cdr(data(c1))))
               data(c1) = Nil;
         }
         if (f.cnt == 2)
            val(f.bnd[1].sym) = (any)(num(val(f.bnd[1].sym)) + 4);
         do {
            if (!isNum(y = car(x))) {
               if (isSym(y))
                  y = val(y);
               else if (isNil(car(y))) {
                  y = cdr(y);
                  if (isNil(a = EVAL(car(y)))) {
                     y = prog(cdr(y));
                     goto for1;
                  }
                  val(At) = a;
                  y = Nil;
               }
               else if (car(y) == T) {
                  y = cdr(y);
                  if (!isNil(a = EVAL(car(y)))) {
                     val(At) = a;
                     y = prog(cdr(y));
                     goto for1;
                  }
                  y = Nil;
               }
               else
                  y = evList(y);
            }
         } while (isCell(x = cdr(x)));
         x = body;
      }
   for1:
      drop(c1);
      if (f.cnt == 2)
         val(f.bnd[1].sym) = f.bnd[1].val;
      val(f.bnd[0].sym) = f.bnd[0].val;
      Env.bind = f.link;
      return y;
   }
   if (!isCell(car(y))) {
      f.cnt = 1;
      f.bnd[0].sym = car(y);
      f.bnd[0].val = val(car(y));
   }
   else {
      f.cnt = 2;
      f.bnd[0].sym = cdar(y);
      f.bnd[0].val = val(cdar(y));
      f.bnd[1].sym = caar(y);
      f.bnd[1].val = val(caar(y));
      val(f.bnd[1].sym) = Zero;
   }
   y = cdr(y);
   val(f.bnd[0].sym) = EVAL(car(y));
   y = cdr(y),  cond = car(y),  y = cdr(y);
   Push(c1,Nil);
   body = x = cdr(x);
   for (;;) {
      if (f.cnt == 2)
         val(f.bnd[1].sym) = (any)(num(val(f.bnd[1].sym)) + 4);
      if (isNil(a = EVAL(cond)))
         break;
      val(At) = a;
      if (f.cnt == 2)
         val(f.bnd[1].sym) = (any)(num(val(f.bnd[1].sym)) + 4);
      do {
         if (!isNum(data(c1) = car(x))) {
            if (isSym(data(c1)))
               data(c1) = val(data(c1));
            else if (isNil(car(data(c1)))) {
               data(c1) = cdr(data(c1));
               if (isNil(a = EVAL(car(data(c1))))) {
                  data(c1) = prog(cdr(data(c1)));
                  goto for2;
               }
               val(At) = a;
               data(c1) = Nil;
            }
            else if (car(data(c1)) == T) {
               data(c1) = cdr(data(c1));
               if (!isNil(a = EVAL(car(data(c1))))) {
                  val(At) = a;
                  data(c1) = prog(cdr(data(c1)));
                  goto for2;
               }
               data(c1) = Nil;
            }
            else
               data(c1) = evList(data(c1));
         }
      } while (isCell(x = cdr(x)));
      if (isCell(y))
         val(f.bnd[0].sym) = prog(y);
      x = body;
   }
for2:
   if (f.cnt == 2)
      val(f.bnd[1].sym) = f.bnd[1].val;
   val(f.bnd[0].sym) = f.bnd[0].val;
   Env.bind = f.link;
   return Pop(c1);
}

// (catch 'any . prg) -> any
any doCatch(any x) {
   any y;
   catchFrame f;

   x = cdr(x),  f.tag = EVAL(car(x)),  f.fin = Zero;
   f.link = CatchPtr,  CatchPtr = &f;
   f.env = Env;
   y = setjmp(f.rst)? Thrown : prog(cdr(x));
   CatchPtr = f.link;
   return y;
}

// (throw 'sym 'any)
any doThrow(any ex) {
   any x, tag;
   catchFrame *p;

   x = cdr(ex),  tag = EVAL(car(x));
   x = cdr(x),  Thrown = EVAL(car(x));
   for (p = CatchPtr;  p;  p = p->link)
      if (p->tag == T  ||  tag == p->tag) {
         unwind(p);
         longjmp(p->rst, 1);
      }
   err(ex, tag, "Tag not found");
}

// (finally exe . prg) -> any
any doFinally(any x) {
   catchFrame f;
   cell c1;

   x = cdr(x),  f.tag = NULL,  f.fin = car(x);
   f.link = CatchPtr,  CatchPtr = &f;
   f.env = Env;
   Push(c1, prog(cdr(x)));
   EVAL(f.fin);
   CatchPtr = f.link;
   return Pop(c1);
}

static outFrame Out;
static struct {  // bindFrame
   struct bindFrame *link;
   int i, cnt;
   struct {any sym; any val;} bnd[2];  // for 'Up' and 'At'
} Brk;

void brkLoad(any x) {
   if (!Env.brk) {
      Env.brk = YES;
      Brk.cnt = 2;
      Brk.bnd[0].sym = Up,  Brk.bnd[0].val = val(Up),  val(Up) = x;
      Brk.bnd[1].sym = At,  Brk.bnd[1].val = val(At);
      Brk.link = Env.bind,  Env.bind = (bindFrame*)&Brk;
      Out.fp = stdout,  pushOutFiles(&Out);
      print(x), newline();
      load(NULL, '!', Nil);
      popOutFiles();
      val(At) = Brk.bnd[1].val;
      val(Up) = Brk.bnd[0].val;
      Env.bind = Brk.link;
      Env.brk = NO;
   }
}

// (! . exe) -> any
any doBreak(any ex) {
   if (!isNil(val(Dbg)))
      brkLoad(cdr(ex));
   return EVAL(cdr(ex));
}

// (e . prg) -> any
any doE(any ex) {
   any x;
   cell c1, at;

   if (!Env.brk)
      err(ex, NULL, "No Break");
   Push(c1,val(Dbg)),  val(Dbg) = Nil;
   Push(at, val(At)),  val(At) = Brk.bnd[1].val;
   if (Env.inFrames && Env.inFrames->link)
      Chr = Env.inFrames->next,  Env.get = Env.inFrames->get,  InFile = Env.inFrames->link->fp;
   popOutFiles();
   x = isCell(cdr(ex))? prog(cdr(ex)) : EVAL(val(Up));
   pushOutFiles(&Out);
   if (Env.inFrames && Env.inFrames->link)
      Env.inFrames->next = Chr,  Chr = 0;
   InFile = stdin,  OutFile = stdout;
   val(At) = data(at);
   val(Dbg) = Pop(c1);
   return x;
}

static void traceIndent(int i, any x, char *s) {
   if (i > 64)
      i = 64;
   while (--i >= 0)
      Env.put(' ');
   if (!isCell(x))
      print(x);
   else
      print(car(x)), space(), print(cdr(x)), space(), print(val(This));
   outString(s);
}

// ($ sym|lst lst . prg) -> any
any doTrace(any x) {
   any foo, body;
   FILE *oSave;
   void (*putSave)(int);
   cell c1;

   x = cdr(x);
   if (isNil(val(Dbg)))
      return prog(cddr(x));
   oSave = OutFile,  OutFile = stderr;
   putSave = Env.put,  Env.put = putStdout;
   foo = car(x);
   x = cdr(x),  body = cdr(x);
   traceIndent(++Trace, foo, " :");
   for (x = car(x);  isCell(x);  x = cdr(x))
      space(), print(val(car(x)));
   if (!isNil(x)) {
      if (x != At)
         space(), print(val(x));
      else {
         int i = Env.next;

         while (--i >= 0)
            space(), print(data(Env.arg[i]));
      }
   }
   newline();
   Env.put = putSave;
   OutFile = oSave;
   Push(c1, prog(body));
   OutFile = stderr;
   Env.put = putStdout;
   traceIndent(Trace--, foo, " = "),  print(data(c1)),  newline();
   Env.put = putSave;
   OutFile = oSave;
   return Pop(c1);
}

// (bye 'num|NIL)
any doBye(any ex) {
   any x = EVAL(cadr(ex));

   bye(isNil(x)? 0 : xNum(ex,x));
}
