#pragma once

#include <cstdint>

/**
 * This defines protocol elements for the S3TP connection between
 * HORST and its clients (like remoteexec)
 */

/**
 * Available message types
 */
enum class MessageType : std::uint8_t {
    NONE	= 0,
    STDIN	= 1,
    STDOUT	= 2,
    STDERR	= 4,
    STARTED	= 8,
    ENDOFFILE	= 16,
};
