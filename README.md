# Braille display in C
## Why?
Just an idea that popped in my head while seeing other cli tools use braille characters to show loading bars/progress/throbbers

## Usage example
The library is structured in a single header file, like many others. 
In a single translation unit add `#define BRAILLE_DISPLAY_IMPLEMENTATION` to actually define the functions.

```c
// in main.c
#define BRAILLE_DISPLAY_IMPLEMENTATION
#include "braille_display.h"
```

