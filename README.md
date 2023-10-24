# Lab1-AES MixColumns
This repository about implementation AES mixcolumns
The MixColumns operation performed by the Rijndael cipher, along with the ShiftRows step, is the primary source of diffusion in Rijndael. Each column is treated as a four-term polynomial b ( x ) = b 3 x 3 + b 2 x 2 + b 1 x + b 0 {\displaystyle b(x)=b_{3}x^{3}+b_{2}x^{2}+b_{1}x+b_{0}} which are elements within the field GF ⁡ ( 2 8 ) {\displaystyle \operatorname {GF} (2^{8})}. The coefficients of the polynomials are elements within the prime sub-field GF ⁡ ( 2 ) {\displaystyle \operatorname {GF} (2)}.
Each column is multiplied with a fixed polynomial a ( x ) = 3 x 3 + x 2 + x + 2 {\displaystyle a(x)=3x^{3}+x^{2}+x+2} modulo x 4 + 1 x^{4}+1; the inverse of this polynomial is a − 1 ( x ) = 11 x 3 + 13 x 2 + 9 x + 14 {\displaystyle a^{-1}(x)=11x^{3}+13x^{2}+9x+14}.
All information represanting MixColumns algorithm read here https://en.wikipedia.org/wiki/Rijndael_MixColumns
