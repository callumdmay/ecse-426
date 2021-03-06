	AREA lab1, CODE, READONLY

	EXPORT IIR_asm
;	R0 = input array pointer
;	R1 = Output array pointer
;	R2 = Length integer
;	R3 = Coeffecient array pointer
; 	R4 = n

ENTRY

IIR_asm
	
	PUSH 		{R5, R4, LR}		; push onto stack
	MOV 			R4, #0				; value of n
    VMOV    	S3, R4  				; value of x[n-1]
    VMOV     	S4, R4    			; value of x[n-2]
    VMOV    	S5, R4  	  			; value of y[n-1]
    VMOV    	S6, R4   		 	; value of y[n-2]
	
Filter								
	; For index n
	VLDR 			S1, [R3]					; load b0
	LDR				R5, [R0, R4, lsl #2]	; set register 5 address R0+4*R4
	VMOV 			S2, R5					; load x[n]
	VMUL.F32 		S7, S1, S2				; y = b0*x[n] 
	
	
	;for index n-1
	VLDR				S1, [R3, #4]		; load b1
	VMLA.F32		S7, S1, S3			; y = y + b1*x[n-1]
	VLDR.32			S1, [R3, #12]		; load a1
	VMLA.F32		S7, S1, S5			; y = y +a1*y[n-1]
		
	;for index n-2
	VLDR				S1, [R3, #8]		; load b2
	VMLA.F32		S7, S1, S4			; y = y + b1*x[n-1]
	VLDR				S1, [R3, #16]		; load a2
	VMLA.F32		S7, S1, S6			; y = y +a1*y[n-1]

;store output and load values for next itteration
Store								
	VMOV.F32 		S6, S5				; y[n-2] <- y[n-1]
	VMOV.F32 		S5, S7				; y[n-1] <- y[n]
	VSTR 			S7, [R1], #4		; store result in index "n" of output array and increase output array address by 4 bits (next index)
	VMOV.F32 		S4, S3				; x[n-2]<- x[n-1]
	VMOV.F32		S3, S2				; x[n-1] <- x[n]
	ADD  				R4, R4, #1			; increase n by 1
	CMP  			R4, R2				; check if index number n is equal to input array length 
	BNE 				Filter					; branch to loop if not equal

Exit									
	POP  				{R5, R4, LR}		; Restore register				
	END
