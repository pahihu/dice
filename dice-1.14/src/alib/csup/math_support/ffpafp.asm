*      TTL     FAST FLOATING POINT ASCII TO FLOAT (FFPAFP)
************************************
* (C) COPYRIGHT 1980 MOTORLA INC.  *
************************************

**********************************************************
*                        FFPAFP                          *
*                    ASCII TO FLOAT                      *
*                                                        *
*      INPUT:  A0 - POINTER TO ASCII STRING OF FORMAT    *
*                   DESCRIBED BELOW                      *
*                                                        *
*      OUTPUT: D7 - FAST FLOATING POINT EQUIVALENT       *
*              A0 - POINTS TO THE CHARACTER WHICH        *
*                   TERMINATED THE SCAN                  *
*                                                        *
*      CONDITION CODES:                                  *
*                N - SET IF RESULT IS NEGATIVE           *
*                Z - SET IF RESULT IS ZERO               *
*                V - SET IF RESULT OVERFLOWED            *
*                C - SET IF INVALID FORMAT DETECTED      *
*                X - UNDEFINED                           *
*                                                        *
*      REGISTERS D3 THRU D6 ARE CHANGED                  *
*                                                        *
*      CODE SIZE: 246 BYTES     STACK WORK: 8 BYTES      *
*                                                        *
*      INPUT FORMAT:                                     *
*                                                        *
*     {SIGN}{DIGITS}{'.'}{DIGITS}{'E'}{SIGN}{DIGITS}     *
*     <*********MANTISSA********><*****EXPONENT****>     *
*                                                        *
*                   SYNTAX RULES                         *
*          BOTH SIGNS ARE OPTIONAL AND ARE '+' OR '-'.   *
*          THE MANTISSA MUST BE PRESENT.                 *
*          THE EXPONENT NEED NOT BE PRESENT.             *
*          THE MANTISSA MAY LEAD WITH A DECIMAL POINT.   *
*          THE MANTISSA NEED NOT HAVE A DECIMAL POINT.   *
*                                                        *
*      EXAMPLES:  ALL OF THESE VALUES REPRESENT THE      *
*                 NUMBER ONE-HUNDRED-TWENTY.             *
*                                                        *
*                       120            .120E3            *
*                       120.          +.120E+03          *
*                      +120.          0.000120E6         *
*                   0000120.00  1200000E-4               *
*                               1200000.00E-0004         *
*                                                        *
*      FLOATING POINT RANGE:                             *
*                                                        *
*          FAST FLOATING POINT SUPPORTS THE VALUE ZERO   *
*          AND NON-ZERO VALUES WITHIN THE FOLLOWING      *
*          BOUNDS -                                      *
*                                                        *
*                   18                             20    *
*    9.22337177 X 10   > +NUMBER >  5.42101070 X 10      *
*                                                        *
*                   18                             -20   *
*   -9.22337177 X 10   > -NUMBER > -2.71050535 X 10      *
*                                                        *
*      PRECISION:                                        *
*                                                        *
*          THIS CONVERSION RESULTS IN A 24 BIT PRECISION *
*          WITH GUARANTEED ERROR LESS THAN OR EQUAL TO   *
*          ONE-HALF LEAST SIGNIFICANT BIT.               *
*                                                        *
*                                                        *
*      NOTES:                                            *
*          1) THIS ROUTINE CALLS THE DUAL-BINARY TO FLOAT*
*             ROUTINE AND CAN BE USED AS AN ILLUSTRATION *
*             OF HOW TO 'FRONT-END' THAT ROUTINE WITH    *
*             A CUSTOMIZED SCANNER.                      *
*          2) UNDERFLOWS RETURN A ZERO WITHOUT ANY       *
*             INDICATORS SET.                            *
*          3) OVERFLOWS WILL RETURN THE MAXIMUM VALUE    *
*             POSSIBLE WITH PROPER SIGN AND THE 'V' BIT  *
*             SET IN THE CCR.                            *
*          4) IF THE 'C' BIT IN THE CCR INDICATES AN     *
*             INVALID PATTERN DETECTED, THEN A0 WILL     *
*             POINT TO THE INVALID CHARACTER.            *
*                                                        *
*      LOGIC SUMMARY:                                    *
*                                                        *
*          A) PROCESS LEADING SIGN                       *
*          B) PROCESS PRE-DECIMALPOINT DIGITS AND        *
*             INCREMENT 10 POWER BIAS FOR EACH           *
*             DIGIT BYPASSED DUE TO 32 BIT OVERFLOW      *
*          C) PROCESS POST-DECIMALPOINT DIGITS           *
*             DECREMENTING THE 10 POWER BIAS FOR EACH    *
*          D) PROCESS THE EXPONENT                       *
*          E) ADD THE 10 POWER BIAS TO THE EXPONENT      *
*          F) CALL 'FFPDBF' ROUTINE TO FINISH CONVERSION *
*                                                        *
*   TIMES: (8 MHZ NO WAIT STATES)                        *
*          374 MICROSECONDS CONVERTING THE STRING        *
*                                                        *
*                                                        *
**********************************************************
         ;PAGE
*FFPAFP   IDNT      1,1      ;FFP ASCII TO FLOAT

*         OPT       PCS

         XDEF      FFPAFP    ;ENTRY POINT
         XREF      FFPDBF


FFPAFP   MOVEQ     #0,D7     ;CLEAR MANTISSA BUILD
         MOVEQ     #0,D6     ;CLEAR SIGN+BASE10 BUILD

* CHECK FOR LEADING SIGN
         BSR       FPANXT    ;OBTAIN NEXT CHARACTER
         BEQ.S     FPANMB    ;BRANCH DIGIT FOUND
         BCS.S     FPANOS    ;BRANCH NO SIGN ENCOUNTERED

* LEADING SIGN ENCOUNTERED
         CMP.B     #'-',D5   ;COMPARE FOR MINUS
         SEQ.B     D6        ;SET ONES IF SO
         SWAP.W    D6        ;SIGN TO HIGH WORD IN D6

* TEST FOR DIGIT OR PERIOD
         BSR       FPANXT    ;OBTAIN NEXT CHARACTER
         BEQ.S     FPANMB    ;BRANCH DIGIT TO BUILD MANTISSA
FPANOS   CMP.B     #'.',D5   ;? LEADING DECIMALPOINT
         BNE.S     FPABAD    ;BRANCH INVALID PATTERN IF NOT

* INSURE AT LEAST ONE DIGIT
         BSR       FPANXT    ;OBTAIN NEXT CHARACTER
         BEQ.S     FPADOF    ;BRANCH IF FRACTION DIGIT

* INVALID PATTERN DETECTED
FPABAD   SUBQ.L    #1,A0     ;POINT TO INVALID CHARACTER
*        ORI       #$01,CCR  ;SET CARRY BIT ON
         DC.L      $003C0001 ;****ASSEMBLER ERROR****
         RTS                 ;RETURN

* PRE-DECIMALPOINT MANTISSA BUILD
FPANXD   BSR       FPANXT    ;NEXT CHARACTER
         BNE.S     FPANOD    ;BRANCH NOT A DIGIT
FPANMB   BSR.S     FPAX10    ;MULTIPLY TIMES TEN
         BCC.S     FPANXD    ;LOOP FOR MORE DIGITS

* PRE-DECIMALPOINT MANTISSA OVERFLOW, COUNT TILL END OR DECIMAL REACHED
FPAMOV   ADDQ.W    #1,D6     ;INCREMENT TEN POWER BY ONE
         BSR.S     FPANXT    ;OBTAIN NEXT PATTERN
         BEQ.S     FPAMOV    ;LOOP UNTIL NON-DIGIT
         CMP.B     #'.',D5   ;? DECIMAL POINT REACHED
         BNE.S     FPATSE    ;NO, NO CHECK FOR EXPONENT

* FLUSH REMAINING FRACTIONAL DIGITS
FPASRD   BSR.S     FPANXT    ;NEXT CHARACTER
         BEQ.S     FPASRD    ;IGNORE IT IF STILL DIGIT
FPATSE   CMP.B     #'E',D5   ;? EXPONENT HERE
         BNE.S     FPACNV    ;NO, FINISHED - GO CONVERT

* NOW PROCESS THE EXPONENT
         BSR.S     FPANXT    ;OBTAIN FIRST DIGIT
         BEQ.S     FPANTE    ;BRANCH GOT IT
         BCS.S     FPABAD    ;BRANCH INVALID FORMAT, NO SIGN OR DIGITS
         ROL.L     #8,D6     ;HIGH BYTE OF D6 INTO LOW
         CMP.B     #'-',D5   ;? MINUS SIGN
         SEQ.B     D6        ;SET ONES OR ZERO
         ROR.L     #8,D6     ;D6 HIGH BYTE IS EXPONENTS SIGN
         BSR.S     FPANXT    ;NOW TO FIRST DIGIT
         BNE.S     FPABAD    ;BRANCH INVALID - DIGIT EXPECTED

* PROCESS EXPONENT'S DIGITS
FPANTE   MOVE.W    D5,D4     ;COPY DIGIT JUST LOADED
FPANXE   BSR.S     FPANXT    ;EXAMINE NEXT CHARACTER
         BNE.S     FPAFNE    ;BRANCH END OF EXPONENT
         MULU.W    #10,D4    ;PREVIOUS VALUE TIMES TEN
         CMP.W     #2000,D4  ;? TOO LARGE
         BHI.S     FPABAD    ;BRANCH EXPONENT WAY OF BASE
         ADD.W     D5,D4     ;ADD LATEST DIGIT
         BRA.S     FPANXE    ;LOOP FOR NEXT CHARACTER

* ADJUST FOR SIGN AND ADD TO ORIGINAL INDEX
FPAFNE   TST.L     D6        ;? WAS EXPONENT NEGATIVE
         BPL.S     FPAADP    ;BRANCH IF SO
         NEG.W     D4        ;CONVERT TO NEGATIVE VALUE
FPAADP   ADD.W     D4,D6     ;FINAL RESULT
FPACNV   SUBQ.L    #1,A0     ;POINT TO TERMINATOR
         JMP       FFPDBF    ;NOW CONVERT TO FLOAT

* PRE-DECIMALPOINT NON-DIGIT ENCOUNTERED
FPANOD   CMP.B     #'.',D5   ;? DECIMAL POINT HERE
         BNE.S     FPATSE    ;NOPE, TRY FOR THE 'E'

* POST-DECIMALPOINT PROCESSING
FPADPN   BSR.S     FPANXT    ;OBTAIN NEXT CHARACTER
         BNE.S     FPATSE    ;NOT A DIGIT, TEST FOR E'
FPADOF   BSR.S     FPAX10    ;TIMES TEN PREVIOUS VALUE
         BCS.S     FPASRD    ;FLUSH IF OVERFLOW NOW
         SUBQ.W    #1,D6     ;ADJUST 10 POWER BIAS
         BRA.S     FPADPN    ;AND TO NEXT CHARACTER

*   *
*   * FPAX10 SUBROUTINE - PROCESS NEXT DIGIT
*   *  OUTPUT: C=0 NO OVERFLOW, C=1 OVERFLOW (D7 UNALTERED)
*   *
FPAX10   MOVE.L    D7,D3     ;COPY VALUE
         LSL.L     #1,D3     ;TIMES TWO
         BCS.S     FPAXRT    ;RETURN IF OVERFLOW
         LSL.L     #1,D3     ;TIMES FOUR
         BCS.S     FPAXRT    ;RETURN IF OVERFLOW
         LSL.L     #1,D3     ;TIMES EIGHT
         BCS.S     FPAXRT    ;RETURN IF OVERFLOW
         ADD.L     D7,D3     ;ADD ONE TO MAKE X 9
         BCS.S     FPAXRT    ;RETURN IF OVERFLOW
         ADD.L     D7,D3     ;ADD ONE TO MAKE X 10
         BCS.S     FPAXRT    ;RETURN IF OVERFLOW
         ADD.L     D5,D3     ;ADD NEW UNITS DIGIT
         BCS.S     FPAXRT    ;RETURN IF OVERFLOW
         MOVE.L    D3,D7     ;UPDATE RESULT
FPAXRT   RTS                 ;RETURN


*
* FPANXT SUBROUTINE - RETURN NEXT INPUT PATTERN
*
*    INPUT:  A0
*
*    OUTPUT:  A0 INCREMENTED BY ONE
*             IF Z=1 THEN DIGIT ENCOUNTERED AND D5.L SET TO BINARY VALUE
*             IF Z=0 THEN D6.B SET TO CHARACTER ENCOUNTERED
*                         AND C=0 IF PLUS OR MINUS SIGN
*                             C=1 IF NOT PLUS OR MINUS SIGN
*

FPANXT   MOVEQ     #0,D5     ;ZERO RETURN REGISTER
         MOVE.B    (A0)+,D5  ;LOAD CHARACTER
         CMP.B     #'+',D5   ;? PLUS SIGN
         BEQ.S     FPASGN    ;BRANCH IF SIGN
         CMP.B     #'-',D5   ;? MINUS SIGN
         BEQ.S     FPASGN    ;BRANCH IF SIGN
         CMP.B     #'0',D5   ;? LOWER THAN A DIGIT
         BCS.S     FPAOTR    ;BRANCH IF NON-SIGNDIGIT
         CMP.B     #'9',D5   ;? HIGHER THAN A DIGIT
         BHI.S     FPAOTR    ;BRANCH IF NON-SIGNDIGIT
* IT IS DIGIT
         AND.B     #$0F,D5   ;TO BINARY
*        MOVE.W    #$0004,CCR ;SET Z=1 AND C=0
         DC.L      $44FC0004 ;***ASSEMBLER ERROR***
         RTS                 ;RETURN
* IT IS SIGN
FPASGN   EQU       *
*        MOVE.W    #$0000,CCR ;SET Z=0 AND C=0
         DC.L      $44FC0000 ;***ASSEMBLER ERROR***
         RTS                 ;RETURN
* IT IS NEITHER SIGN NOR DIGIT
FPAOTR   EQU       *
*        MOVE.W    #$0001,CCR ;SET Z=0 AND C=1
         DC.L      $44FC0001 ;***ASSEMBLER ERROR***
         RTS                 ;RETURN

         END
