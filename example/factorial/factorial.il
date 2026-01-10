fun main() @u32:
L0:
  arg @u32 7
  $05.0 @u32 = call factorial_recursive, 1
  arg @u32 7
  $09.0 @u32 = call factorial_while, 1
  $10.0 @u32 = sub @u32 $05.0, $09.0
  ret $10.0

fun factorial_recursive($n.0 @u32) @u32:
L2:
  $02.0 @u32 = $n.0
  $06.0 @bool = equ @u32 $n.0, 1
  if $06.0 then L4 else L5
L5:
  $13.0 @u32 = sub @u32 $02.0, 1
  arg @u32 $13.0
  $15.0 @u32 = call factorial_recursive, 1
  $16.0 @u32 = mul @u32 $02.0, $15.0
  $01.1 @u32 = $16.0
  goto L3
L3:
  $01.0 @u32 = phi [ L4: $01.2, L5: $01.1 ]
  ret $01.0
L4:
  $01.2 @u32 = 1
  goto L3

fun factorial_while($n.0 @u32) @u32:
L7:
  $02.0 @u32 = $n.0
  $03.0 @u32 = 1
L9:
  $03.1 @u32 = phi [ L10: $03.2, L7: $03.0 ]
  $02.1 @u32 = phi [ L10: $02.2, L7: $02.0 ]
  $09.0 @bool = neq @u32 $02.1, 0
  if $09.0 then L10 else L11
L11:
  ret $03.1
L10:
  $12.0 @u32 = mul @u32 $03.1, $02.1
  $03.2 @u32 = $12.0
  $16.0 @u32 = sub @u32 $02.1, 1
  $02.2 @u32 = $16.0
  goto L9

