#pragma once

#include <boost/function.hpp>

#include "Utility/HttpAsync.hpp"

/// Keeps authentication cookies in sync between Protected Model IE and unprotected IE
class AuthenticationMarshallar
{
    std::string domain;

public:
    AuthenticationMarshallar(const char* domain);
    ~AuthenticationMarshallar(void);
    std::string Authenticate(const char* url, const char* ticket);
    Aya::HttpFuture AuthenticateAsync(const char* url, const char* ticket);
};
