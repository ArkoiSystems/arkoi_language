fun main(a @u32) @u32:
L0:
  %01 @u32 = alloca
  arg @u32 1
  arg @u32 2
  arg @u32 3
  arg @u32 4
  arg @u32 5
  arg @u32 6
  arg @u32 7
  arg @u32 8
  $27 @u64 = call calling_convention, 8
  $28 @u32 = cast @u64 $27
  store @u32 $28, %01
  $29 @u32 = load %01
  ret $29

fun calling_convention(a @u32, b @u32, c @u32, d @u32, e @u32, f @u32, g @u32, h @u32) @u64:
L2:
  %01 @u64 = alloca
  store @u64 1, %01
  $12 @u64 = load %01
  ret $12

