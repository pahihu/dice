*     TTL     FAST FLOATING POINT POWER OF TEN TABLE (FFP10TBL)
************************************
* (C) COPYRIGHT 1980 MOTORLA INC.  *
************************************

*****************************************
*         POWER OF TEN TABLE            *
*                                       *
*  EACH ENTRY CORRESPONDS TO A FLOATING *
*  POINT POWER OF TEN WITH A 16 BIT     *
*  EXPONENT AND 32 BIT MANTISSA.        *
*  THIS TABLE IS USED TO INSURE         *
*  PRECISE CONVERSIONS TO AND FROM      *
*  FLOATING POINT AND EXTERNAL FORMATS. *
*  THS IS USED IN ROUTINES "FFPDBF" AND *
*  "FFPFASC".                           *
*                                       *
*          CODE SIZE: 288 BYTES         *
*                                       *
*****************************************
         ;PAGE
*FFP10TBL IDNT    1,1  ;FFP POWER OF TEN TABLE

         XDEF      FFP10TBL    ;ENTRY POINT
*        XREF      FFPCPYRT    ;COPYRIGHT STUB


         DC.W      64        ;10**19
         DC.L      $8AC72305
         DC.W      60        ;10**18
         DC.L      $DE0B6B3A
         DC.W      57        ;10**17
         DC.L      $16345785<<3+7
         DC.W      54        ;10**16
         DC.L      $2386F26F<<2+3
         DC.W      50        ;10**15
         DC.L      $38D7EA4C<<2+2
         DC.W      47        ;10**14
         DC.L      $5AF3107A<<1+1
         DC.W      44        ;10**13
         DC.L      $9184E72A
         DC.W      40        ;10**12
         DC.L      $E8D4A510
         DC.W      37        ;10**11
         DC.L      $174876E8<<3
         DC.W      34        ;10**10
         DC.L      $2540BE40<<2
         DC.W      30        ;10**9
         DC.L      1000000000<<2
         DC.W      27        ;10**8
         DC.L      100000000<<5
         DC.W      24        ;10**7
         DC.L      10000000<<8
         DC.W      20        ;10**6
         DC.L      1000000<<12
         DC.W      17        ;10**5
         DC.L      100000<<15
         DC.W      14        ;10**4
         DC.L      10000<<18
         DC.W      10        ;10**3
         DC.L      1000<<22
         DC.W      7         ;10**2
         DC.L      100<<25
         DC.W      4         ;10**1
         DC.L      10<<28
FFP10TBL DC.W      1         ;10**0
         DC.L      1<<31
         DC.W      -3        ;10**-1
         DC.L      $CCCCCCCD
         DC.W      -6        ;10**-2
         DC.L      $A3D70A3D
         DC.W      -9        ;10**-3
         DC.L      $83126E98
         DC.W      -13       ;10**-4
         DC.L      $D1B71759
         DC.W      -16       ;10**-5
         DC.L      $A7C5AC47
         DC.W      -19       ;10**-6
         DC.L      $8637BD06
         DC.W      -23       ;10**-7
         DC.L      $D6BF94D6
         DC.W      -26       ;10**-8
         DC.L      $ABCC7712
         DC.W      -29       ;10**-9
         DC.L      $89705F41
         DC.W      -33       ;10**-10
         DC.L      $DBE6FECF
         DC.W      -36       ;10**-11
         DC.L      $AFEBFF0C
         DC.W      -39       ;10**-12
         DC.L      $8CBCCC09
         DC.W      -43       ;10**-13
         DC.L      $E12E1342
         DC.W      -46       ;10**-14
         DC.L      $B424DC35
         DC.W      -49       ;10**-15
         DC.L      $901D7CF7
         DC.W      -53       ;10**-16
         DC.L      $E69594BF
         DC.W      -56       ;10**-17
         DC.L      $B877AA32
         DC.W      -59       ;10**-18
         DC.L      $9392EE8F
         DC.W      -63       ;10**-19
         DC.L      $EC1E4A7E
         DC.W      -66       ;10**-20
         DC.L      $BCE50865
         DC.W      -69       ;10**-21
         DC.L      $971DA050
         DC.W      -73       ;10**-22
         DC.L      $F1C90081
         DC.W      -76       ;10**-23
         DC.L      $C16D9A01
         DC.W      -79       ;10**-24
         DC.L      $9ABE14CD
         DC.W      -83       ;10**-25
         DC.L      $F79687AE
         DC.W      -86       ;10**-26
         DC.L      $C6120625
         DC.W      -89       ;10**-27
         DC.L      $9E74D1B8
         DC.W      -93       ;10**-28
         DC.L      $FD87B5F3
 
         END
