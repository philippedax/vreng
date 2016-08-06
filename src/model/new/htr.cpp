#include <stdio.h> 
#include <string.h> 
#include "HTRFormat.h" 
bool HTRFormat::ImportData(const char * filename) 
* { 
* i
* nt r
* e
* ad, 
* i
* , j,
* wher
* e
* ; 
* i
* nt p
* o
* s[8]
* ;
*  // 
*  U
*  sed 
*  t
*  o de
*  t
*  ermi
*  n
*  e th
*  e
*   pos
*   i
*   tion
*   of t
*   h
*   e ne
*   x
*   t ch
*   a
*   r to
*   writ
*   e 
*   c
*   har 
*   l
*   ine[
*   8
*   ][40
*   ]
*   ;   
*    // 
*    U
*    sed 
*    t
*    o st
*    o
*    re t
*    h
*    e at
*    t
*    ribu
*    t
*    e an
*    d
*     the
*     corr
*     e
*     spon
*     d
*     ing 
*     v
*     alue 
*     c
*     har 
*     b
*     uffe
*     r
*     [409
*     7
*     ]; 
*     i
*     nt s
*     e
*     ctio
*     n
*      = 0
*      ;
*       // 
*       I
*       ndic
*       a
*       tes 
*       w
*       hich
*       sect
*       i
*       on i
*       s
*        cur
*        r
*        entl
*        y
*         bei
*         n
*         g pr
*         o
*         cess
*         e
*         d 
*         N
*         ODE 
*         *
*         tnod
*         e
*         ;   
*          // 
*          U
*          sed 
*          w
*          hen 
*          s
*          etti
*          n
*          g up
*          the 
*          b
*          ase 
*          p
*          osit
*          i
*          ons 
*          a
*          nd f
*          r
*          ames 
*          f
*          loat
*          **ba
*          s
*          e, *
*          *
*          rot,
*          **ar
*          o
*          t, *
*          *
*          trot
*          ; 
*          f
*          loat
*          ang[
*          3
*          ], n
*          u
*          m, d
*          e
*          n; 
*          b
*          ool 
*          e
*          of=f
*          a
*          lse; 
*          b
*          ase=
*          n
*          ew f
*          l
*          oat*
*          [
*          3]; 
*          r
*          ot=n
*          e
*          w fl
*          o
*          at*[
*          3
*          ]; 
*          a
*          rot=
*          n
*          ew f
*          l
*          oat*
*          [
*          3]; 
*          t
*          rot=
*          n
*          ew f
*          l
*          oat*
*          [
*          3]; 
*          f
*          or (
*          i
*          =0; 
*          i
*          <3; 
*          i
*          ++) 
*          { 
*           bas
*           e
*           [i]=
*           n
*           ew f
*           l
*           oat[
*           3
*           ]; 
*            rot
*            [
*            i]=n
*            e
*            w fl
*            o
*            at[3
*            ]
*            ; 
*             aro
*             t
*             [i]=
*             n
*             ew f
*             l
*             oat[
*             3
*             ]; 
*              tro
*              t
*              [i]=
*              n
*              ew f
*              l
*              oat[
*              3
*              ]; 
*              } 
*              h
*              eade
*              r
*              ->ca
*              l
*              lib 
*              =
*               1.0
*               f
*               ; 
*               h
*               eade
*               r
*               ->sc
*               a
*               lefa
*               c
*               tor 
*               =
*                1.0
*                f
*                ; 
*                  F
*                  I
*                  LE *
*                  f
*                  ile 
*                  =
*                   fop
*                   e
*                   n(fi
*                   l
*                   enam
*                   e
*                   , "r
*                   b
*                   "); 
*                   i
*                   f (f
*                   i
*                   le) 
*                   { 
*                    // 
*                    P
*                    roce
*                    s
*                    s th
*                    e
*                     "He
*                     a
*                     der"
*                     sect
*                     i
*                     on o
*                     f
*                      the
*                      file 
*                       rea
*                       d
*                        = f
*                        r
*                        ead(
*                        b
*                        uffe
*                        r
*                        ,1,4
*                        0
*                        96,f
*                        i
*                        le); 
*                         buf
*                         f
*                         er[r
*                         e
*                         ad] 
*                         =
*                          '\0'
*                          ; 
*                           i =
*                           strs
*                           t
*                           rEx(
*                           b
*                           uffe
*                           r
*                           , "[
*                           H
*                           EADE
*                           R
*                           ]"); 
*                            i+=
*                            s
*                            trst
*                            r
*                            Ex(b
*                            u
*                            ffer
*                            + i,
*                            char
*                            (
*                            10))
*                            ; 
*                             whi
*                             l
*                             e (b
*                             u
*                             ffer[
*                             +
*                             +i] 
*                             <
*                              32)
*                              ; 
*                               whe
*                               r
*                               e = 
*                               p
*                               os[0
*                               ]
*                                = p
*                                o
*                                s[1]
*                                = po
*                                s
*                                [2] 
*                                =
*                                 pos
*                                 [
*                                 3] =
*                                 pos[
*                                 4
*                                 ] = 
*                                 p
*                                 os[5
*                                 ]
*                                  = p
*                                  o
*                                  s[6]
*                                  = po
*                                  s
*                                  [7] 
*                                  =
*                                   0; 
*                                    // 
*                                    P
*                                    roce
*                                    s
*                                    s ea
*                                    c
*                                    h li
*                                    n
*                                    e in
*                                    the 
*                                    h
*                                    eade
*                                    r 
*                                     whi
*                                     l
*                                     e (r
*                                     e
*                                     ad &
*                                     &
*                                      !eo
*                                      f
*                                      ) 
*                                       { 
*                                          w
*                                          h
*                                          ile 
*                                          (
*                                          i<re
*                                          a
*                                          d &&
*                                          !eof
*                                          ) 
*                                             { 
*                                             if (
*                                             b
*                                             uffe
*                                             r
*                                             [i] 
*                                             =
*                                             ='#'
*                                             || b
*                                             u
*                                             ffer
*                                             [
*                                             i] =
*                                             =
*                                              char
*                                              (
*                                              10)
*                                              ) 
*                                              { 
*                                                //
*                                                Proc
*                                                e
*                                                ss l
*                                                i
*                                                ne 
*                                                  li
*                                                  n
*                                                  e[1]
*                                                  [
*                                                  pos[
*                                                  1
*                                                  ]] =
*                                                  line
*                                                  [
*                                                  0][p
*                                                  o
*                                                  s[0]
*                                                  ]
*                                                   = '
*                                                   \
*                                                   0'; 
*                                                     if
*                                                     (lin
*                                                     e
*                                                     [0][
*                                                     0
*                                                     ]=='
*                                                     [
*                                                     ') 
*                                                       { 
*                                                       i
*                                                       f (+
*                                                       +
*                                                       sect
*                                                       i
*                                                       on==
*                                                       2
*                                                       ) 
*                                                       { 
*                                                        // 
*                                                        B
*                                                        ody 
*                                                        s
*                                                        truc
*                                                        t
*                                                        ure 
*                                                        h
*                                                        as b
*                                                        e
*                                                        en r
*                                                        e
*                                                        ad a
*                                                        n
*                                                        d re
*                                                        a
*                                                        dy t
*                                                        o
*                                                         pro
*                                                         c
*                                                         ess 
*                                                         t
*                                                         he b
*                                                         a
*                                                         se p
*                                                         o
*                                                         siti
*                                                         o
*                                                         ns 
*                                                          // 
*                                                          S
*                                                          o as
*                                                          s
*                                                          ign 
*                                                          t
*                                                          he G
*                                                          L
*                                                          OBAL
*                                                          node
*                                                          to t
*                                                          h
*                                                          e ro
*                                                          o
*                                                          t po
*                                                          i
*                                                          nter 
*                                                           roo
*                                                           t
*                                                            = 0
*                                                            ; 
*                                                             for
*                                                             (int
*                                                             j=0;
*                                                             j<cu
*                                                             r
*                                                             rent
*                                                             n
*                                                             ode 
*                                                             &
*                                                             & !r
*                                                             o
*                                                             ot; 
*                                                             +
*                                                             +j) 
*                                                              { 
*                                                                 i
*                                                                 f
*                                                                  (st
*                                                                  r
*                                                                  comp
*                                                                  E
*                                                                  x(no
*                                                                  d
*                                                                  elis
*                                                                  t
*                                                                  [j]->
*                                                                  n
*                                                                  ame,
*                                                                  "GLO
*                                                                  B
*                                                                  AL")
*                                                                  ) 
*                                                                     { 
*                                                                     r
*                                                                     oot 
*                                                                     =
*                                                                      nod
*                                                                      e
*                                                                      list
*                                                                      [
*                                                                      j]->
*                                                                      c
*                                                                      hild
*                                                                      r
*                                                                      en[0
*                                                                      ]
*                                                                      ; 
*                                                                      root
*                                                                      -
*                                                                      >par
*                                                                      e
*                                                                      nt=0
*                                                                      ; 
*                                                                      NODE
*                                                                      **te
*                                                                      m
*                                                                      p = 
*                                                                      (
*                                                                      NODE
*                                                                      *
*                                                                      *) m
*                                                                      a
*                                                                      lloc
*                                                                      (
*                                                                      size
*                                                                      o
*                                                                      f(NO
*                                                                      D
*                                                                      E*) 
*                                                                      *
*                                                                       hea
*                                                                       d
*                                                                       er->n
*                                                                       o
*                                                                       ofse
*                                                                       g
*                                                                       ment
*                                                                       s
*                                                                       ); 
*                                                                       int 
*                                                                       m
*                                                                       ; 
*                                                                       for 
*                                                                       (
*                                                                       m=0;
*                                                                       m < 
*                                                                       j
*                                                                       ; ++
*                                                                       m
*                                                                       ) 
*                                                                         te
*                                                                         m
*                                                                         p[m]
*                                                                         = no
*                                                                         d
*                                                                         elis
*                                                                         t
*                                                                         [m]; 
*                                                                         for 
*                                                                         (
*                                                                         m=j+
*                                                                         1
*                                                                         ; m<
*                                                                         =
*                                                                         head
*                                                                         e
*                                                                         r->n
*                                                                         o
*                                                                         ofse
*                                                                         g
*                                                                         ment
*                                                                         s
*                                                                         ; ++
*                                                                         m
*                                                                         ) 
*                                                                           te
*                                                                           m
*                                                                           p[m-1
*                                                                           ]
*                                                                            = n
*                                                                            o
*                                                                            deli
*                                                                            s
*                                                                            t[m]
*                                                                            ; 
*                                                                            for 
*                                                                            (
*                                                                            m=0;
*                                                                            m<he
*                                                                            a
*                                                                            der->
*                                                                            n
*                                                                            ooff
*                                                                            r
*                                                                            ames
*                                                                            ;
*                                                                             ++m
*                                                                             ) 
*                                                                             { 
*                                                                               fr
*                                                                               e
*                                                                               e(no
*                                                                               d
*                                                                               elis
*                                                                               t
*                                                                               [j]->
*                                                                               f
*                                                                               ros
*                                                                               e
*                                                                               t[m]
*                                                                               )
*                                                                               ; 
*                                                                                 fr
*                                                                                 e
*                                                                                 e(no
*                                                                                 d
*                                                                                 elis
*                                                                                 t
*                                                                                 [j]->
*                                                                                 f
*                                                                                 reul
*                                                                                 e
*                                                                                 r[m]
*                                                                                 )
*                                                                                 ; 
*                                                                                 M
*                                                                                 o
*                                                                                 ti
*                                                                                 o
*                                                                                 n
*                                                                                 C
*                                                                                 a
*                                                                                 p
*                                                                                 t
*                                                                                 u
*                                                                                 r
*                                                                                 e
*                                                                                 F
*                                                                                 i
*                                                                                 l
*                                                                                 e
*                                                                                  F
*                                                                                  o
*                                                                                  r
*                                                                                  m
*                                                                                  a
*                                                                                  t
*                                                                                  s
*                                                                                  E
*                                                                                  xp
*                                                                                  l
*                                                                                  a
*                                                                                  i
*                                                                                  n
*                                                                                  e
*                                                                                  d
*                                                                                  32
*                                                                                  } 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist[
*                                                                                  j
*                                                                                  ]->f
*                                                                                  r
*                                                                                  oset
*                                                                                  )
*                                                                                  ; 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist[
*                                                                                  j
*                                                                                  ]->f
*                                                                                  r
*                                                                                  eule
*                                                                                  r
*                                                                                  ); 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist[
*                                                                                  j
*                                                                                  ]->s
*                                                                                  c
*                                                                                  ale)
*                                                                                  ; 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist[
*                                                                                  j
*                                                                                  ]->n
*                                                                                  a
*                                                                                  me); 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist[
*                                                                                  j
*                                                                                  ]->c
*                                                                                  h
*                                                                                  ildr
*                                                                                  e
*                                                                                  n); 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist[
*                                                                                  j
*                                                                                  ]); 
*                                                                                  free
*                                                                                  (
*                                                                                  node
*                                                                                  l
*                                                                                  ist)
*                                                                                  ; 
*                                                                                  node
*                                                                                  l
*                                                                                  ist 
*                                                                                  =
*                                                                                   tem
*                                                                                   p
*                                                                                   ; 
*                                                                                   --cu
*                                                                                   r
*                                                                                   rent
*                                                                                   n
*                                                                                   ode; 
*                                                                                      } 
*                                                                                       } 
*                                                                                       } 
*                                                                                       i
*                                                                                       f (s
*                                                                                       e
*                                                                                       ctio
*                                                                                       n
*                                                                                       >2) 
*                                                                                       { 
*                                                                                        cha
*                                                                                        r
*                                                                                         tem
*                                                                                         p
*                                                                                         [40]
*                                                                                         ; 
*                                                                                          j =
*                                                                                          1; 
*                                                                                           whi
*                                                                                           l
*                                                                                           e((t
*                                                                                           e
*                                                                                           mp[j
*                                                                                           -
*                                                                                           1]=l
*                                                                                           i
*                                                                                           ne[0
*                                                                                           ]
*                                                                                           [j])
*                                                                                           && l
*                                                                                           i
*                                                                                           ne[0
*                                                                                           ]
*                                                                                           [++j
*                                                                                           ]
*                                                                                           !=']
*                                                                                           '
*                                                                                           ); 
*                                                                                            tem
*                                                                                            p
*                                                                                            [j-1]
*                                                                                            = '
*                                                                                            \
*                                                                                            0'; 
*                                                                                             tno
*                                                                                             d
*                                                                                             e = 
*                                                                                             0
*                                                                                             ; 
*                                                                                              for
*                                                                                              (j=0
*                                                                                              ;
*                                                                                               j<c
*                                                                                               u
*                                                                                               rren
*                                                                                               t
*                                                                                               node
*                                                                                               && !
*                                                                                               t
*                                                                                               node
*                                                                                               ;
*                                                                                                ++j
*                                                                                                ) 
*                                                                                                { 
*                                                                                                   i
*                                                                                                   f
*                                                                                                    (!s
*                                                                                                    t
*                                                                                                    rcmp
*                                                                                                    (
*                                                                                                    node
*                                                                                                    l
*                                                                                                    ist[
*                                                                                                    j
*                                                                                                    ]->n
*                                                                                                    a
*                                                                                                    me, 
*                                                                                                    t
*                                                                                                    emp)
*                                                                                                    ) 
*                                                                                                    tnod
*                                                                                                    e
*                                                                                                     = n
*                                                                                                     o
*                                                                                                     deli
*                                                                                                     s
*                                                                                                     t[j]
*                                                                                                     ; 
*                                                                                                      } 
*                                                                                                       if 
*                                                                                                       (
*                                                                                                       !tno
*                                                                                                       d
*                                                                                                       e) 
*                                                                                                        { 
*                                                                                                           i
*                                                                                                           f
*                                                                                                            (st
*                                                                                                            r
*                                                                                                            cmp(
*                                                                                                            t
*                                                                                                            emp,
*                                                                                                            "End
*                                                                                                            O
*                                                                                                            fFil
*                                                                                                            e
*                                                                                                            ")) 
*                                                                                                               { 
*                                                                                                               strc
*                                                                                                               p
*                                                                                                               y(er
*                                                                                                               r
*                                                                                                               or, 
*                                                                                                               "
*                                                                                                               Unkn
*                                                                                                               o
*                                                                                                               wn n
*                                                                                                               o
*                                                                                                               de h
*                                                                                                               a
*                                                                                                               s be
*                                                                                                               e
*                                                                                                               n en
*                                                                                                               c
*                                                                                                               ount
*                                                                                                               e
*                                                                                                               red 
*                                                                                                               w
*                                                                                                               hile
*                                                                                                               proc
*                                                                                                               e
*                                                                                                               ssin
*                                                                                                               g
*                                                                                                                the 
*                                                                                                                f
*                                                                                                                rame
*                                                                                                                s
*                                                                                                                "); 
*                                                                                                                fclo
*                                                                                                                s
*                                                                                                                e(fi
*                                                                                                                l
*                                                                                                                e); 
*                                                                                                                retu
*                                                                                                                r
*                                                                                                                n fa
*                                                                                                                l
*                                                                                                                se; 
*                                                                                                                   } 
*                                                                                                                      e
*                                                                                                                      l
*                                                                                                                      se 
*                                                                                                                         { 
*                                                                                                                         eof=
*                                                                                                                         t
*                                                                                                                         rue; 
*                                                                                                                            } 
*                                                                                                                             } 
*                                                                                                                             } 
*                                                                                                                               } 
*                                                                                                                                 el
*                                                                                                                                 s
*                                                                                                                                 e if
*                                                                                                                                 (lin
*                                                                                                                                 e
*                                                                                                                                 [0][
*                                                                                                                                 0
*                                                                                                                                 ] &&
*                                                                                                                                 line
*                                                                                                                                 [
*                                                                                                                                 1][0
*                                                                                                                                 ]
*                                                                                                                                 ) 
*                                                                                                                                   { 
*                                                                                                                                   i
*                                                                                                                                   f (!
*                                                                                                                                   s
*                                                                                                                                   ecti
*                                                                                                                                   o
*                                                                                                                                   n) 
*                                                                                                                                   { 
*                                                                                                                                    if 
*                                                                                                                                    (
*                                                                                                                                    !Pro
*                                                                                                                                    c
*                                                                                                                                    essH
*                                                                                                                                    e
*                                                                                                                                    ader
*                                                                                                                                    (
*                                                                                                                                    line
*                                                                                                                                    ,
*                                                                                                                                     pos
*                                                                                                                                     )
*                                                                                                                                     ) 
*                                                                                                                                      { 
*                                                                                                                                         f
*                                                                                                                                         c
*                                                                                                                                         lose
*                                                                                                                                         (
*                                                                                                                                         file
*                                                                                                                                         )
*                                                                                                                                         ; 
*                                                                                                                                            r
*                                                                                                                                            e
*                                                                                                                                            turn
*                                                                                                                                            fals
*                                                                                                                                            e
*                                                                                                                                            ; 
*                                                                                                                                             } 
*                                                                                                                                             } 
*                                                                                                                                             e
*                                                                                                                                             lse 
*                                                                                                                                             i
*                                                                                                                                             f (s
*                                                                                                                                             e
*                                                                                                                                             ctio
*                                                                                                                                             n
*                                                                                                                                             ==1) 
*                                                                                                                                             { 
*                                                                                                                                              if 
*                                                                                                                                              (
*                                                                                                                                              !Pro
*                                                                                                                                              c
*                                                                                                                                              essS
*                                                                                                                                              e
*                                                                                                                                              gmen
*                                                                                                                                              t
*                                                                                                                                              s(li
*                                                                                                                                              n
*                                                                                                                                              e, p
*                                                                                                                                              o
*                                                                                                                                              s)) 
*                                                                                                                                               { 
*                                                                                                                                                  f
*                                                                                                                                                  c
*                                                                                                                                                  lose
*                                                                                                                                                  (
*                                                                                                                                                  file
*                                                                                                                                                  )
*                                                                                                                                                  ; 
*                                                                                                                                                     r
*                                                                                                                                                     e
*                                                                                                                                                     turn
*                                                                                                                                                     fals
*                                                                                                                                                     e
*                                                                                                                                                     ; 
*                                                                                                                                                      } 
*                                                                                                                                                      } 
*                                                                                                                                                      e
*                                                                                                                                                      lse 
*                                                                                                                                                      i
*                                                                                                                                                      f (s
*                                                                                                                                                      e
*                                                                                                                                                      ctio
*                                                                                                                                                      n
*                                                                                                                                                      ==2) 
*                                                                                                                                                      { 
*                                                                                                                                                       // 
*                                                                                                                                                       S
*                                                                                                                                                       etup
*                                                                                                                                                       the 
*                                                                                                                                                       b
*                                                                                                                                                       ase 
*                                                                                                                                                       p
*                                                                                                                                                       osit
*                                                                                                                                                       i
*                                                                                                                                                       ons 
*                                                                                                                                                       f
*                                                                                                                                                       or t
*                                                                                                                                                       h
*                                                                                                                                                       e se
*                                                                                                                                                       g
*                                                                                                                                                       ment 
*                                                                                                                                                        lin
*                                                                                                                                                        e
*                                                                                                                                                        [7][
*                                                                                                                                                        p
*                                                                                                                                                        os[7
*                                                                                                                                                        ]
*                                                                                                                                                        ] = 
*                                                                                                                                                        l
*                                                                                                                                                        ine[
*                                                                                                                                                        6
*                                                                                                                                                        ][po
*                                                                                                                                                        s
*                                                                                                                                                        [6]]
*                                                                                                                                                        = li
*                                                                                                                                                        n
*                                                                                                                                                        e[5]
*                                                                                                                                                        [
*                                                                                                                                                        pos[
*                                                                                                                                                        5
*                                                                                                                                                        ]] =
*                                                                                                                                                        line
*                                                                                                                                                        [
*                                                                                                                                                        4][p
*                                                                                                                                                        o
*                                                                                                                                                        s[4]
*                                                                                                                                                        ]
*                                                                                                                                                         = 
*                                                                                                                                                         l
*                                                                                                                                                         i
*                                                                                                                                                         ne[3
*                                                                                                                                                         ]
*                                                                                                                                                         [pos
*                                                                                                                                                         [
*                                                                                                                                                         3]] 
*                                                                                                                                                         =
*                                                                                                                                                          lin
*                                                                                                                                                          e
*                                                                                                                                                          [2][
*                                                                                                                                                          p
*                                                                                                                                                          os[2
*                                                                                                                                                          ]
*                                                                                                                                                          ] = 
*                                                                                                                                                          l
*                                                                                                                                                          ine[
*                                                                                                                                                          1
*                                                                                                                                                          ][po
*                                                                                                                                                          s
*                                                                                                                                                          [1]]
*                                                                                                                                                          = '\
*                                                                                                                                                          0
*                                                                                                                                                          '; 
*                                                                                                                                                           tno
*                                                                                                                                                           d
*                                                                                                                                                           e = 
*                                                                                                                                                           0
*                                                                                                                                                           ; 
*                                                                                                                                                            for
*                                                                                                                                                            (j=0;
*                                                                                                                                                            j<cu
*                                                                                                                                                            r
*                                                                                                                                                            rent
*                                                                                                                                                            n
*                                                                                                                                                            ode 
*                                                                                                                                                            &
*                                                                                                                                                            & !t
*                                                                                                                                                            n
*                                                                                                                                                            ode;
*                                                                                                                                                            ++j
*                                                                                                                                                            ) 
*                                                                                                                                                               i
*                                                                                                                                                               f
*                                                                                                                                                                (!s
*                                                                                                                                                                t
*                                                                                                                                                                rcmp
*                                                                                                                                                                (
*                                                                                                                                                                node
*                                                                                                                                                                l
*                                                                                                                                                                ist[
*                                                                                                                                                                j
*                                                                                                                                                                ]->n
*                                                                                                                                                                a
*                                                                                                                                                                me, 
*                                                                                                                                                                l
*                                                                                                                                                                ine[
*                                                                                                                                                                0
*                                                                                                                                                                ])) 
*                                                                                                                                                                tnod
*                                                                                                                                                                e
*                                                                                                                                                                 = n
*                                                                                                                                                                 o
*                                                                                                                                                                 deli
*                                                                                                                                                                 s
*                                                                                                                                                                 t[j]
*                                                                                                                                                                 ; 
*                                                                                                                                                                  Set
*                                                                                                                                                                  u
*                                                                                                                                                                  pCol
*                                                                                                                                                                  o
*                                                                                                                                                                  ur(t
*                                                                                                                                                                  n
*                                                                                                                                                                  ode)
*                                                                                                                                                                  ; 
*                                                                                                                                                                   Set
*                                                                                                                                                                   u
*                                                                                                                                                                   pOff
*                                                                                                                                                                   s
*                                                                                                                                                                   et(t
*                                                                                                                                                                   n
*                                                                                                                                                                   ode,
*                                                                                                                                                                   (flo
*                                                                                                                                                                   a
*                                                                                                                                                                   t) a
*                                                                                                                                                                   t
*                                                                                                                                                                   of(l
*                                                                                                                                                                   i
*                                                                                                                                                                   ne[1
*                                                                                                                                                                   ]
*                                                                                                                                                                   ) * 
*                                                                                                                                                                   h
*                                                                                                                                                                   eade
*                                                                                                                                                                   r
*                                                                                                                                                                   ->ca
*                                                                                                                                                                   l
*                                                                                                                                                                   lib,
*                                                                                                                                                                   (flo
*                                                                                                                                                                   a
*                                                                                                                                                                   t) a
*                                                                                                                                                                   t
*                                                                                                                                                                   of(l
*                                                                                                                                                                   i
*                                                                                                                                                                   ne[2
*                                                                                                                                                                   ]
*                                                                                                                                                                   )* 
*                                                                                                                                                                   h
*                                                                                                                                                                   e
*                                                                                                                                                                   ader
*                                                                                                                                                                   -
*                                                                                                                                                                   >cal
*                                                                                                                                                                   l
*                                                                                                                                                                   ib, 
*                                                                                                                                                                   (
*                                                                                                                                                                   floa
*                                                                                                                                                                   t
*                                                                                                                                                                   ) at
*                                                                                                                                                                   o
*                                                                                                                                                                   f(li
*                                                                                                                                                                   n
*                                                                                                                                                                   e[3]
*                                                                                                                                                                   )
*                                                                                                                                                                    * h
*                                                                                                                                                                    e
*                                                                                                                                                                    ader
*                                                                                                                                                                    -
*                                                                                                                                                                    >cal
*                                                                                                                                                                    l
*                                                                                                                                                                    ib); 
*                                                                                                                                                                     tno
*                                                                                                                                                                     d
*                                                                                                                                                                     e->e
*                                                                                                                                                                     u
*                                                                                                                                                                     ler[
*                                                                                                                                                                     x
*                                                                                                                                                                     pos]
*                                                                                                                                                                     =
*                                                                                                                                                                     (flo
*                                                                                                                                                                     a
*                                                                                                                                                                     t) a
*                                                                                                                                                                     t
*                                                                                                                                                                     of(l
*                                                                                                                                                                     i
*                                                                                                                                                                     ne[4
*                                                                                                                                                                     ]
*                                                                                                                                                                     ); 
*                                                                                                                                                                      tno
*                                                                                                                                                                      d
*                                                                                                                                                                      e->e
*                                                                                                                                                                      u
*                                                                                                                                                                      ler[
*                                                                                                                                                                      y
*                                                                                                                                                                      pos]
*                                                                                                                                                                      =
*                                                                                                                                                                      (flo
*                                                                                                                                                                      a
*                                                                                                                                                                      t) a
*                                                                                                                                                                      t
*                                                                                                                                                                      of(l
*                                                                                                                                                                      i
*                                                                                                                                                                      ne[5
*                                                                                                                                                                      ]
*                                                                                                                                                                      ); 
*                                                                                                                                                                       tno
*                                                                                                                                                                       d
*                                                                                                                                                                       e->e
*                                                                                                                                                                       u
*                                                                                                                                                                       ler[
*                                                                                                                                                                       z
*                                                                                                                                                                       pos]
*                                                                                                                                                                       =
*                                                                                                                                                                       (flo
*                                                                                                                                                                       a
*                                                                                                                                                                       t) a
*                                                                                                                                                                       t
*                                                                                                                                                                       of(l
*                                                                                                                                                                       i
*                                                                                                                                                                       ne[6
*                                                                                                                                                                       ]
*                                                                                                                                                                       ); 
*                                                                                                                                                                        if 
*                                                                                                                                                                        (
*                                                                                                                                                                        !hea
*                                                                                                                                                                        d
*                                                                                                                                                                        er->d
*                                                                                                                                                                        e
*                                                                                                                                                                        gre
*                                                                                                                                                                        e
*                                                                                                                                                                        s) 
*                                                                                                                                                                         { 
*                                                                                                                                                                         M
*                                                                                                                                                                         o
*                                                                                                                                                                         ti
*                                                                                                                                                                         o
*                                                                                                                                                                         n
*                                                                                                                                                                         C
*                                                                                                                                                                         a
*                                                                                                                                                                         p
*                                                                                                                                                                         t
*                                                                                                                                                                         u
*                                                                                                                                                                         r
*                                                                                                                                                                         e
*                                                                                                                                                                         F
*                                                                                                                                                                         i
*                                                                                                                                                                         l
*                                                                                                                                                                         e
*                                                                                                                                                                          F
*                                                                                                                                                                          o
*                                                                                                                                                                          r
*                                                                                                                                                                          m
*                                                                                                                                                                          a
*                                                                                                                                                                          t
*                                                                                                                                                                          s
*                                                                                                                                                                          E
*                                                                                                                                                                          xp
*                                                                                                                                                                          l
*                                                                                                                                                                          a
*                                                                                                                                                                          i
*                                                                                                                                                                          n
*                                                                                                                                                                          e
*                                                                                                                                                                          d
*                                                                                                                                                                          33
*                                                                                                                                                                             t
*                                                                                                                                                                             n
*                                                                                                                                                                             ode->
*                                                                                                                                                                             e
*                                                                                                                                                                             uler
*                                                                                                                                                                             [
*                                                                                                                                                                             xpos
*                                                                                                                                                                             ]
*                                                                                                                                                                             *=57
*                                                                                                                                                                             .
*                                                                                                                                                                             2957
*                                                                                                                                                                             7
*                                                                                                                                                                             95f
*                                                                                                                                                                             ; 
*                                                                                                                                                                                t
*                                                                                                                                                                                n
*                                                                                                                                                                                ode->
*                                                                                                                                                                                e
*                                                                                                                                                                                uler
*                                                                                                                                                                                [
*                                                                                                                                                                                ypos
*                                                                                                                                                                                ]
*                                                                                                                                                                                *=57
*                                                                                                                                                                                .
*                                                                                                                                                                                2957
*                                                                                                                                                                                7
*                                                                                                                                                                                95f
*                                                                                                                                                                                ; 
*                                                                                                                                                                                   t
*                                                                                                                                                                                   n
*                                                                                                                                                                                   ode->
*                                                                                                                                                                                   e
*                                                                                                                                                                                   uler
*                                                                                                                                                                                   [
*                                                                                                                                                                                   zpos
*                                                                                                                                                                                   ]
*                                                                                                                                                                                   *=57
*                                                                                                                                                                                   .
*                                                                                                                                                                                   2957
*                                                                                                                                                                                   7
*                                                                                                                                                                                   95f
*                                                                                                                                                                                   ; 
*                                                                                                                                                                                    } 
*                                                                                                                                                                                     tno
*                                                                                                                                                                                     d
*                                                                                                                                                                                     e->l
*                                                                                                                                                                                     e
*                                                                                                                                                                                     ngth
*                                                                                                                                                                                     = (f
*                                                                                                                                                                                     l
*                                                                                                                                                                                     oat)
*                                                                                                                                                                                     atof
*                                                                                                                                                                                     (
*                                                                                                                                                                                     line
*                                                                                                                                                                                     [
*                                                                                                                                                                                     7]) 
*                                                                                                                                                                                     *
*                                                                                                                                                                                      hea
*                                                                                                                                                                                      d
*                                                                                                                                                                                      er->c
*                                                                                                                                                                                      a
*                                                                                                                                                                                      llib
*                                                                                                                                                                                      ; 
*                                                                                                                                                                                       tno
*                                                                                                                                                                                       d
*                                                                                                                                                                                       e->D
*                                                                                                                                                                                       O
*                                                                                                                                                                                       Fs =
*                                                                                                                                                                                       XROT
*                                                                                                                                                                                       | YR
*                                                                                                                                                                                       O
*                                                                                                                                                                                       T | 
*                                                                                                                                                                                       Z
*                                                                                                                                                                                       ROT 
*                                                                                                                                                                                       |
*                                                                                                                                                                                        XTR
*                                                                                                                                                                                        A
*                                                                                                                                                                                         | Y
*                                                                                                                                                                                         T
*                                                                                                                                                                                         RA |
*                                                                                                                                                                                         ZTRA
*                                                                                                                                                                                         ; 
*                                                                                                                                                                                         } 
*                                                                                                                                                                                         e
*                                                                                                                                                                                         lse 
*                                                                                                                                                                                         i
*                                                                                                                                                                                         f (s
*                                                                                                                                                                                         e
*                                                                                                                                                                                         ctio
*                                                                                                                                                                                         n
*                                                                                                                                                                                         >2) 
*                                                                                                                                                                                         { 
*                                                                                                                                                                                          // 
*                                                                                                                                                                                          P
*                                                                                                                                                                                          roce
*                                                                                                                                                                                          s
*                                                                                                                                                                                          s th
*                                                                                                                                                                                          e
*                                                                                                                                                                                           fra
*                                                                                                                                                                                           m
*                                                                                                                                                                                           e in
*                                                                                                                                                                                           f
*                                                                                                                                                                                           orma
*                                                                                                                                                                                           t
*                                                                                                                                                                                           ion 
*                                                                                                                                                                                           f
*                                                                                                                                                                                           or t
*                                                                                                                                                                                           n
*                                                                                                                                                                                           ode 
*                                                                                                                                                                                            lin
*                                                                                                                                                                                            e
*                                                                                                                                                                                            [7][
*                                                                                                                                                                                            p
*                                                                                                                                                                                            os[7
*                                                                                                                                                                                            ]
*                                                                                                                                                                                            ] = 
*                                                                                                                                                                                            l
*                                                                                                                                                                                            ine[
*                                                                                                                                                                                            6
*                                                                                                                                                                                            ][po
*                                                                                                                                                                                            s
*                                                                                                                                                                                            [6]]
*                                                                                                                                                                                            = li
*                                                                                                                                                                                            n
*                                                                                                                                                                                            e[5]
*                                                                                                                                                                                            [
*                                                                                                                                                                                            pos[
*                                                                                                                                                                                            5
*                                                                                                                                                                                            ]] =
*                                                                                                                                                                                            line
*                                                                                                                                                                                            [
*                                                                                                                                                                                            4][p
*                                                                                                                                                                                            o
*                                                                                                                                                                                            s[4]
*                                                                                                                                                                                            ]
*                                                                                                                                                                                             = 
*                                                                                                                                                                                             l
*                                                                                                                                                                                             i
*                                                                                                                                                                                             ne[3
*                                                                                                                                                                                             ]
*                                                                                                                                                                                             [pos
*                                                                                                                                                                                             [
*                                                                                                                                                                                             3]] 
*                                                                                                                                                                                             =
*                                                                                                                                                                                              lin
*                                                                                                                                                                                              e
*                                                                                                                                                                                              [2][
*                                                                                                                                                                                              p
*                                                                                                                                                                                              os[2
*                                                                                                                                                                                              ]
*                                                                                                                                                                                              ] = 
*                                                                                                                                                                                              l
*                                                                                                                                                                                              ine[
*                                                                                                                                                                                              1
*                                                                                                                                                                                              ][po
*                                                                                                                                                                                              s
*                                                                                                                                                                                              [1]]
*                                                                                                                                                                                              = '\
*                                                                                                                                                                                              0
*                                                                                                                                                                                              '; 
*                                                                                                                                                                                               lon
*                                                                                                                                                                                               g
*                                                                                                                                                                                                fra
*                                                                                                                                                                                                m
*                                                                                                                                                                                                e = 
*                                                                                                                                                                                                a
*                                                                                                                                                                                                tol(
*                                                                                                                                                                                                l
*                                                                                                                                                                                                ine[
*                                                                                                                                                                                                0
*                                                                                                                                                                                                ]) - 
*                                                                                                                                                                                                1
*                                                                                                                                                                                                ; 
*                                                                                                                                                                                                 if 
*                                                                                                                                                                                                 (
*                                                                                                                                                                                                 !fra
*                                                                                                                                                                                                 m
*                                                                                                                                                                                                 e) 
*                                                                                                                                                                                                  { 
*                                                                                                                                                                                                     a
*                                                                                                                                                                                                     n
*                                                                                                                                                                                                     g[0]
*                                                                                                                                                                                                     =
*                                                                                                                                                                                                     tnod
*                                                                                                                                                                                                     e
*                                                                                                                                                                                                     ->eu
*                                                                                                                                                                                                     l
*                                                                                                                                                                                                     er[x
*                                                                                                                                                                                                     p
*                                                                                                                                                                                                     os]*
*                                                                                                                                                                                                     0
*                                                                                                                                                                                                     .017
*                                                                                                                                                                                                     4
*                                                                                                                                                                                                     53f; 
*                                                                                                                                                                                                        a
*                                                                                                                                                                                                        n
*                                                                                                                                                                                                        g[1]
*                                                                                                                                                                                                        =
*                                                                                                                                                                                                        tnod
*                                                                                                                                                                                                        e
*                                                                                                                                                                                                        ->eu
*                                                                                                                                                                                                        l
*                                                                                                                                                                                                        er[y
*                                                                                                                                                                                                        p
*                                                                                                                                                                                                        os]*
*                                                                                                                                                                                                        0
*                                                                                                                                                                                                        .017
*                                                                                                                                                                                                        4
*                                                                                                                                                                                                        53f; 
*                                                                                                                                                                                                           a
*                                                                                                                                                                                                           n
*                                                                                                                                                                                                           g[2]
*                                                                                                                                                                                                           =
*                                                                                                                                                                                                           tnod
*                                                                                                                                                                                                           e
*                                                                                                                                                                                                           ->eu
*                                                                                                                                                                                                           l
*                                                                                                                                                                                                           er[z
*                                                                                                                                                                                                           p
*                                                                                                                                                                                                           os]*
*                                                                                                                                                                                                           0
*                                                                                                                                                                                                           .017
*                                                                                                                                                                                                           4
*                                                                                                                                                                                                           53f; 
*                                                                                                                                                                                                              C
*                                                                                                                                                                                                              a
*                                                                                                                                                                                                              lcRo
*                                                                                                                                                                                                              t
*                                                                                                                                                                                                              atio
*                                                                                                                                                                                                              n
*                                                                                                                                                                                                              Matr
*                                                                                                                                                                                                              i
*                                                                                                                                                                                                              x(an
*                                                                                                                                                                                                              g
*                                                                                                                                                                                                              , ba
*                                                                                                                                                                                                              s
*                                                                                                                                                                                                              e, t
*                                                                                                                                                                                                              r
*                                                                                                                                                                                                              ot, 
*                                                                                                                                                                                                              a
*                                                                                                                                                                                                              rot,
*                                                                                                                                                                                                              xpos
*                                                                                                                                                                                                              ,
*                                                                                                                                                                                                               ypo
*                                                                                                                                                                                                               s
*                                                                                                                                                                                                               ); 
*                                                                                                                                                                                                                } 
*                                                                                                                                                                                                                 if 
*                                                                                                                                                                                                                 (
*                                                                                                                                                                                                                 head
*                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                 r->d
*                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                 gree
*                                                                                                                                                                                                                 s
*                                                                                                                                                                                                                 ) 
*                                                                                                                                                                                                                  { 
*                                                                                                                                                                                                                     a
*                                                                                                                                                                                                                     n
*                                                                                                                                                                                                                     g[0]
*                                                                                                                                                                                                                     =
*                                                                                                                                                                                                                     (flo
*                                                                                                                                                                                                                     a
*                                                                                                                                                                                                                     t) a
*                                                                                                                                                                                                                     t
*                                                                                                                                                                                                                     of(l
*                                                                                                                                                                                                                     i
*                                                                                                                                                                                                                     ne[4
*                                                                                                                                                                                                                     ]
*                                                                                                                                                                                                                     )*0.
*                                                                                                                                                                                                                     0
*                                                                                                                                                                                                                     1745
*                                                                                                                                                                                                                     3
*                                                                                                                                                                                                                     f; 
*                                                                                                                                                                                                                        a
*                                                                                                                                                                                                                        n
*                                                                                                                                                                                                                        g[1]
*                                                                                                                                                                                                                        =
*                                                                                                                                                                                                                        (flo
*                                                                                                                                                                                                                        a
*                                                                                                                                                                                                                        t) a
*                                                                                                                                                                                                                        t
*                                                                                                                                                                                                                        of(l
*                                                                                                                                                                                                                        i
*                                                                                                                                                                                                                        ne[5
*                                                                                                                                                                                                                        ]
*                                                                                                                                                                                                                        )*0.
*                                                                                                                                                                                                                        0
*                                                                                                                                                                                                                        1745
*                                                                                                                                                                                                                        3
*                                                                                                                                                                                                                        f; 
*                                                                                                                                                                                                                           a
*                                                                                                                                                                                                                           n
*                                                                                                                                                                                                                           g[2]
*                                                                                                                                                                                                                           =
*                                                                                                                                                                                                                           (flo
*                                                                                                                                                                                                                           a
*                                                                                                                                                                                                                           t) a
*                                                                                                                                                                                                                           t
*                                                                                                                                                                                                                           of(l
*                                                                                                                                                                                                                           i
*                                                                                                                                                                                                                           ne[6
*                                                                                                                                                                                                                           ]
*                                                                                                                                                                                                                           )*0.
*                                                                                                                                                                                                                           0
*                                                                                                                                                                                                                           1745
*                                                                                                                                                                                                                           3
*                                                                                                                                                                                                                           f; 
*                                                                                                                                                                                                                            } 
*                                                                                                                                                                                                                             els
*                                                                                                                                                                                                                             e 
*                                                                                                                                                                                                                              { 
*                                                                                                                                                                                                                                 a
*                                                                                                                                                                                                                                 n
*                                                                                                                                                                                                                                 g[0]
*                                                                                                                                                                                                                                 =
*                                                                                                                                                                                                                                 (flo
*                                                                                                                                                                                                                                 a
*                                                                                                                                                                                                                                 t) a
*                                                                                                                                                                                                                                 t
*                                                                                                                                                                                                                                 of(l
*                                                                                                                                                                                                                                 i
*                                                                                                                                                                                                                                 ne[4
*                                                                                                                                                                                                                                 ]
*                                                                                                                                                                                                                                 ); 
*                                                                                                                                                                                                                                    a
*                                                                                                                                                                                                                                    n
*                                                                                                                                                                                                                                    g[1]
*                                                                                                                                                                                                                                    =
*                                                                                                                                                                                                                                    (flo
*                                                                                                                                                                                                                                    a
*                                                                                                                                                                                                                                    t) a
*                                                                                                                                                                                                                                    t
*                                                                                                                                                                                                                                    of(l
*                                                                                                                                                                                                                                    i
*                                                                                                                                                                                                                                    ne[5]
*                                                                                                                                                                                                                                    )
*                                                                                                                                                                                                                                    ; 
*                                                                                                                                                                                                                                       a
*                                                                                                                                                                                                                                       n
*                                                                                                                                                                                                                                       g[2]
*                                                                                                                                                                                                                                       =
*                                                                                                                                                                                                                                       (flo
*                                                                                                                                                                                                                                       a
*                                                                                                                                                                                                                                       t) a
*                                                                                                                                                                                                                                       t
*                                                                                                                                                                                                                                       of(l
*                                                                                                                                                                                                                                       i
*                                                                                                                                                                                                                                       ne[6
*                                                                                                                                                                                                                                       ]
*                                                                                                                                                                                                                                       ); 
*                                                                                                                                                                                                                                        } 
*                                                                                                                                                                                                                                         Cal
*                                                                                                                                                                                                                                         c
*                                                                                                                                                                                                                                         Rota
*                                                                                                                                                                                                                                         t
*                                                                                                                                                                                                                                         ionM
*                                                                                                                                                                                                                                         a
*                                                                                                                                                                                                                                         trix
*                                                                                                                                                                                                                                         (
*                                                                                                                                                                                                                                         ang,
*                                                                                                                                                                                                                                         rot,
*                                                                                                                                                                                                                                         trot
*                                                                                                                                                                                                                                         ,
*                                                                                                                                                                                                                                          aro
*                                                                                                                                                                                                                                          t
*                                                                                                                                                                                                                                          , xp
*                                                                                                                                                                                                                                          o
*                                                                                                                                                                                                                                          s, y
*                                                                                                                                                                                                                                          p
*                                                                                                                                                                                                                                          os); 
*                                                                                                                                                                                                                                           mat
*                                                                                                                                                                                                                                           m
*                                                                                                                                                                                                                                           ult(
*                                                                                                                                                                                                                                           b
*                                                                                                                                                                                                                                           ase,
*                                                                                                                                                                                                                                           rot,
*                                                                                                                                                                                                                                           trot
*                                                                                                                                                                                                                                           ,
*                                                                                                                                                                                                                                            3, 
*                                                                                                                                                                                                                                            3
*                                                                                                                                                                                                                                            ); 
*                                                                                                                                                                                                                                             // 
*                                                                                                                                                                                                                                             D
*                                                                                                                                                                                                                                             ecom
*                                                                                                                                                                                                                                             p
*                                                                                                                                                                                                                                             ose 
*                                                                                                                                                                                                                                             i
*                                                                                                                                                                                                                                             nto 
*                                                                                                                                                                                                                                             Z
*                                                                                                                                                                                                                                             YX 
*                                                                                                                                                                                                                                              tno
*                                                                                                                                                                                                                                              d
*                                                                                                                                                                                                                                              e->f
*                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                              eule
*                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                              [fra
*                                                                                                                                                                                                                                              m
*                                                                                                                                                                                                                                              e][0]
*                                                                                                                                                                                                                                              =
*                                                                                                                                                                                                                                              (flo
*                                                                                                                                                                                                                                              a
*                                                                                                                                                                                                                                              t) a
*                                                                                                                                                                                                                                              t
*                                                                                                                                                                                                                                              an(t
*                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                              ot[1
*                                                                                                                                                                                                                                              ]
*                                                                                                                                                                                                                                              [0]/
*                                                                                                                                                                                                                                              t
*                                                                                                                                                                                                                                              rot[
*                                                                                                                                                                                                                                              0
*                                                                                                                                                                                                                                              ][0]
*                                                                                                                                                                                                                                              )
*                                                                                                                                                                                                                                              ; 
*                                                                                                                                                                                                                                               if 
*                                                                                                                                                                                                                                               (
*                                                                                                                                                                                                                                               base
*                                                                                                                                                                                                                                               [
*                                                                                                                                                                                                                                               0][0
*                                                                                                                                                                                                                                               ]
*                                                                                                                                                                                                                                               <0) 
*                                                                                                                                                                                                                                                  i
*                                                                                                                                                                                                                                                  f
*                                                                                                                                                                                                                                                   (ba
*                                                                                                                                                                                                                                                   s
*                                                                                                                                                                                                                                                   e[1]
*                                                                                                                                                                                                                                                   [
*                                                                                                                                                                                                                                                   0]<0
*                                                                                                                                                                                                                                                   ) 
*                                                                                                                                                                                                                                                   tnod
*                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                   ->fr
*                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                   uler
*                                                                                                                                                                                                                                                   [
*                                                                                                                                                                                                                                                   fram
*                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                   ][0]
*                                                                                                                                                                                                                                                   -
*                                                                                                                                                                                                                                                   =3.1
*                                                                                                                                                                                                                                                   4
*                                                                                                                                                                                                                                                   1592
*                                                                                                                                                                                                                                                   f
*                                                                                                                                                                                                                                                   ; 
*                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                      l
*                                                                                                                                                                                                                                                      se 
*                                                                                                                                                                                                                                                      tnod
*                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                      ->fr
*                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                      uler
*                                                                                                                                                                                                                                                      [
*                                                                                                                                                                                                                                                      fram
*                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                      ][0]
*                                                                                                                                                                                                                                                      +
*                                                                                                                                                                                                                                                      =3.1
*                                                                                                                                                                                                                                                      4
*                                                                                                                                                                                                                                                      1592
*                                                                                                                                                                                                                                                      f
*                                                                                                                                                                                                                                                      ; 
*                                                                                                                                                                                                                                                       num
*                                                                                                                                                                                                                                                       =
*                                                                                                                                                                                                                                                       (flo
*                                                                                                                                                                                                                                                       a
*                                                                                                                                                                                                                                                       t) (
*                                                                                                                                                                                                                                                       t
*                                                                                                                                                                                                                                                       rot[
*                                                                                                                                                                                                                                                       0
*                                                                                                                                                                                                                                                       ][2]
*                                                                                                                                                                                                                                                       *
*                                                                                                                                                                                                                                                       sin(
*                                                                                                                                                                                                                                                       t
*                                                                                                                                                                                                                                                       node
*                                                                                                                                                                                                                                                       -
*                                                                                                                                                                                                                                                       >fre
*                                                                                                                                                                                                                                                       u
*                                                                                                                                                                                                                                                       ler[
*                                                                                                                                                                                                                                                       f
*                                                                                                                                                                                                                                                       rame
*                                                                                                                                                                                                                                                       ]
*                                                                                                                                                                                                                                                       [0])
*                                                                                                                                                                                                                                                       -
*                                                                                                                                                                                                                                                       trot
*                                                                                                                                                                                                                                                       [
*                                                                                                                                                                                                                                                       1][2
*                                                                                                                                                                                                                                                       ]
*                                                                                                                                                                                                                                                       *cos
*                                                                                                                                                                                                                                                       (
*                                                                                                                                                                                                                                                       tnod
*                                                                                                                                                                                                                                                       e
*                                                                                                                                                                                                                                                       -> 
*                                                                                                                                                                                                                                                       f
*                                                                                                                                                                                                                                                       r
*                                                                                                                                                                                                                                                       eule
*                                                                                                                                                                                                                                                       r
*                                                                                                                                                                                                                                                       [fra
*                                                                                                                                                                                                                                                       m
*                                                                                                                                                                                                                                                       e][0
*                                                                                                                                                                                                                                                       ]
*                                                                                                                                                                                                                                                       )); 
*                                                                                                                                                                                                                                                        den
*                                                                                                                                                                                                                                                        =
*                                                                                                                                                                                                                                                        (flo
*                                                                                                                                                                                                                                                        a
*                                                                                                                                                                                                                                                        t) (
*                                                                                                                                                                                                                                                        -
*                                                                                                                                                                                                                                                        trot
*                                                                                                                                                                                                                                                        [
*                                                                                                                                                                                                                                                        0][1
*                                                                                                                                                                                                                                                        ]
*                                                                                                                                                                                                                                                        *sin
*                                                                                                                                                                                                                                                        (
*                                                                                                                                                                                                                                                        tnod
*                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                        ->fr
*                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                        uler
*                                                                                                                                                                                                                                                        [
*                                                                                                                                                                                                                                                        fram
*                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                        ][0]
*                                                                                                                                                                                                                                                        )
*                                                                                                                                                                                                                                                        +tro
*                                                                                                                                                                                                                                                        t
*                                                                                                                                                                                                                                                        [1][
*                                                                                                                                                                                                                                                        1
*                                                                                                                                                                                                                                                        ]*co
*                                                                                                                                                                                                                                                        s
*                                                                                                                                                                                                                                                        (tno
*                                                                                                                                                                                                                                                        d
*                                                                                                                                                                                                                                                        e-> 
*                                                                                                                                                                                                                                                        f
*                                                                                                                                                                                                                                                        r
*                                                                                                                                                                                                                                                        eule
*                                                                                                                                                                                                                                                        r
*                                                                                                                                                                                                                                                        [fra
*                                                                                                                                                                                                                                                        m
*                                                                                                                                                                                                                                                        e][0
*                                                                                                                                                                                                                                                        ]
*                                                                                                                                                                                                                                                        )); 
*                                                                                                                                                                                                                                                         tno
*                                                                                                                                                                                                                                                         d
*                                                                                                                                                                                                                                                         e->f
*                                                                                                                                                                                                                                                         r
*                                                                                                                                                                                                                                                         eule
*                                                                                                                                                                                                                                                         r
*                                                                                                                                                                                                                                                         [fra
*                                                                                                                                                                                                                                                         m
*                                                                                                                                                                                                                                                         e][2
*                                                                                                                                                                                                                                                         ]
*                                                                                                                                                                                                                                                         =(fl
*                                                                                                                                                                                                                                                         o
*                                                                                                                                                                                                                                                         at) 
*                                                                                                                                                                                                                                                         a
*                                                                                                                                                                                                                                                         tan(
*                                                                                                                                                                                                                                                         n
*                                                                                                                                                                                                                                                         um/d
*                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                         n)*5
*                                                                                                                                                                                                                                                         7
*                                                                                                                                                                                                                                                         .295
*                                                                                                                                                                                                                                                         7
*                                                                                                                                                                                                                                                         795f
*                                                                                                                                                                                                                                                         ; 
*                                                                                                                                                                                                                                                          if 
*                                                                                                                                                                                                                                                          (
*                                                                                                                                                                                                                                                          den<
*                                                                                                                                                                                                                                                          0
*                                                                                                                                                                                                                                                          ) 
*                                                                                                                                                                                                                                                             i
*                                                                                                                                                                                                                                                             f
*                                                                                                                                                                                                                                                              (nu
*                                                                                                                                                                                                                                                              m
*                                                                                                                                                                                                                                                              <0) 
*                                                                                                                                                                                                                                                              tnod
*                                                                                                                                                                                                                                                              e
*                                                                                                                                                                                                                                                              ->fr
*                                                                                                                                                                                                                                                              e
*                                                                                                                                                                                                                                                              uler
*                                                                                                                                                                                                                                                              [
*                                                                                                                                                                                                                                                              fram
*                                                                                                                                                                                                                                                              e
*                                                                                                                                                                                                                                                              ][2]
*                                                                                                                                                                                                                                                              -
*                                                                                                                                                                                                                                                              =180
*                                                                                                                                                                                                                                                              ; 
*                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                 l
*                                                                                                                                                                                                                                                                 se 
*                                                                                                                                                                                                                                                                 tnod
*                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                 ->fr
*                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                 uler
*                                                                                                                                                                                                                                                                 [
*                                                                                                                                                                                                                                                                 fram
*                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                 ][2]
*                                                                                                                                                                                                                                                                 +
*                                                                                                                                                                                                                                                                 =180
*                                                                                                                                                                                                                                                                 ; 
*                                                                                                                                                                                                                                                                  num
*                                                                                                                                                                                                                                                                  =
*                                                                                                                                                                                                                                                                  -tro
*                                                                                                                                                                                                                                                                  t
*                                                                                                                                                                                                                                                                  [2][
*                                                                                                                                                                                                                                                                  0
*                                                                                                                                                                                                                                                                  ]; 
*                                                                                                                                                                                                                                                                   den
*                                                                                                                                                                                                                                                                   =
*                                                                                                                                                                                                                                                                   (flo
*                                                                                                                                                                                                                                                                   a
*                                                                                                                                                                                                                                                                   t) (
*                                                                                                                                                                                                                                                                   t
*                                                                                                                                                                                                                                                                   rot[
*                                                                                                                                                                                                                                                                   0
*                                                                                                                                                                                                                                                                   ][0]
*                                                                                                                                                                                                                                                                   *
*                                                                                                                                                                                                                                                                   cos(
*                                                                                                                                                                                                                                                                   t
*                                                                                                                                                                                                                                                                   node
*                                                                                                                                                                                                                                                                   -
*                                                                                                                                                                                                                                                                   >fre
*                                                                                                                                                                                                                                                                   u
*                                                                                                                                                                                                                                                                   ler[
*                                                                                                                                                                                                                                                                   f
*                                                                                                                                                                                                                                                                   rame
*                                                                                                                                                                                                                                                                   ]
*                                                                                                                                                                                                                                                                   [0])
*                                                                                                                                                                                                                                                                   +
*                                                                                                                                                                                                                                                                   trot
*                                                                                                                                                                                                                                                                   [
*                                                                                                                                                                                                                                                                   1][0
*                                                                                                                                                                                                                                                                   ]
*                                                                                                                                                                                                                                                                   *sin
*                                                                                                                                                                                                                                                                   (
*                                                                                                                                                                                                                                                                   tnod
*                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                   -> 
*                                                                                                                                                                                                                                                                   f
*                                                                                                                                                                                                                                                                   r
*                                                                                                                                                                                                                                                                   eule
*                                                                                                                                                                                                                                                                   r
*                                                                                                                                                                                                                                                                   [fra
*                                                                                                                                                                                                                                                                   m
*                                                                                                                                                                                                                                                                   e][0
*                                                                                                                                                                                                                                                                   ]
*                                                                                                                                                                                                                                                                   )); 
*                                                                                                                                                                                                                                                                    tno
*                                                                                                                                                                                                                                                                    d
*                                                                                                                                                                                                                                                                    e->f
*                                                                                                                                                                                                                                                                    r
*                                                                                                                                                                                                                                                                    eule
*                                                                                                                                                                                                                                                                    r
*                                                                                                                                                                                                                                                                    [fra
*                                                                                                                                                                                                                                                                    m
*                                                                                                                                                                                                                                                                    e][1
*                                                                                                                                                                                                                                                                    ]
*                                                                                                                                                                                                                                                                    =(fl
*                                                                                                                                                                                                                                                                    o
*                                                                                                                                                                                                                                                                    at) 
*                                                                                                                                                                                                                                                                    a
*                                                                                                                                                                                                                                                                    tan(
*                                                                                                                                                                                                                                                                    n
*                                                                                                                                                                                                                                                                    um/d
*                                                                                                                                                                                                                                                                    e
*                                                                                                                                                                                                                                                                    n)*5
*                                                                                                                                                                                                                                                                    7
*                                                                                                                                                                                                                                                                    .295
*                                                                                                                                                                                                                                                                    7
*                                                                                                                                                                                                                                                                    795f
*                                                                                                                                                                                                                                                                    ; 
*                                                                                                                                                                                                                                                                     if 
*                                                                                                                                                                                                                                                                     (
*                                                                                                                                                                                                                                                                     den<
*                                                                                                                                                                                                                                                                     0
*                                                                                                                                                                                                                                                                     ) 
*                                                                                                                                                                                                                                                                        i
*                                                                                                                                                                                                                                                                        f
*                                                                                                                                                                                                                                                                         (nu
*                                                                                                                                                                                                                                                                         m
*                                                                                                                                                                                                                                                                         <0) 
*                                                                                                                                                                                                                                                                         tnod
*                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                         ->fr
*                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                         uler
*                                                                                                                                                                                                                                                                         [
*                                                                                                                                                                                                                                                                         fram
*                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                         ][1]
*                                                                                                                                                                                                                                                                         -
*                                                                                                                                                                                                                                                                         =180
*                                                                                                                                                                                                                                                                         ; 
*                                                                                                                                                                                                                                                                            e
*                                                                                                                                                                                                                                                                            l
*                                                                                                                                                                                                                                                                            se 
*                                                                                                                                                                                                                                                                            tnod
*                                                                                                                                                                                                                                                                            e
*                                                                                                                                                                                                                                                                            ->fr
*                                                                                                                                                                                                                                                                            e
*                                                                                                                                                                                                                                                                            uler
*                                                                                                                                                                                                                                                                            [
*                                                                                                                                                                                                                                                                            fram
*                                                                                                                                                                                                                                                                            e
*                                                                                                                                                                                                                                                                            ][1]
*                                                                                                                                                                                                                                                                            +
*                                                                                                                                                                                                                                                                            =180
*                                                                                                                                                                                                                                                                            ; 
*                                                                                                                                                                                                                                                                             tno
*                                                                                                                                                                                                                                                                             d
*                                                                                                                                                                                                                                                                             e->f
*                                                                                                                                                                                                                                                                             r
*                                                                                                                                                                                                                                                                             eule
*                                                                                                                                                                                                                                                                             r
*                                                                                                                                                                                                                                                                             [fra
*                                                                                                                                                                                                                                                                             m
*                                                                                                                                                                                                                                                                             e][z
*                                                                                                                                                                                                                                                                             p
*                                                                                                                                                                                                                                                                             os]*
*                                                                                                                                                                                                                                                                             =
*                                                                                                                                                                                                                                                                             57.2
*                                                                                                                                                                                                                                                                             9
*                                                                                                                                                                                                                                                                             5779
*                                                                                                                                                                                                                                                                             5
*                                                                                                                                                                                                                                                                             f; 
*                                                                                                                                                                                                                                                                              tno
*                                                                                                                                                                                                                                                                              d
*                                                                                                                                                                                                                                                                              e->f
*                                                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                                                              oset
*                                                                                                                                                                                                                                                                              [
*                                                                                                                                                                                                                                                                              fram
*                                                                                                                                                                                                                                                                              e
*                                                                                                                                                                                                                                                                              ][0]
*                                                                                                                                                                                                                                                                              =
*                                                                                                                                                                                                                                                                              (flo
*                                                                                                                                                                                                                                                                              a
*                                                                                                                                                                                                                                                                              t) a
*                                                                                                                                                                                                                                                                              t
*                                                                                                                                                                                                                                                                              of(l
*                                                                                                                                                                                                                                                                              i
*                                                                                                                                                                                                                                                                              ne[1
*                                                                                                                                                                                                                                                                              ]
*                                                                                                                                                                                                                                                                              )*he
*                                                                                                                                                                                                                                                                              a
*                                                                                                                                                                                                                                                                              der->
*                                                                                                                                                                                                                                                                              c
*                                                                                                                                                                                                                                                                              alli
*                                                                                                                                                                                                                                                                              b
*                                                                                                                                                                                                                                                                              ; 
*                                                                                                                                                                                                                                                                               tno
*                                                                                                                                                                                                                                                                               d
*                                                                                                                                                                                                                                                                               e->f
*                                                                                                                                                                                                                                                                               r
*                                                                                                                                                                                                                                                                               oset
*                                                                                                                                                                                                                                                                               [
*                                                                                                                                                                                                                                                                               fram
*                                                                                                                                                                                                                                                                               e
*                                                                                                                                                                                                                                                                               ][1]=
*                                                                                                                                                                                                                                                                               (
*                                                                                                                                                                                                                                                                               floa
*                                                                                                                                                                                                                                                                               t
*                                                                                                                                                                                                                                                                               ) at
*                                                                                                                                                                                                                                                                               o
*                                                                                                                                                                                                                                                                               f(li
*                                                                                                                                                                                                                                                                               n
*                                                                                                                                                                                                                                                                               e[2]
*                                                                                                                                                                                                                                                                               )
*                                                                                                                                                                                                                                                                               *hea
*                                                                                                                                                                                                                                                                               d
*                                                                                                                                                                                                                                                                               er->
*                                                                                                                                                                                                                                                                               c
*                                                                                                                                                                                                                                                                               alli
*                                                                                                                                                                                                                                                                               b
*                                                                                                                                                                                                                                                                               ; 
*                                                                                                                                                                                                                                                                                tno
*                                                                                                                                                                                                                                                                                d
*                                                                                                                                                                                                                                                                                e->f
*                                                                                                                                                                                                                                                                                r
*                                                                                                                                                                                                                                                                                oset
*                                                                                                                                                                                                                                                                                [
*                                                                                                                                                                                                                                                                                fram
*                                                                                                                                                                                                                                                                                e
*                                                                                                                                                                                                                                                                                ][2]
*                                                                                                                                                                                                                                                                                =
*                                                                                                                                                                                                                                                                                (flo
*                                                                                                                                                                                                                                                                                a
*                                                                                                                                                                                                                                                                                t) a
*                                                                                                                                                                                                                                                                                t
*                                                                                                                                                                                                                                                                                of(l
*                                                                                                                                                                                                                                                                                i
*                                                                                                                                                                                                                                                                                ne[3
*                                                                                                                                                                                                                                                                                ]
*                                                                                                                                                                                                                                                                                )*he
*                                                                                                                                                                                                                                                                                a
*                                                                                                                                                                                                                                                                                der->
*                                                                                                                                                                                                                                                                                c
*                                                                                                                                                                                                                                                                                alli
*                                                                                                                                                                                                                                                                                b
*                                                                                                                                                                                                                                                                                ; 
*                                                                                                                                                                                                                                                                                 tno
*                                                                                                                                                                                                                                                                                 d
*                                                                                                                                                                                                                                                                                 e->s
*                                                                                                                                                                                                                                                                                 c
*                                                                                                                                                                                                                                                                                 ale[
*                                                                                                                                                                                                                                                                                 f
*                                                                                                                                                                                                                                                                                 rame
*                                                                                                                                                                                                                                                                                 ]
*                                                                                                                                                                                                                                                                                 =(fl
*                                                                                                                                                                                                                                                                                 o
*                                                                                                                                                                                                                                                                                 at) 
*                                                                                                                                                                                                                                                                                 a
*                                                                                                                                                                                                                                                                                 tof(
*                                                                                                                                                                                                                                                                                 l
*                                                                                                                                                                                                                                                                                 ine[
*                                                                                                                                                                                                                                                                                 7
*                                                                                                                                                                                                                                                                                 ]); 
*                                                                                                                                                                                                                                                                                 } 
*                                                                                                                                                                                                                                                                                   } 
*                                                                                                                                                                                                                                                                                     //
*                                                                                                                                                                                                                                                                                     Move
*                                                                                                                                                                                                                                                                                     onto
*                                                                                                                                                                                                                                                                                     the 
*                                                                                                                                                                                                                                                                                     n
*                                                                                                                                                                                                                                                                                     ext 
*                                                                                                                                                                                                                                                                                     l
*                                                                                                                                                                                                                                                                                     ine 
*                                                                                                                                                                                                                                                                                     a
*                                                                                                                                                                                                                                                                                     nd c
*                                                                                                                                                                                                                                                                                     l
*                                                                                                                                                                                                                                                                                     ear c
*                                                                                                                                                                                                                                                                                     u
*                                                                                                                                                                                                                                                                                     rren
*                                                                                                                                                                                                                                                                                     t
*                                                                                                                                                                                                                                                                                      lin
*                                                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                                                       inf
*                                                                                                                                                                                                                                                                                       o
*                                                                                                                                                                                                                                                                                       rmat
*                                                                                                                                                                                                                                                                                       i
*                                                                                                                                                                                                                                                                                       on 
*                                                                                                                                                                                                                                                                                       M
*                                                                                                                                                                                                                                                                                       o
*                                                                                                                                                                                                                                                                                       ti
*                                                                                                                                                                                                                                                                                       o
*                                                                                                                                                                                                                                                                                       n
*                                                                                                                                                                                                                                                                                       C
*                                                                                                                                                                                                                                                                                       a
*                                                                                                                                                                                                                                                                                       p
*                                                                                                                                                                                                                                                                                       t
*                                                                                                                                                                                                                                                                                       u
*                                                                                                                                                                                                                                                                                       r
*                                                                                                                                                                                                                                                                                       e
*                                                                                                                                                                                                                                                                                       F
*                                                                                                                                                                                                                                                                                       i
*                                                                                                                                                                                                                                                                                       l
*                                                                                                                                                                                                                                                                                       e
*                                                                                                                                                                                                                                                                                        F
*                                                                                                                                                                                                                                                                                        o
*                                                                                                                                                                                                                                                                                        r
*                                                                                                                                                                                                                                                                                        m
*                                                                                                                                                                                                                                                                                        a
*                                                                                                                                                                                                                                                                                        t
*                                                                                                                                                                                                                                                                                        s
*                                                                                                                                                                                                                                                                                        E
*                                                                                                                                                                                                                                                                                        xp
*                                                                                                                                                                                                                                                                                        l
*                                                                                                                                                                                                                                                                                        a
*                                                                                                                                                                                                                                                                                        i
*                                                                                                                                                                                                                                                                                        n
*                                                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                                                        d
*                                                                                                                                                                                                                                                                                        34
*                                                                                                                                                                                                                                                                                          j=
*                                                                                                                                                                                                                                                                                          s
*                                                                                                                                                                                                                                                                                          trst
*                                                                                                                                                                                                                                                                                          r
*                                                                                                                                                                                                                                                                                          Ex(b
*                                                                                                                                                                                                                                                                                          u
*                                                                                                                                                                                                                                                                                          ffer
*                                                                                                                                                                                                                                                                                          + i,
*                                                                                                                                                                                                                                                                                          char
*                                                                                                                                                                                                                                                                                          (
*                                                                                                                                                                                                                                                                                          10))
*                                                                                                                                                                                                                                                                                          ; 
*                                                                                                                                                                                                                                                                                            if
*                                                                                                                                                                                                                                                                                            (j==
*                                                                                                                                                                                                                                                                                            -
*                                                                                                                                                                                                                                                                                            1) 
*                                                                                                                                                                                                                                                                                              { 
*                                                                                                                                                                                                                                                                                              i
*                                                                                                                                                                                                                                                                                              f (b
*                                                                                                                                                                                                                                                                                              u
*                                                                                                                                                                                                                                                                                              ffer
*                                                                                                                                                                                                                                                                                              [
*                                                                                                                                                                                                                                                                                              4095
*                                                                                                                                                                                                                                                                                              ]
*                                                                                                                                                                                                                                                                                              !=10
*                                                                                                                                                                                                                                                                                              ) 
*                                                                                                                                                                                                                                                                                              { 
*                                                                                                                                                                                                                                                                                               rea
*                                                                                                                                                                                                                                                                                               d
*                                                                                                                                                                                                                                                                                                = f
*                                                                                                                                                                                                                                                                                                r
*                                                                                                                                                                                                                                                                                                ead(
*                                                                                                                                                                                                                                                                                                b
*                                                                                                                                                                                                                                                                                                uffe
*                                                                                                                                                                                                                                                                                                r
*                                                                                                                                                                                                                                                                                                , 1,
*                                                                                                                                                                                                                                                                                                4096
*                                                                                                                                                                                                                                                                                                ,
*                                                                                                                                                                                                                                                                                                 fil
*                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                 ); 
*                                                                                                                                                                                                                                                                                                  i =
*                                                                                                                                                                                                                                                                                                  strs
*                                                                                                                                                                                                                                                                                                  t
*                                                                                                                                                                                                                                                                                                  rEx(
*                                                                                                                                                                                                                                                                                                  b
*                                                                                                                                                                                                                                                                                                  uffe
*                                                                                                                                                                                                                                                                                                  r
*                                                                                                                                                                                                                                                                                                  , ch
*                                                                                                                                                                                                                                                                                                  a
*                                                                                                                                                                                                                                                                                                  r(10
*                                                                                                                                                                                                                                                                                                  )
*                                                                                                                                                                                                                                                                                                  ); 
*                                                                                                                                                                                                                                                                                                  } 
*                                                                                                                                                                                                                                                                                                  e
*                                                                                                                                                                                                                                                                                                  lse 
*                                                                                                                                                                                                                                                                                                  { 
*                                                                                                                                                                                                                                                                                                   rea
*                                                                                                                                                                                                                                                                                                   d
*                                                                                                                                                                                                                                                                                                    = f
*                                                                                                                                                                                                                                                                                                    r
*                                                                                                                                                                                                                                                                                                    ead(
*                                                                                                                                                                                                                                                                                                    b
*                                                                                                                                                                                                                                                                                                    uffe
*                                                                                                                                                                                                                                                                                                    r
*                                                                                                                                                                                                                                                                                                    , 1,
*                                                                                                                                                                                                                                                                                                    4096
*                                                                                                                                                                                                                                                                                                    ,
*                                                                                                                                                                                                                                                                                                     fil
*                                                                                                                                                                                                                                                                                                     e
*                                                                                                                                                                                                                                                                                                     ); 
*                                                                                                                                                                                                                                                                                                      i=0
*                                                                                                                                                                                                                                                                                                      ; 
*                                                                                                                                                                                                                                                                                                      } 
*                                                                                                                                                                                                                                                                                                      b
*                                                                                                                                                                                                                                                                                                      uffe
*                                                                                                                                                                                                                                                                                                      r
*                                                                                                                                                                                                                                                                                                      [409
*                                                                                                                                                                                                                                                                                                      6
*                                                                                                                                                                                                                                                                                                      ] = 
*                                                                                                                                                                                                                                                                                                      '
*                                                                                                                                                                                                                                                                                                      \0'; 
*                                                                                                                                                                                                                                                                                                        } 
*                                                                                                                                                                                                                                                                                                          el
*                                                                                                                                                                                                                                                                                                          s
*                                                                                                                                                                                                                                                                                                          e 
*                                                                                                                                                                                                                                                                                                          i
*                                                                                                                                                                                                                                                                                                          +=j; 
*                                                                                                                                                                                                                                                                                                            wh
*                                                                                                                                                                                                                                                                                                            e
*                                                                                                                                                                                                                                                                                                            re =
*                                                                                                                                                                                                                                                                                                            pos[
*                                                                                                                                                                                                                                                                                                            0
*                                                                                                                                                                                                                                                                                                            ] = 
*                                                                                                                                                                                                                                                                                                            p
*                                                                                                                                                                                                                                                                                                            os[1
*                                                                                                                                                                                                                                                                                                            ]
*                                                                                                                                                                                                                                                                                                             = p
*                                                                                                                                                                                                                                                                                                             o
*                                                                                                                                                                                                                                                                                                             s[2]
*                                                                                                                                                                                                                                                                                                             = po
*                                                                                                                                                                                                                                                                                                             s
*                                                                                                                                                                                                                                                                                                             [3] 
*                                                                                                                                                                                                                                                                                                             =
*                                                                                                                                                                                                                                                                                                              pos[
*                                                                                                                                                                                                                                                                                                              4
*                                                                                                                                                                                                                                                                                                              ] = 
*                                                                                                                                                                                                                                                                                                              p
*                                                                                                                                                                                                                                                                                                              os[5
*                                                                                                                                                                                                                                                                                                              ]
*                                                                                                                                                                                                                                                                                                               = p
*                                                                                                                                                                                                                                                                                                               o
*                                                                                                                                                                                                                                                                                                               s[6]
*                                                                                                                                                                                                                                                                                                               = po
*                                                                                                                                                                                                                                                                                                               s
*                                                                                                                                                                                                                                                                                                               [7] 
*                                                                                                                                                                                                                                                                                                               =
*                                                                                                                                                                                                                                                                                                                0; 
*                                                                                                                                                                                                                                                                                                                } 
*                                                                                                                                                                                                                                                                                                                if (
*                                                                                                                                                                                                                                                                                                                b
*                                                                                                                                                                                                                                                                                                                uffe
*                                                                                                                                                                                                                                                                                                                r
*                                                                                                                                                                                                                                                                                                                [i] 
*                                                                                                                                                                                                                                                                                                                >
*                                                                                                                                                                                                                                                                                                                 44 
*                                                                                                                                                                                                                                                                                                                 &
*                                                                                                                                                                                                                                                                                                                 & bu
*                                                                                                                                                                                                                                                                                                                 f
*                                                                                                                                                                                                                                                                                                                 fer[
*                                                                                                                                                                                                                                                                                                                 i
*                                                                                                                                                                                                                                                                                                                 ] < 
*                                                                                                                                                                                                                                                                                                                 1
*                                                                                                                                                                                                                                                                                                                 23) 
*                                                                                                                                                                                                                                                                                                                   li
*                                                                                                                                                                                                                                                                                                                   n
*                                                                                                                                                                                                                                                                                                                   e[wh
*                                                                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                                                                   re][
*                                                                                                                                                                                                                                                                                                                   p
*                                                                                                                                                                                                                                                                                                                   os[w
*                                                                                                                                                                                                                                                                                                                   h
*                                                                                                                                                                                                                                                                                                                   ere]
*                                                                                                                                                                                                                                                                                                                   +
*                                                                                                                                                                                                                                                                                                                   +] =
*                                                                                                                                                                                                                                                                                                                   buff
*                                                                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                                                                   r[i+
*                                                                                                                                                                                                                                                                                                                   +
*                                                                                                                                                                                                                                                                                                                   ]; 
*                                                                                                                                                                                                                                                                                                                   else
*                                                                                                                                                                                                                                                                                                                   if (
*                                                                                                                                                                                                                                                                                                                   (
*                                                                                                                                                                                                                                                                                                                   buff
*                                                                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                                                                   r[i]
*                                                                                                                                                                                                                                                                                                                   =
*                                                                                                                                                                                                                                                                                                                   =32 
*                                                                                                                                                                                                                                                                                                                   |
*                                                                                                                                                                                                                                                                                                                   | bu
*                                                                                                                                                                                                                                                                                                                   f
*                                                                                                                                                                                                                                                                                                                   fer[
*                                                                                                                                                                                                                                                                                                                   i
*                                                                                                                                                                                                                                                                                                                   ]==9
*                                                                                                                                                                                                                                                                                                                   )
*                                                                                                                                                                                                                                                                                                                    && 
*                                                                                                                                                                                                                                                                                                                    p
*                                                                                                                                                                                                                                                                                                                    os[w
*                                                                                                                                                                                                                                                                                                                    h
*                                                                                                                                                                                                                                                                                                                    ere]
*                                                                                                                                                                                                                                                                                                                    >
*                                                                                                                                                                                                                                                                                                                    0)//
*                                                                                                                                                                                                                                                                                                                    && (
*                                                                                                                                                                                                                                                                                                                    w
*                                                                                                                                                                                                                                                                                                                    here
*                                                                                                                                                                                                                                                                                                                    =
*                                                                                                                                                                                                                                                                                                                    =0 |
*                                                                                                                                                                                                                                                                                                                    |
*                                                                                                                                                                                                                                                                                                                     sec
*                                                                                                                                                                                                                                                                                                                     t
*                                                                                                                                                                                                                                                                                                                     ion>
*                                                                                                                                                                                                                                                                                                                     1
*                                                                                                                                                                                                                                                                                                                     )) 
*                                                                                                                                                                                                                                                                                                                     { 
*                                                                                                                                                                                                                                                                                                                       ++
*                                                                                                                                                                                                                                                                                                                       w
*                                                                                                                                                                                                                                                                                                                       here
*                                                                                                                                                                                                                                                                                                                       ; 
*                                                                                                                                                                                                                                                                                                                         ++
*                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                         ; 
*                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                         else 
*                                                                                                                                                                                                                                                                                                                           ++
*                                                                                                                                                                                                                                                                                                                           i
*                                                                                                                                                                                                                                                                                                                           ; 
*                                                                                                                                                                                                                                                                                                                              } 
*                                                                                                                                                                                                                                                                                                                                 r
*                                                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                                                 ad =
*                                                                                                                                                                                                                                                                                                                                 frea
*                                                                                                                                                                                                                                                                                                                                 d
*                                                                                                                                                                                                                                                                                                                                 (buf
*                                                                                                                                                                                                                                                                                                                                 f
*                                                                                                                                                                                                                                                                                                                                 er, 
*                                                                                                                                                                                                                                                                                                                                 1
*                                                                                                                                                                                                                                                                                                                                 , 40
*                                                                                                                                                                                                                                                                                                                                 9
*                                                                                                                                                                                                                                                                                                                                 6, f
*                                                                                                                                                                                                                                                                                                                                 i
*                                                                                                                                                                                                                                                                                                                                 le); 
*                                                                                                                                                                                                                                                                                                                                    b
*                                                                                                                                                                                                                                                                                                                                    u
*                                                                                                                                                                                                                                                                                                                                    ffer
*                                                                                                                                                                                                                                                                                                                                    [
*                                                                                                                                                                                                                                                                                                                                    4096
*                                                                                                                                                                                                                                                                                                                                    ]
*                                                                                                                                                                                                                                                                                                                                     = '
*                                                                                                                                                                                                                                                                                                                                     \
*                                                                                                                                                                                                                                                                                                                                     0'; 
*                                                                                                                                                                                                                                                                                                                                        i
*                                                                                                                                                                                                                                                                                                                                        =
*                                                                                                                                                                                                                                                                                                                                        0; 
*                                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                                          hea
*                                                                                                                                                                                                                                                                                                                                          d
*                                                                                                                                                                                                                                                                                                                                          er->d
*                                                                                                                                                                                                                                                                                                                                          e
*                                                                                                                                                                                                                                                                                                                                          gree
*                                                                                                                                                                                                                                                                                                                                          s
*                                                                                                                                                                                                                                                                                                                                          =tru
*                                                                                                                                                                                                                                                                                                                                          e
*                                                                                                                                                                                                                                                                                                                                          ; 
*                                                                                                                                                                                                                                                                                                                                           hea
*                                                                                                                                                                                                                                                                                                                                           d
*                                                                                                                                                                                                                                                                                                                                           er->e
*                                                                                                                                                                                                                                                                                                                                           u
*                                                                                                                                                                                                                                                                                                                                           ler[
*                                                                                                                                                                                                                                                                                                                                           0
*                                                                                                                                                                                                                                                                                                                                           ][0]
*                                                                                                                                                                                                                                                                                                                                           =
*                                                                                                                                                                                                                                                                                                                                           head
*                                                                                                                                                                                                                                                                                                                                           e
*                                                                                                                                                                                                                                                                                                                                           r->e
*                                                                                                                                                                                                                                                                                                                                           u
*                                                                                                                                                                                                                                                                                                                                           ler[
*                                                                                                                                                                                                                                                                                                                                           0
*                                                                                                                                                                                                                                                                                                                                           ][1]
*                                                                                                                                                                                                                                                                                                                                           =
*                                                                                                                                                                                                                                                                                                                                           0; 
*                                                                                                                                                                                                                                                                                                                                            hea
*                                                                                                                                                                                                                                                                                                                                            d
*                                                                                                                                                                                                                                                                                                                                            er->e
*                                                                                                                                                                                                                                                                                                                                            u
*                                                                                                                                                                                                                                                                                                                                            ler[
*                                                                                                                                                                                                                                                                                                                                            0
*                                                                                                                                                                                                                                                                                                                                            ][2
*                                                                                                                                                                                                                                                                                                                                            ]
*                                                                                                                                                                                                                                                                                                                                            =1; 
*                                                                                                                                                                                                                                                                                                                                             hea
*                                                                                                                                                                                                                                                                                                                                             d
*                                                                                                                                                                                                                                                                                                                                             er->e
*                                                                                                                                                                                                                                                                                                                                             u
*                                                                                                                                                                                                                                                                                                                                             ler[
*                                                                                                                                                                                                                                                                                                                                             1
*                                                                                                                                                                                                                                                                                                                                             ][0]
*                                                                                                                                                                                                                                                                                                                                             =
*                                                                                                                                                                                                                                                                                                                                             head
*                                                                                                                                                                                                                                                                                                                                             e
*                                                                                                                                                                                                                                                                                                                                             r->e
*                                                                                                                                                                                                                                                                                                                                             u
*                                                                                                                                                                                                                                                                                                                                             ler[
*                                                                                                                                                                                                                                                                                                                                             1
*                                                                                                                                                                                                                                                                                                                                             ][2]
*                                                                                                                                                                                                                                                                                                                                             =
*                                                                                                                                                                                                                                                                                                                                             0; 
*                                                                                                                                                                                                                                                                                                                                              hea
*                                                                                                                                                                                                                                                                                                                                              d
*                                                                                                                                                                                                                                                                                                                                              er->e
*                                                                                                                                                                                                                                                                                                                                              u
*                                                                                                                                                                                                                                                                                                                                              ler[
*                                                                                                                                                                                                                                                                                                                                              1
*                                                                                                                                                                                                                                                                                                                                              ][1]
*                                                                                                                                                                                                                                                                                                                                              =
*                                                                                                                                                                                                                                                                                                                                              1; 
*                                                                                                                                                                                                                                                                                                                                               hea
*                                                                                                                                                                                                                                                                                                                                               d
*                                                                                                                                                                                                                                                                                                                                               er->e
*                                                                                                                                                                                                                                                                                                                                               u
*                                                                                                                                                                                                                                                                                                                                               ler[
*                                                                                                                                                                                                                                                                                                                                               2
*                                                                                                                                                                                                                                                                                                                                               ][1]
*                                                                                                                                                                                                                                                                                                                                               =
*                                                                                                                                                                                                                                                                                                                                               head
*                                                                                                                                                                                                                                                                                                                                               e
*                                                                                                                                                                                                                                                                                                                                               r->e
*                                                                                                                                                                                                                                                                                                                                               u
*                                                                                                                                                                                                                                                                                                                                               ler[
*                                                                                                                                                                                                                                                                                                                                               2
*                                                                                                                                                                                                                                                                                                                                               ][2]
*                                                                                                                                                                                                                                                                                                                                               =
*                                                                                                                                                                                                                                                                                                                                               0; 
*                                                                                                                                                                                                                                                                                                                                                hea
*                                                                                                                                                                                                                                                                                                                                                d
*                                                                                                                                                                                                                                                                                                                                                er->e
*                                                                                                                                                                                                                                                                                                                                                u
*                                                                                                                                                                                                                                                                                                                                                ler[
*                                                                                                                                                                                                                                                                                                                                                2
*                                                                                                                                                                                                                                                                                                                                                ][0]
*                                                                                                                                                                                                                                                                                                                                                =
*                                                                                                                                                                                                                                                                                                                                                1; 
*                                                                                                                                                                                                                                                                                                                                                 fcl
*                                                                                                                                                                                                                                                                                                                                                 o
*                                                                                                                                                                                                                                                                                                                                                 se(f
*                                                                                                                                                                                                                                                                                                                                                 i
*                                                                                                                                                                                                                                                                                                                                                 le); 
*                                                                                                                                                                                                                                                                                                                                                  for
*                                                                                                                                                                                                                                                                                                                                                  (i=0
*                                                                                                                                                                                                                                                                                                                                                  ;
*                                                                                                                                                                                                                                                                                                                                                   i<3
*                                                                                                                                                                                                                                                                                                                                                   ;
*                                                                                                                                                                                                                                                                                                                                                    i++
*                                                                                                                                                                                                                                                                                                                                                    ) 
*                                                                                                                                                                                                                                                                                                                                                     { 
*                                                                                                                                                                                                                                                                                                                                                        d
*                                                                                                                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                                                                                                                        lete
*                                                                                                                                                                                                                                                                                                                                                        [
*                                                                                                                                                                                                                                                                                                                                                        ] ba
*                                                                                                                                                                                                                                                                                                                                                        s
*                                                                                                                                                                                                                                                                                                                                                        e[i]
*                                                                                                                                                                                                                                                                                                                                                        ; 
*                                                                                                                                                                                                                                                                                                                                                           d
*                                                                                                                                                                                                                                                                                                                                                           e
*                                                                                                                                                                                                                                                                                                                                                           lete
*                                                                                                                                                                                                                                                                                                                                                           [
*                                                                                                                                                                                                                                                                                                                                                           ] ro
*                                                                                                                                                                                                                                                                                                                                                           t
*                                                                                                                                                                                                                                                                                                                                                           [i]; 
*                                                                                                                                                                                                                                                                                                                                                              d
*                                                                                                                                                                                                                                                                                                                                                              e
*                                                                                                                                                                                                                                                                                                                                                              lete
*                                                                                                                                                                                                                                                                                                                                                              [
*                                                                                                                                                                                                                                                                                                                                                              ] ar
*                                                                                                                                                                                                                                                                                                                                                              o
*                                                                                                                                                                                                                                                                                                                                                              t[i]
*                                                                                                                                                                                                                                                                                                                                                              ; 
*                                                                                                                                                                                                                                                                                                                                                                 d
*                                                                                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                                                                                 lete
*                                                                                                                                                                                                                                                                                                                                                                 [
*                                                                                                                                                                                                                                                                                                                                                                 ] tr
*                                                                                                                                                                                                                                                                                                                                                                 o
*                                                                                                                                                                                                                                                                                                                                                                 t[i]
*                                                                                                                                                                                                                                                                                                                                                                 ; 
*                                                                                                                                                                                                                                                                                                                                                                  } 
*                                                                                                                                                                                                                                                                                                                                                                   del
*                                                                                                                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                                                                                                                   te[]
*                                                                                                                                                                                                                                                                                                                                                                   base
*                                                                                                                                                                                                                                                                                                                                                                   ; 
*                                                                                                                                                                                                                                                                                                                                                                    del
*                                                                                                                                                                                                                                                                                                                                                                    e
*                                                                                                                                                                                                                                                                                                                                                                    te[]
*                                                                                                                                                                                                                                                                                                                                                                    rot; 
*                                                                                                                                                                                                                                                                                                                                                                     del
*                                                                                                                                                                                                                                                                                                                                                                     e
*                                                                                                                                                                                                                                                                                                                                                                     te[]
*                                                                                                                                                                                                                                                                                                                                                                     arot
*                                                                                                                                                                                                                                                                                                                                                                     ; 
*                                                                                                                                                                                                                                                                                                                                                                      del
*                                                                                                                                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                                                                                                                                      te[]
*                                                                                                                                                                                                                                                                                                                                                                      trot
*                                                                                                                                                                                                                                                                                                                                                                      ; 
*                                                                                                                                                                                                                                                                                                                                                                       ret
*                                                                                                                                                                                                                                                                                                                                                                       u
*                                                                                                                                                                                                                                                                                                                                                                       rn t
*                                                                                                                                                                                                                                                                                                                                                                       r
*                                                                                                                                                                                                                                                                                                                                                                       ue; 
*                                                                                                                                                                                                                                                                                                                                                                       } 
*                                                                                                                                                                                                                                                                                                                                                                       e
*                                                                                                                                                                                                                                                                                                                                                                       lse 
*                                                                                                                                                                                                                                                                                                                                                                       { 
*                                                                                                                                                                                                                                                                                                                                                                        str
*                                                                                                                                                                                                                                                                                                                                                                        c
*                                                                                                                                                                                                                                                                                                                                                                        py(e
*                                                                                                                                                                                                                                                                                                                                                                        r
*                                                                                                                                                                                                                                                                                                                                                                        ror,
*                                                                                                                                                                                                                                                                                                                                                                        "Can
*                                                                                                                                                                                                                                                                                                                                                                        n
*                                                                                                                                                                                                                                                                                                                                                                        ot O
*                                                                                                                                                                                                                                                                                                                                                                        p
*                                                                                                                                                                                                                                                                                                                                                                        en F
*                                                                                                                                                                                                                                                                                                                                                                        i
*                                                                                                                                                                                                                                                                                                                                                                        le")
*                                                                                                                                                                                                                                                                                                                                                                        ; 
*                                                                                                                                                                                                                                                                                                                                                                         ret
*                                                                                                                                                                                                                                                                                                                                                                         u
*                                                                                                                                                                                                                                                                                                                                                                         rn f
*                                                                                                                                                                                                                                                                                                                                                                         a
*                                                                                                                                                                                                                                                                                                                                                                         lse; 
*                                                                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                                                                         bo
*                                                                                                                                                                                                                                                                                                                                                                         o
*                                                                                                                                                                                                                                                                                                                                                                         l HT
*                                                                                                                                                                                                                                                                                                                                                                         R
*                                                                                                                                                                                                                                                                                                                                                                         Form
*                                                                                                                                                                                                                                                                                                                                                                         a
*                                                                                                                                                                                                                                                                                                                                                                         t::P
*                                                                                                                                                                                                                                                                                                                                                                         r
*                                                                                                                                                                                                                                                                                                                                                                         oces
*                                                                                                                                                                                                                                                                                                                                                                         s
*                                                                                                                                                                                                                                                                                                                                                                         Head
*                                                                                                                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                                                                                                                         r(ch
*                                                                                                                                                                                                                                                                                                                                                                         a
*                                                                                                                                                                                                                                                                                                                                                                         r lin
*                                                                                                                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                                                                                                                         [2][
*                                                                                                                                                                                                                                                                                                                                                                         4
*                                                                                                                                                                                                                                                                                                                                                                         0], 
*                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                         nt p
*                                                                                                                                                                                                                                                                                                                                                                         o
*                                                                                                                                                                                                                                                                                                                                                                         s[2]
*                                                                                                                                                                                                                                                                                                                                                                         ) 
*                                                                                                                                                                                                                                                                                                                                                                         { 
*                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                         f (s
*                                                                                                                                                                                                                                                                                                                                                                         t
*                                                                                                                                                                                                                                                                                                                                                                         rcom
*                                                                                                                                                                                                                                                                                                                                                                         p
*                                                                                                                                                                                                                                                                                                                                                                         Ex(l
*                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                         ne[0
*                                                                                                                                                                                                                                                                                                                                                                         ]
*                                                                                                                                                                                                                                                                                                                                                                         , "F
*                                                                                                                                                                                                                                                                                                                                                                         I
*                                                                                                                                                                                                                                                                                                                                                                         LETY
*                                                                                                                                                                                                                                                                                                                                                                         P
*                                                                                                                                                                                                                                                                                                                                                                         E")) 
*                                                                                                                                                                                                                                                                                                                                                                         { 
*                                                                                                                                                                                                                                                                                                                                                                          if 
*                                                                                                                                                                                                                                                                                                                                                                          (
*                                                                                                                                                                                                                                                                                                                                                                          !str
*                                                                                                                                                                                                                                                                                                                                                                          c
*                                                                                                                                                                                                                                                                                                                                                                          ompE
*                                                                                                                                                                                                                                                                                                                                                                          x
*                                                                                                                                                                                                                                                                                                                                                                          (lin
*                                                                                                                                                                                                                                                                                                                                                                          e
*                                                                                                                                                                                                                                                                                                                                                                          [1],
*                                                                                                                                                                                                                                                                                                                                                                          "HTR
*                                                                                                                                                                                                                                                                                                                                                                          "
*                                                                                                                                                                                                                                                                                                                                                                          )) 
*                                                                                                                                                                                                                                                                                                                                                                           { 
*                                                                                                                                                                                                                                                                                                                                                                              s
*                                                                                                                                                                                                                                                                                                                                                                              t
*                                                                                                                                                                                                                                                                                                                                                                              rcpy
*                                                                                                                                                                                                                                                                                                                                                                              (
*                                                                                                                                                                                                                                                                                                                                                                              erro
*                                                                                                                                                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                                                                                                                                                              , "F
*                                                                                                                                                                                                                                                                                                                                                                              i
*                                                                                                                                                                                                                                                                                                                                                                              lety
*                                                                                                                                                                                                                                                                                                                                                                              p
*                                                                                                                                                                                                                                                                                                                                                                              e is
*                                                                                                                                                                                                                                                                                                                                                                              not 
*                                                                                                                                                                                                                                                                                                                                                                              H
*                                                                                                                                                                                                                                                                                                                                                                              TR")
*                                                                                                                                                                                                                                                                                                                                                                              ; 
*                                                                                                                                                                                                                                                                                                                                                                                 r
*                                                                                                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                                                                                                 turn
*                                                                                                                                                                                                                                                                                                                                                                                 fals
*                                                                                                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                                                                                                 ; 
*                                                                                                                                                                                                                                                                                                                                                                                  } 
*                                                                                                                                                                                                                                                                                                                                                                                  } 
*                                                                                                                                                                                                                                                                                                                                                                                  e
*                                                                                                                                                                                                                                                                                                                                                                                  lse 
*                                                                                                                                                                                                                                                                                                                                                                                  i
*                                                                                                                                                                                                                                                                                                                                                                                  f (s
*                                                                                                                                                                                                                                                                                                                                                                                  t
*                                                                                                                                                                                                                                                                                                                                                                                  rcom
*                                                                                                                                                                                                                                                                                                                                                                                  p
*                                                                                                                                                                                                                                                                                                                                                                                  Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                  i
*                                                                                                                                                                                                                                                                                                                                                                                  ne[0
*                                                                                                                                                                                                                                                                                                                                                                                  ]
*                                                                                                                                                                                                                                                                                                                                                                                  , "D
*                                                                                                                                                                                                                                                                                                                                                                                  A
*                                                                                                                                                                                                                                                                                                                                                                                  TATY
*                                                                                                                                                                                                                                                                                                                                                                                  P
*                                                                                                                                                                                                                                                                                                                                                                                  E")) 
*                                                                                                                                                                                                                                                                                                                                                                                  { 
*                                                                                                                                                                                                                                                                                                                                                                                  M
*                                                                                                                                                                                                                                                                                                                                                                                  o
*                                                                                                                                                                                                                                                                                                                                                                                  ti
*                                                                                                                                                                                                                                                                                                                                                                                  o
*                                                                                                                                                                                                                                                                                                                                                                                  n
*                                                                                                                                                                                                                                                                                                                                                                                  C
*                                                                                                                                                                                                                                                                                                                                                                                  a
*                                                                                                                                                                                                                                                                                                                                                                                  p
*                                                                                                                                                                                                                                                                                                                                                                                  t
*                                                                                                                                                                                                                                                                                                                                                                                  u
*                                                                                                                                                                                                                                                                                                                                                                                  r
*                                                                                                                                                                                                                                                                                                                                                                                  e
*                                                                                                                                                                                                                                                                                                                                                                                  F
*                                                                                                                                                                                                                                                                                                                                                                                  i
*                                                                                                                                                                                                                                                                                                                                                                                  l
*                                                                                                                                                                                                                                                                                                                                                                                  e
*                                                                                                                                                                                                                                                                                                                                                                                   F
*                                                                                                                                                                                                                                                                                                                                                                                   o
*                                                                                                                                                                                                                                                                                                                                                                                   r
*                                                                                                                                                                                                                                                                                                                                                                                   m
*                                                                                                                                                                                                                                                                                                                                                                                   a
*                                                                                                                                                                                                                                                                                                                                                                                   t
*                                                                                                                                                                                                                                                                                                                                                                                   s
*                                                                                                                                                                                                                                                                                                                                                                                   E
*                                                                                                                                                                                                                                                                                                                                                                                   xp
*                                                                                                                                                                                                                                                                                                                                                                                   l
*                                                                                                                                                                                                                                                                                                                                                                                   a
*                                                                                                                                                                                                                                                                                                                                                                                   i
*                                                                                                                                                                                                                                                                                                                                                                                   n
*                                                                                                                                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                                                                                                                                   d
*                                                                                                                                                                                                                                                                                                                                                                                   35
*                                                                                                                                                                                                                                                                                                                                                                                    // 
*                                                                                                                                                                                                                                                                                                                                                                                    D
*                                                                                                                                                                                                                                                                                                                                                                                    atat
*                                                                                                                                                                                                                                                                                                                                                                                    y
*                                                                                                                                                                                                                                                                                                                                                                                    pe d
*                                                                                                                                                                                                                                                                                                                                                                                    e
*                                                                                                                                                                                                                                                                                                                                                                                    scri
*                                                                                                                                                                                                                                                                                                                                                                                    b
*                                                                                                                                                                                                                                                                                                                                                                                    es t
*                                                                                                                                                                                                                                                                                                                                                                                    h
*                                                                                                                                                                                                                                                                                                                                                                                    e or
*                                                                                                                                                                                                                                                                                                                                                                                    d
*                                                                                                                                                                                                                                                                                                                                                                                    er o
*                                                                                                                                                                                                                                                                                                                                                                                    f
*                                                                                                                                                                                                                                                                                                                                                                                     tra
*                                                                                                                                                                                                                                                                                                                                                                                     n
*                                                                                                                                                                                                                                                                                                                                                                                     sfor
*                                                                                                                                                                                                                                                                                                                                                                                     m
*                                                                                                                                                                                                                                                                                                                                                                                     atio
*                                                                                                                                                                                                                                                                                                                                                                                     n
*                                                                                                                                                                                                                                                                                                                                                                                      com
*                                                                                                                                                                                                                                                                                                                                                                                      p
*                                                                                                                                                                                                                                                                                                                                                                                      osit
*                                                                                                                                                                                                                                                                                                                                                                                      i
*                                                                                                                                                                                                                                                                                                                                                                                      on o
*                                                                                                                                                                                                                                                                                                                                                                                      f
*                                                                                                                                                                                                                                                                                                                                                                                       tra
*                                                                                                                                                                                                                                                                                                                                                                                       n
*                                                                                                                                                                                                                                                                                                                                                                                       slat
*                                                                                                                                                                                                                                                                                                                                                                                       i
*                                                                                                                                                                                                                                                                                                                                                                                       on, 
*                                                                                                                                                                                                                                                                                                                                                                                       r
*                                                                                                                                                                                                                                                                                                                                                                                       otat
*                                                                                                                                                                                                                                                                                                                                                                                       i
*                                                                                                                                                                                                                                                                                                                                                                                       on 
*                                                                                                                                                                                                                                                                                                                                                                                        // 
*                                                                                                                                                                                                                                                                                                                                                                                        a
*                                                                                                                                                                                                                                                                                                                                                                                        nd s
*                                                                                                                                                                                                                                                                                                                                                                                        c
*                                                                                                                                                                                                                                                                                                                                                                                        ale 
*                                                                                                                                                                                                                                                                                                                                                                                        v
*                                                                                                                                                                                                                                                                                                                                                                                        alue
*                                                                                                                                                                                                                                                                                                                                                                                        s
*                                                                                                                                                                                                                                                                                                                                                                                        .  A
*                                                                                                                                                                                                                                                                                                                                                                                        l
*                                                                                                                                                                                                                                                                                                                                                                                        most
*                                                                                                                                                                                                                                                                                                                                                                                        alwa
*                                                                                                                                                                                                                                                                                                                                                                                        y
*                                                                                                                                                                                                                                                                                                                                                                                        s "H
*                                                                                                                                                                                                                                                                                                                                                                                        T
*                                                                                                                                                                                                                                                                                                                                                                                        RS" 
*                                                                                                                                                                                                                                                                                                                                                                                        -
*                                                                                                                                                                                                                                                                                                                                                                                         Tra
*                                                                                                                                                                                                                                                                                                                                                                                         n
*                                                                                                                                                                                                                                                                                                                                                                                         slat
*                                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                                         on, 
*                                                                                                                                                                                                                                                                                                                                                                                         R
*                                                                                                                                                                                                                                                                                                                                                                                         otat
*                                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                                         on, 
*                                                                                                                                                                                                                                                                                                                                                                                         S
*                                                                                                                                                                                                                                                                                                                                                                                         cale 
*                                                                                                                                                                                                                                                                                                                                                                                          if 
*                                                                                                                                                                                                                                                                                                                                                                                          (
*                                                                                                                                                                                                                                                                                                                                                                                          !str
*                                                                                                                                                                                                                                                                                                                                                                                          c
*                                                                                                                                                                                                                                                                                                                                                                                          ompE
*                                                                                                                                                                                                                                                                                                                                                                                          x
*                                                                                                                                                                                                                                                                                                                                                                                          (lin
*                                                                                                                                                                                                                                                                                                                                                                                          e
*                                                                                                                                                                                                                                                                                                                                                                                          [1],
*                                                                                                                                                                                                                                                                                                                                                                                          "HTR
*                                                                                                                                                                                                                                                                                                                                                                                          S
*                                                                                                                                                                                                                                                                                                                                                                                          ")) 
*                                                                                                                                                                                                                                                                                                                                                                                           { 
*                                                                                                                                                                                                                                                                                                                                                                                              s
*                                                                                                                                                                                                                                                                                                                                                                                              t
*                                                                                                                                                                                                                                                                                                                                                                                              rcpy
*                                                                                                                                                                                                                                                                                                                                                                                              (
*                                                                                                                                                                                                                                                                                                                                                                                              erro
*                                                                                                                                                                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                                                                                                                                                                              , st
*                                                                                                                                                                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                                                                                                                                                                              cat(
*                                                                                                                                                                                                                                                                                                                                                                                              "
*                                                                                                                                                                                                                                                                                                                                                                                              Erro
*                                                                                                                                                                                                                                                                                                                                                                                              r
*                                                                                                                                                                                                                                                                                                                                                                                              : In
*                                                                                                                                                                                                                                                                                                                                                                                              c
*                                                                                                                                                                                                                                                                                                                                                                                              ompa
*                                                                                                                                                                                                                                                                                                                                                                                              t
*                                                                                                                                                                                                                                                                                                                                                                                              ible
*                                                                                                                                                                                                                                                                                                                                                                                              tran
*                                                                                                                                                                                                                                                                                                                                                                                              s
*                                                                                                                                                                                                                                                                                                                                                                                              form
*                                                                                                                                                                                                                                                                                                                                                                                              a
*                                                                                                                                                                                                                                                                                                                                                                                              tion
*                                                                                                                                                                                                                                                                                                                                                                                              comp
*                                                                                                                                                                                                                                                                                                                                                                                              o
*                                                                                                                                                                                                                                                                                                                                                                                              siti
*                                                                                                                                                                                                                                                                                                                                                                                              o
*                                                                                                                                                                                                                                                                                                                                                                                              n: "
*                                                                                                                                                                                                                                                                                                                                                                                              ,
*                                                                                                                                                                                                                                                                                                                                                                                              line
*                                                                                                                                                                                                                                                                                                                                                                                              [
*                                                                                                                                                                                                                                                                                                                                                                                              1]))
*                                                                                                                                                                                                                                                                                                                                                                                              ; 
*                                                                                                                                                                                                                                                                                                                                                                                               } 
*                                                                                                                                                                                                                                                                                                                                                                                               } 
*                                                                                                                                                                                                                                                                                                                                                                                               e
*                                                                                                                                                                                                                                                                                                                                                                                               lse 
*                                                                                                                                                                                                                                                                                                                                                                                               i
*                                                                                                                                                                                                                                                                                                                                                                                               f (s
*                                                                                                                                                                                                                                                                                                                                                                                               t
*                                                                                                                                                                                                                                                                                                                                                                                               rcom
*                                                                                                                                                                                                                                                                                                                                                                                               p
*                                                                                                                                                                                                                                                                                                                                                                                               Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                               i
*                                                                                                                                                                                                                                                                                                                                                                                               ne[0
*                                                                                                                                                                                                                                                                                                                                                                                               ]
*                                                                                                                                                                                                                                                                                                                                                                                               , "F
*                                                                                                                                                                                                                                                                                                                                                                                               I
*                                                                                                                                                                                                                                                                                                                                                                                               LEVE
*                                                                                                                                                                                                                                                                                                                                                                                               R
*                                                                                                                                                                                                                                                                                                                                                                                               SION
*                                                                                                                                                                                                                                                                                                                                                                                               "
*                                                                                                                                                                                                                                                                                                                                                                                               )) 
*                                                                                                                                                                                                                                                                                                                                                                                               { 
*                                                                                                                                                                                                                                                                                                                                                                                                if 
*                                                                                                                                                                                                                                                                                                                                                                                                (
*                                                                                                                                                                                                                                                                                                                                                                                                atoi
*                                                                                                                                                                                                                                                                                                                                                                                                (
*                                                                                                                                                                                                                                                                                                                                                                                                line
*                                                                                                                                                                                                                                                                                                                                                                                                [
*                                                                                                                                                                                                                                                                                                                                                                                                1])!
*                                                                                                                                                                                                                                                                                                                                                                                                =
*                                                                                                                                                                                                                                                                                                                                                                                                1) 
*                                                                                                                                                                                                                                                                                                                                                                                                 { 
*                                                                                                                                                                                                                                                                                                                                                                                                    s
*                                                                                                                                                                                                                                                                                                                                                                                                    t
*                                                                                                                                                                                                                                                                                                                                                                                                    rcpy
*                                                                                                                                                                                                                                                                                                                                                                                                    (
*                                                                                                                                                                                                                                                                                                                                                                                                    erro
*                                                                                                                                                                                                                                                                                                                                                                                                    r
*                                                                                                                                                                                                                                                                                                                                                                                                    , "F
*                                                                                                                                                                                                                                                                                                                                                                                                    i
*                                                                                                                                                                                                                                                                                                                                                                                                    le v
*                                                                                                                                                                                                                                                                                                                                                                                                    e
*                                                                                                                                                                                                                                                                                                                                                                                                    rsio
*                                                                                                                                                                                                                                                                                                                                                                                                    n
*                                                                                                                                                                                                                                                                                                                                                                                                     not
*                                                                                                                                                                                                                                                                                                                                                                                                     not 
*                                                                                                                                                                                                                                                                                                                                                                                                     r
*                                                                                                                                                                                                                                                                                                                                                                                                     ecog
*                                                                                                                                                                                                                                                                                                                                                                                                     n
*                                                                                                                                                                                                                                                                                                                                                                                                     ised
*                                                                                                                                                                                                                                                                                                                                                                                                     "
*                                                                                                                                                                                                                                                                                                                                                                                                     ); 
*                                                                                                                                                                                                                                                                                                                                                                                                        r
*                                                                                                                                                                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                                                                                                                                                                        turn
*                                                                                                                                                                                                                                                                                                                                                                                                        fals
*                                                                                                                                                                                                                                                                                                                                                                                                        e
*                                                                                                                                                                                                                                                                                                                                                                                                        ; 
*                                                                                                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                                                                                                                                                         lse 
*                                                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                                                         f (s
*                                                                                                                                                                                                                                                                                                                                                                                                         t
*                                                                                                                                                                                                                                                                                                                                                                                                         rcom
*                                                                                                                                                                                                                                                                                                                                                                                                         p
*                                                                                                                                                                                                                                                                                                                                                                                                         Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                                                         ne[0
*                                                                                                                                                                                                                                                                                                                                                                                                         ]
*                                                                                                                                                                                                                                                                                                                                                                                                         , "N
*                                                                                                                                                                                                                                                                                                                                                                                                         U
*                                                                                                                                                                                                                                                                                                                                                                                                         MSEG
*                                                                                                                                                                                                                                                                                                                                                                                                         M
*                                                                                                                                                                                                                                                                                                                                                                                                         ENTS
*                                                                                                                                                                                                                                                                                                                                                                                                         "
*                                                                                                                                                                                                                                                                                                                                                                                                         )) 
*                                                                                                                                                                                                                                                                                                                                                                                                         { 
*                                                                                                                                                                                                                                                                                                                                                                                                          hea
*                                                                                                                                                                                                                                                                                                                                                                                                          d
*                                                                                                                                                                                                                                                                                                                                                                                                          er->n
*                                                                                                                                                                                                                                                                                                                                                                                                          o
*                                                                                                                                                                                                                                                                                                                                                                                                          ofse
*                                                                                                                                                                                                                                                                                                                                                                                                          g
*                                                                                                                                                                                                                                                                                                                                                                                                          ment
*                                                                                                                                                                                                                                                                                                                                                                                                          s
*                                                                                                                                                                                                                                                                                                                                                                                                           = a
*                                                                                                                                                                                                                                                                                                                                                                                                           t
*                                                                                                                                                                                                                                                                                                                                                                                                           oi(l
*                                                                                                                                                                                                                                                                                                                                                                                                           i
*                                                                                                                                                                                                                                                                                                                                                                                                           ne[1
*                                                                                                                                                                                                                                                                                                                                                                                                           ]
*                                                                                                                                                                                                                                                                                                                                                                                                           ); 
*                                                                                                                                                                                                                                                                                                                                                                                                            nod
*                                                                                                                                                                                                                                                                                                                                                                                                            e
*                                                                                                                                                                                                                                                                                                                                                                                                            list
*                                                                                                                                                                                                                                                                                                                                                                                                            = (N
*                                                                                                                                                                                                                                                                                                                                                                                                            O
*                                                                                                                                                                                                                                                                                                                                                                                                            DE**
*                                                                                                                                                                                                                                                                                                                                                                                                            )
*                                                                                                                                                                                                                                                                                                                                                                                                             mal
*                                                                                                                                                                                                                                                                                                                                                                                                             l
*                                                                                                                                                                                                                                                                                                                                                                                                             oc(s
*                                                                                                                                                                                                                                                                                                                                                                                                             i
*                                                                                                                                                                                                                                                                                                                                                                                                             zeof
*                                                                                                                                                                                                                                                                                                                                                                                                             (
*                                                                                                                                                                                                                                                                                                                                                                                                             NODE
*                                                                                                                                                                                                                                                                                                                                                                                                             *
*                                                                                                                                                                                                                                                                                                                                                                                                             ) * 
*                                                                                                                                                                                                                                                                                                                                                                                                             (
*                                                                                                                                                                                                                                                                                                                                                                                                             head
*                                                                                                                                                                                                                                                                                                                                                                                                             e
*                                                                                                                                                                                                                                                                                                                                                                                                             r->n
*                                                                                                                                                                                                                                                                                                                                                                                                             o
*                                                                                                                                                                                                                                                                                                                                                                                                             ofse
*                                                                                                                                                                                                                                                                                                                                                                                                             g
*                                                                                                                                                                                                                                                                                                                                                                                                             ment
*                                                                                                                                                                                                                                                                                                                                                                                                             s
*                                                                                                                                                                                                                                                                                                                                                                                                              + 1
*                                                                                                                                                                                                                                                                                                                                                                                                              )
*                                                                                                                                                                                                                                                                                                                                                                                                              ); 
*                                                                                                                                                                                                                                                                                                                                                                                                               cur
*                                                                                                                                                                                                                                                                                                                                                                                                               r
*                                                                                                                                                                                                                                                                                                                                                                                                               entn
*                                                                                                                                                                                                                                                                                                                                                                                                               o
*                                                                                                                                                                                                                                                                                                                                                                                                               de =
*                                                                                                                                                                                                                                                                                                                                                                                                               0; 
*                                                                                                                                                                                                                                                                                                                                                                                                               } 
*                                                                                                                                                                                                                                                                                                                                                                                                               e
*                                                                                                                                                                                                                                                                                                                                                                                                               lse 
*                                                                                                                                                                                                                                                                                                                                                                                                               i
*                                                                                                                                                                                                                                                                                                                                                                                                               f (s
*                                                                                                                                                                                                                                                                                                                                                                                                               t
*                                                                                                                                                                                                                                                                                                                                                                                                               rcom
*                                                                                                                                                                                                                                                                                                                                                                                                               p
*                                                                                                                                                                                                                                                                                                                                                                                                               Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                                               i
*                                                                                                                                                                                                                                                                                                                                                                                                               ne[0
*                                                                                                                                                                                                                                                                                                                                                                                                               ]
*                                                                                                                                                                                                                                                                                                                                                                                                               , "N
*                                                                                                                                                                                                                                                                                                                                                                                                               U
*                                                                                                                                                                                                                                                                                                                                                                                                               MFRA
*                                                                                                                                                                                                                                                                                                                                                                                                               M
*                                                                                                                                                                                                                                                                                                                                                                                                               ES")
*                                                                                                                                                                                                                                                                                                                                                                                                               ) 
*                                                                                                                                                                                                                                                                                                                                                                                                                hea
*                                                                                                                                                                                                                                                                                                                                                                                                                d
*                                                                                                                                                                                                                                                                                                                                                                                                                er->n
*                                                                                                                                                                                                                                                                                                                                                                                                                o
*                                                                                                                                                                                                                                                                                                                                                                                                                offr
*                                                                                                                                                                                                                                                                                                                                                                                                                a
*                                                                                                                                                                                                                                                                                                                                                                                                                mes 
*                                                                                                                                                                                                                                                                                                                                                                                                                =
*                                                                                                                                                                                                                                                                                                                                                                                                                 ato
*                                                                                                                                                                                                                                                                                                                                                                                                                 l
*                                                                                                                                                                                                                                                                                                                                                                                                                 (lin
*                                                                                                                                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                                                                                                                                 [1])
*                                                                                                                                                                                                                                                                                                                                                                                                                 ; 
*                                                                                                                                                                                                                                                                                                                                                                                                                 e
*                                                                                                                                                                                                                                                                                                                                                                                                                 lse 
*                                                                                                                                                                                                                                                                                                                                                                                                                 i
*                                                                                                                                                                                                                                                                                                                                                                                                                 f (s
*                                                                                                                                                                                                                                                                                                                                                                                                                 t
*                                                                                                                                                                                                                                                                                                                                                                                                                 rcom
*                                                                                                                                                                                                                                                                                                                                                                                                                 p
*                                                                                                                                                                                                                                                                                                                                                                                                                 Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                                                 i
*                                                                                                                                                                                                                                                                                                                                                                                                                 ne[0
*                                                                                                                                                                                                                                                                                                                                                                                                                 ]
*                                                                                                                                                                                                                                                                                                                                                                                                                 , "D
*                                                                                                                                                                                                                                                                                                                                                                                                                 A
*                                                                                                                                                                                                                                                                                                                                                                                                                 TAFRA
*                                                                                                                                                                                                                                                                                                                                                                                                                 M
*                                                                                                                                                                                                                                                                                                                                                                                                                 ERAT
*                                                                                                                                                                                                                                                                                                                                                                                                                 E
*                                                                                                                                                                                                                                                                                                                                                                                                                 ")) 
*                                                                                                                                                                                                                                                                                                                                                                                                                  hea
*                                                                                                                                                                                                                                                                                                                                                                                                                  d
*                                                                                                                                                                                                                                                                                                                                                                                                                  er->d
*                                                                                                                                                                                                                                                                                                                                                                                                                  a
*                                                                                                                                                                                                                                                                                                                                                                                                                  tara
*                                                                                                                                                                                                                                                                                                                                                                                                                  t
*                                                                                                                                                                                                                                                                                                                                                                                                                  e = 
*                                                                                                                                                                                                                                                                                                                                                                                                                  a
*                                                                                                                                                                                                                                                                                                                                                                                                                  toi(
*                                                                                                                                                                                                                                                                                                                                                                                                                  l
*                                                                                                                                                                                                                                                                                                                                                                                                                  ine[
*                                                                                                                                                                                                                                                                                                                                                                                                                  1
*                                                                                                                                                                                                                                                                                                                                                                                                                  ]); 
*                                                                                                                                                                                                                                                                                                                                                                                                                  e
*                                                                                                                                                                                                                                                                                                                                                                                                                  lse 
*                                                                                                                                                                                                                                                                                                                                                                                                                  i
*                                                                                                                                                                                                                                                                                                                                                                                                                  f (s
*                                                                                                                                                                                                                                                                                                                                                                                                                  t
*                                                                                                                                                                                                                                                                                                                                                                                                                  rcom
*                                                                                                                                                                                                                                                                                                                                                                                                                  p
*                                                                                                                                                                                                                                                                                                                                                                                                                  Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                                                  i
*                                                                                                                                                                                                                                                                                                                                                                                                                  ne[0
*                                                                                                                                                                                                                                                                                                                                                                                                                  ]
*                                                                                                                                                                                                                                                                                                                                                                                                                  , "E
*                                                                                                                                                                                                                                                                                                                                                                                                                  U
*                                                                                                                                                                                                                                                                                                                                                                                                                  LERR
*                                                                                                                                                                                                                                                                                                                                                                                                                  O
*                                                                                                                                                                                                                                                                                                                                                                                                                  TATI
*                                                                                                                                                                                                                                                                                                                                                                                                                  O
*                                                                                                                                                                                                                                                                                                                                                                                                                  NORD
*                                                                                                                                                                                                                                                                                                                                                                                                                  E
*                                                                                                                                                                                                                                                                                                                                                                                                                  R")) 
*                                                                                                                                                                                                                                                                                                                                                                                                                   // 
*                                                                                                                                                                                                                                                                                                                                                                                                                   S
*                                                                                                                                                                                                                                                                                                                                                                                                                   etup
*                                                                                                                                                                                                                                                                                                                                                                                                                   the 
*                                                                                                                                                                                                                                                                                                                                                                                                                   e
*                                                                                                                                                                                                                                                                                                                                                                                                                   uler
*                                                                                                                                                                                                                                                                                                                                                                                                                   rota
*                                                                                                                                                                                                                                                                                                                                                                                                                   t
*                                                                                                                                                                                                                                                                                                                                                                                                                   ion 
*                                                                                                                                                                                                                                                                                                                                                                                                                   a
*                                                                                                                                                                                                                                                                                                                                                                                                                   ngle 
*                                                                                                                                                                                                                                                                                                                                                                                                                    for
*                                                                                                                                                                                                                                                                                                                                                                                                                    (int
*                                                                                                                                                                                                                                                                                                                                                                                                                    i=0;
*                                                                                                                                                                                                                                                                                                                                                                                                                    i<3;
*                                                                                                                                                                                                                                                                                                                                                                                                                    ++i) 
*                                                                                                                                                                                                                                                                                                                                                                                                                       s
*                                                                                                                                                                                                                                                                                                                                                                                                                       w
*                                                                                                                                                                                                                                                                                                                                                                                                                       itch
*                                                                                                                                                                                                                                                                                                                                                                                                                       (lin
*                                                                                                                                                                                                                                                                                                                                                                                                                       e
*                                                                                                                                                                                                                                                                                                                                                                                                                       [1][
*                                                                                                                                                                                                                                                                                                                                                                                                                       i
*                                                                                                                                                                                                                                                                                                                                                                                                                       ]&0x
*                                                                                                                                                                                                                                                                                                                                                                                                                       d
*                                                                                                                                                                                                                                                                                                                                                                                                                       f) 
*                                                                                                                                                                                                                                                                                                                                                                                                                          { 
*                                                                                                                                                                                                                                                                                                                                                                                                                          case
*                                                                                                                                                                                                                                                                                                                                                                                                                          'Z': 
*                                                                                                                                                                                                                                                                                                                                                                                                                            zp
*                                                                                                                                                                                                                                                                                                                                                                                                                            o
*                                                                                                                                                                                                                                                                                                                                                                                                                            s = 
*                                                                                                                                                                                                                                                                                                                                                                                                                            i
*                                                                                                                                                                                                                                                                                                                                                                                                                            ; 
*                                                                                                                                                                                                                                                                                                                                                                                                                              bre
*                                                                                                                                                                                                                                                                                                                                                                                                                              a
*                                                                                                                                                                                                                                                                                                                                                                                                                              k; 
*                                                                                                                                                                                                                                                                                                                                                                                                                              case
*                                                                                                                                                                                                                                                                                                                                                                                                                              'Y': 
*                                                                                                                                                                                                                                                                                                                                                                                                                                yp
*                                                                                                                                                                                                                                                                                                                                                                                                                                o
*                                                                                                                                                                                                                                                                                                                                                                                                                                s = 
*                                                                                                                                                                                                                                                                                                                                                                                                                                i
*                                                                                                                                                                                                                                                                                                                                                                                                                                ; 
*                                                                                                                                                                                                                                                                                                                                                                                                                                  br
*                                                                                                                                                                                                                                                                                                                                                                                                                                  e
*                                                                                                                                                                                                                                                                                                                                                                                                                                  ak; 
*                                                                                                                                                                                                                                                                                                                                                                                                                                  case
*                                                                                                                                                                                                                                                                                                                                                                                                                                  'X': 
*                                                                                                                                                                                                                                                                                                                                                                                                                                    xp
*                                                                                                                                                                                                                                                                                                                                                                                                                                    o
*                                                                                                                                                                                                                                                                                                                                                                                                                                    s = 
*                                                                                                                                                                                                                                                                                                                                                                                                                                    i
*                                                                                                                                                                                                                                                                                                                                                                                                                                    ; 
*                                                                                                                                                                                                                                                                                                                                                                                                                                      br
*                                                                                                                                                                                                                                                                                                                                                                                                                                      e
*                                                                                                                                                                                                                                                                                                                                                                                                                                      ak; 
*                                                                                                                                                                                                                                                                                                                                                                                                                                         } 
*                                                                                                                                                                                                                                                                                                                                                                                                                                         e
*                                                                                                                                                                                                                                                                                                                                                                                                                                         lse 
*                                                                                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                                                                                         f (s
*                                                                                                                                                                                                                                                                                                                                                                                                                                         t
*                                                                                                                                                                                                                                                                                                                                                                                                                                         rcom
*                                                                                                                                                                                                                                                                                                                                                                                                                                         p
*                                                                                                                                                                                                                                                                                                                                                                                                                                         Ex(l
*                                                                                                                                                                                                                                                                                                                                                                                                                                         i
*                                                                                                                                                                                                                                                                                                                                                                                                                                         ne[0
*                                                                                                                                                                                                                                                                                                                                                                                                                                         ]
*                                                                                                                                                                                                                                                                                                                                                                                                                                         , "C
*                                                                                                                                                                                                                                                                                                                                                                                                                                         A
*                                                                                                                                                                                                                                                                                                                                                                                                                                         LIBR
*                                                                                                                                                                                                                                                                                                                                                                                                                                         A
*                                                                                                                                                                                                                                                                                                                                                                                                                                         TION
*                                                                                                                                                                                                                                                                                                                                                                                                                                         U
*                                                                                                                                                                                                                                                                                                                                                                                                                                         NITS
*                                                                                                                                                                                                                                                                                                                                                                                                                                         "
*                                                                                                                                                                                                                                                                                                                                                                                                                                         )) 
*                                                                                                                                                                                                                                                                                                                                                                                                                                          if 
*                                                                                                                                                                                                                                                                                                                                                                                                                                          (
*                                                                                                                                                                                                                                                                                                                                                                                                                                          strc
*                                                                                                                                                                                                                                                                                                                                                                                                                                          o
*                                                                                                                                                                                                                                                                                                                                                                                                                                          mpEx
*                                                                                                                                                                                                                                                                                                                                                                                                                                          (
*                                                                                                                                                                                                                                                                                                                                                                                                                                          line
*                                                                                                                                                                                                                                                                                                                                                                                                                                          [
*                                                                                                                                                                                                                                                                                                                                                                                                                                          1], 
*                                                                                                                                                                                                                                                                                                                                                                                                                                          "
*                                                                                                                                                                                                                                                                                                                                                                                                                                          MM")
*                                                                                                                                                                                                                                                                                                                                                                                                                                          ) 
*                                                                                                                                                                                                                                                                                                                                                                                                                                             h
*                                                                                                                                                                                                                                                                                                                                                                                                                                             e
*                                                                                                                                                                                                                                                                                                                                                                                                                                             ader
*                                                                                                                                                                                                                                                                                                                                                                                                                                             -
*                                                                                                                                                                                                                                                                                                                                                                                                                                             >cal
*                                                                                                                                                                                                                                                                                                                                                                                                                                             l
*                                                                                                                                                                                                                                                                                                                                                                                                                                             ib =
*                                                                                                                                                                                                                                                                                                                                                                                                                                             1.0f
*/ 10
0
0.0f
; 
 els
e
 if 
(
strc
o
mpEx
(
line
[
1], 
"
DM")
) 
   h
e
ader
-
>cal
l
ib =
1.0f
/ 10
.
0f; 
 els
e
 if 
(
strc
o
mpEx
(
line
[
1], 
"
CM")
) 
   h
e
ader
-
>cal
l
ib =
1.0f
/ 10
0
.0f; 
 els
e
 if 
(
strc
o
mpEx
(
line
[
1], 
"
INCH
E
S")) 
   h
e
ader
-
>cal
l
ib =
1.0f
/ 40
.
0f; 
 els
e 
   h
e
ader
-
>cal
l
ib = 
1
.0f
; 
e
lse 
i
f (s
t
rcom
p
Ex(l
i
ne[0
]
, "R
O
TATI
O
NUNI
T
S")) 
 if 
(
strc
o
mpEx
(
line
[
1], 
"
DEGR
E
ES")
) 
   h
e
ader
-
>deg
r
ees 
=
 tru
e
; 
 els
e 
   h
e
ader
-
>deg
r
ees 
=
 fal
s
e; 
e
lse 
i
f (s
t
rcom
p
Ex(l
i
ne[0
]
, "S
C
ALEF
A
CTOR
"
)) 
 hea
d
er->s
c
alef
a
ctor
= (f
l
oat)
atof
(
line
[
1])
; 
r
etur
n
 tru
e
; 
} 
bo
o
l HT
R
Form
a
t::P
r
oces
s
Segm
e
nts(
c
har 
l
ine[
2
][40
]
, in
t
 pos
[
2]) 
{ 
/
/ Th
i
s me
t
hod 
s
impl
y
 rea
d
s in
each
segm
e
nt a
n
d ma
k
es s
u
re t
h
at a
node
exis
t
s fo
r
 it,
link
i
ng 
th
e
 nod
e
s as
it g
o
es 
/
/ Cr
e
ate 
a
n ar
r
ay o
f
 nod
e
s th
a
t wi
l
l ho
l
d th
e
 nam
e
s of
each
segm
e
nt w
h
ich 
w
ill 
b
e li
n
ked 
N
ODE 
*
* pare
* n
* t = 
* 0
* , *c
* h
* ild 
* =
*  0, 
*  *
*  *tem
*  p
*  ;   
*/
/ te
m
p us
e
d to
add 
n
ew c
h
ildr
e
n to
pare
n
t no
d
es 
i
nt i
; 
/
/ li
n
e[0]
= Ch
i
ld  
  li
n
e[1]
= Pa
r
ent 
/
/ Ge
t
 the
posi
t
ion 
w
ithi
n
 the
segm
e
nt s
t
ruct
u
re o
f
 eac
h
 nod
e
 - i
f
 the
r
e is
no c
u
rren
t
 ent
r
y 
th
e
n cr
e
ate 
a
 new
on a
t
 the
end 
f
or (
i
=0; 
i
<cur
r
entn
o
de; 
+
+i) 
{ 
 if 
(
!str
c
mp(n
o
deli
s
t[i]
-
>nam
e
, li
n
e[0]
)
) 
M
o
ti
o
n
C
a
p
t
u
r
e
F
i
l
e
 F
o
r
m
a
t
s
E
xp
l
a
i
n
e
d
36
   c
h
ild 
=
 nod
e
list
[
i]; 
 if 
(
!str
c
mp(n
o
deli
s
t[i]
-
>nam
e
, li
n
e[1]
)
) 
   p
a
rent
= no
d
elis
t
[i]; 
} 
i
f (c
h
ild 
&
& chi
l
d==p
a
rent
) 
{ 
 str
c
py(e
r
ror,
"Err
o
r: A
cycl
i
c ch
i
ld-pa
r
ent 
r
elat
i
onsh
i
p ha
s
 bee
n
 det
e
cted
in t
h
e Se
g
men
t 
se
c
tion
of t
h
e fi
l
e"); 
 ret
u
rn f
a
lse; 
} 
i
f (!
c
hild
) 
{ 
 nod
e
list
[
curr
e
ntno
d
e] =
(NOD
E
*) m
a
lloc
(
size
o
f(NO
D
E)); 
 nod
e
list
[
curr
e
ntnod
e
]->n
a
me =
(cha
r
*) m
a
lloc
(
size
o
f(ch
a
r) *
(str
l
en(l
i
ne[0
]
) + 
1
)); 
 nod
e
list
[
curr
e
ntno
d
e]->o
b
ject
=
0; 
 nod
e
list
[
curr
e
ntno
d
e]->n
o
ofco
n
stra
i
nts=
0
; 
 nod
e
list
[
curr
e
ntno
d
e]->c
o
nstr
a
ints
=
0; 
 str
c
py(n
o
deli
s
t[cu
r
rent
n
ode]
-
>nam
e
, li
n
e[0]
)
; 
 Set
u
pChi
l
dren
(
node
l
ist[
c
urre
n
tnod
e
], 0
)
; 
 Set
u
pFra
m
es(n
o
deli
s
t[cu
r
rent
n
ode]
,
 hea
d
er->n
o
offr
a
mes)
; 
 chi
l
d = 
n
odel
i
st[0
]
; 
 chi
l
d = 
n
odel
i
st[c
u
rren
t
node
+
+]; 
} 
i
f (!
p
aren
t
) 
{ 
 nod
e
list
[
curr
e
ntno
d
e] =
(NOD
E
*) m
a
lloc
(
size
o
f(NO
D
E)); 
 nod
e
list
[
curr
e
ntnod
e
]->n
a
me =
(cha
r
*) m
a
lloc
(
size
o
f(ch
a
r) *
(str
l
en(l
i
ne[1
]
) + 
1
)); 
 nod
e
list
[
curr
e
ntno
d
e]->o
b
ject
=
0; 
 nod
e
list
[
curr
e
ntno
d
e]->n
o
ofco
n
stra
i
nts=
0
; 
 nod
e
list
[
curr
e
ntno
d
e]->c
o
nstr
a
ints
=
0; 
 str
c
py(n
o
deli
s
t[cu
r
rent
n
ode]
-
>nam
e
, li
n
e[1]
)
; 
 Set
u
pChi
l
dren
(
node
l
ist[
c
urre
n
tnod
e
], 0
)
; 
 Set
u
pFra
m
es(n
o
deli
s
t[cu
r
rent
n
ode]
,
 hea
d
er->n
o
offr
a
mes)
; 
 par
e
nt =
node
l
ist[
c
urre
n
tnod
e
++]; 
} 
c
hild
-
>par
e
nt=p
a
rent
; 
i
f (p
a
rent
-
>chi
l
dren
) 
{ 
 // 
 // P
 // aren
 // t
 //  alr
 //  e
 //  ady 
 //  h
 //  as c
 //  h
 //  ildr
 //  e
 //  n 
 //   tem
 //   p
 //    = (
 //    N
 //    ODE*
 //    *
 //    ) ma
 //    l
 //    loc(
 //    s
 //    izeo
 //    f
 //    (NODE
 //    *
 //    ) * 
 //    p
 //    aren
 //    t
 //    ->n
 //    o
 //    ofch
 //    i
 //    ldre
 //    n
 //    ); 
 //     for
 //     (i=0
 //     ;
 //      i<p
 //      a
 //      rent
 //      -
 //      >noo
 //      f
 //      chil
 //      d
 //      ren;
 //      ++i) 
 //         t
 //         e
 //         mp[i
 //         ]
 //          = p
 //          a
 //          rent
 //          -
 //          >chi
 //          l
 //          dren
 //          [
 //          i]; 
 //           fre
 //           e
 //           (par
 //           e
 //           nt->c
 //           h
 //           ildr
 //           e
 //           n); 
 //            par
 //            e
 //            nt->c
 //            h
 //            ildr
 //            e
 //            n = 
 //            (
 //            NODE
 //            *
 //            *) m
 //            a
 //            lloc
 //            (
 //            size
 //            o
 //            f(NO
 //            D
 //            E*) 
 //            *
 //             ++p
 //             a
 //             ren
 //             t
 //             ->no
 //             o
 //             fchi
 //             l
 //             dren
 //             )
 //             ; 
 //              for
 //              (i=0
 //              ;
 //               i<p
 //               a
 //               rent
 //               -
 //               >noo
 //               f
 //               chil
 //               d
 //               ren;
 //               ++i) 
 //                  p
 //                  a
 //                  rent
 //                  -
 //                  >chi
 //                  l
 //                  dren
 //                  [
 //                  i] =
 //                  temp
 //                  [
 //                  i]; 
 //                   fre
 //                   e
 //                   (tem
 //                   p
 //                   ); 
 //                    par
 //                    e
 //                    nt->c
 //                    h
 //                    ildr
 //                    e
 //                    n[pa
 //                    r
 //                    ent
 //                    -
 //                    >noo
 //                    f
 //                    chil
 //                    d
 //                    ren-1
 //                    ]
 //                     = c
 //                     h
 //                     ild
 //                     ; 
 //                     } 
 //                     e
 //                     lse 
 //                     { 
 //                      // 
 //                      P
 //                      aren
 //                      t
 //                       doe
 //                       s
 //                        not
 //                        have
 //                        any 
 //                        c
 //                        hild
 //                        r
 //                        en 
 //                         Set
 //                         u
 //                         pChi
 //                         l
 //                         dren
 //                         (
 //                         pare
 //                         n
 //                         t, +
 //                         +
 //                         pare
 //                         n
 //                         t->n
 //                         o
 //                         ofch
 //                         i
 //                         ldre
 //                         n
 //                         ); 
 //                          par
 //                          e
 //                          nt->c
 //                          h
 //                          ildr
 //                          e
 //                          n[0]
 //                          = ch
 //                          i
 //                          ld; 
 //                          } 
 //                          i
 //                          f (c
 //                          u
 //                          rren
 //                          t
 //                          node
 //                          >
 //                          head
 //                          e
 //                          r->n
 //                          o
 //                          ofse
 //                          g
 //                          ment
 //                          s
 //                          +1) 
 //                          { 
 //                           str
 //                           c
 //                           py(e
 //                           r
 //                           ror,
 //                           "The
 //                           r
 //                           e ar
 //                           e
 //                            mor
 //                            e
 //                             seg
 //                             m
 //                             ents
 //                             in t
 //                             h
 //                             e da
 //                             t
 //                             afil
 //                             e
 //                              tha
 //                              n
 //                               spe
 //                               c
 //                               ifie
 //                               d
 //                                in 
 //                                t
 //                                he h
 //                                e
 //                                ader
 //                                sect
 //                                i
 //                                on 
 //                                of
 //                                the 
 //                                f
 //                                ile.
 //                                \
 //                                nCan
 //                                n
 //                                ot o
 //                                p
 //                                en d
 //                                a
 //                                ta f
 //                                i
 //                                le")
 //                                ; 
 //                                 ret
 //                                 u
 //                                 rn f
 //                                 a
 //                                 lse; 
 //                                 } 
 //                                 e
 //                                 lse 
 //                                  ret
 //                                  u
 //                                  rn t
 //                                  r
 //                                  ue; 
 //                                  } 
 //                                  bo
 //                                  o
 //                                  l HT
 //                                  R
 //                                  Form
 //                                  a
 //                                  t::E
 //                                  x
 //                                  port
 //                                  D
 //                                  ata(
 //                                  c
 //                                  onst
 //                                  char
 //                                  *
 //                                   fil
 //                                   e
 //                                   name
 //                                   ) 
 //                                   { 
 //                                   s
 //                                   trcp
 //                                   y
 //                                   (err
 //                                   o
 //                                   r, "
 //                                   D
 //                                   ata 
 //                                   E
 //                                   xpor
 //                                   t
 //                                    for
 //                                    HTR 
 //                                    f
 //                                    orma
 //                                    t
 //                                     has
 //                                     not 
 //                                     b
 //                                     een 
 //                                     i
 //                                     mple
 //                                     m
 //                                     ente
 //                                     d
 //                                     "); 
 //                                     r
 //                                     etur
 //                                     n
 //                                      fal
 //                                      s
 //                                      e; 
 //                                      } 
 //
