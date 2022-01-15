LOD R1,R15,R15
WRD 8192 //U R1 se stavlja 8192 - pozicija video memorije
LOD R2, R15, R15
WRD 72
STO R2, R1 // Upisivanje R2 na poziciju u memoriji mem[R1] // prikaz slova h na ekranu
LOD R3, R15, R15
WRD 0xFF02 // učitavanje bita, ako je pritisnut enter
MIF R15,R3,R3 // čeka se pritisak entera
// Upisivanje vrijednosti na disk kontroler
LOD R4, R15, R15
WRD 0xFFFE // Ucitavanje pozicije disk kontrolera u R4
LOD R5, R15, R15
WRD 2 // Upisivanje vrijednosti 2 u R5
STO R5,R5,R4 // mem[R4] = R5
// ???????
