fun main() @u64:
L0:
  arg @f64 5
  $07.0 @bool = call ok, 1
  $08.0 @u32 = cast @bool $07.0
  $09.0 @u32 = mul @u32 1, $08.0
  $12.0 @u32 = add @u32 $09.0, 1
  $13.0 @s32 = cast @u32 $12.0
  arg @s32 $13.0
  arg @f64 10.5
  $18.0 @f32 = call test1, 2
  $20.0 @f32 = mul @f32 $18.0, 2.01
  $23.0 @f32 = sub @f32 $20.0, 42
  $24.0 @u64 = cast @f32 $23.0
  ret $24.0

fun ok($foo1.0 @f64) @bool:
L2:
  $02.0 @f64 = $foo1.0
  $06.0 @bool = gth @f64 $foo1.0, 5
  if $06.0 then L4 else L5
L5:
  $12.0 @bool = goe @f64 $02.0, 10
  if $12.0 then L7 else L8
L8:
  $18.0 @bool = neq @f64 $02.0, 0
  if $18.0 then L10 else L11
L11:
  $02.4 @f64 = 21
  goto L12
L12:
  $02.3 @f64 = phi [ L10: $02.5, L11: $02.4 ]
  goto L9
L9:
  $02.2 @f64 = phi [ L7: $02.6, L12: $02.3 ]
  goto L6
L6:
  $02.1 @f64 = phi [ L4: $02.7, L9: $02.2 ]
  $25.0 @s32 = div @s32 4, 2
  arg @s32 $25.0
  arg @f64 $02.1
  $29.0 @f32 = call test2, 2
  $30.0 @bool = cast @f32 $29.0
  ret $30.0
L10:
  $02.5 @f64 = 10
  goto L12
L7:
  $02.6 @f64 = 20
  goto L9
L4:
  $02.7 @f64 = 0
  goto L6

fun test1($foo2.0 @s32, $bar.0 @f64) @f32:
L13:
  $02.0 @s32 = $foo2.0
  $03.0 @f64 = $bar.0
  $07.0 @f64 = cast @s32 $foo2.0
  $09.0 @bool = loe @f64 $07.0, $bar.0
  if $09.0 then L15 else L16
L16:
  $21.0 @f64 = cast @s32 $02.0
  $23.0 @bool = equ @f64 $21.0, $03.0
  if $23.0 then L18 else L19
L19:
  $27.0 @f64 = cast @s32 $02.0
  $29.0 @f64 = mul @f64 $27.0, $03.0
  $30.0 @f32 = cast @f64 $29.0
  $04.3 @f32 = $30.0
  goto L20
L20:
  $04.2 @f32 = phi [ L18: $04.4, L19: $04.3 ]
  goto L17
L17:
  $04.1 @f32 = phi [ L15: $04.5, L20: $04.2 ]
  ret $04.1
L18:
  $25.0 @f32 = cast @s32 $02.0
  $04.4 @f32 = $25.0
  goto L20
L15:
  $12.0 @f64 = cast @s32 $02.0
  $13.0 @f64 = mul @f64 $03.0, $12.0
  $16.0 @bool = lth @s32 $02.0, $02.0
  $17.0 @f64 = cast @bool $16.0
  $18.0 @f64 = add @f64 $13.0, $17.0
  $19.0 @f32 = cast @f64 $18.0
  $04.5 @f32 = $19.0
  goto L17

fun test2($foo2.0 @s32, $bar.0 @f64) @f32:
L21:
  $02.0 @s32 = $foo2.0
  $03.0 @f64 = $bar.0
  $05.0 @f64 = cast @s32 $foo2.0
  $07.0 @bool = lth @f64 $05.0, $bar.0
  if $07.0 then L23 else L24
L24:
  $14.0 @f64 = cast @s32 $02.0
  $16.0 @f64 = mul @f64 $14.0, $03.0
  $03.2 @f64 = $16.0
  $18.0 @f32 = cast @f64 $16.0
  $01.1 @f32 = $18.0
  goto L22
L22:
  $03.1 @f64 = phi [ L23: $03.0, L25: $03.2 ]
  $01.0 @f32 = phi [ L23: $01.2, L25: $01.1 ]
  ret $01.0
L23:
  $10.0 @f64 = cast @s32 $02.0
  $11.0 @f64 = mul @f64 $03.0, $10.0
  $12.0 @f32 = cast @f64 $11.0
  $01.2 @f32 = $12.0
  goto L22

