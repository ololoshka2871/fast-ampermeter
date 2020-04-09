#ifndef SOFTRESET_H
#define SOFTRESET_H

/*!
 * \ingroup Utils
 */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Reset CPU
 */
void SoftReset() __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif // SOFTRESET_H
