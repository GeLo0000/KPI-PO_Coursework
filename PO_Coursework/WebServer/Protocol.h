#pragma once

#include <cstdint>

// --- Message Type Constants (1 byte) ---

// Client Requests
constexpr uint8_t kCmdSearch = 0x01;

// Server Responses
constexpr uint8_t kRespOk = 0x01;
constexpr uint8_t kRespEmpty = 0x02;
constexpr uint8_t kRespError = 0x03;

/*
PROTOCOL DESCRIPTION:

1. Client Request (SEARCH):
   [1 byte ] Command  (kCmdSearch)
   [4 bytes] Length   (Length of the word in bytes)
   [N bytes] Word     (The word text, no null terminator)

   Example for "fox":
   [0x01] [0x00, 0x00, 0x00, 0x03] ['f', 'o', 'x']

2. Server Response:
   [1 byte ] Status   (kRespOk or kRespEmpty)
   [4 bytes] Count    (Number of files found)

   ... List of files follows. For each file: ...
   [4 bytes] NameLen  (Length of the filename)
   [N bytes] Name     (Filename)
*/