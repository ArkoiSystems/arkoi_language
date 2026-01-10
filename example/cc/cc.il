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
  ret $28.0

fun calling_convention($a.0 @f32, $b.0 @f32, $c.0 @f32, $d.0 @u32, $e.0 @u32, $f.0 @u32, $g.0 @u32, $h.0 @u32) @u64:
L2:
  $03.0 @f32 = $b.0
  $04.0 @f32 = $c.0
  $10.0 @bool = 0
  $11.0 @bool = 0
  $13.0 @bool = cast @f32 $a.0
  if $13.0 then L8 else L9
L9:
  $11.1 @bool = phi [ L7: $11.2, L8: $11.0, L2: $11.0 ]
  if $11.1 then L4 else L5
L5:
  $18.0 @bool = cast @f32 $04.0
  if $18.0 then L4 else L6
L6:
  $10.1 @bool = phi [ L5: $10.0, L4: $10.2 ]
  $20.0 @u64 = cast @bool $10.1
  ret $20.0
L4:
  $10.2 @bool = 1
  goto L6
L8:
  $15.0 @bool = cast @f32 $03.0
  if $15.0 then L7 else L9
L7:
  $11.2 @bool = 1
  goto L9

