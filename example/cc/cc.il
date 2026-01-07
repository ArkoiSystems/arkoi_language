fun main(a @u32) @u32:
L0:
  %01 @u32 = alloca
  arg @f32 0
  arg @f32 2
  arg @f32 3
  arg @u32 4
  arg @u32 5
  arg @u32 6
  arg @u32 7
  arg @u32 8
  $27 @u64 = call calling_convention, 8
  $28 @u32 = cast @u64 $27
  %01 @u32 = store $28
  $29 @u32 = load %01
  ret $29

fun calling_convention(a @f32, b @f32, c @f32, d @u32, e @u32, f @u32, g @u32, h @u32) @u64:
L2:
  %01 @u64 = alloca
  %02 @f32 = alloca
  %03 @f32 = alloca
  %04 @f32 = alloca
  %02 @f32 = store a
  %03 @f32 = store b
  %04 @f32 = store c
  %10 @bool = alloca
  %10 @bool = store 0
  %11 @bool = alloca
  %11 @bool = store 0
  $12 @f32 = load %02
  $13 @bool = cast @f32 $12
  if $13 then L8 else L9
L9:
  $11 @bool = phi [  ]
  $16 @bool = load %11
  if $16 then L4 else L5
L5:
  $17 @f32 = load %04
  $18 @bool = cast @f32 $17
  if $18 then L4 else L6
L6:
  $10 @bool = phi [  ]
  $19 @bool = load %10
  $20 @u64 = cast @bool $19
  %01 @u64 = store $20
  $21 @u64 = load %01
  ret $21
L4:
  %10 @bool = store 1
  goto L6
L8:
  $14 @f32 = load %03
  $15 @bool = cast @f32 $14
  if $15 then L7 else L9
L7:
  %11 @bool = store 1
  goto L9

