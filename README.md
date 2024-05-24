# libitc

<img src="https://img.shields.io/badge/C-99-gray?color=blue" alt="C99"> <img src="https://img.shields.io/github/actions/workflow/status/sdimovv/libitc/.github%2Fworkflows%2Fbuild-and-run-tests.yml?branch=main&logo=github" alt="Build Status"> <a href="https://github.com/sdimovv/libitc/releases/latest"><img src="https://img.shields.io/github/v/release/sdimovv/libitc" alt="Latest GitHub Release"></a> <a href="./LICENSE"><img src="https://img.shields.io/github/license/sdimovv/libitc" alt="License AGPL-3.0"></a>

A tiny, pure C99, library implementing the [Interval Tree Clocks](https://gsd.di.uminho.pt/members/cbm/ps/itc2008.pdf) mechanism introduced by Almeida et al. in 2008.

## What Are Interval Tree Clocks?

Interval Tree Clocks (ITC) are a generalisation of the [Vector Clock](https://en.wikipedia.org/wiki/Vector_clock) and [Version Vector](https://en.wikipedia.org/wiki/Version_vector) mechanisms, allowing for scalable and efficient management of a
highly dynamic number of replicas/processes in a distributed system.


## Features

* Implements the full ITC mechanism as described in the research paper.
* Written in C99. It has no dependencies apart from a few C standard library
  header files (`CMock` is only used for unit testing).
* Minimises stack usage by **not** relying on recursion.
* Generally tries to be as efficient and as small as possible.
* Can be configured to use either 32 or 64-bit event counters.
* Provides handy serialisation and deserialisation APIs.
* Provides an optional "extended" API interface (based on
  [this article](https://ferd.ca/interval-tree-clocks.html)), giving you more fine-grained control over the ITC lifecycle. This is not part of the original mechanism and is intended for more advanced use cases.

## Getting Started

### Prerequisites

* C99-compatible compiler
* The [Meson build system](https://mesonbuild.com/)

### Building

The build process is split into two stages - [configuration](#build-configuration) and [compilation](#compilation).

#### Build Configuration

To configure the build use the `meson setup` command:

```bash
meson setup -Doptimization=2 -Ddebug=false build
```

See [Meson built-in options](https://mesonbuild.com/Builtin-options.html) for more information on the used options.

#### Feature Configuration

libitc strives to be flexible and allows turning on/off optional features. This allows you to alter/extend its feature set depending on your needs.

See [`ITC_config.h`](./libitc/include/ITC_config.h) for all available options.

You can modify the header file directly to change the default configuration, or you can provide alternative values via `CFLAGS` during the [build configuration](#build-configuration) stage.

For example, to enable the extended API, you can configure the build like so:

```bash
CFLAGS="-DITC_CONFIG_ENABLE_EXTENDED_API=1" meson setup -Doptimization=2 -Ddebug=false build-with-extended-api
```

#### Compilation

To compile the code simply run:

```bash
meson install -C name-of-the-setup-directory
```

This will produce both statically and dynamically linked library variants, which can be found under `./name-of-the-setup-directory/bin`.

### Linking

To use the library simply link your executable against it. For example, assuming you have a `main.c` and want to link it against the produced static `libitc.a` using `gcc`:

```bash
gcc main.c ./name-of-the-setup-directory/bin/libitc.a -I./libitc/include -o main
```

Or, if your project uses Meson as its build system, you can incorporate the libitc project as a subproject of your project instead.

### Usage Examples

Let's go over some basic usage examples.

#### Hello World

Let's start simple.

Create a `Stamp`, add an `Event` to it, then proceed to destroy it and exit.

<details>
<summary>Code:</summary>

```c
#include "ITC.h"

#include <stddef.h> /* For access to the `NULL` macro */

int main(void)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS;
    ITC_Stamp_t *pt_Stamp = NULL;

    /* Allocate the Stamp */
    t_Status = ITC_Stamp_newSeed(&pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Add an Event */
        t_Status = ITC_Stamp_event(pt_Stamp);
    }

    /* Passing a `NULL` to `ITC_Stamp_destroy` is safe, but let's be prudent */
    if (pt_Stamp)
    {
        /* Deallocate the Stamp */
        t_OpStatus = ITC_Stamp_destroy(&pt_Stamp);

        if (t_OpStatus != ITC_STATUS_SUCCESS)
        {
            /* Return the last error */
            t_Status = t_OpStatus;
        }
    }

    return t_Status;
}
```

</details>

#### Create-Fork-Event-Compare

Create a `Stamp`, fork it, add `Event`s to both stamps (making them **concurrent**), and then compare them. Finally, destroy both stamps and exit.

<details>
<summary>Code:</summary>

```c
#include "ITC.h"

#include <stddef.h> /* For access to the `NULL` macro */

int main(void)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS;
    ITC_Stamp_t *pt_Stamp1 = NULL;
    ITC_Stamp_t *pt_Stamp2 = NULL;
    ITC_Stamp_Comparison_t t_Result;

    /* Allocate the Stamp */
    t_Status = ITC_Stamp_newSeed(&pt_Stamp1);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Fork the Stamp */
        t_Status = ITC_Stamp_fork(&pt_Stamp1, &pt_Stamp2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Add an Event to Stamp1 */
        t_Status = ITC_Stamp_event(pt_Stamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Add an Event to Stamp2 */
        t_Status = ITC_Stamp_event(pt_Stamp2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare the Stamps */
        t_Status = ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result);

        if (t_Result != ITC_STAMP_COMPARISON_CONCURRENT)
        {
            /* Something is not right, these Stamps should be concurrent */
            t_Status = ITC_STATUS_FAILURE;
        }
    }

    /* Passing a `NULL` to `ITC_Stamp_destroy` is safe, but let's be prudent */
    if (pt_Stamp1)
    {
        /* Deallocate Stamp1 */
        t_OpStatus = ITC_Stamp_destroy(&pt_Stamp1);

        if (t_OpStatus != ITC_STATUS_SUCCESS)
        {
            /* Return the last error */
            t_Status = t_OpStatus;
        }
    }
    if (pt_Stamp2)
    {
        /* Deallocate Stamp2 */
        t_OpStatus = ITC_Stamp_destroy(&pt_Stamp2);

        if (t_OpStatus != ITC_STATUS_SUCCESS)
        {
            /* Return the last error */
            t_Status = t_OpStatus;
        }
    }

    return t_Status;
}
```

</details>

#### Create-Fork-Event-Peek-Compare-Join

Create a `Stamp`, fork it, add an `Event` to it (making one stamp **greater than** the other). Compare the stamps. Share causal history via a `Peek Stamp`, then compare the stamps again. Then proceed to join the stamps back into a `Seed Stamp`. Finally, deallocate the remaining stamp and exit.

<details>
<summary>Code:</summary>

```c
#include "ITC.h"

#include <stddef.h> /* For access to the `NULL` macro */

int main(void)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS;
    ITC_Stamp_t *pt_Stamp1 = NULL;
    ITC_Stamp_t *pt_Stamp2 = NULL;
    ITC_Stamp_t *pt_PeekStamp1 = NULL;
    ITC_Stamp_Comparison_t t_Result;

    /* Allocate the Stamp */
    t_Status = ITC_Stamp_newSeed(&pt_Stamp1);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Fork the Stamp */
        t_Status = ITC_Stamp_fork(&pt_Stamp1, &pt_Stamp2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Add an Event to Stamp1 */
        t_Status = ITC_Stamp_event(pt_Stamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare the Stamps */
        t_Status = ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result);

        if (t_Result != ITC_STAMP_COMPARISON_GREATER_THAN)
        {
            /* Something is not right, Stamp1 should be greater than Stamp2 */
            t_Status = ITC_STATUS_FAILURE;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare the Stamps the other way around */
        t_Status = ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result);

        if (t_Result != ITC_STAMP_COMPARISON_LESS_THAN)
        {
            /* Something is not right, Stamp2 should be less than Stamp1 */
            t_Status = ITC_STATUS_FAILURE;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Create a peek Stamp */
        t_Status = ITC_Stamp_newPeek(pt_Stamp1, &pt_PeekStamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Share the causal history of Stamp1 with Stamp2.
        * No need to deallocate `pt_PeekStamp1`. `ITC_Stamp_join`
        * will deallocate it on exit, to prevent it from being used
        * again after joining. */
        t_Status = ITC_Stamp_join(&pt_Stamp2, &pt_PeekStamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare the Stamps */
        t_Status = ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result);

        if (t_Result != ITC_STAMP_COMPARISON_EQUAL ||
            t_Result != ITC_STAMP_COMPARISON_GREATER_THAN)
        {
            /* Something is not right, Stamp1 should be greater than or equal to
            * Stamp2 because the causal history was shared */
            t_Status = ITC_STATUS_FAILURE;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare the Stamps the other way around */
        t_Status = ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result);

        if (t_Result != ITC_STAMP_COMPARISON_EQUAL ||
            t_Result != ITC_STAMP_COMPARISON_GREATER_THAN)
        {
            /* Something is not right, Stamp1 should be greater than or equal to
            * Stamp2 because the causal history was shared */
            t_Status = ITC_STATUS_FAILURE;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Join Stamp2 with Stamp1.
        * No need to deallocate `pt_Stamp2`. `ITC_Stamp_join`
        * will deallocate it on exit, to prevent it from being used
        * again after joining. */
        t_Status = ITC_Stamp_join(&pt_Stamp1, &pt_Stamp2);
    }

    /* Passing a `NULL` to `ITC_Stamp_destroy` is safe, but let's be prudent */
    if (pt_Stamp1)
    {
        /* Deallocate Stamp1 */
        t_OpStatus = ITC_Stamp_destroy(&pt_Stamp1);

        if (t_OpStatus != ITC_STATUS_SUCCESS)
        {
            /* Return the last error */
            t_Status = t_OpStatus;
        }
    }
    if (pt_Stamp2)
    {
        /* Deallocate Stamp2 */
        t_OpStatus = ITC_Stamp_destroy(&pt_Stamp2);

        if (t_OpStatus != ITC_STATUS_SUCCESS)
        {
            /* Return the last error */
            t_Status = t_OpStatus;
        }
    }

    return t_Status;
}
```

</details>

#### Ser-Des

Serialise and deserialise a Stamp.

> :bulb: If the [extended API](#feature-configuration) is enabled, identical operations
can be performed on `ID`s and `Events` as well.

<details>
<summary>Code:</summary>

```c
#include "ITC.h"

#include <stddef.h> /* For access to the `NULL` macro */
#include <stdint.h>

int main(void)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS;
    ITC_Stamp_t *pt_Stamp = NULL;
    uint8_t ru8_StampBuffer[10] = { 0 };
    uint32_t u32_StampBufferCurrentLen = sizeof(ru8_StampBuffer);

    /* Allocate the Stamp */
    t_Status = ITC_Stamp_newSeed(&pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Serialise the Stamp.
         * NOTE: `u32_StampBufferCurrentLen` will be set to the length of
         * the data in the buffer on exit */
        t_Status = ITC_SerDes_serialiseStamp(
            pt_Stamp, &ru8_StampBuffer[0], &u32_StampBufferCurrentLen);
    }

    if (t_Status == ITC_STATUS_INSUFFICIENT_RESOURCES)
    {
        /* Allocate a bigger buffer and try again */
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Destroy the Stamp */
        t_Status = ITC_Stamp_destroy(&pt_Stamp);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Deserialise the Stamp */
        t_Status = ITC_SerDes_deserialiseStamp(
            &ru8_StampBuffer[0], u32_StampBufferCurrentLen, &pt_Stamp);
    }

    /* Passing a `NULL` to `ITC_Stamp_destroy` is safe, but let's be prudent */
    if (pt_Stamp)
    {
        /* Destroy the Stamp */
        t_OpStatus = ITC_Stamp_destroy(&pt_Stamp);

        if (t_OpStatus != ITC_STATUS_SUCCESS)
        {
            /* Return the last error */
            t_Status = t_OpStatus;
        }
    }

    return t_Status;
}
```

</details>

## Running The Unit Tests

If you wish to run the unit tests for yourself, you can do so via the following commands:

```bash
meson setup -Dtests=true test-build
meson test -C test-build
```

> :bulb: If you have [Valgind](https://valgrind.org/) installed and available on your `$PATH`, Meson will automatically use it to check for memory leaks or other undesired behaviour while executing the unit tests.

## License

Released under AGPL-3.0 license, see [LICENSE](./LICENSE) for details.

## Special Thanks To

* Paulo Sérgio Almeida, Carlos Baquero and Victor Fonte for writing the [ITC research paper](http://hydra.azilian.net/Papers/Interval%20Tree%20Clocks.pdf)
* Fred Hebert, for laying down his thoughts on some of the shortcomings of ITC in his [article](https://ferd.ca/interval-tree-clocks.html)
