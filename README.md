# libitc

A tiny, pure C99, library implementing the [Interval Tree Clocks](https://gsd.di.uminho.pt/members/cbm/ps/itc2008.pdf) mechanism introduced by Almeida et al. in 2008.

## What Are Interval Tree Clocks?

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

How do I build this library?

### Prerequisites

* C99-compatible compiler
* The [Meson build system](https://mesonbuild.com/)

### Building The Library

The build process is split into two stages - [configuration](#build-configuration) and [compilation](#compilation).

#### Build Configuration

To configure the build use the `meson setup` command:

```sh
meson setup -Doptimization=2 -Ddebug=false build
```

See [Meson built-in options](https://mesonbuild.com/Builtin-options.html) for more information on the used build options.

#### Feature Configuration

libitc strives to be flexible and allows turning on/off optional features. This allows you to alter/extend its feature set depending on your needs.

See [`ITC_config.h`](./libitc/include/ITC_config.h) for all available options.

You can modify the header file directly to change the default configuration, or you can provide alternative values via `CFLAGS` during the [build configuration](#build-configuration) stage.

For example, to enable the extended API, you can configure the build like so:

```sh
CFLAGS="-DITC_CONFIG_ENABLE_EXTENDED_API=1" meson setup -Doptimization=2 -Ddebug=false build-with-extended-api
```

#### Compilation

To compile the code simply run:

```sh
meson install -C name-of-the-setup-directory
```

This will produce both statically and dynamically linked library variants, which can be found under `./name-of-the-setup-directory/bin`.

### Linking

To use the library simply link your executable against it. For example, assuming you have a `main.c` and want to link it against the produced static `libitc.a` using `gcc`:

```sh
gcc main.c ./name-of-the-setup-directory/bin/libitc.a -I./libitc/include -o main
```

Or, if your project uses Meson as its build system, you can incorporate the libitc project as a subproject of your project instead.

## Running The Unit Tests

If you wish to run the unit tests for yourself, you can do so via the following commands:

```sh
meson setup -Dtests=true test-build
meson test -C test-build
```

> :bulb: If you have [Valgind](https://valgrind.org/) installed and available on your `$PATH`, Meson will automatically use it to check for possible memory leaks or other undesired behaviour while executing the unit tests.

## License

Released under AGPL-3.0 license, see [LICENSE](./LICENSE) for details.

## Special Thanks To

* Paulo Sérgio Almeida, Carlos Baquero and Victor Fonte for writing the [ITC research paper](http://hydra.azilian.net/Papers/Interval%20Tree%20Clocks.pdf)
* Fred Hebert, for laying down his thoughts on some of the shortcomings of ITC in his [article](https://ferd.ca/interval-tree-clocks.html)
