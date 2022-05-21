*     TTL     FAST FLOATING POINT DUAL-BINARY FLOAT (FFPDBF)
************************************
* (C) COPYRIGHT 1980 MOTOROLA INC. *
************************************

**********************************************************
*                                                        *
*          FAST FLOATING POINT DUAL-BINARY TO FLOAT      *
*                                                        *
*      INPUT:  D6 BIT #16 - REPRESENTS SIGN (0=POSITIVE) *
*                                           (1=NEGATIVE) *
*              D6.W - REPRESENTS BASE TEN EXPONENT       *
*                     CONSIDERING D7 A BINARY INTEGER    *
*              D7 -   BINARY INTEGER MANTISSA            *
*                                                        *
*      OUTPUT: D7 - FAST FLOATING POINT EQUIVALENT       *
*                                                        *
*      CONDITION CODES:                                  *
*                N - SET IF RESULT IS NEGATIVE           *
*                Z - SET IF RESULT IS ZERO               *
*                V - SET IF RESULT OVERFLOWED            *
*                C - CLEARED                             *
*                X - UNDEFINED                           *
*                                                        *
*      REGISTERS D3 THRU D6 DESTROYED                    *
*                                                        *
*      CODE SIZE: 164 BYTES     STACK WORK AREA: 4 BYTES *
*                                                        *
*                                                        *
*      PRECISION:                                        *
*                                                        *
*          THIS CONVERSION RESULTS IN A 24 BIT PRECISION *
*          WITH GUARANTEED ERROR LESS THAN OR EQUAL TO   *
*          ONE-HALF LEAST SIGNIFICANT BIT.               *
*                                                        *
*                                                        *
*      NOTES:                                            *
*          1) THE INPUT FORMATS HAVE BEEN DESIGNED FOR   *
*             EASE OF PARSING TEXT FOR CONVERSION TO     *
*             FLOATING POINT.  SEE FFPASF FOR COMMENTS   *
*             DESCRIBING THE METHOD FOR SETUP TO THIS    *
*             ROUTINE.                                   *
*          2) UNDERFLOWS RETURN A ZERO WITHOUT ANY       *
*             INDICATORS SET.                            *
*          3) OVERFLOWS WILL RETURN THE MAXIMUM VALUE    *
*             POSSIBLE WITH PROPER SIGN AND THE 'V' BIT  *
*             SET IN THE CCR REGISTER.                   *
*                                                        *
**********************************************************
         ;PAGE
*FFPDBF   IDNT      1,1                 FFP DUAL-BINARY TO FLOAT

*         OPT       PCS

         XDEF      FFPDBF              ;ENTRY POINT
         XREF      FFP10TBL            ;POWER OF TEN TABLE


* NORMALIZE THE INPUT BINARY MANTISSA
FFPDBF   MOVEQ     #32,D5    ;SETUP BASE 2 EXPONENT MAX
         TST.L     D7        ;? TEST FOR ZERO
         BEQ       FPDRTN    ;RETURN, NO CONVERSION NEEDED
         BMI.S     FPDINM    ;BRANCH INPUT ALREADY NORMALIZED
         MOVEQ     #31,D5    ;PREPARE FOR NORMALIZE LOOP
FPDNMI   ADD.L     D7,D7     ;SHIFT UP BY ONE
         DBMI      D5,FPDNMI ;DECREMENT AND LOOP IF NOT YET

* INSURE INPUT 10 POWER INDEX NOT WAY OFF BASE
FPDINM   CMP.W     #18,D6    ;? WAY TOO LARGE
         BGT.S     FPDOVF    ;BRANCH OVERFLOW
         CMP.W     #-28,D6   ;? WAY TOO SMALL
         BLT.S     FPDRT0    ;RETURN ZERO IF UNDERFLOW
         MOVE.W    D6,D4     ;COPY 10 POWER INDEX
         NEG.W     D4        ;INVERT TO GO PROPER DIRECTION
         MULS.W    #6,D4     ;TIMES FOUR FOR INDEX
         MOVE.L    A0,-(SP)  ;SAVE WORK ADDRESS REGISTER
         LEA       FFP10TBL,A0 ;LOAD TABLE ADDRESS
         ADD.W     0(A0,D4.W),D5 ;ADD EXPONENTS FOR MULTIPLY
         MOVE.W    D5,D6     ;SAVE RESULT EXPONENT IN D6.W

* NOW PERFORM 32 BIT MULTIPLY OF INPUT WITH POWER OF TEN TABLE
         MOVE.L    2(A0,D4.W),D3 ;LOAD TABLE MANTISSA VALUE
         MOVE.L    (SP),A0   ;RESTORE WORK REGISTER
         MOVE.L    D3,(SP)   ;NOW SAVE TABLE MANTISSA ON STACK
         MOVE.W    D7,D5     ;COPY INPUT VALUE
         MULU.W    D3,D5     ;TABLELOW X INPUTLOW
         CLR.W     D5        ;LOW END NO LONGER TAKES AFFECT
         SWAP.W    D5        ;SAVE INTERMEDIATE SUM
         MOVEQ     #0,D4     ;CREATE A ZERO FOR DOUBLE PRECISION
         SWAP.W    D3        ;TO HIGH TABLE WORD
         MULU.W    D7,D3     ;INPUTLOW X TABLEHIGH
         ADD.L     D3,D5     ;ADD ANOTHER PARTIAL SUM
         ADDX.B    D4,D4     ;CREATE CARRY IF ANY
         SWAP.W    D7        ;NOW TO INPUT HIGH
         MOVE.W    D7,D3     ;COPY TO WORK REGISTER
         MULU.W    2(SP),D3  ;TABLELOW X INPUTHIGH
         ADD.L     D3,D5     ;ADD ANOTHER PARTIAL
         BCC.S     FPDNOC    ;BRANCH NO CARRY
         ADDQ.B    #1,D4     ;ADD ANOTHER CARRY
FPDNOC   MOVE.W    D4,D5     ;CONCAT HIGH WORK WITH LOW
         SWAP.W    D5        ;AND CORRECT POSITIONS
         MULU.W    (SP),D7   ;TABLEHIGH X INPUTHIGH
         LEA       4(SP),SP  ;CLEAN UP STACK
         ADD.L     D5,D7     ;FINAL PARTIAL PRODUCT
         BMI.S     FPDNON    ;BRANCH IF NO NEED TO NORMALIZE
         ADD.L     D7,D7     ;NORMALIZE
         SUBQ.W    #1,D6     ;ADJUST EXPONENT
FPDNON   ADD.L     #$80,D7   ;ROUND RESULT TO 24 BITS
         BCC.S     FPDROK    ;BRANCH ROUND DID NOT OVERFLOW
         ROXR.L    #1,D7     ;ADJUST BACK
         ADDQ.W    #1,D6     ;AND INCREMENT EXPONENT
FPDROK   MOVEQ     #9,D3     ;PREPARE TO FINALIZE EXPONENT TO 7 BITS
         MOVE.W    D6,D4     ;SAVE SIGN OF EXPONENT
         ASL.W     D3,D6     ;FORCE 7 BIT PRECISION
         BVS.S     FPDXOV    ;BRANCH EXPONENT OVERFLOW
         EORI.W    #$8000,D6 ;EXPONENT BACK FROM 2'S-COMPLEMENT
         LSR.L     D3,D6     ;PLACE INTO LOW BYTE WITH SIGN
         MOVE.B    D6,D7     ;INSERT INTO RESULT
         BEQ.S     FPDRT0    ;RETURN ZERO IF EXPONENT ZERO
FPDRTN   RTS                 ;RETURN

* RETURN ZERO FOR UNDERFLOW
FPDRT0   MOVEQ     #0,D7     ;LOAD ZERO
         RTS                 ;RETURN

* EXPONENT OVERFLOW/UNDERFLOW
FPDXOV   TST.W     D4        ;TEST ORIGINAL SIGN
         BMI.S     FPDRT0    ;BRANCH UNDERFLOW TO RETURN ZERO
FPDOVF   MOVEQ     #-1,D7    ;CREATE ALL ONES
         SWAP.W    D6        ;SIGN TO LOW BIT
         ROXR.B    #1,D6     ;SIGN TO X BIT
         ROXR.B    #1,D7     ;SIGN INTO HIGHEST POSSIBLE RESULT
         TST.B     D7        ;CLEAR CARRY BIT
*        ORI       #$02,CCR  ;SET OVERFLOW ON
         DC.L      $003C0002 ;****ASSEMBLER ERROR****
         RTS                 ;RETURN TO CALLER WITH OVERFLOW


         END
