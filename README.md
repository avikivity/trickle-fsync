# trickle-fsync: periodically issue fsyncs to a mountpoint

## Overview

Some SSDs that have a large write-back cache can accumulate a large
backlog, so that when an fsync finally hits them, it can take a long
time to process.

This program tries to prevent large buffers from accumulating by
periodically forcing a flush to reach the disk.

## Usage

Run the program as follows:

    trickle-fsync /path/to/mountpoint period

Where `/path/to/mountpoint` is a directory on the problematic disk,
and `period` is the time in seconds between flushes. Fractions are supported,
and the default is 0.1 seconds.

## Building

Install the boost program_options library (including headers and static library)
and a C++ compiler, and run

   make

This generates a static executable that can run anywhere.