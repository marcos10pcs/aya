#include "aya/Crypt.hpp"
#include "RbxFormat.hpp"
#include <boost/scoped_array.hpp>

#include <iostream>
#include <string>
#include <sstream>

#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

namespace
{
// The stuff between "-----BEGIN PUBLIC KEY-----" is base64 encoded string, the key is also base64 encoded & should work.

// Public key matching MercuryCore's data/private.pem.
// The private key remains server-side; only this public key is compiled into the client.
char publicKey[] = {"-----BEGIN PUBLIC KEY-----\n"
                    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA47jNmtEn6lbnOStIRRaa\n"
                    "Jv8PScicD1PSBMDzL0QBQhl7nwxK5Dcg+FKspz6TmoQ7x6fXYQ0QBQpCxCKJc07o\n"
                    "lYhxXhHCRMKPyXWxl0PHXdFB+j8bKin+P3DsdFgGa3Lo8DSsAkJv4btFjwCWTiZ0\n"
                    "9pdZt/NZpHjg91/6iOQ0fytokFpEpKhYbnA/rHOZWHlRciOhjV4oVLj8UXr7SmX3\n"
                    "7oJ4HmfFCRGi8q/hWhAVD1iZq+kmolk5g7KSA7OwI228mhUPJ9JgvouOL27a3W37\n"
                    "WIsPhpuhxi6zUfr6mo6xENwLZSdttfQAQGk8qYlmZqIzpk4ZryVBIyG9uh47l/3g\n"
                    "BwIDAQAB\n"
                    "-----END PUBLIC KEY-----\n"};

// Helper Fn for Base64 Decode, pass out the unsigned char** with the caller responsibility to free out
int unbase64(char* input, unsigned char** out)
{
    BIO *b64, *bmem;

    // base64 encoding is longer than the original 'string'
    int length = strlen(input);
    unsigned char* buffer = (unsigned char*)malloc(length);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);

    length = BIO_read(bmem, buffer, length);
    buffer[length] = 0;

    BIO_free_all(bmem);

    *out = buffer; // caller frees
    return length;
}

// verify that the plaintext message is matching signature (base64 encoded)
int verifyMessage(char* message, char* sig_b64)
{
    SHA_CTX sha1_ctx = {0};
    unsigned char digest[SHA_DIGEST_LENGTH];
    int rc = 1;

    rc = SHA1_Init(&sha1_ctx);
    if (1 != rc)
        throw Aya::runtime_error("Error during SHA1_Init");

    rc = SHA1_Update(&sha1_ctx, (unsigned char*)message, strlen(message));
    if (1 != rc)
        throw Aya::runtime_error("Error during SHA1_Update");

    rc = SHA1_Final(digest, &sha1_ctx);
    if (1 != rc)
        throw Aya::runtime_error("Error during SHA1_Final");

    unsigned char* signature;
    int length = unbase64(sig_b64, &signature);

    // WARNING: no error checking for brevity
    BIO* bio = BIO_new_mem_buf(publicKey, sizeof(publicKey));

    RSA* rsa_key = 0;
    rsa_key = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);

    int ret = RSA_verify(NID_sha1, digest, SHA_DIGEST_LENGTH, signature, length, rsa_key);

    ERR_print_errors_fp(stdout);

    RSA_free(rsa_key);
    BIO_free(bio);
    free(signature);

    return ret;
}
} // namespace

namespace Aya
{
Crypt::Crypt() {}

Crypt::~Crypt() {}

// Crypt implementation with RSA security using SHA1 algorithm for signature verification.
void Crypt::verifySignatureBase64(std::string messageStr, std::string signatureStr)
{
#ifndef AYA_TEST_BUILD
    if (!verifyMessage((char*)messageStr.c_str(), (char*)signatureStr.c_str()))
        throw std::runtime_error(""); // message hidden on purpose - prevent reverse engineering
#endif
}

} // namespace Aya
