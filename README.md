# resource-fs
Access your resources with ordinary FILE I/O

The upcoming `#embed` keyword is very exciting for embedding resources in C programs. But I feel it still falls short of what it could have been. Why not make it easy to embed resources and have them accessible with ordinary file operations? That's what this tool does!

# Usage

First just add the `resource_fs` script to your source tree, anywhere will do, but I tend to choose `scripts/`. Then add it to your build system. For `cmake`, it's as simple as this:

```cmake
cmake_minimum_required(VERSION 3.15)
project(example)

add_executable(example
    main.c
    ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)

# The following is what generates the resources.c file
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    COMMAND python3 scripts/resource_fs --output ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS res/example_resource.txt
    DEPENDS res/another_resource.txt
    DEPENDS config.yaml
)

add_custom_target(generate_file ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)
```

Next, create a config YAML file, which looks like this:

```yaml
resources:
  - filename: ./res/example_resource.txt
    description: This is an example resource file.
    # description is optional
    # if no name is specified, that's ok, we'll just use the basename of the filename
  - filename: ./res/another_resource.txt
    name: alternate_name.md
    description: This is an example resource file.
options:
    use_embed: false # i don't have a compiler that supports embed yet, but it *should* work
```

Finally, just do your FILE I/O like normal, except that you prefix the filename with `res:/`.

```c
#include <stdio.h>

int main(void) {

	FILE *f = fopen("res:/example_resource.txt", "rb");
	if (f) {
		char buf[100];
		size_t read = fread(buf, 1, sizeof(buf), f);
		if (read > 0) {
			printf("Read %zu bytes: %.*s\n", read, (int)read, buf);
		}
		fclose(f);
	} else {
		printf("Failed to open file\n");
	}

	return 0;
}
```

You can of course still use `fopen` like normal to access other files, only
filenames which match those of the resources get re-routed.

# Notes

* You may only open the resource files in read-only modes, `"r"` and `"rb"`.

# Future work

* I may add automatic compression support at some point in the future.

# How does it work?

GCC and Clang's libc exports its symbols as weak symbols. This is so things like `LD_PRELOAD`
can override features of the standard library for instrumentation and debugging purposes...
But, it's not only useful for that. You can override them in your own code too!

So, we simply generate a source file which exports an `fopen` function matching the signature
of the original and add our special sauce there.

If we're trying to open a non-resource file, we just use `dlsym(RTLD_NEXT, "fopen");`
to get the "next" implementation of `fopen`, AKA the original one so we can just call the original as needed.

If we're trying to open a resource file that was found, linux offers a handy function called
`fmemopen` which lets us wrap a `char *` with a `FILE*` object that can be used like normal.

So, once we have `FILE*`, the experience is seamless.
