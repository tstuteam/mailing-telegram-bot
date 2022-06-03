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

#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

namespace MailingApp {
/**
 * @brief Type aliasing for User ID.
 */
using UserId = int64_t;

/**
 * @brief Type aliasing for Command in Chat.
 */
using Command = std::string;

/**
 * @brief Split message with max length.
 *
 * @param message Long message.
 * @param max Max length of one unit of message.
 * @return std::vector<std::string> Message splitted in units with some max
 * size.
 */
std::vector<std::string> split_message(const std::string message,
                                       const int max = 4096);

/**
 * @brief Read and parse database file.
 *
 * @param path Filepath to database file.
 * @return std::optional<std::set<MailingApp::UserId>> Users ID's.
 */
std::optional<std::set<MailingApp::UserId>> read_db(const std::string path);

/**
 * @brief Erasing database and writing new users to this database.
 *
 * @param path Filepath to users database.
 * @param users Users that is going to be written to database.
 */
void update_db(const std::string path, const std::set<UserId> users);
} // namespace MailingApp

#endif /* SRC_LIB_H */