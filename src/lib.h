/**
 * @file lib.h
 * @author Andrey Malov (ndrmlv@ya.ru)
 * @brief Simple library for string operations and other stuff.
 * @version 0.1
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SRC_LIB_H
#define SRC_LIB_H

#include <iostream>

namespace MailingApp {
/**
 * @brief Type aliasing for User ID.
 */
using UserId = int64_t;

/**
 * @brief Token for accessing api.
 * TODO: That's is not secure. Must to use getenv to get this token from OS
 */
const std::string token = "5246372133:AAH3XvfTmCJIywDsL4acJZcGu7F8suep4DY";

/**
 * @brief Admin's User ID.
 * TODO: That's is not secure. Must to use getenv to get this id from OS
 */
const UserId admin_id = 379420471;
} // namespace MailingApp

#endif /* SRC_LIB_H */