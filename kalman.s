.syntax unified
.align 16
.section .text, "x"
.global kalman

//R0 = pointer to kalman_state {q,r,x,p,k} (offsets 0,4,8,12,16)
//S0 = measurement
kalman:	
	VLDR S1, [R0] //q
	VLDR S2, [R0, #4] //r
	VLDR S3, [R0, #8] //x
	VLDR S4, [R0, #12] //p
	
	VADD.F32 S4, S4, S1 //p=p+q
	
	VADD.F32 S6, S4, S2 //p+r
	VDIV.F32 S5, S4, S6 //k=p/(p+r)
	
	VSUB.F32 S7, S0, S3 //S7 = measurement-x
	VMLA.F32 S3, S5, S7 //x=x+k*(measurement-x)
	
	VMOV.F32 S8, #1.0
	VSUB S7, S8, S5 //S7 = 1-k
	VMUL S4, S7, S4 //p=(1-k) * p
	
	VSTR S3, [R0, #8] //store x
	VSTR S4, [R0, #12] //store p
	VSTR S5, [R0, #16] //store k
	BX LR