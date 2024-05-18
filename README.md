# libitc

A tiny, pure C99, library implementing the [Interval Tree Clocks](https://gsd.di.uminho.pt/members/cbm/ps/itc2008.pdf) mechanism introduced by Almeida et al. in 2008.

## What are Interval Tree Clocks?

Interval Tree Clocks (ITC) are a generalisation of the [Vector Clock](https://en.wikipedia.org/wiki/Vector_clock) and [Version Vector](https://en.wikipedia.org/wiki/Version_vector) mechanisms, allowing for scalable and efficient management of a
highly dynamic number of replicas/processes in a distributed system.


## Features

* Implements the full ITC mechanism as described in the original research paper from 2008
* Written in C99. It has no dependencies apart from a few C standard library
  header files
* Minimises stack usage by **not** relying on recursion
* Generally tries to be as efficient and as small as possible
* Can be configured to use either 32 or 64-bit event counters
* Provides handy serialisation and deserialisation APIs
* Provides an optional "extended" API interface (based on
  [this article](https://ferd.ca/interval-tree-clocks.html)), giving you more fine-grained control over the ITC lifecycle. This is not part of the original
  mechanism and is intended for more advanced use cases.

## Getting Started

How do I build and use this library?

### Prerequisites

* C99-compatible compiler
* The [meson build system](https://mesonbuild.com/)

### Building the library

Building the library is a 2-step process:

#### Configure

To configure the build use the `meson setup` command:

```sh
meson setup -Doptimization=2 -Ddebug=false build
```

See [Meson builtin options](https://mesonbuild.com/Builtin-options.html) for more information on the used build options.

#### Compile

To compile the code simply run:

```sh
meson install -C build
```

By default, this will produce both statically and dynamically linked library variants, which can be found under `./build/bin`.

### Running unit tests

If you wish to run the unit tests for yourself, you can do so via the following commands:

```sh
meson setup -Dtests=true test-build
meson test -C test-build
```

## Usage

To use the library simply link your executable against it. For example, assumming you have a `main.c` and want to link it against the produced static `libitc.a`, using `gcc`:

```sh
gcc main.c ./build/bin/libitc.a -I./libitc/include -o main
```

Or, if your project uses Meson as its build system, you can incorporate the libitc project as a subproject of your project instead.

## Custom configuration (advanced)

libitc provides several configuration options, which enhance or alter its feature set. See [`ITC_config.h`](./libitc/include/ITC_config.h) for all available options.

You can modify the header file to change the default options, or you can provide alternative values via `CFLAGS` during the [build configuration](#configure) stage.

For example, to enable the extended API, you can configure the build like so:

```sh
CFLAGS="-DITC_CONFIG_ENABLE_EXTENDED_API=1" meson setup -Doptimization=2 -Ddebug=false build-with-extended-api
```

Then, compile as usual:
```sh
meson install -C build-with-extended-api
```

## License

Released under AGPL-3.0 license, see [LICENSE](./LICENSE) for details.

## Special thanks to

* Paulo Sérgio Almeida, Carlos Baquero and Victor Fonte for writing the [ITC research paper](http://hydra.azilian.net/Papers/Interval%20Tree%20Clocks.pdf)
* Fred Hebert, for laying down his thoughts on some of the shortcomings of ITC in his [article](https://ferd.ca/interval-tree-clocks.html)
