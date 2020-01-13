## cppWasmDecomp
The goal of this program is to take a webassembly file (.wasm) and turn it into a psuedo code version that could (theoretically) be used to decompile the webassembly module. 

It kinda accomplishes this. As of now, all it does is move the module into an intermediate representation that looks a bit jank. 

## Example Usage and Output
Once this is compiled, this can be used through the command line as follows:

```wasmDecomp test.wasm```

Currently, this prints to stdout. This will change in the future. An example output follows:

```
WASM Version: 1
Done sectioning file.

Section  0 is not present.
Section  1 present.                   6 Bytes
Section  2 is not present.
Section  3 present.                   2 Bytes
Section  4 present.                   4 Bytes
Section  5 present.                   3 Bytes
Section  6 present.                   1 Bytes
Section  7 present.                  29 Bytes
Section  8 is not present.
Section  9 is not present.
Section 10 present.                  13 Bytes
Section 11 is not present.

i32 _Z11powersOfTwoi(i32 __i32_invar_0)
	(i32_const 1) i32_shl (local_get 0)
end
```

This is printing some debug information, and the last item is implicitly returned in this pseudo code.

## Other Items

### Major issues
* When working with the WASM format, there are several places where things were not clear from what I found in the WASM documents. For example, the section size is constantly 1 byte larger than it should be represented as. I do not know if this was because of the service I used, but the existance of a 0x00 byte following the section size is hardcoded.
* There is currently no representation of what kinds of information needs to be on the stack for a function to be called correctly. For example, the opcode 0x37 is defined to take an i32 and an i64. While this *should* be validated in the compilation step, there is no verification steps for these things on this end. On a similar note, WASM memory addresses are compleatly unhandled; though variables in the function accessed through memory can be treated like a pointer.
* Sometimes, things break. The tests that I have are very basic and do not cover everything. If you see something, say something!

### Todo List
- [ ] Translation layer to allow decompilation to languages (In progress, see output_translator branch)
- [ ] Exterminate raw pointers once and for all
- [ ] Ensure that each size that gets parsed gets parsed correctly (i32's can be anywhere from 1 - 4 bytes long, and we don't know until we get there)
- [ ] Make sure that operations that rely on other sections are taken care of (see Tests\imported_strings.wasm)
