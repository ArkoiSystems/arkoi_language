fun main() @u32:
L0:
  %01 @u32 = alloca
  arg @u32 7
  $05 @u32 = call factorial_recursive, 1
  arg @u32 7
  $09 @u32 = call factorial_while, 1
  $10 @u32 = sub @u32 $05, $09
  %01 @u32 = store $10
  $11 @u32 = load %01
  ret $11

fun factorial_recursive(n @u32) @u32:
L2:
  %01 @u32 = alloca
  %02 @u32 = alloca
  %02 @u32 = store n
  $03 @u32 = load %02
  $06 @bool = equ @u32 $03, 1
  if $06 then L4 else L5
L5:
  $09 @u32 = load %02
  $10 @u32 = load %02
  $13 @u32 = sub @u32 $10, 1
  arg @u32 $13
  $15 @u32 = call factorial_recursive, 1
  $16 @u32 = mul @u32 $09, $15
  %01 @u32 = store $16
  goto L3
L3:
  $01 @u32 = phi [  ]
  $17 @u32 = load %01
  ret $17
L4:
  %01 @u32 = store 1
  goto L3

fun factorial_while(n @u32) @u32:
L7:
  %01 @u32 = alloca
  %02 @u32 = alloca
  %02 @u32 = store n
  %03 @u32 = alloca
  %03 @u32 = store 1
L9:
  $03 @u32 = phi [  ]
  $02 @u32 = phi [  ]
  $06 @u32 = load %02
  $09 @bool = neq @u32 $06, 0
  if $09 then L10 else L11
L11:
  $17 @u32 = load %03
  %01 @u32 = store $17
  $18 @u32 = load %01
  ret $18
L10:
  $10 @u32 = load %03
  $11 @u32 = load %02
  $12 @u32 = mul @u32 $10, $11
  %03 @u32 = store $12
  $13 @u32 = load %02
  $16 @u32 = sub @u32 $13, 1
  %02 @u32 = store $16
  goto L9

