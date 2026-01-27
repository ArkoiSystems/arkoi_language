fun main() @u32:
L0:
  arg @u32 20
  $05.0 @u32 = call fib, 1
  ret $05.0

fun fib($n.0 @u32) @u32:
L2:
  $02.0 @u32 = $n.0
  $06.0 @bool = loe @u32 $n.0, 1
  if $06.0 then L4 else L5
L5:
  $11.0 @u32 = sub @u32 $02.0, 1
  arg @u32 $11.0
  $13.0 @u32 = call fib, 1
  $17.0 @u32 = sub @u32 $02.0, 2
  arg @u32 $17.0
  $19.0 @u32 = call fib, 1
  $20.0 @u32 = add @u32 $13.0, $19.0
  $01.1 @u32 = $20.0
  goto L3
L3:
  $01.0 @u32 = phi [ L4: $01.2, L6: $01.1 ]
  ret $01.0
L4:
  $01.2 @u32 = $02.0
  goto L3

