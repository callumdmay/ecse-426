	AREA lab1, CODE, READONLY

	EXPORT IIR_asm
;	R1 = input array pointer
;	R2 = Output array pointer
;	R3 = Length integer
;	R4 = Coeffecient array pointer
IIR_asm
	
	PUSH 	{R5, R4, LR}		;
	MOV 	R4, #0		; value of "n"
	MOV		R5, #0		;
	VLDR	S1, [R4]	; coeffiecient register
	VLDR	S2, [R4]	; value of "x[n]"
	VLDR	S3, [R4]	; value of "x[n-1]"
	VLDR 	S4, [R4]	; value of "x[n-2]"e
	VLDR	S5, [R4]	; value of "y[n-1]"
	VLDR	S6, [R4]	; value of "y[n-2]"
	VLDR	S7, [R4]	; value of y[n]
	
	CMP		R4, #0	;
	BEQ		Out	;
	
Loop								;
	; For index n
	VLDR 	S1, [R3]				; load b0
	LDR	R5, [R0, R4, lsl #2]		; set register 5 address R0+4*R4
	VMOV 	S2, R5					; load x[n]
	VMUL.F32 	S7, S1, S2			; y = b0*x[n] 
	
	
	;for index n-1
	VLDR	S1, [R3, #4]	; load b1
	VMLA.F32		S7, S1, S3	; y = y + b1*x[n-1]
	VLDR	S1, [R3, #12]	; load a1
	VMLA.F32		S7, S1, S5		; y = y +a1*y[n-1]
		
	;for index n-2
	VLDR	S1, [R3, #8]	; load b2
	VMLA.F32	S7, S1, S4	; y = y + b1*x[n-1]
	VLDR	S1, [R3, #16]	; load a2
	VMLA.F32	S7, S1, S6		; y = y +a1*y[n-1]

	; store and load values for next itteration
Out								;
	VMOV.F32 S6, S5				; y[n-2] <- y[n-1]
	VMOV.F32 S5, S7				; y[n-1] <- y[n]
	VSTR S7, [R1]				; store result in index "n" of output array
	ADD R1, R1, #4				; increase output array address by 4 bits (next index)
	VMOV.F32 S4, S3				; x[n-2]<- x[n-1]
	VMOV.F32 S3, S2				; x[n-1] <- x[n]
	ADD  R4, R4, #1				; increase n by 1
	CMP  R4, R2					; check if index number n is equal to input array length 
	BNE Loop					; branch to loop if not equal
	
	POP  {R5, R4, LR}					;
	END