LOD R1,R15,R15
WRD 8192
LOD R3,R15,R15
WRD 32
MUL R3,R3,R3
MUL R3,R3,R3
LOD R5,R15,R15
WRD 32
MUL R5,R5,R5
MUL R5,R5,R5
LOD R6,R15,R15
WRD 14
SUB R5,R5,R6
ADD R3,R3,R5 ; R3 = 65522 Keyboard address
LOD R1,R15,R15
WRD 32
LOD R2,R15,R15
WRD 2
MUL R1,R1,R1
MUL R1,R1,R2 ; R1 = 0000 1000 0000 0000 - bitmask for key 'A'
LOD R14,R15,R15
WRD 18 ; Set loop address
MAJ R15,R15,R14