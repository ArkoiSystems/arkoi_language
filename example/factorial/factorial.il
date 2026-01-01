fun main() @u32:
L0:
  %01 @u32 = alloca
  arg @u32 7
  $05 @u32 = call factorial_recursive, 1
  arg @u32 7
  $09 @u32 = call factorial_while, 1
  $10 @u32 = sub @u32 $05, $09
  store @u32 $10, %01
  $11 @u32 = load %01
  ret $11

fun factorial_recursive(n @u32) @u32:
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
  $15 @u32 = call factorial_recursive, 1
  $16 @u32 = mul @u32 $09, $15
  store @u32 $16, %01
  goto L3
L4:
  store @u32 1, %01
  goto L3
L3:
  $17 @u32 = load %01
  ret $17

fun factorial_while(n @u32) @u32:
L7:
  %01 @u32 = alloca
  %02 @u32 = alloca
  store @u32 n, %02
  %03 @u32 = alloca
  store @u32 1, %03
L9:
  $06 @u32 = load %02
  $09 @bool = gth @u32 $06, 1
  if $09 then L10 else L11
L10:
  $10 @u32 = load %03
  $11 @u32 = load %02
  $12 @u32 = mul @u32 $10, $11
  store @u32 $12, %03
  $13 @u32 = load %02
  $16 @u32 = sub @u32 $13, 1
  store @u32 $16, %02
  goto L9
L11:
  $17 @u32 = load %03
  store @u32 $17, %01
  $18 @u32 = load %01
  ret $18

