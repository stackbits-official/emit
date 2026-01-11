# Table Of Contents

- [Introduction](#introduction)
    - [Motivation](#motivation)
- [Integration](#integration)
    - [Requirements](#requirements)
    - [CMake](#cmake)
- [License](#license)

# Introduction

```emit``` is a header-only and minimal event system library written in modern C++. 

## Motivation

I made ```emit``` as a chance to make the simplest and fastest event system I could without sacrificing user experience and API simplicity.

# Integration

```emit``` is a header-only library, meaning that simply including ```<emit/emit.hpp>``` is enough to include the whole library. 

## Requirements

To be able to use ```emit```, you will need a compiler that supports at least C++17.

## CMake

Currently the only build system supported by ```emit``` is ```CMake```. ```emit```'s ```CMake``` configuration supports various usage forms, including ```find_package``` for global installation and  ```add_submodule``` for embedding.

# License

Code and documentation Copyright (c) 2026 Lucas Kuchel.

Code released under [the MIT license](./LICENSE).