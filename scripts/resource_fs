#!/bin/env python3

import argparse
import os
import uuid
from typing import Any, Generator

import yaml
from jinja2 import Template

c_template = """
#define _POSIX_C_SOURCE 200809L
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	const char *name;
	char *data;
	size_t size;
} resource_t;

{% for res in data.resources %}
// {{ res.filename }}
char {{ res.var_name }}[] =
{{ res.content }};
{% endfor %}

const resource_t resources[] = {
{%- for res in data.resources %}
	{"{{ res.name }}", {{ res.var_name }}, sizeof({{ res.var_name }})},
{%- endfor %}
	{NULL, NULL, 0}
};

typedef FILE *(*fopen_type)(const char *__restrict filename, const char *__restrict modes);

FILE *fopen(const char *__restrict filename, const char *__restrict modes) {

	static fopen_type libc_open = NULL;

	/* check if the file is in our resources */
	for (int i = 0; resources[i].name != NULL; i++) {
		if (strcmp(filename, resources[i].name) == 0) {

			/* check if the mode is valid */
			if (strcmp(modes, "r") != 0 && strcmp(modes, "rb") != 0) {
				return NULL;
			}

			/* return a FILE pointer to our resource */
			return fmemopen(resources[i].data, resources[i].size, modes);
		}
	}

	/* get reference to original (libc provided) fopen */
	if (!libc_open) {
		*(void **)(&libc_open) = dlsym(RTLD_NEXT, "fopen");
	}

	return libc_open(filename, modes);
}
"""

def chunk_list(data: bytes, chunk_size: int) -> Generator[bytes, Any, Any]:
    for i in range(0, len(data), chunk_size):
        yield data[i : i + chunk_size]


def main(config: str, output: str) -> None:

    with open(args.config, "r") as f:
        config = yaml.safe_load(f)

    resources = config["resources"]

    for resource in resources:
        if "filename" not in resource:
            raise ValueError("Resource must have a name")

        if "name" not in resource:
            resource["name"] = "res:/" + os.path.basename(resource["filename"])
        else:
            resource["name"] = "res:/" + resource["name"]

        resource_id = uuid.uuid4()
        resource["var_name"] = f"res_{resource_id.hex}"

        with open(resource["filename"], "rb") as f:
            data: bytes = f.read()

            data_str: str = ""
            for chunk in chunk_list(data, 16):
                data_str += (
                    '    "'
                    + "\\x"
                    + "\\x".join(f"{byte:02x}" for byte in chunk)
                    + '"\n'
                )

            resource["content"] = data_str.rstrip()

    template = Template(c_template)
    generated_code = template.render(data={"resources": resources})

    with open(output, "w") as f:
        f.write(generated_code)
        f.write("\n")

    print(f"Resource file generated at {args.output}")


if __name__ == "__main__":

    # Set up argument parser
    parser = argparse.ArgumentParser(description="Generate resource file")
    parser.add_argument(
        "--config",
        type=str,
        default="config.yaml",
        help="Path to the configuration file",
    )
    parser.add_argument(
        "--output",
        type=str,
        default="resources.c",
        help="Path to the output file",
    )
    args = parser.parse_args()
    main(args.config, args.output)



