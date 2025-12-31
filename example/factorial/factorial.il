fun main() @u32:
L0:
  %01 @u32 = alloca
  arg @u32 7
  $05 @u32 = call factorial, 1
  store @u32 $05, %01
  $06 @u32 = load %01
  ret $06

fun factorial(n @u32) @u32:
L2:
  %01 @u32 = alloca
  %02 @u32 = alloca
  store @u32 n, %02
  $03 @u32 = load %02
  $06 @bool = lth @u32 $03, 2
  if $06 then L4 else L5
L5:
  $09 @u32 = load %02
  $10 @u32 = load %02
  $13 @u32 = sub @u32 $10, 1
  arg @u32 $13
  $15 @u32 = call factorial, 1
  $16 @u32 = mul @u32 $09, $15
  store @u32 $16, %01
  goto L3
L4:
  store @u32 1, %01
  goto L3
L3:
  $17 @u32 = load %01
  ret $17

