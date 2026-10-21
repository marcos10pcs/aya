
#include "AuthenticationMarshallar.hpp"
#include "Utility/Http.hpp"
#include "aya/Boost.hpp"

#undef min
#undef max

#include "DataModel/ContentProvider.hpp"
#include <sstream>
#include <boost/algorithm/string.hpp>

AuthenticationMarshallar::AuthenticationMarshallar(const char* domain)
    : domain(domain)
{
}

AuthenticationMarshallar::~AuthenticationMarshallar(void) {}

std::string buildUrl(const char* url, const char* ticket)
{
    std::string compound = url;
    if (ticket)
    {
        compound += "?suggest=";
        compound += ticket;
    }

    return compound;
}

std::string AuthenticationMarshallar::Authenticate(const char* url, const char* ticket)
{
    try
    {
        // Post our ticket back to Roblox to suggest a re-authentication
        std::string result;
        {
            Aya::Http http(buildUrl(url, ticket).c_str());
            http.setAuthDomain(domain);
            http.get(result);
        }

        // The http content is the new ticket
        return result;
    }
    catch (std::exception&)
    {
        // Report Error!
        return "";
    }
}

Aya::HttpFuture AuthenticationMarshallar::AuthenticateAsync(const char* url, const char* ticket)
{
    Aya::HttpOptions options;
    options.addHeader(Aya::Http::kRBXAuthenticationNegotiation, domain);
    return Aya::HttpAsync::get(buildUrl(url, ticket), options);
}