Code style
----------

1. Please use ``fixed integers``, see `stdint.h`_. Why? This library is compatible to different boards which use different architectures (16bit vs 32bit). So unfixed ``int`` has different sizes on different environments and may cause unpredictable behaviour.

2. If possible: use advantages of `c++11`, e.g. `constexpr`.

3. Use tab in source files. Space in examples.

4. Add documentation to **every** new function or parameter. Add documentation to most steps in your source code.
