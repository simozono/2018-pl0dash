JMP 57
PUSH FP
LOAD FP,SP
PUSHUP
PUSHUP
LOAD A,1
PUSH A
POP A
STORE A,#(FP-1)
LOAD A,1
PUSH A
POP A
STORE A,#(FP-2)
LOAD A,#(FP-1)
PUSH A
LOAD A,#(FP+2)
PUSH A
POP B
POP A
CMPLE
PUSH C
POP C
JPC 45
LOAD A,#(FP-2)
PUSH A
LOAD A,#(FP+3)
PUSH A
POP B
POP A
MULTI
PUSH C
POP A
STORE A,#(FP-2)
LOAD A,#(FP-1)
PUSH A
LOAD A,1
PUSH A
POP B
POP A
PLUS
PUSH C
POP A
STORE A,#(FP-1)
JMP 14
LOAD A,#(FP-2)
PUSH A
POP C
LOAD SP,FP
POP FP
RET 2
LOAD A,0
PUSH A
POP C
LOAD SP,FP
POP FP
RET 2
LOAD A,1
PUSH A
POP A
STORE A,#(800)
LOAD A,#(800)
PUSH A
LOAD A,5
PUSH A
POP B
POP A
CMPLE
PUSH C
POP C
JPC 91
LOAD A,#(800)
PUSH A
LOAD A,#(800)
PUSH A
CALL 2
PUSH C
POP A
PRINT A
PRINTLN
LOAD A,#(800)
PUSH A
LOAD A,1
PUSH A
POP B
POP A
PLUS
PUSH C
POP A
STORE A,#(800)
JMP 61
END
