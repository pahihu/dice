*        TTL       FAST FLOATING POINT ASCII ROUND ROUTINE (FFPARND)
****************************************
* (C) COPYRIGHT 1981 BY MOTOROLA INC.  *
****************************************

***********************************************
*                  FFPARND                    *
*           ASCII ROUND SUBROUTINE            *
*                                             *
*  THIS ROUTINE IS NORMALLY CALLED AFTER THE  *
*  'FFPFPA' FLOAT TO ASCII ROUTINE AND ACTS   *
*  UPON ITS RESULTS.                          *
*                                             *
*  INPUT:  D6 - ROUNDING MAGNITUDE IN BINARY  *
*               AS EXPLAINED BELOW.           *
*          D7 - BINARY REPRESENTATION OF THE  *
*               BASE 10 EXPONENT.             *
*          SP ->  RETURN ADDRESS AND OUTPUT   *
*                 FROM FFPFPA ROUTINE         *
*                                             *
*  OUTPUT: THE ASCII VALUE ON THE STACK IS    *
*          CORRECTLY ROUNDED                  *
*                                             *
*          THE CONDITION CODES ARE UNDEFINED  *
*                                             *
*          ALL REGISTERS TRANSPARENT          *
*                                             *
*     THE ROUNDING PRECISION REPRESENTS THE   *
*     POWER OF TEN TO WHICH THE ROUNDING WILL *
*     OCCUR.  (I.E. A -2 MEANS ROUND THE DIGIT*
*     IN THE HUNDREDTH POSITION FOR RESULTANT *
*     ROUNDING TO TENTHS.)  A POSITIVE VALUE  *
*     INDICATES ROUNDING TO THE LEFT OF THE   *
*     DECIMAL POINT (0 IS UNITS, 1 IS TENS    *
*     E.T.C.)                                 *
*                                             *
*     THE BASE TEN EXPONENT IN BINARY IS D7   *
*     FROM THE 'FFPFPA' ROUTINE OR COMPUTED BY*
*     THE CALLER.                             *
*                                             *
*     THE STACK CONTAINS THE RETURN ADDRESS   *
*     FOLLOWED BY THE ASCII NUMBER AS FROM    *
*     THE 'FFPFPA' ROUTINE.  SEE THE          *
*     DESCRIPTION OF THAT ROUTINE FOR THE     *
*     REQUIRED FORMAT.                        *
*                                             *
*  EXAMPLE:                                   *
*                                             *
*  INPUT PATTERN '+.98765432+01' = 9.8765432  *
*                                             *
*     ROUND +1 IS +.00000000+00 =  0.         *
*     ROUND  0 IS +.10000000+02 = 10.         *
*     ROUND -1 IS +.10000000+02 = 10.         *
*     ROUND -2 IS +.99000000+01 =  9.9        *
*     ROUND -3 IS +.98800000+01 =  9.88       *
*     ROUND -6 IS +.98765400+01 =  9.87654    *
*                                             *
*  NOTES:                                     *
*     1) IF THE ROUNDING DIGIT IS TO THE LEFT *
*        OF THE MOST SIGNIFICANT DIGIT, A ZERO*
*        RESULTS.  IF THE ROUNDING DIGIT IS TO*
*        THE RIGHT OF THE LEAST SIGNIFICANT   *
*        DIGIT, THEN NO ROUNDING OCCURS       *
*     2) ROUNDING IS HANDY FOR ELIMINATING THE*
*        DANGLING '999...' PROBLEM COMMON WITH*
*        FLOAT TO DECIMAL CONVERSIONS.        *
*     3) POSITIONS FROM THE ROUNDED DIGIT AND *
*        TO THE RIGHT ARE SET TO ZEROES.      *
*     4) THE EXPONENT MAY BE AFFECTED.        *
*     5) ROUNDING IS FORCED BY ADDING FIVE.   *
*     6) THE BINARY EXPONENT IN D7 MAY BE     *
*        PRE-BIASED BY THE CALLER TO PROVIDE  *
*        ENHANCED EDITING CONTROL.            *
*     7) THE RETURN ADDRESS IS REMOVED FROM   *
*        THE STACK UPON EXIT.                 *
***********************************************
         ;PAGE
*FFPARND  IDNT      1,1       FFP ASCII ROUND SUBROUTNE

*         OPT       PCS

         XDEF      FFPARND   ;ENTRY POINT


FFPARND  MOVEM.L   D7/A0,-(SP)         ;SAVE WORK ON STACK
         SUB.W     D6,D7               ;COMPUTE ROUNDING DIGIT OFFSET
         BLE.S     FAFZRO              ;BRANCH IF LARGER THAN VALUE
         CMP.W     #8,D7               ;INSURE NOT PAST LAST DIGIT
         BHI       FARTN               ;RETURN IF SO
         LEA       8+4+1(SP,D7),A0     ;POINT TO ROUNDING DIGIT
         CMP.B     #'5',(A0)           ;? MUST ROUNDUP
         BCC.S     FADORND             ;YES, ROUND
         SUBQ.W    #1,D7               ;? ROUND LEADING DIGIT ZERO (D7=1)
         BNE.S     FAZEROL             ;NO, ZERO IT OUT
FAFZRO   LEA       8+4+2(SP),A0        ;FORCE ZEROES ALL THE WAY ACROSS
         MOVE.L    #'E+00',8+4+10(SP)  ;FORCE ZERO EXPONENT
         MOVE.B    #'+',8+4(SP)        ;ZERO IS ALWAYS POSITIVE
         BRA.S     FAZEROL             ;ZERO MANTISSA THEN RETURN

* ROUND UP MUST OCCUR
FADORND  MOVE.L    A0,-(SP)            ;SAVE ZERO START ADDRESS ON STACK
FACARRY  CMP.B     #'.',-(A0)          ;? HIT BEGINNING
         BEQ.S     FASHIFT             ;YES, SHIFT DOWN
         ADDQ.B    #1,(A0)             ;LOOK AT NEXT CHARACTER
         CMP.B     #'9'+1,(A0)         ;? PAST NINE
         BNE.S     FAZERO              ;NO, NOW ZERO THE END
         MOVE.B    #'0',(A0)           ;FORCE ZERO FOR OVERFLOW
         BRA.S     FACARRY             ;LOOP FOR CARRY

* OVERFLOW PAST TOP DIGIT - SHIFT RIGHT AND UP EXPONENT
FASHIFT  ADDQ.L    #1,(SP)             ;ZERO PAD STARTS ONE LOWER NOW
         ADDQ.L    #1,A0               ;BACK TO LEADING DIGIT
         MOVEQ     #$31,D7             ;DEFAULT FIRST DIGIT ASCII ONE
         SWAP.W    D7                  ;INITIALIZE OLD DIGIT
         MOVE.B    (A0),D7             ;PRE-LOAD CURRENT DIGIT
FASHFTR  SWAP.W    D7                  ;TO PREVIOUS DIGIT
         MOVE.B    D7,(A0)+            ;STORE INTO THIS POSITION
         MOVE.B    (A0),D7             ;LOAD UP NEXT DIGIT
         CMP.B     #'E',D7             ;? THE END
         BNE.S     FASHFTR             ;NO, SHIFT ANOTHER TO THE RIGHT

* INCREMENT EXPONENT FOR SHIFT RIGHT
         CMP.B     #'+',1(A0)          ;? POSITIVE EXPONENT
         ADDQ.L    #3,A0               ;POINT TO LEAST EXP DIGIT
         BNE.S     FANGEXP             ;BRANCH NEGATIVE EXPONENT
         ADDQ.B    #1,(A0)             ;ADD ONE TO EXPONENT
         CMP.B     #'9'+1,(A0)         ;? OVERFLOW PAST NINE
         BNE.S     FAZERO              ;NO, NOW ZERO

                 SUB.B     #10,(A0)                        ;ADJUST BACK                                                  V1.4/V1.5

         ADDQ.B    #1,-(A0)            ;CARRY TO NEXT DIGIT
         BRA.S     FAZERO              ;AND NOW ZERO END
FANGEXP  CMP.W     #'01',-1(A0)        ;? GOING FROM 1 TO +0
         BNE.S     FANGOK              ;BRANCH IF NO
         MOVE.B    #'+',-2(A0)         ;CHANGE MINUS TO PLUS
FANGOK   SUBQ.B    #1,(A0)             ;SUBTRACT ON FROM EXPONENT
         CMP.B     #'0'-1,(A0)         ;? UNDERFLOW BELOW ZERO
         BNE.S     FAZERO              ;NO, ZERO REMAINDER

                 ADD.B     #10,(A0)                        ;ADJUST BACK                                                  V1.4

         SUBQ.B    #1,-(A0)            ;BORROW FROM NEXT DIGIT

* ZERO THE DIGITS PAST PRECISION REQUIRED
FAZERO   MOVE.L    (SP)+,A0            ;RELOAD SAVED PRECISION
FAZEROL  CMP.B     #'E',(A0)           ;? AT END
         BEQ.S     FARTN               ;BRANCH IF SO
         MOVE.B    #'0',(A0)+          ;ZERO NEXT DIGIT
         BRA.S     FAZEROL             ;AND TEST AGAIN

* RETURN TO THE CALLER
FARTN    MOVEM.L   (SP)+,D7/A0         ;RESTORE REGISTERS
         RTS                           ;RETURN

         END
