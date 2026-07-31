# Representative program

<!-- spec-sections: 16 -->

This compact, illustrative program brings together the specification's ownership,
borrowing, methods, hooks, and failure effects. It is not a standalone library
implementation.

!!! abstract "At a glance"

    `File` owns a native handle, borrowed methods inspect or mutate it, an owning
    parameter consumes it, and `__drop__` performs deterministic cleanup.

## Assumptions

The example assumes that `Buffer` and the helper functions
`open_native_file`, `native_file_size`, `flush_native_file`,
`read_entire_file`, `close_native_file`, `print`, and `process_buffer` are declared
elsewhere in the `example.file_demo` module or supplied by its environment. Their
implementations and any platform-specific error translation are outside this
illustration.

## Illustrative program

```arkoi
module example.file_demo

failure IOFail:
    not_found
    permission_denied

pub resource File:
    handle @u64

pub fun File.open(path @&string) !IOFail @File:
    handle @u64 = open_native_file(path)!

    return File(
        handle = handle,
    )

pub fun File.size(self @&File) @usize:
    return native_file_size(self.handle)

pub fun File.flush(self @&mut File) !IOFail:
    flush_native_file(self.handle)!

pub fun File.into_buffer(file @own File) !IOFail @Buffer:
    buffer @Buffer = read_entire_file(&file)!
    return move(buffer)

fun File.__drop__(self @&mut File):
    close_native_file(self.handle)

fun inspect(file @&File):
    print(file.size())

fun main() !IOFail:
    path @string = "input.txt"
    file @mut File = File.open(&path)!

    inspect(&file)
    file.flush()!

    buffer @Buffer = File.into_buffer(move(file))!
    process_buffer(&buffer)
```

## Concepts in the example

| Construct | Role |
| --- | --- |
| `module example.file_demo` | Gives the source file its explicit module identity. |
| `failure IOFail` and postfix `!` | Define and propagate the operation's failure effect. |
| `resource File` | Makes the native handle a non-copyable, deterministically cleaned-up value. |
| `File.open` | Acts as an associated constructor that returns a new `File`. |
| `&File` / `&mut File` | Borrow the file for read-only inspection or mutation. |
| `@own File` and `move(file)` | Transfer the resource into `File.into_buffer`. |
| `File.__drop__` | Closes the handle when the resource's lifetime ends. |

After the move into `File.into_buffer`, `main` no longer uses `file`; cleanup still
belongs to the consuming operation and the resource hook.

## Related topics

- [Modules and imports](modules-and-imports.md)
- [Function returns](functions-returns.md)
- [Ownership and moves](ownership-moves.md)
- [Resource lifecycle](resource-lifecycle.md)
- [References and lifetimes](references-lifetimes.md)
- [Methods and visibility](methods-visibility.md)
- [Failures](failures.md)
