# Loupe
A modern and extendible reflection library for C++, designed with game development in mind.

Loupe's API is designed to be succinct and easy to use. Instead of scanning pre-main, Loupe produces your application's reflection tables when requested - you may store the result of this globally if you wish. Your reflection tables can be serialized to disk along with your data, allowing you to read data back safely even as your application continues to change.

Loupe does not require Run-Time Type Information (RRTI) or Exceptions, and is dependent only on the C++ standard library.

# 1.0 Features
- Diff between blobs to detect and reconcile changes
- Serialization ready (bring your own archiver)
- User-defined metadata attributes

# Post 1.0 Features
- Reflection of functions
- Single header mode
- Blob coverage detection, serializing only visited properties
- Support beyond the Windows platform
- Debugger .natvis files
- Support for custom allocators
- metadata support of enums
- metadata with data (such as a range bound for floats)

# Test Project Dependencies
- [Catch 2.13.9](https://github.com/catchorg/Catch2/tree/v2.x)
- [Cereal 1.3.2](https://github.com/USCiLab/cereal)
