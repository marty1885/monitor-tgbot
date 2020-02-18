#pragma once

#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>

#include <openssl/sha.h>

inline std::string sha256(const std::string str)
{
	unsigned char hash[SHA256_DIGEST_LENGTH+1];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ss;
	for(int i=0;i<SHA256_DIGEST_LENGTH; i++)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	return ss.str();
}

inline std::string generate_salt(size_t characters = 16)
{
	// We'll trust C++ to be secure for now
	std::random_device rd{};
	std::mt19937 rng(rd());
	const std::string alphabet="0123456789abcdfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::uniform_int_distribution<int> dist(0, alphabet.size()-1);
	std::string res;
	res.reserve(characters);
	for(size_t i=0;i<characters;i++)
		res += alphabet[dist(rng)];
	return res;
}

inline std::string gen_password(const std::string& password)
{
	std::string salt = generate_salt();
	std::string hash = sha256(salt+password);
	return "SHA256:"+salt+":"+hash;
}