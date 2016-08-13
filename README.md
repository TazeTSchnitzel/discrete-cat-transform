# A beautifully over-engineered PPM desaturator

This code was intended to eventually become an (M)JPEG encoder, and possibly decoder.

But what it actually is an over-engineered gamma-correct 50% desaturator for 24bpp binary-format PPM files, which internally represents pixel values with packed half-precision 16-bit floats (via F16C).

Most of this over-engineering will need to be thrown away to write the actual “JPEG encoder” part, but I like what I have here, so this repo will stay untouched, including the silly `discrete-cat-transform` name, which alludes to the Discrete Cosine Transform, which this version of the code doesn't actually do, because I hadn't gotten round to the JPEG bit.

It is written in C99, using Intel SSE intrinsics for converting back and forth between half- and single-precision floats.

Build with `make`. Use `make asm` if you want to inspect the beautiful SIMD code the compiler generates.

This may only work on my machine using clang. For one thing, it builds targeting Haswell (because my machine uses a Haswell CPU) and requires the presence of the F16C instruction set (because I was lazy, this is greatly over-engineered, and my machine supports it).

Usage is `./discrete-cat-transform in.ppm out.ppm`.
