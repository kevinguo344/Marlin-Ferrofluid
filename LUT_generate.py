import math

FP_ONE = 1 << 16

lut = []
for i in range(256):
    r = i / 255.0
    val = math.sqrt(1.0 + r*r)
    lut.append(int(round(val * FP_ONE)))

for i, v in enumerate(lut):
    print(f"{v},", end="\n" if (i+1)%8==0 else " ")