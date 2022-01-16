ORG $8193
LOD R1,R15,R15
WRD 72
LOD R3, R3, R15
WRD 0xFF02
MIF R15,R3,R3
// Upisivanje vrijednosti na disk kontroler
LOD R4, R4, R15
WRD 0xFFFE // Ucitavanje pozicije disk kontrolera u R4
LOD R5,R15,R15
WRD 2
STO R5,R5,R4