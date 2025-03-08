# 📚 Tomlfy Library

## 📝 Overview

Tomlfy is a simple C library for creating, manipulating, and printing TOML (Tom's Obvious, Minimal Language) data structures. This library provides functions to create various TOML value types, add them to arrays or tables, and print them in a human-readable format.

## ✨ Features

- 📝 Create TOML string, integer, float, boolean, array, and table values.
- ➕ Add items to TOML arrays and key-value pairs to TOML tables.
- 🖨️ Print TOML values in a readable format.
- 🗑️ Free allocated memory for TOML values.

## ⚙️ Installation

To use the Tomlfy library, simply include the `tomlfy.h` file in your project and compile them together with your source code.

## 🚀 Usage

### 🔧 Creating TOML Values

You can create various types of TOML values using the provided functions:

```c
Toml *string_value = tomlfy_create_string("Hello, World!");
Toml *int_value = tomlfy_create_integer(42);
Toml *float_value = tomlfy_create_float(3.14);
Toml *bool_value = tomlfy_create_boolean(1);
```
