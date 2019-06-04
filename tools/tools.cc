#include "tools.h"

#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pssr.h>
#include <cryptopp/rsa.h>
#include <cryptopp/whrlpool.h>

namespace tools
{

using Signer = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Signer;
using Verifier = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Verifier;

KeyPairHex RsaGenerateHexKeyPair(unsigned int aKeySize)
{
    KeyPairHex keyPair;
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, aKeySize);
    CryptoPP::RSA::PublicKey publicKey(privateKey);

    publicKey.Save(CryptoPP::HexEncoder(
                new CryptoPP::StringSink(keyPair.publicKey)).Ref());
    privateKey.Save(CryptoPP::HexEncoder(
                new CryptoPP::StringSink(keyPair.privateKey)).Ref());

    return keyPair;
}

std::string RsaSignString(const std::string &aPrivateKeyStrHex,
        const std::string &aMessage)
{
    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.Load(CryptoPP::StringSource(aPrivateKeyStrHex, true,
                new CryptoPP::HexDecoder()).Ref());

    std::string signature;
    Signer signer(privateKey);
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::StringSource ss(aMessage, true,
            new CryptoPP::SignerFilter(rng, signer,
                new CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(signature))));

    return signature;
}

bool RsaVerifyString(const std::string &aPublicKeyStrHex,
        const std::string &aMessage,
        const std::string &aSignatureStrHex)
{
    CryptoPP::RSA::PublicKey publicKey;
    publicKey.Load(CryptoPP::StringSource(aPublicKeyStrHex, true,
                new CryptoPP::HexDecoder()).Ref());

    std::string decodedSignature;
    CryptoPP::StringSource ss(aSignatureStrHex, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::StringSink(decodedSignature)));

    bool result = false;
    Verifier verifier(publicKey);
    CryptoPP::StringSource ss2(decodedSignature + aMessage, true,
            new CryptoPP::SignatureVerificationFilter(verifier,
                new CryptoPP::ArraySink((byte*)&result,
                    sizeof(result))));

    return result;
}

std::string Sha256(const std::string &input)
{
    CryptoPP::SHA256 hash;
    byte digest[CryptoPP::SHA256::DIGESTSIZE ];
    hash.CalculateDigest(digest, (byte *)input.c_str(), input.length());

    CryptoPP::HexEncoder encoder;
    std::string output;

    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;
}

} //tools

