fun main($a.0 @u32) @u32:
L0:
  arg @f32 0
  arg @f32 2
  arg @f32 3
  arg @u32 4
  arg @u32 5
  arg @u32 6
  arg @u32 7
  arg @u32 8
  $27.0 @u64 = call calling_convention, 8
  $28.0 @u32 = cast @u64 $27.0
  $01.0 @u32 = $28.0
  $29.0 @u32 = $01.0
  ret $29.0

fun calling_convention($a.0 @f32, $b.0 @f32, $c.0 @f32, $d.0 @u32, $e.0 @u32, $f.0 @u32, $g.0 @u32, $h.0 @u32) @u64:
L2:
  $02.0 @f32 = $a.0
  $03.0 @f32 = $b.0
  $04.0 @f32 = $c.0
  $10.0 @bool = 0
  $11.0 @bool = 0
  $12.0 @f32 = $02.0
  $13.0 @bool = cast @f32 $12.0
  if $13.0 then L8 else L9
L9:
  $11.1 @bool = phi [ L7: $11.2, L8: $11.0, L2: $11.0 ]
  $16.0 @bool = $11.1
  if $16.0 then L4 else L5
L5:
  $17.0 @f32 = $04.0
  $18.0 @bool = cast @f32 $17.0
  if $18.0 then L4 else L6
L6:
  $10.1 @bool = phi [ L5: $10.0, L4: $10.2 ]
  $19.0 @bool = $10.1
  $20.0 @u64 = cast @bool $19.0
  $01.0 @u64 = $20.0
  $21.0 @u64 = $01.0
  ret $21.0
L4:
  $10.2 @bool = 1
  goto L6
L8:
  $14.0 @f32 = $03.0
  $15.0 @bool = cast @f32 $14.0
  if $15.0 then L7 else L9
L7:
  $11.2 @bool = 1
  goto L9

