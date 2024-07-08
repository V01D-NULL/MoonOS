## Tests

The frameworks used to conduct tests are Unity, CMock and Ceedling by ThrowTheSwitch (all tools are documented on their [github](https://github.com/ThrowTheSwitch)).

Currently there are only tests configured for the x86 architecture on the acpi board.

To create a test for a new board or arch create a new project-{ARCH}-{BOARD}.yml and copy paste an existing yml config. Then, change any hardcoded source paths and alter compiler configurations as needed.

## Running tests

Run make test in the project root or manually invoke ceedling in this directory with `CEEDLING_MAIN_PROJECT_FILE` set to the yml file you want to use.