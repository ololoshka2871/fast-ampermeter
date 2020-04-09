/*!
 * \file
 * \brief Stub for libc _init() function
 */

/*! \defgroup Utils Utility and halpers functions
 * @{
 */

/*!
 * \brief _init Empty stub
 *
 * Libc requires _init() to perform some initialization before global
 * constructors run. I would love if this symbol is defined as weak in
 * newlib-nano (libc), but it is not.
 */
void _init(void) __attribute__((weak));

/// Empty
void _init(void) {}

/*! @} */
