fun main() @u64:
L0:
  %01 @u64 = alloca
  arg @f64 5
  $07 @bool = call ok, 1
  $08 @u32 = cast @bool $07
  $09 @u32 = mul @u32 1, $08
  $12 @u32 = add @u32 $09, 1
  $13 @s32 = cast @u32 $12
  arg @s32 $13
  arg @f64 10.5
  $18 @f32 = call test1, 2
  $20 @f32 = mul @f32 $18, 2.01
  $23 @f32 = sub @f32 $20, 42
  $24 @u64 = cast @f32 $23
  %01 @u64 = store $24
  $25 @u64 = load %01
  ret $25

fun ok(foo1 @f64) @bool:
L2:
  %01 @bool = alloca
  %02 @f64 = alloca
  %02 @f64 = store foo1
  $03 @f64 = load %02
  $06 @bool = gth @f64 $03, 5
  if $06 then L4 else L5
L5:
  $09 @f64 = load %02
  $12 @bool = goe @f64 $09, 10
  if $12 then L7 else L8
L8:
  $15 @f64 = load %02
  $18 @bool = neq @f64 $15, 0
  if $18 then L10 else L11
L11:
  %02 @f64 = store 21
  goto L12
L12:
  $02 @f64 = phi [  ]
  goto L9
L9:
  $02 @f64 = phi [  ]
  goto L6
L6:
  $02 @f64 = phi [  ]
  $25 @s32 = div @s32 4, 2
  arg @s32 $25
  $27 @f64 = load %02
  arg @f64 $27
  $29 @f32 = call test2, 2
  $30 @bool = cast @f32 $29
  %01 @bool = store $30
  $31 @bool = load %01
  ret $31
L10:
  %02 @f64 = store 10
  goto L12
L7:
  %02 @f64 = store 20
  goto L9
L4:
  %02 @f64 = store 0
  goto L6

fun test1(foo2 @s32, bar @f64) @f32:
L13:
  %01 @f32 = alloca
  %02 @s32 = alloca
  %03 @f64 = alloca
  %02 @s32 = store foo2
  %03 @f64 = store bar
  %04 @f32 = alloca
  %04 @f32 = store 0
  $06 @s32 = load %02
  $07 @f64 = cast @s32 $06
  $08 @f64 = load %03
  $09 @bool = loe @f64 $07, $08
  if $09 then L15 else L16
L16:
  $20 @s32 = load %02
  $21 @f64 = cast @s32 $20
  $22 @f64 = load %03
  $23 @bool = equ @f64 $21, $22
  if $23 then L18 else L19
L19:
  $26 @s32 = load %02
  $27 @f64 = cast @s32 $26
  $28 @f64 = load %03
  $29 @f64 = mul @f64 $27, $28
  $30 @f32 = cast @f64 $29
  %04 @f32 = store $30
  goto L20
L20:
  $04 @f32 = phi [  ]
  goto L17
L17:
  $04 @f32 = phi [  ]
  $31 @f32 = load %04
  %01 @f32 = store $31
  $32 @f32 = load %01
  ret $32
L18:
  $24 @s32 = load %02
  $25 @f32 = cast @s32 $24
  %04 @f32 = store $25
  goto L20
L15:
  $10 @f64 = load %03
  $11 @s32 = load %02
  $12 @f64 = cast @s32 $11
  $13 @f64 = mul @f64 $10, $12
  $14 @s32 = load %02
  $15 @s32 = load %02
  $16 @bool = lth @s32 $14, $15
  $17 @f64 = cast @bool $16
  $18 @f64 = add @f64 $13, $17
  $19 @f32 = cast @f64 $18
  %04 @f32 = store $19
  goto L17

fun test2(foo2 @s32, bar @f64) @f32:
L21:
  %01 @f32 = alloca
  %02 @s32 = alloca
  %03 @f64 = alloca
  %02 @s32 = store foo2
  %03 @f64 = store bar
  $04 @s32 = load %02
  $05 @f64 = cast @s32 $04
  $06 @f64 = load %03
  $07 @bool = lth @f64 $05, $06
  if $07 then L23 else L24
L24:
  $13 @s32 = load %02
  $14 @f64 = cast @s32 $13
  $15 @f64 = load %03
  $16 @f64 = mul @f64 $14, $15
  %03 @f64 = store $16
  $17 @f64 = load %03
  $18 @f32 = cast @f64 $17
  %01 @f32 = store $18
  goto L22
L22:
  $03 @f64 = phi [  ]
  $01 @f32 = phi [  ]
  $19 @f32 = load %01
  ret $19
L23:
  $08 @f64 = load %03
  $09 @s32 = load %02
  $10 @f64 = cast @s32 $09
  $11 @f64 = mul @f64 $08, $10
  $12 @f32 = cast @f64 $11
  %01 @f32 = store $12
  goto L22

