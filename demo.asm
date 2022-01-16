LOD R1,R15,R15
WRD 8192
LOD R2,R15,R15
WRD 72//0x48 //72 //H
STO R2,R2,R1 // mem[8192]=72 trebalo bi se ispisati H na ekranu
// Upisivanje vrijednosti na disk kontroler
LOD R3, R15, R15
WRD 65534 //0xFFFE // Ucitavanje pozicije disk kontrolera za izbor komande u R3
LOD R4, R15, R15
WRD 65533 //0xFFFD // Ucitavanje pozicije disk kontrolera za izbor sektora u R4
LOD R5, R15, R15
WRD 65532 //0xFFFC // Ucitavanje pozicije disk kontrolera za izbor prenos podataka u R5
LOD R6, R15, R15
WRD 65 //'A' // Ucitavanje podataka za upis na disk u R6
LOD R7, R15, R15
WRD 2 // 2  write za disk u R7
LOD R8, R15, R15
WRD 5 // 5 broj sektora za disk u R8
STO R8,R8,R4 // mem[R4] = R8= 5 odabir sektora
STO R7,R7,R3 // mem[R3] = R7= 2 komanda write
STO R6,R6,R5 // mem[R5] = R6 upis na disk ono što je u R6=65
// kraj
LOD R15,R15,R15
WRD 0
